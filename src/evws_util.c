/*
 * libevws
 *
 * Copyright (c) 2013 github.com/crunchyfrog
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "evws_util.h"

#include <string.h>
#include <ctype.h>
#include <nettle/base64.h>
#include <nettle/sha.h>

#include "http_parser.h"

enum ws_header {
  NOT_RELEVANT = 0,
  UPGRADE = 1,
  CONNECTION = 2,
  SEC_WEBSOCKET_KEY = 3,
  SEC_WEBSOCKET_VERSION = 4,
};

struct http_wsparse_info {
  char *accept_key;
  unsigned char found_upgrade : 1;
  unsigned char found_connection : 1;
  unsigned char found_key : 1;
  unsigned char found_version : 1;
  // internal use
  enum ws_header header;
};

#define STRNCASEEQL(data, lstring, len) \
  ((len) == sizeof((lstring)) - 1 && !strncasecmp((data), (lstring), (len)))

static int on_header_field(http_parser* parser, const char *data, size_t len) {
  struct http_wsparse_info* info = (struct http_wsparse_info*)parser->data;
  if (STRNCASEEQL(data, "Upgrade", len)) {
    info->header = UPGRADE;
  } else if (STRNCASEEQL(data, "Connection", len)) {
    info->header = CONNECTION;
  } else if (STRNCASEEQL(data, "Sec-WebSocket-Key", len)) {
    info->header = SEC_WEBSOCKET_KEY;
  } else if (STRNCASEEQL(data, "Sec-WebSocket-Version", len)) {
    info->header = SEC_WEBSOCKET_VERSION;
  } else {
    info->header = NOT_RELEVANT;
  }
  return 0;
}

static void sha1(uint8_t *dst, const uint8_t *src, size_t src_length) {
  struct sha1_ctx ctx;
  sha1_init(&ctx);
  sha1_update(&ctx, src_length, src);
  sha1_digest(&ctx, SHA1_DIGEST_SIZE, dst);
}

static void base64(uint8_t *dst, const uint8_t *src, size_t src_length) {
  struct base64_encode_ctx ctx;
  base64_encode_init(&ctx);
  base64_encode_raw(dst, src_length, src);
}

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

static void create_accept_key(char *dst, const char *client_key) {
  uint8_t sha1buf[20], key_src[60];
  memcpy(key_src, client_key, 24);
  memcpy(key_src+24, WS_GUID, 36);
  sha1(sha1buf, key_src, sizeof(key_src));
  base64((uint8_t*)dst, sha1buf, 20);
  dst[BASE64_ENCODE_RAW_LENGTH(20)] = '\0';
}

static long atoin(const char *data, size_t len) {
  int n = 0;
  while (len && isspace((int)*data)) {
    --len, ++data;
  }

  while (len-- && isdigit((int)*data)) {
    n = n*10 + *data++ -'0';
  }

  return n;
}

static int header_is_value(const char *data, size_t len,
    const char* value, size_t value_len) {
  const char* endofdata = data + len;
  while(data < endofdata && isspace((int)*data)) {
    data++;
  }
  if (endofdata - data < value_len || strncasecmp(data, value, value_len)) {
    return 0;
  }
  data += value_len;
  while(data < endofdata && isspace((int)*data)) {
    data++;
  }
  return data == endofdata;
}

static int header_has_value(const char *data, size_t len,
    const char* value, size_t value_len) {
  const char* start = data;
  const char* endofdata = data + len;
  while (start < endofdata) {
    while (start < endofdata && *start == ',') {
      start++;
    }
    if (start == endofdata) {
      break;
    }
    const char* end = start;
    while (end < endofdata && *end != ',') {
      end++;
    }
    if (header_is_value(start, end - start, value, value_len)) {
      return 1;
    }
    start = end;
  }
  return 0;
}

static int on_header_value(http_parser* parser, const char *data, size_t len) {
  struct http_wsparse_info* info = (struct http_wsparse_info*)parser->data;
  switch(info->header) {
  case UPGRADE:
    if (!header_is_value(data, len, "websocket", sizeof("websocket") -1)) {
      return -1;
    }
    info->found_upgrade = 1;
    break;
  case CONNECTION:
    if (!header_has_value(data, len, "Upgrade", sizeof("Upgrade") - 1)) {
      return -1;
    }
    info->found_connection = 1;
    break;
  case SEC_WEBSOCKET_KEY: {
    if (len && isspace((int)*data)) {
      --len, ++data;
    }
    if (len < 24) {
      return -1;
    }
    create_accept_key(info->accept_key, data);
    info->found_key = 1;
    break;
  }
  case SEC_WEBSOCKET_VERSION:
    if (atoin(data, len) != 13) {
      return -1;
    }
    info->found_version = 1;
    break;
  default:
    break;
  }
  return 0;
}

static int on_headers_complete(http_parser* parser) {
  if (parser->method != HTTP_GET) {
    return -1;
  }
  if (parser->http_major < 1 ||
      (parser->http_major == 1 && parser->http_minor < 1)) {
    return -1;
  }
  struct http_wsparse_info* info = (struct http_wsparse_info*)parser->data;
  if (!info->found_upgrade || !info->found_connection ||
      !info->found_key || !info->found_version) {
    return -1;
  }
  return 0;
}

int evaluate_websocket_handshake(const char* data, size_t len,
    char accept_key[29]) {
  http_parser parser;
  http_parser_init(&parser, HTTP_REQUEST);
  struct http_wsparse_info info;
  memset(&info, 0, sizeof(info));
  info.accept_key = accept_key;
  parser.data = &info;
  http_parser_settings settings;
  memset(&settings, 0, sizeof(settings));
  settings.on_header_field = &on_header_field;
  settings.on_header_value = &on_header_value;
  settings.on_headers_complete = &on_headers_complete;
  size_t plen = http_parser_execute(&parser, &settings, (char*)data, len);

  if (plen != len || !parser.upgrade) {
    return -1;
  }

  return 0;
}

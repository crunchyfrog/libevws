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

#include <stdio.h>
#include <string.h>

#include "evws_util.h"

struct header_test {
  const char* headers;
  const char* accept_key;
};

struct header_test header_tests[] = {
    { // Chrome Version 28.0.1500.72 m
    "GET /?encoding=text HTTP/1.1\r\n"
    "Upgrade: websocket\r\n"
    "Connection: Upgrade\r\n"
    "Host: localhost:9001\r\n"
    "Origin: http://www.websocket.org\r\n"
    "Pragma: no-cache\r\n"
    "Cache-Control: no-cache\r\n"
    "Sec-WebSocket-Key: pHyydPcbwh5fM6Hx1sPeZQ==\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "Sec-WebSocket-Extensions: x-webkit-deflate-frame\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n"
    "\r\n",
    "QA00zyF0fJsKXlDYJwV4Fa+7HMw="
    },

    { // Firefox 22.0
    "GET /?encoding=text HTTP/1.1\r\n"
    "Host: localhost:9001\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:22.0) Gecko/20100101 Firefox/22.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "Accept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "Origin: http://www.websocket.org\r\n"
    "Sec-WebSocket-Key: R8ymknNrQc1CQAjSiYNtFQ==\r\n"
    "Connection: keep-alive, Upgrade\r\n"
    "Pragma: no-cache\r\n"
    "Cache-Control: no-cache\r\n"
    "Upgrade: websocket\r\n"
     "\r\n",
     "7v/4oMg5T2mJU9lBXWItRkf6nr8="
    },

    { // Internet Explorer 10.0.9200.16635
    "GET /?encoding=text HTTP/1.1\r\n"
    "Origin: http://www.websocket.org\r\n"
    "Sec-WebSocket-Key: aUC41cqHrXtpjwemZXdBBw==\r\n"
    "Connection: Upgrade\r\n"
    "Upgrade: Websocket\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)\r\n"
    "Host: localhost:9001\r\n"
    "DNT: 1\r\n"
    "Cache-Control: no-cache\r\n"
    "\r\n",
    "UcIt6wyrPyq1VoEzXxGz2bOilUQ="
    },

    { // Non-WebSocket request
    "GET / HTTP/1.1\r\n"
    "Host: localhost:9001\r\n"
    "Connection: keep-alive\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36\r\n"
    "Accept-Encoding: gzip,deflate,sdch\r\n"
    "Accept-Language: en-US,en;q=0.8\r\n"
    "",
    NULL
    },

    { // Case test
    "GET / HTTP/1.1\r\n"
    "Upgrade: wEbSoCkEt\r\n"
    "Connection: UpGrAdE\r\n"
    "Host: localhost:9001\r\n"
    "Sec-WebSocket-Key: VM5KcH8ujx84hnTSWcC8wA==\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "\r\n",
    "mUSUfTLpQTxYDKtnoIc8cQyFsiM="
    },

    { // Wrong version
    "GET / HTTP/1.1\r\n"
    "Upgrade: wEbSoCkEt\r\n"
    "Connection: UpGrAdE\r\n"
    "Host: localhost:9001\r\n"
    "Sec-WebSocket-Key: VM5KcH8ujx84hnTSWcC8wA==\r\n"
    "Sec-WebSocket-Version: 12\r\n"
    "\r\n",
    NULL
    },

    { // Missing connection
    "GET / HTTP/1.1\r\n"
    "Upgrade: websocket\r\n"
    "Host: localhost:9001\r\n"
    "Sec-WebSocket-Key: VM5KcH8ujx84hnTSWcC8wA==\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "\r\n",
    NULL
    },

    { // Weird (but acceptable) upgrade and connection header
    "GET / HTTP/1.1\r\n"
    "Upgrade:       websocket     \r\n"
    "Connection: ,upgradi   ,,,upgrade   , keep-alive \r\n"
    "Host: localhost:9001\r\n"
    "Sec-WebSocket-Key: VM5KcH8ujx84hnTSWcC8wA==\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "\r\n",
    "mUSUfTLpQTxYDKtnoIc8cQyFsiM="
    },

    { // Bad key
    "GET / HTTP/1.1\r\n"
    "Upgrade: websocket\r\n"
    "Connection: upgrade\r\n"
    "Host: localhost:9001\r\n"
    "Sec-WebSocket-Key: VM5KcH8ujx84nTSWcC8wA==\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "\r\n",
    NULL
    },
};

int main(int argc, char** argv) {
  int i;
  for (i = 0; i < sizeof(header_tests)/sizeof(struct header_test); i++) {
    char accept_key[29];
    int ret = evaluate_websocket_handshake(header_tests[i].headers,
        strlen(header_tests[i].headers), accept_key);
    if (header_tests[i].accept_key == NULL) {
      if (ret == 0) {
        fprintf(stderr, "FAIL: header_test %d incorrectly accepted:\n %s", i,
            header_tests[i].headers);
        return -1;
      }
    } else {
      if (ret == -1) {
        fprintf(stderr, "FAIL: header_test %d incorrectly rejected:\n %s", i,
            header_tests[i].headers);
        return -1;
      }
      if (strncmp(header_tests[i].accept_key, accept_key, 29)) {
        fprintf(stderr, "FAIL: header_test %d incorrect accept key:\n %s\n"
            "good key: %s\nreturned key: %s\n", i, header_tests[i].headers,
            header_tests[i].accept_key, accept_key);
        return -1;
      }
    }
  }
  return 0;
}

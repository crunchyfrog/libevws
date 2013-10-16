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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "evws/wslistener.h"
#include "evws/evws.h"

void message_handler(struct evwsconn* conn, enum evws_data_type data_type,
    const unsigned char* data, int len, void* user_data) {
  evwsconn_send_message(conn, data_type, data, len);
}

void done_handler(struct evwsconn* conn, void* user_data) {
  evwsconn_free(conn);
}

void new_wsconnection(struct evwsconnlistener *wslistener,
    struct evwsconn *conn, struct sockaddr *address, int socklen,
    void* user_data) {
  evwsconn_set_cbs(conn, message_handler, done_handler, done_handler, NULL);
}

void ws_listener_error(struct evwsconnlistener *wslistener, void* user_data) {
  fprintf(stderr, "Error on Web Socket listener: %s\n", strerror(errno));
  exit(-1);
}

void read_cb(struct bufferevent *bev, void* user_data) {
  struct evbuffer* input = bufferevent_get_input(bev);
  size_t len = evbuffer_get_length(input);
  unsigned char* data = evbuffer_pullup(input, len);
  struct evbuffer* output = bufferevent_get_output(bev);
  evbuffer_add(output, data, len);
  evbuffer_drain(input, len);
}

/*static*/
void event_cb(struct bufferevent *bev, short events, void *user_data) {
  bufferevent_free(bev);
}

void new_connection(struct evconnlistener *evlistener,
    evutil_socket_t fd, struct sockaddr *address, int socklen,
    void *user_data) {
  struct event_base* base = evconnlistener_get_base(evlistener);
  struct bufferevent *bev = bufferevent_socket_new(base, fd,
      BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
  bufferevent_enable(bev, EV_READ|EV_WRITE);
}

void listener_error(struct evconnlistener *evlistener, void *user_data) {
  fprintf(stderr, "Error on socket listener: %s\n", strerror(errno));
  exit(-1);
}

int main(int argc, char** argv) {
  struct event_base* base = event_base_new();

  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  sin.sin_port = htons(80);

  struct evwsconnlistener* levws = evwsconnlistener_new_bind(
      base, new_wsconnection, NULL,
      LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, NULL,
      (struct sockaddr*)&sin, sizeof(sin));
  if (!levws) {
    fprintf(stderr, "Error creating Web Socket listener: %s\n",
        strerror(errno));
    exit(-1);
  }
  evwsconnlistener_set_error_cb(levws, ws_listener_error);

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  sin.sin_port = htons(8000);

  struct evconnlistener *evlistener = evconnlistener_new_bind(
      base, new_connection, NULL,
      LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
      (struct sockaddr*)&sin, sizeof(sin));
  if (!evlistener) {
    fprintf(stderr, "Error creating socket listener: %s\n",
        strerror(errno));
    exit(-1);
  }
  evconnlistener_set_error_cb(evlistener, listener_error);

  event_base_dispatch(base);

  return 0;
}

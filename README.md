libevws
=============

This is a C library for handling WebSocket and Websocket Secure connections.  The library itself is pretty small and it mainly serves to glue together a number of well-written C libraries to provide this functionality.

## Requirements

 * [libevent](http://libevent.org/)
 * [wslay](https://tatsuhiro-t.github.io/wslay/index.html)
 * [nettle](http://www.lysator.liu.se/~nisse/nettle/)
 * [OpenSSL](https://www.openssl.org/)

## Install

    ./autogen.sh
    ./configure
    make
    make check # optional
    make install

## API

The API attempts to closely mirror libevent's API for raw sockets.  Complete documentation is [here](http://crunchyfrog.github.io/libevws/doxygen/html/).

A simple WebSocket echo server can be found in the examples directory [here](https://github.com/crunchyfrog/libevws/blob/master/examples/echo_server.c).

## Tests

The echo server in the examples directory passes all of the [Autobahn WebSocket Tests](http://autobahn.ws/): [Results](http://crunchyfrog.github.io/libevws/autobahn/)

There is a small amount of unit testing also included.

## Motivation

This library is intended to be a simple lightweight WebSocket server library.  It is also intended that it can be easily used in a server that also handles raw sockets (see example [here](https://github.com/crunchyfrog/libevws/blob/master/examples/dual_echo_server.c)) or does lots of other IO via libevent.

## Status

This is v0.1 of the library so, while tested and used successfully, it has not been battle-hardened.

## Possible TODOs

 * Server initiated pings (the server does already correctly reply to all pings received)
 * Improve efficiency by eliminating bufferevent

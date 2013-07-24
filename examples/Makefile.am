#
# libevws
#
# Copyright (c) 2013 github.com/crunchyfrog
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 

AM_CFLAGS = -Wall -O2 -I${top_srcdir}/src -I${top_srcdir}/src/include

noinst_PROGRAMS = echo_server dual_echo_server

echo_server_SOURCES = echo_server.c
echo_server_LDADD = ${top_builddir}/src/libevws.la
echo_server_LDFLAGS = -static

dual_echo_server_SOURCES = dual_echo_server.c
dual_echo_server_LDADD = ${top_builddir}/src/libevws.la
dual_echo_server_LDFLAGS = -static

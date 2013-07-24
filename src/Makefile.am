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

SUBDIRS = include

AM_CFLAGS = -Wall -O2 -I$(srcdir)/include -I$(builddir)/include

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libevws.pc
DISTCLEANFILES = $(pkgconfig_DATA)

lib_LTLIBRARIES = libevws.la

OBJECTS = evws_util.c evws.c http_parser.c wslistener.c
HFILES = evws_util.h evws-internal.h http_parser.h

libevws_la_SOURCES = $(HFILES) $(OBJECTS)
libevws_la_LDFLAGS = -no-undefined -version-info 0:0:0

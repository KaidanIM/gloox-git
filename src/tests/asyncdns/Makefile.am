##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual

noinst_PROGRAMS = asyncdns_test

asyncdns_test_SOURCES = asyncdns_test.cpp
asyncdns_test_LDADD = ../../dns.o ../../asyncdns.o \
                        ../../mutex.o ../../thread.o ../../util.o \
                        ../../gloox.o ../../logsink.o
asyncdns_test_CFLAGS = $(CPPFLAGS)

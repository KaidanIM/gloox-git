##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual

noinst_PROGRAMS = tlsschannel_test

tlsschannel_test_SOURCES = tlsschannel_test.cpp
tlsschannel_test_LDADD = ../../tlsschannelclient.o ../../tlsschannelserver.o ../../tlsschannelbase.o ../../gloox.o ../../mutex.o
tlsschannel_test_CFLAGS = $(CPPFLAGS)

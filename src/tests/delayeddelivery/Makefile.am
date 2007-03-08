##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = delayeddelivery_test

delayeddelivery_test_SOURCES = delayeddelivery_test.cpp ../../delayeddelivery.cpp ../../tag.cpp \
		../../jid.cpp ../../prep.cpp ../../main.cpp
delayeddelivery_test_LDADD =
delayeddelivery_test_CFLAGS = $(CPPFLAGS)

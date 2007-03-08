##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = xdelayeddelivery_test

xdelayeddelivery_test_SOURCES = xdelayeddelivery_test.cpp ../../xdelayeddelivery.cpp ../../tag.cpp \
		../../jid.cpp ../../prep.cpp ../../main.cpp
xdelayeddelivery_test_LDADD =
xdelayeddelivery_test_CFLAGS = $(CPPFLAGS)

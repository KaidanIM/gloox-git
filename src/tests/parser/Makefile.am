##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = parser_test

parser_test_SOURCES = parser_test.cpp parser.cpp tag.cpp stanza.cpp jid.cpp prep.cpp clientbase.cpp
parser_test_LDADD =
parser_test_CFLAGS = $(CPPFLAGS)


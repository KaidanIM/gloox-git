##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = tag_test tag_perf

tag_test_SOURCES = tag_test.cpp ../../tag.cpp
tag_test_LDADD =
tag_test_CFLAGS = $(CPPFLAGS)

tag_perf_SOURCES = tag_perf.cpp ../../tag.cpp
tag_perf_LDADD =
tag_perf_CFLAGS = $(CPPFLAGS)

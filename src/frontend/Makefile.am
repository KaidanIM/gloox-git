##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g -fno-inline $(all_includes)

bin_PROGRAMS = frontend

frontend_SOURCES = frontend.cpp
frontend_LDADD = ../jlib/libclient.la  -lpthread
frontend_LDFLAGS = $(all_libraries) $(KDE_RPATH)

METASOURCES = AUTO

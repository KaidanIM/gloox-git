##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g -fno-inline

SUBDIRS = wrapper

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = jclient.cpp thread.cpp jthread.cpp disco.cpp roster.cpp \
			adhoc.cpp privatexml.cpp prep.cpp registration.cpp
libgloox_la_LDFLAGS = -version-info 0:2:0 -no-undefined
libgloox_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS)
libgloox_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)

libglooxincludedir = $(includedir)/gloox
libglooxinclude_HEADERS = adhoc.h                 jclient.h            registration.h \
			adhoccommandprovider.h  jthread.h            registrationhandler.h \
			connectionlistener.h    messagehandler.h     roster.h \
			disco.h                 prep.h               rosterhelper.h \
			discohandler.h          presencehandler.h    rosterlistener.h \
			disconodehandler.h      privatexml.h         subscriptionhandler.h \
			iqhandler.h             privatexmlhandler.h  thread.h
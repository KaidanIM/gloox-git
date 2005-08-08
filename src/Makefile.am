##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g3 -fno-inline

SUBDIRS = wrapper

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = clientbase.cpp jclient.cpp jcomponent.cpp thread.cpp jthread.cpp \
			disco.cpp adhoc.cpp privatexml.cpp prep.cpp registration.cpp dns.cpp \
			nonsaslauth.cpp rosteritem.cpp rostermanager.cpp privacyitem.cpp \
			bookmarkstorage.cpp annotations.cpp privacymanager.cpp
libgloox_la_LDFLAGS = -version-info 0:3:0 -no-undefined -no-allow-shlib-undefined
libgloox_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS) -lpthread -lresolv
libgloox_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)

libglooxincludedir = $(includedir)/gloox
libglooxinclude_HEADERS = adhoc.h                 jclient.h            registration.h \
			adhoccommandprovider.h  jthread.h            registrationhandler.h \
			connectionlistener.h    messagehandler.h     roster.h \
			disco.h                 prep.h                \
			discohandler.h          presencehandler.h    rosterlistener.h \
			disconodehandler.h      privatexml.h         subscriptionhandler.h \
			iqhandler.h             privatexmlhandler.h  thread.h
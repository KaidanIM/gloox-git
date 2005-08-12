##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g3 -fno-inline

SUBDIRS = wrapper

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = thread.cpp jthread.cpp clientbase.cpp jclient.cpp jcomponent.cpp \
			dns.cpp prep.cpp disco.cpp adhoc.cpp privatexml.cpp registration.cpp \
			nonsaslauth.cpp rosteritem.cpp rostermanager.cpp privacyitem.cpp \
			bookmarkstorage.cpp annotations.cpp privacymanager.cpp
libgloox_la_LDFLAGS = -version-info 0:3:0 -no-undefined -no-allow-shlib-undefined
libgloox_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS) -lpthread -lresolv
libgloox_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)

libglooxincludedir = $(includedir)/gloox
libglooxinclude_HEADERS = adhoccommandprovider.h  dns.h                 privacymanager.h \
			  adhoc.h                 iqhandler.h           privatexml.h \
			  annotations.h           jclient.h             privatexmlhandler.h \
			  annotationshandler.h    jcomponent.h          registration.h \
			  bookmarkhandler.h       jthread.h             registrationhandler.h \
			  bookmarkstorage.h       messagehandler.h      rosteritem.h \
			  clientbase.h            nonsaslauth.h         rosterlistener.h \
			  connectionlistener.h    prep.h                rostermanager.h \
			  disco.h                 presencehandler.h     subscriptionhandler.h \
			  discohandler.h          privacyitem.h         thread.h \
			  disconodehandler.h      privacylisthandler.h

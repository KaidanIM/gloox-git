##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = . tests

AM_CPPFLAGS = -g3 -fno-inline -Wall -pedantic -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = jid.cpp parser.cpp connection.cpp clientbase.cpp tag.cpp stanza.cpp\
			dns.cpp prep.cpp client.cpp component.cpp \
			disco.cpp adhoc.cpp privatexml.cpp registration.cpp \
			nonsaslauth.cpp rosteritem.cpp rostermanager.cpp privacyitem.cpp \
			bookmarkstorage.cpp annotations.cpp privacymanager.cpp lastactivity.cpp \
			flexoff.cpp dataform.cpp dataformfield.cpp dataformbase.cpp
libgloox_la_LDFLAGS = -version-info 0:7:0 -no-undefined -no-allow-shlib-undefined
libgloox_la_LIBADD = -lm -lresolv
libgloox_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)

libglooxincludedir = $(includedir)/gloox
libglooxinclude_HEADERS = adhoccommandprovider.h  privacymanager.h      tag.h \
			  adhoc.h                 iqhandler.h           privatexml.h \
			  annotations.h           client.h              privatexmlhandler.h \
			  annotationshandler.h    component.h           registration.h \
			  bookmarkhandler.h       registrationhandler.h jid.h \
			  bookmarkstorage.h       messagehandler.h      rosteritem.h \
			  clientbase.h            rosterlistener.h      taghandler.h \
			  connectionlistener.h    rostermanager.h       gloox.h\
			  disco.h                 presencehandler.h     subscriptionhandler.h \
			  discohandler.h          privacyitem.h         stanza.h\
			  disconodehandler.h      privacylisthandler.h  loghandler.h \
			  lastactivity.h          lastactivityhandler.h flexoff.h \
			  flexoffhandler.h        dataform.h            dataformfield.h \
			  dataformitem.h          dataformbase.h        dataformresult.h

noinst_HEADERS = prep.h parser.h connection.h dns.h nonsaslauth.h

##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = src

EXTRA_DIST = LICENSE Doxyfile gloox.pc.in

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = gloox.pc

bin_SCRIPTS = gloox-config

apidox:
	doxygen

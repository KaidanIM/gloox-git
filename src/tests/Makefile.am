##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = dataform dataformfield parser tag

test: ${SUBDIRS}
	@for i in $?; do \
	  $$i/$${i}_test;    \
	done;

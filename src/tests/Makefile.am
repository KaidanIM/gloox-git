##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = dataform dataformfield parser tag md5 sha

test: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all; \
	done;
	@for i in $?; do \
	  $$i/$${i}_test;    \
	done;


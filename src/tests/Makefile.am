##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = base64 dataform dataformfield delayeddelivery gpgencrypted gpgsigned md5 oob parser sha tag \
		vcardupdate xdelayeddelivery

test: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all; \
	done;
	@for i in $?; do \
	  $$i/$${i}_test;    \
	done;


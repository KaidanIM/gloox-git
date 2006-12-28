##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = base64 dataform dataformfield md5 parser sha tag vcardupdate xdelayeddelivery delayeddelivery oob \
		gpgencrypted gpgsigned

test: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all; \
	done;
	@for i in $?; do \
	  $$i/$${i}_test;    \
	done;


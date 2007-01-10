##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = base64 dataform dataformfield delayeddelivery gpgencrypted gpgsigned md5 oob parser sha stanza \
		stanzaextensionfactory tag vcardupdate xdelayeddelivery xpath

test: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all; \
	done;
	@for i in $?; do \
	  $$i/$${i}_test;    \
	done;

leakcheck: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all; \
	done;
	@for i in $?; do \
	  valgrind -q --leak-check=full --show-reachable=yes $$i/$${i}_test;    \
	done;

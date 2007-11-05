##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = base64 chatstatefilter client clientbase dataform dataformfield dataformreported dataformitem \
          delayeddelivery error \
          gpgencrypted gpgsigned iq jid md5 message messageeventfilter nickname oob parser prep presence pubsubmanager search \
          sha simanager stanzaextensionfactory subscription tag tlsgnutls vcardupdate \
          xpath zlib util

check: test

test: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all || exit; \
	done;
	@for i in $?; do \
	  $$i/$${i}_test;    \
	done;

leaktest: leakcheck

leakcheck: ${SUBDIRS}
	@for i in $?; do \
	  ${MAKE} -C $$i all || exit; \
	done;
	@for i in $?; do \
	  valgrind -q --leak-check=full --show-reachable=yes $$i/$${i}_test;    \
	done;

##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = adhoc adhoccommand adhoccommandnote amprule amp base64 \
          chatstatefilter client clientbase connectionbosh dataform dataformfield \
          dataformreported dataformitem delayeddelivery discoinfo discoitems disco \
          error featureneg gpgencrypted gpgsigned \
          inbandbytestreamibb inbandbytestream iq \
          jid jinglesession \
          md5 message messageeventfilter \
          mucroommucadmin mucroommucowner mucroommucuser \
          nickname nonsaslauthquery nonsaslauth \
          oob parser prep presence pubsubevent pubsubmanager \
          registrationquery registration \
          rostermanagerquery rostermanager searchquery search \
          sha simanager stanzaextensionfactory subscription tag tlsgnutls \
          uniquemucroomunique vcardupdate \
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

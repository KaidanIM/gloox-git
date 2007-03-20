#!/bin/sh

TESTDIR=src/tests
TESTS=`ls -l $TESTDIR |grep ^d |awk '{print $9}'`
LOGFILE=tests.log

cat /dev/null >$LOGFILE
for i in $TESTS; do
  $TESTDIR/$i/$i\_test >>$LOGFILE
  if [ $? -eq 0 ]; then
    echo "[PASS] $i"
  else
    echo "[FAIL] $i (see tests.log)"    
  fi
done

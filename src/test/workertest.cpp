/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 * published under the GPL
 */


#include "workertest.h"
#include "../worker/worker.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  WorkerTest f;
  f.start();

  return 0;
}

WorkerTest::WorkerTest()
{
}

WorkerTest::~WorkerTest()
{
}

void WorkerTest::start()
{
  c = new Worker( "mailmon", "mail", "test", "jabber.cc" );
}

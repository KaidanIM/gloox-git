/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 * published under the GPL
 */


#include "feedertest.h"
#include "../feeder/feeder.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  FeederTest f;
  f.start();

  return 0;
}

FeederTest::FeederTest()
{
}

FeederTest::~FeederTest()
{
}

void FeederTest::start()
{
  c = new Feeder( "jline", "feeder", "jline", "camaya.net" );
  c->registerInfoHandler( this );
  c->connect();
}

void FeederTest::connected()
{
  printf( "connected\n" );
}

void FeederTest::disconnected()
{
  printf( "disconnected\n" );
}

void FeederTest::rosterChanged( iksid* from, ikshowtype show )
{
  printf( "someone special: %s: %d\n", from->full, show );
}

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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
  c = new Feeder( "remon", "feeder", "remon", "camaya.net" );
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

void FeederTest::itemAdded( const string& jid )
{
  printf("buddy added: %s\n", jid.c_str());
}

void FeederTest::itemRemoved( const string& jid )
{
  printf("removed %s\n", jid.c_str());
}

void FeederTest::itemChanged( const string& jid, int status )
{
  printf("roster update: %s: %d\n", jid.c_str(), status);
}

bool FeederTest::subscriptionRequest( const string& jid, const string& msg )
{
  printf("subscribing %s, reason: %s\n", jid.c_str(), msg.c_str() );
  return true;
}

void FeederTest::roster( RosterHelper::RosterMap roster )
{
  printf("received roster. i am too stupid to display it right now\n");
}

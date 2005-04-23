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
#include <sys/types.h>
#include <signal.h>

FeederTest f;

int main(int argc, char *argv[])
{
  signal(SIGINT, sigHandler);

  if( f.setCmdLineArgs( argc, argv ) )
    f.start();
  else
    return 1;

  return 0;
}

void sigHandler( int /*signal*/ )
{
  f.c->disconnect();
}

FeederTest::FeederTest()
  : m_debug( false ), m_data( 1 )
{
}

FeederTest::~FeederTest()
{
}

void FeederTest::start()
{
  if( m_self.empty() )
    m_self = "remon@camaya.net/feeder";

  if( m_passwd.empty() )
    m_passwd = "remon";

  c = new Feeder( m_self, m_passwd, m_debug );
  c->registerInfoHandler( this );
  c->registerPollHandler( this );
  c->registerResultHandler( this );
  c->connect();
  delete c;
}

void FeederTest::connected()
{
  printf( "connected\n" );
}

void FeederTest::disconnected()
{
  printf( "disconnected\n" );
}

bool FeederTest::subscriptionRequest( const string& jid, const char* msg )
{
  printf( "subscribing %s, reason: %s\n", jid.c_str(), msg );
  return true;
}

char* FeederTest::poll()
{
/*  if( m_data % 10 )
  {
*/
    char* tmp = (char*)malloc( sizeof( int ) );
    sprintf( tmp, "%d", ++m_data );
    return tmp;
//   }
//   else
//   {
//     ++m_data;
//     return 0;
//   }

}

void FeederTest::handleResult( const string& result )
{
  printf( "received result: %s\n", result.c_str() );
}

bool FeederTest::hasData()
{
  return true;
}

bool FeederTest::setCmdLineArgs( int argc, char *argv[] )
{
  for (int i=0;i<argc;++i )
  {
    if ( argv[i][0] == '-' )
      switch ( argv[i][1] )
    {
      case 'd':
        m_debug = true;
        break;

      case 's':
        if ( argv[++i] && argv[i][0] != '-' )
        {
          m_self = argv[i];
        }
        else
        {
          printf( "parameter -s requires a Jabber ID.\n" );
          return false;
        }
        break;

      case 'p':
        if ( argv[++i] && argv[i][0] != '-' )
        {
          m_passwd =  argv[i];
        }
        else
        {
          printf( "parameter -p requires a password.\n" );
          return false;
        }
        break;
    }
  }
}

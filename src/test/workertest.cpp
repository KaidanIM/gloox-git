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


#include "workertest.h"
#include "../worker/worker.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

int main( int argc, char *argv[] )
{
  WorkerTest f;
  f.setCmdLineArgs( argc, argv );
  f.start();

  return 0;
}

WorkerTest::WorkerTest()
  : m_debug( false ) 
{
}

WorkerTest::~WorkerTest()
{
}

void WorkerTest::start()
{
  if( m_feeder.empty() )
    m_feeder = "remon@camaya.net/feeder";

  c = new Worker( "jline", "worker01", "jline", "camaya.net", m_debug );
  c->registerInfoHandler( this );
  c->registerDataHandler( this );
  c->setFeeder( m_feeder );
  c->connect();
}

void WorkerTest::connected()
{
  printf( "connected\n" );
}

void WorkerTest::disconnected()
{
  printf( "disconnected\n" );
}

void WorkerTest::data( const char* data)
{
  printf( "received data: %s\n", data );
  sleep( 5 );
  c->result( RESULT_SUCCESS, "ok" );
}

void WorkerTest::setCmdLineArgs( int argc, char *argv[] )
{
  for (int i=0;i<argc;++i )
  {
    if ( argv[i][0] == '-' )
      switch ( argv[i][1] )
    {
      case 'f':
        if ( argv[++i] && argv[i][0] != '-' )
        {
          m_feeder = argv[i];
        }
        else
        {
          printf( "parameter -f requires a Jabber ID. using default.\n" );
          m_feeder = "remon@camaya.net/feeder";
        }
        break;

      case 'd':
        m_debug = true;
        break;
    }
  }
}

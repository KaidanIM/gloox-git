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
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

WorkerTest f;

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

WorkerTest::WorkerTest()
  : m_debug( false )
{
  m_primes.push_back(2);
  m_primes.push_back(3);
  m_primes.push_back(5);
  m_primes.push_back(7);
  m_primes.push_back(11);
  m_primes.push_back(13);
  m_primes.push_back(17);
  m_primes.push_back(19);
  m_primes.push_back(23);
  m_primes.push_back(29);
  m_primes.push_back(21);
}

WorkerTest::~WorkerTest()
{
  delete c;
}

void WorkerTest::start()
{
  if( m_feeder.empty() )
    m_feeder = "remon@camaya.net/feeder";

  if( m_passwd.empty() )
    m_passwd = "jline";

  if( m_self.empty() )
    m_self = "jline@camaya.net/worker";

  c = new Worker( m_self, m_passwd, m_debug );
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
  int num = atoi( data );
  printf( "prime factors for %d: ", num );
  c->result( RESULT_SUCCESS, factorise( num ) );
}

char* WorkerTest::factorise( int number )
{
  int num = number;
  char* result = 0;
  IntList::const_iterator it = m_primes.begin();
  int rem = -1;
  while( rem != 0 && num != 1 )
  {
//     printf("using prime number: %d\n", (*it));
    int rem = num % (*it);
    if( rem == 0 )
    {
      if( !result )
      {
        result = (char*)realloc( result, sizeof( int ) );
        sprintf( result, "%d", (*it) );
      }
      else
      {
        result = (char*)realloc( result, strlen( result ) + sizeof( char ) + sizeof( int ) );
        sprintf( result, "%s*%d", result, (*it) );
      }
      num = num / (*it);
      it = m_primes.begin();
    }
    else
    {
      ++it;
      if( it == m_primes.end() )
        break;
    }
  }
  if( result )
    printf( "%s\n", result );
  else
  {
    m_primes.push_back( number );
    printf( "none. %d is prime\n", number );
  }
  return result;
}

bool WorkerTest::setCmdLineArgs( int argc, char *argv[] )
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
          printf( "parameter -f requires a Jabber ID.\n" );
          return false;
        }
        break;

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

  return true;
}

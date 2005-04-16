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




#include "worker.h"

using namespace std;


Worker::Worker( const string username, const string resource,
                const string password, const string server,
                int port, bool debug )
  : m_infoHandler( 0 ), m_dataHandler( 0 )
{
  c = new JClient( username, resource, password, server, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerIqHandler( this, XMLNS_IQ_DATA );
  c->registerIqHandler( this, XMLNS_IQ_RESULT );
  c->registerSubscriptionHandler( this );
  c->setVersion( "Worker", "0.1" );
}

Worker::~Worker()
{
  
}

void Worker::connect()
{
  c->connect();
}

void Worker::handleIq( const char* xmlns, ikspak* pak )
{
  if( iks_strncmp( XMLNS_IQ_DATA, xmlns, iks_strlen( XMLNS_IQ_DATA ) ) == 0 )
  {
    
  }
  else if( iks_strncmp( XMLNS_IQ_RESULT, xmlns, iks_strlen( XMLNS_IQ_RESULT ) ) == 0 )
  {
    
  }
  else
    printf( "unhandled xmlns: %s\n", xmlns );
}

void Worker::handleSubscription( iksid* from, iksubtype type, const char* msg )
{
}

void Worker::registerDataHandler( DataHandler* dh )
{
  m_dataHandler = dh;
}

void Worker::registerInfoHandler( InfoHandlerWorker* ih )
{
  m_infoHandler = ih;
}

void Worker::result( ResultCode code, const char* result )
{
  
}

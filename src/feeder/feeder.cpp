/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */




#include "feeder.h"

using namespace std;


Feeder::Feeder( const string username, const string resource,
                const string password, const string server,
                int port, bool debug )
{
  c = new JClient( username, resource, password, server, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerConnectionListener( this );
  c->registerMessageHandler( this );
  c->setVersion( "Feeder", "0.1" );
  c->connect();
}

Feeder::~Feeder()
{
  
}

bool Feeder::push( const char* data )
{
  PresenceList::const_iterator it = m_presence.begin();
  for( it; it != m_presence.end(); ++it )
  {
    if( (*it).second == "available" )
    {
      c->send( (*it).first, data );
      return true;
    }
  }
  return false;
}

void Feeder::registerPollHandler( PollHandler* ph )
{
  m_pollHandler = ph;
}

void Feeder::handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg )
{
  m_presence[from->full] = (char*) msg;
  if ( ( strncmp( msg, "available", 9 ) == 0 ) && m_poll )
  {
    char* data = m_pollHandler->poll();
    if ( data )
      c->send( from->full, data );
  }
}

void Feeder::handleMessage( iksid* from, iksubtype type, const char* msg )
{
  
}

void Feeder::onConnect()
{
  
}

void Feeder::onDisconnect()
{
  
}

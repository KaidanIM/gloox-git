/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */




#include "worker.h"

using namespace std;


Worker::Worker( const string username, const string resource,
                const string password, const string server,
                int port, bool debug )
{
  c = new JClient( username, resource, password, server, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerMessageHandler( this );
  c->registerSubscriptionHandler( this );
  c->connect();
}

Worker::~Worker()
{
  
}

void Worker::handleMessage( iksid* from, iksubtype type, const char* msg )
{
}

void Worker::handleSubscription( iksid* from, iksubtype type, const char* msg )
{
}

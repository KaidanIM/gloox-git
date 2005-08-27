#include "gloox/client.h"
#include "gloox/connectionlistener.h"
#include "gloox/discohandler.h"
#include "gloox/disco.h"
#include "gloox/rostermanager.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class RosterTest : public RosterListener, ConnectionListener
{
  public:
    RosterTest() {};
    ~RosterTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->setAutoPresence( true );
      j->setInitialPriority( 5 );
      j->registerConnectionListener( this );
      j->rosterManager()->registerRosterListener( this );
      j->disco()->setVersion( "rosterTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      j->connect();

      delete( j );
    }

    virtual void onConnect()
    {
    };

    virtual void onDisconnect( ConnectionError e ) { printf( "disco_test: disconnected\n" ); };

    virtual bool onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
      return true;
    };

    virtual void itemSubscribed( const std::string& jid )
    {
      printf( "subscribed %s\n", jid.c_str() );
    }

    virtual void itemAdded( const std::string& jid )
    {
      printf( "added %s\n", jid.c_str() );
    }

    virtual void itemUnsubscribed( const std::string& jid )
    {
      printf( "unsubscribed %s\n", jid.c_str() );
    }

    virtual void itemRemoved( const std::string& jid )
    {
      printf( "removed %s\n", jid.c_str() );
    }

    virtual void itemUpdated( const std::string& jid )
    {
      printf( "updated %s\n", jid.c_str() );
    }

    virtual void roster( Roster& roster )
    {
      printf( "roster arriving\nitems:\n" );
      RosterListener::Roster::const_iterator it = roster.begin();
      for( it; it != roster.end(); ++it )
      {
        printf( "jid: %s, name: %s, subscription: %d\n",
                (*it).second->jid().c_str(), (*it).second->name().c_str(),
                (*it).second->subscription() );
        StringList g = (*it).second->groups();
        StringList::const_iterator it_g = g.begin();
        for( it_g; it_g != g.end(); ++it_g )
          printf( "\tgroup: %s\n", (*it_g).c_str() );
      }
      j->rosterManager()->unsubscribe( "js@example.org", true );
    }

    virtual void itemChanged( RosterItem& item, int status, const std::string& msg )
    {
      printf( "item changed: %s\n", item.jid().c_str() );
    }

    virtual void itemAvailable( RosterItem& item, const std::string& msg )
    {
      printf( "item online: %s\n", item.jid().c_str() );
    }

    virtual bool subscriptionRequest( const std::string& jid )
    {
      printf( "subscription: %s\n", jid.c_str() );
      StringList groups;
      j->rosterManager()->subscribe( jid, "", groups );
      return true;
    }

    virtual bool unsubscriptionRequest( const std::string& jid )
    {
      printf( "unsubscription: %s\n", jid.c_str() );
      return true;
    }

  private:
    Client *j;
};

int main( int argc, char* argv[] )
{
  RosterTest *r = new RosterTest();
  r->start();
  delete( r );
}

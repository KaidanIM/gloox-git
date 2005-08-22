#include "../jclient.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../rostermanager.h"
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

      j = new JClient();
      j->setServer( "example.org" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->setAutoPresence( true );
      j->setInitialPriority( 5 );
      j->rosterManager()->registerRosterListener( this );
      j->disco()->setVersion( "rosterTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      j->connect();

      delete( j );
    }

    virtual void onConnect()
    {
    };

    virtual void onDisconnect() { printf( "disco_test: disconnected\n" ); };

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

    virtual void roster( Roster roster )
    {
      printf( "roster arriving\n" );
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

    virtual bool subscriptionRequest( const std::string& jid, const std::string& msg )
    {
      printf( "subscriprion: %s\n", jid.c_str() );
      return true;
    }

  private:
    JClient *j;
};

int main( int argc, char* argv[] )
{
  RosterTest *r = new RosterTest();
  r->start();
  delete( r );
}

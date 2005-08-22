#include "../client.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../privacylisthandler.h"
#include "../disco.h"
#include "../privacymanager.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class PLTest : public PrivacyListHandler, ConnectionListener
{
  public:
    PLTest() {};
    ~PLTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      j = new Client();
      j->setServer( "example.org" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->setAutoPresence( true );
      j->setInitialPriority( 5 );

      j->registerConnectionListener(this );
      j->disco()->setVersion( "PLTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      p = new PrivacyManager( j );
      p->registerPrivacyListHandler( this );

      j->connect();

      delete( j );
    };

    virtual void onConnect()
    {
      p->requestListNames();
    };

    virtual void onDisconnect() { printf( "disco_test: disconnected\n" ); };

    virtual void handlePrivacyListNames( const std::string& active, const std::string& def,
                                         const PrivacyListHandler::StringList& lists )
    {
      printf( "received PL...\n" );
      printf( "active list: %s\n", active.c_str() );
      printf( "default list: %s\n", def.c_str() );
      printf( "all lists:\n" );
      PrivacyListHandler::StringList::const_iterator it = lists.begin();
      for( it; it != lists.end(); it++ )
      {
        printf( "%s\n", (*it).c_str() );
      }

      PrivacyListHandler::PrivacyList l;
      PrivacyItem item( PrivacyItem::TYPE_JID, PrivacyItem::ACTION_DENY,
                        PrivacyItem::PACKET_MESSAGE, "me@there.com" );
      l.push_back( item );
      p->store( "mnyList", l );
//       p->unsetDefault();
//       p->unsetActive();
      p->requestList( "mnyList" );
    };

    virtual void handlePrivacyListResult( const std::string& id, resultEnum result )
    {
      printf( "result for id '%s': %d\n", id.c_str(), result );
    };

    virtual void handlePrivacyList( const std::string& name, PrivacyList& items )
    {
      printf( "received list: %s\n", name.c_str() );
      PrivacyListHandler::PrivacyList::iterator it = items.begin();
      for( it; it != items.end(); it++ )
      {
        printf( "item: type: %d, action: %d, packetType: %d, value: %s\n",
                (*it).type(), (*it).action(), (*it).packetType(), (*it).value().c_str() );
      }
    };

    virtual void handlePrivacyListChanged( const std::string& name )
    {
      printf( "list changed: %s\n", name.c_str() );
    };

  private:
    Client *j;
    PrivacyManager *p;
};

int main( int argc, char* argv[] )
{
  PLTest *t = new PLTest();
  t->start();
  delete( t );
}

#include "../client.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../privatexml.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class PrivateXMLTest : public PrivateXMLHandler, ConnectionListener
{
  public:
    PrivateXMLTest() {};
    ~PrivateXMLTest() {};

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
      j->disco()->setVersion( "privateXMLTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      p = new PrivateXML( j );
      p->registerPrivateXMLHandler( this, "test", "http://camaya.net/jabber/test" );

      j->connect();

      delete( j );
    }

    virtual void onConnect()
    {
      p->requestXML( "test", "http://camaya.net/jabber/test" );
    };

    virtual void onDisconnect() { printf( "disco_test: disconnected\n" ); };

    virtual void handlePrivateXML( const std::string& tag, const std::string& xmlns, const Tag& xml )
    {
      printf( "received privatexml...\n" );
      Tag x( "test" );
      x.addAttrib( "xmlns", "http://camaya.net/jabber/test" );
      std::string id = j->getID();
      Tag b( "blah", id );
      p->storeXML( x, "http://camaya.net/jabber/test" );
    }

  private:
    Client *j;
    PrivateXML *p;
};

int main( int argc, char* argv[] )
{
  PrivateXMLTest *r = new PrivateXMLTest();
  r->start();
  delete( r );
}

#include "../jclient.h"
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

      j = new JClient();
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

      j->connect( true );

      delete( j );
    }

    virtual void onConnect()
    {
      p->requestXML( "test", "http://camaya.net/jabber/test" );
    };

    virtual void handlePrivateXML( const string& tag, const string& xmlns, ikspak* pak )
    {
      printf( "received privatexml...\n" );
      iks *x = iks_new( "test" );
      iks_insert_attrib( x, "xmlns", "http://camaya.net/jabber/test" );
      string id = j->getID();
      iks_insert_cdata( iks_insert( x, "bla" ), id.c_str(), id.length() );
      p->storeXML( x, "http://camaya.net/jabber/test" );
    }

  private:
    JClient *j;
    PrivateXML *p;
};

int main( int argc, char* argv[] )
{
  PrivateXMLTest *r = new PrivateXMLTest();
  r->start();
  delete( r );
}

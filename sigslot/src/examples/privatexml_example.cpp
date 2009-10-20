#include "../client.h"
#include "../sigslot.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../privatexml.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <cstdio> // [s]print[f]

class PrivateXMLTest : public PrivateXMLHandler, public has_slots<>
{
  public:
    PrivateXMLTest() {}
    virtual ~PrivateXMLTest() {}

    void start()
    {

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );

      j->onConnect.Connect( this, &PrivateXMLTest::onConnect );
      j->onDisconnect.Connect( this, &PrivateXMLTest::onDisconnect );
      j->onTLSConnect.Connect( this, &PrivateXMLTest::onTLSConnect );

      j->disco()->setVersion( "privateXMLTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      p = new PrivateXML( j );

      j->connect();

      delete( p );
      delete( j );
    }

    void onConnect()
    {
      p->requestXML( "test", "http://camaya.net/jabber/test", this );
    }

    void onDisconnect( ConnectionError /*e*/ ) { printf( "disco_test: disconnected\n" ); }

    void onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
    }

    virtual void handlePrivateXML( const Tag* /*xml*/ )
    {
      printf( "received privatexml...\n" );
      Tag *x = new Tag( "test" );
      x->addAttribute( "xmlns", "http://camaya.net/jabber/test" );
      std::string id = j->getID();
      Tag *b = new Tag( "blah", id );
      x->addChild( b );
      p->storeXML( x, this );
    }

    virtual void handlePrivateXMLResult( const std::string& /*uid*/, PrivateXMLResult /*result*/ )
    {
    }

  private:
    Client *j;
    PrivateXML *p;
};

int main( int /*argc*/, char** /*argv*/ )
{
  PrivateXMLTest *r = new PrivateXMLTest();
  r->start();
  delete( r );
  return 0;
}

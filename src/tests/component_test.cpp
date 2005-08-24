#include "../component.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class ComponentTest : public DiscoHandler, ConnectionListener
{
  public:
    ComponentTest() {};
    ~ComponentTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      j = new Component( XMLNS_COMPONENT_ACCEPT, "example.org",
                          "component.example.org", "secret", 5000 );
      j->setStreamTo( "component.example.org" );
      j->disco()->setVersion( "componentTest", GLOOX_VERSION );

      j->registerConnectionListener( this );

      j->connect();

      delete( j );
    }

    virtual void onConnect()
    {
      printf( "connected -- disconnecting...\n" );
//       iks *x = iks_new( "presence" );
//       j->send( x );
//       j->disconnect( STATE_DISCONNECTED );
    };

    virtual void onDisconnect() { printf( "component: disconnected\n" ); };

    virtual bool onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
      return true;
    };

  private:
    Component *j;
};

int main( int argc, char* argv[] )
{
  ComponentTest *r = new ComponentTest();
  r->start();
  delete( r );
}

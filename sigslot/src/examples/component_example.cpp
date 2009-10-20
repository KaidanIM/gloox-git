#include "../component.h"
#include "../sigslot.h"
#include "../loghandler.h"
#include "../discohandler.h"
#include "../disco.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <cstdio> // [s]print[f]

class ComponentTest : public DiscoHandler, public has_slots<>, LogHandler
{
  public:
    ComponentTest() {}
    virtual ~ComponentTest() {}

    void start()
    {

      j = new Component( XMLNS_COMPONENT_ACCEPT, "example.org",
                         "component.example.org", "secret", 5000 );
      j->disco()->setVersion( "componentTest", GLOOX_VERSION );

      j->onConnect.Connect( this, &ComponentTest::onConnect );
      j->onDisconnect.Connect( this, &ComponentTest::onDisconnect );
      j->onTLSConnect.Connect( this, &ComponentTest::onTLSConnect );

      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

      j->connect();

      delete( j );
    }

    void onConnect()
    {
      printf( "connected -- disconnecting...\n" );
//       j->disconnect( STATE_DISCONNECTED );
    }

    void onDisconnect( ConnectionError /*e*/ ) { printf( "component: disconnected\n" ); }

    void onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
    }

    virtual void handleDiscoInfo( const JID& /*iq*/, const Disco::Info&, int /*context*/ )
    {
      printf( "handleDiscoInfoResult}\n" );
    }

    virtual void handleDiscoItems( const JID& /*iq*/, const Disco::Items&, int /*context*/ )
    {
      printf( "handleDiscoItemsResult\n" );
    }

    virtual void handleDiscoError( const JID& /*iq*/, const Error*, int /*context*/ )
    {
      printf( "handleDiscoError\n" );
    }

    virtual void handleLog( LogLevel level, LogArea area, const std::string& message )
    {
      printf("log: level: %d, area: %d, %s\n", level, area, message.c_str() );
    }

  private:
    Component *j;
};

int main( int /*argc*/, char** /*argv*/ )
{
  ComponentTest *r = new ComponentTest();
  r->start();
  delete( r );
  return 0;
}

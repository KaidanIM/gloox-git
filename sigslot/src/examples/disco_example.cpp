#include "../client.h"
#include "../sigslot.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../stanza.h"
#include "../gloox.h"
#include "../loghandler.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <cstdio> // [s]print[f]

class DiscoTest : public DiscoHandler, public has_slots<>, LogHandler
{
  public:
    DiscoTest() {}
    virtual ~DiscoTest() {}

    void start()
    {

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->disableRoster();

      j->onConnect.Connect( this, &DiscoTest::onConnect );
      j->onDisconnect.Connect( this, &DiscoTest::onDisconnect );
      j->onTLSConnect.Connect( this, &DiscoTest::onTLSConnect );

      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "discoTest", GLOOX_VERSION, "linux" );
      j->disco()->setIdentity( "client", "bot" );
      StringList ca;
      ca.push_back( "/path/to/cacert.crt" );
      j->setCACerts( ca );
      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

      j->connect();

      delete( j );
    }

    void onConnect()
    {
    }

    void onDisconnect( ConnectionError e ) { printf( "disco_test: disconnected: %d\n", e ); }

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
    Client *j;
};

int main( int /*argc*/, char** /*argv*/ )
{
  DiscoTest *r = new DiscoTest();
  r->start();
  delete( r );
  return 0;
}

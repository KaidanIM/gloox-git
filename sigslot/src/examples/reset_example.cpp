#include "../client.h"
#include "../sigslot.h"
#include "../disco.h"
#include "../rostermanager.h"
#include "../loghandler.h"
#include "../logsink.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <cstdio> // [s]print[f]

class RosterTest : public has_slots<>, LogHandler
{
  public:
    RosterTest() {}
    virtual ~RosterTest() {}

    void start()
    {

      JID jid( "hurkhurkss@example.net/gloox" );
      j = new Client( jid, "hurkhurks" );

      j->onConnect.Connect( this, &RosterTest::onConnect );
      j->onDisconnect.Connect( this, &RosterTest::onDisconnect );
      j->onTLSConnect.Connect( this, &RosterTest::onTLSConnect );

      j->disco()->setVersion( "resetTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

      printf( "first run: %d\n", j->connect() );
      printf( "second run: %d\n", j->connect() );

      delete( j );
    }

    void onConnect()
    {
      j->disconnect();
    }

    void onDisconnect( ConnectionError e )
    {
      printf( "reset_test: disconnected: %d\n", e );
    }

    void onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
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
  RosterTest *r = new RosterTest();
  r->start();
  delete( r );
  return 0;
}

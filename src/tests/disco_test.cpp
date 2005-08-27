#include "gloox/client.h"
#include "gloox/connectionlistener.h"
#include "gloox/discohandler.h"
#include "gloox/disco.h"
#include "gloox/stanza.h"
#include "gloox/gloox.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class DiscoTest : public DiscoHandler, ConnectionListener
{
  public:
    DiscoTest() {};
    ~DiscoTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->disableRoster();
      j->registerConnectionListener( this );
      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "discoTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );
      StringList ca;
      ca.push_back( "/path/to/cacert.crt" );
      j->setCACerts( ca );

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

    virtual void handleDiscoInfoResult( const std::string& id, const Stanza& stanza )
    {
      printf( "handleDiscoInfoResult}\n" );
    }

    virtual void handleDiscoItemsResult( const std::string& id, const Stanza& stanza )
    {
      printf( "handleDiscoItemsResult\n" );
    }

  private:
    Client *j;
};

int main( int argc, char* argv[] )
{
  DiscoTest *r = new DiscoTest();
  r->start();
  delete( r );
}

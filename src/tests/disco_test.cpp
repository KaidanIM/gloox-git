#include "../client.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../stanza.h"
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

      j = new Client();
      j->setServer( "example.org" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->disableRoster();
      j->registerConnectionListener( this );
      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "discoTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      j->connect();

      delete( j );
    }

    virtual void onConnect()
    {
      j->disco()->getDiscoInfo( "hurkhurk@example.org/gloox" );
      j->disco()->getDiscoItems( "hurkhurk@example.org/gloox" );
    };

    virtual void onDisconnect() { printf( "disco_test: disconnected\n" ); };

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

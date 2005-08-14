#include "../jclient.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
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

      j = new JClient();
      j->setServer( "example.org" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->disableRoster();
      j->registerConnectionListener( this );
      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "discoTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      j->connect( true );

      delete( j );
    }

    virtual void onConnect()
    {
      j->disco()->getDiscoInfo( "hurkhurk@example.org/gloox" );
      j->disco()->getDiscoItems( "hurkhurk@example.org/gloox" );
    };

    virtual void handleDiscoInfoResult( const string& id, const ikspak* pak )
    {
      printf( "handleDiscoInfoResult}\n" );
    }

    virtual void handleDiscoItemsResult( const string& id, const ikspak* pak )
    {
      printf( "handleDiscoItemsResult\n" );
    }

  private:
    JClient *j;
};

int main( int argc, char* argv[] )
{
  DiscoTest *r = new DiscoTest();
  r->start();
  delete( r );
}

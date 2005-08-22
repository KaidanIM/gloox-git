#include "../client.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../adhoccommandprovider.h"
#include "../disco.h"
#include "../adhoc.h"
#include "../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class AdhocTest : public ConnectionListener, AdhocCommandProvider
{
  public:
    AdhocTest() {};
    ~AdhocTest() {};

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
      j->disco()->setVersion( "adhocTest", GLOOX_VERSION );
      j->disco()->setIdentity( "client", "bot" );

      a = new Adhoc( j, j->disco() );
      a->registerAdhocCommandProvider( this, "helloworld", "Hello World!" );
      a->registerAdhocCommandProvider( this, "config", "Configuration" );
      a->registerAdhocCommandProvider( this, "shutdown", "Shutdown" );

      j->connect();

      delete( j );
    }

    void handleAdhocCommand( const std::string& command, const Tag& tag )
    {
      if( command == "helloworld" )
        printf( "Hello World!\n" );
      else if( command == "config" )
        printf( "configuration called\n" );
      else if( command == "shutdown" )
      {
        printf( "shutting down\n" );
        j->disconnect( STATE_DISCONNECTED );
      }
    }

    virtual void onConnect()
    {
    };

    virtual void onDisconnect() { printf( "disco_test: disconnected\n" ); };

  private:
    Client *j;
    Adhoc *a;
};

int main( int argc, char* argv[] )
{
  AdhocTest *r = new AdhocTest();
  r->start();
  delete( r );
}

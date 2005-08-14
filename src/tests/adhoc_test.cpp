#include "../jclient.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../adhoccommandprovider.h"
#include "../disco.h"
#include "../adhoc.h"
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

      j = new JClient();
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

      j->connect( true );

      delete( j );
    }

    void handleAdhocCommand( const string& command, const iks* x )
    {
      if( command == "helloworld" )
        printf( "Hello World!\n" );
      else if( command == "config" )
        printf( "configuration called\n" );
      else if( command == "shutdown" )
      {
        printf( "shutting down\n" );
        j->disconnect();
      }
    }

    virtual void onConnect()
    {
    };

  private:
    JClient *j;
    Adhoc *a;
};

int main( int argc, char* argv[] )
{
  AdhocTest *r = new AdhocTest();
  r->start();
  delete( r );
}

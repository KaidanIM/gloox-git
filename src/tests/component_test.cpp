#include "../jcomponent.h"
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

      j = new JComponent( XMLNS_COMPONENT_ACCEPT, "example.org",
                          "component.example.org", "secret", 5000 );
      j->setStreamTo( "component.example.org" );
      j->disco()->setVersion( "componentTest", GLOOX_VERSION );

      j->registerConnectionListener( this );

      j->connect( true );

      delete( j );
    }

    virtual void onConnect()
    {
      printf( "connected\n" );
      iks *x = iks_new( "presence" );
      j->send( x );
    };


  private:
    JComponent *j;
};

int main( int argc, char* argv[] )
{
  ComponentTest *r = new ComponentTest();
  r->start();
  delete( r );
}

#include "../jclient.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../registration.h"

#include <stdio.h>
#include <locale.h>
#include <string>

class RegTest : public RegistrationHandler, ConnectionListener
{
  public:
    RegTest() {};
    ~RegTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      j = new JClient();
      j->setServer( "jabber.cc" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->setTls( false );
      j->disableRoster();
      j->registerConnectionListener( this );
      j->setDebug( true );
      j->set_log_hook();


      m_reg = new Registration( j );
      m_reg->registerRegistrationHandler( this );

      bool block = true;
      j->connect( block );

      printf( "throttling\n");

      while( !block )
        sleep( 5 );

    }

    virtual void onConnect()
    {
//       printf( "requesting reg fields\n" );
//       m_reg->fetchRegistrationFields();

//       printf("changing password\n");
      m_reg->changePassword( "hurkhurk", "abssscdef" );

//       printf("unregistering\n");
//       m_reg->removeAccount();
    };

    virtual void handleRegistrationFields( int fields, string instructions )
    {
      printf( "fields: %d\ninstructions: %s\n", fields, instructions.c_str() );
      Registration::fieldStruct vals;
      vals.username = "hurkhurk";
      vals.password = "hurkhurks";
      m_reg->createAccount( fields, vals );
    };

    virtual void handleRegistrationResult( resultEnum result )
    {
      printf( "result: %d\n", result );
//       j->disconnect();
    };

  private:
    Registration *m_reg;
    JClient *j;

};

int main( int argc, char* argv[] )
{
  RegTest *r = new RegTest();
  r->start();
}

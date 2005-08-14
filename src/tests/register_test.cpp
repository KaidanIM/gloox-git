#include "../jclient.h"
#include "../prep.h"
#include "../connectionlistener.h"
#include "../registration.h"
using namespace gloox;

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
      j->setServer( "example.org" );
      j->setResource( "gloox" );
      j->setUsername( "hurkhurk" );
      j->setPassword( "hurkhurks" );
      j->disableRoster();
      j->disableDisco();
      j->registerConnectionListener( this );

      m_reg = new Registration( j );
      m_reg->registerRegistrationHandler( this );

      j->connect( true );

      delete( m_reg );
      delete( j );
    }

    virtual void onConnect()
    {
      // requesting reg fields
//       m_reg->fetchRegistrationFields();

      // changing password
      m_reg->changePassword( "hurkhurks" );

      // unregistering
//       m_reg->removeAccount();
    };

    virtual void handleRegistrationFields( int fields, string instructions )
    {
      printf( "fields: %d\ninstructions: %s\n", fields, instructions.c_str() );
      Registration::fieldStruct vals;
      vals.username = Prep::nodeprep( "hurkhurk" );
      vals.password = "hurkhurks";
      m_reg->createAccount( fields, vals );
    };

    virtual void handleRegistrationResult( resultEnum result )
    {
      printf( "result: %d\n", result );
      j->disconnect();
    };

  private:
    Registration *m_reg;
    JClient *j;
};

int main( int argc, char* argv[] )
{
  RegTest *r = new RegTest();
  r->start();
  delete( r );
}

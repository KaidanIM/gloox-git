#include "../client.h"
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

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->disableRoster();
      j->disableDisco();
      j->registerConnectionListener( this );

      m_reg = new Registration( j );
      m_reg->registerRegistrationHandler( this );

      j->connect();

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

    virtual void onDisconnect( ConnectionError e ) { printf( "disco_test: disconnected\n" ); };

    virtual bool onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
      return true;
    };

    virtual void handleRegistrationFields( int fields, std::string instructions )
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
    Client *j;
};

int main( int argc, char* argv[] )
{
  RegTest *r = new RegTest();
  r->start();
  delete( r );
}

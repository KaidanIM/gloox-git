#include "../client.h"
#include "../messagehandler.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../stanza.h"
#include "../gloox.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class DiscoTest : public DiscoHandler, MessageHandler
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
      j->setAutoPresence( true );
      j->setInitialPriority( 5 );
      j->registerMessageHandler( this );
      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "messageTest", GLOOX_VERSION );
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

    virtual void onDisconnect() { printf( "message_test: disconnected\n" ); };

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

    virtual void handleMessage( Stanza *stanza )
    {
      printf( "type: %d, subject: %s, message: %s, thread id: %s\n", stanza->subtype(),
              stanza->subject().c_str(), stanza->message().c_str(), stanza->thread().c_str() );
      Tag *m = new Tag( "message" );
      m->addAttrib( "from", j->jid().full() );
      m->addAttrib( "to", stanza->from().full() );
      m->addAttrib( "type", "chat" );
      Tag *b = new Tag( "body", "You said:\n> " + stanza->message() + "\nI like that statement." );
      m->addChild( b );
      if( !stanza->subject().empty() )
      {
        Tag *s = new Tag( "subject", "Re:" +  stanza->subject() );
        m->addChild( s );
      }
      j->send( m );
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

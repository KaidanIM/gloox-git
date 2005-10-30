#include "../client.h"
#include "../messagehandler.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../stanza.h"
#include "../gloox.h"
#include "../lastactivity.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class DiscoTest : public DiscoHandler, MessageHandler, ConnectionListener
{
  public:
    DiscoTest() {};
    virtual ~DiscoTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->setAutoPresence( true );
      j->setInitialPriority( -1 );
      j->registerConnectionListener( this );
      j->registerMessageHandler( this );
      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "messageTest", GLOOX_VERSION, "Linux" );
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

    virtual void onDisconnect( ConnectionError e )
    {
      printf( "message_test: disconnected: %d\n", e );
      if( e == CONN_AUTHENTICATION_FAILED )
        printf( "auth failed. reason: %d\n", j->authError() );
    };

    virtual bool onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
      return true;
    };

    virtual void handleDiscoInfoResult( Stanza * /*stanza*/, int /*context*/ )
    {
      printf( "handleDiscoInfoResult}\n" );
    }

    virtual void handleDiscoItemsResult( Stanza * /*stanza*/, int /*context*/ )
    {
      printf( "handleDiscoItemsResult\n" );
    }

    virtual void handleDiscoError( Stanza * /*stanza*/, int /*context*/ )
    {
      printf( "handleDiscoError\n" );
    }

    virtual void handleMessage( Stanza *stanza )
    {
      printf( "type: %d, subject: %s, message: %s, thread id: %s\n", stanza->subtype(),
              stanza->subject().c_str(), stanza->body().c_str(), stanza->thread().c_str() );
      Tag *m = new Tag( "message" );
      m->addAttrib( "from", j->jid().full() );
      m->addAttrib( "to", stanza->from().full() );
      m->addAttrib( "type", "chat" );
      Tag *b = new Tag( "body", "You said:\n> " + stanza->body() + "\nI like that statement." );
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

int main( int /*argc*/, char* /*argv[]*/ )
{
  DiscoTest *r = new DiscoTest();
  r->start();
  delete( r );
  return 0;
  return 0;
}

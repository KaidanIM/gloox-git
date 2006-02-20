#include "../client.h"
#include "../messagesessionhandler.h"
#include "../messageeventhandler.h"
#include "../chatstatehandler.h"
#include "../connectionlistener.h"
#include "../discohandler.h"
#include "../disco.h"
#include "../stanza.h"
#include "../gloox.h"
#include "../lastactivity.h"
#include "../loghandler.h"
#include "../logsink.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class MessageTest : public DiscoHandler, MessageSessionHandler, ConnectionListener, LogHandler,
                    MessageEventHandler, MessageHandler, ChatStateHandler
{
  public:
    MessageTest() : m_session( 0 ) {};

    virtual ~MessageTest() {};

    void start()
    {
      setlocale( LC_ALL, "" );

      JID jid( "hurkhurk@example.org/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->setAutoPresence( true );
      j->setInitialPriority( 4 );
      j->registerConnectionListener( this );
      j->setAutoMessageSession( true, this, FilterMessageEvents & FilterChatStates );
      j->disco()->registerDiscoHandler( this );
      j->disco()->setVersion( "messageTest", GLOOX_VERSION, "Linux" );
      j->disco()->setIdentity( "client", "bot" );
      StringList ca;
      ca.push_back( "/path/to/cacert.crt" );
      j->setCACerts( ca );

      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

      if( j->connect(false) )
      {
        ConnectionError ce = ConnNoError;
        while( ce == ConnNoError )
        {
          ce = j->recv();
        }
        printf( "ce: %d\n", ce );
      }

      // cleanup
      if( m_session )
      {
        m_session->removeMessageHandler();
        m_session->removeMessageEventHandler();
        m_session->removeChatStateHandler();
        delete( m_session );
      }
      delete( j );
    }

    virtual void onConnect()
    {
      printf( "connected!!!\n" );
    };

    virtual void onDisconnect( ConnectionError e )
    {
      printf( "message_test: disconnected: %d\n", e );
      if( e == ConnAuthenticationFailed )
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

      std::string msg = "You said:\n> " + stanza->body() + "\nI like that statement.";
      std::string sub;
      if( !stanza->subject().empty() )
        sub = "Re: " +  stanza->subject();

      m_session->raiseMessageEvent( MessageEventDisplayed );
      sleep( 1 );
      m_session->raiseMessageEvent( MessageEventComposing );
      sleep( 2 );
      m_session->send( msg, sub );

      if( stanza->body() == "quit" )
        j->disconnect();
    }

    virtual void handleMessageEvent( const JID& from, MessageEventType event )
    {
      printf( "received event: %d from: %s\n", event, from.full().c_str() );
    }

    virtual void handleChatState( const JID& from, ChatStateType state )
    {
      printf( "received state: %d from: %s\n", state, from.full().c_str() );
    }

    virtual void handleMessageSession( MessageSession *session )
    {
      m_session = session;
      printf( "got new session\n");
      m_session->registerMessageHandler( this );
      m_session->registerMessageEventHandler( this );
      m_session->registerChatStateHandler( this );
    }

    virtual void handleLog( LogLevel level, LogArea area, const std::string& message )
    {
      printf("log: level: %d, area: %d, %s\n", level, area, message.c_str() );
    };

  private:
    Client *j;
    MessageSession *m_session;
};

int main( int /*argc*/, char* /*argv[]*/ )
{
  MessageTest *r = new MessageTest();
  r->start();
  delete( r );
  return 0;
  return 0;
}

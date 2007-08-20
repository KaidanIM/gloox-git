#include "../client.h"
#include "../messagesessionhandler.h"
#include "../messageeventhandler.h"
#include "../messageeventfilter.h"
#include "../chatstatehandler.h"
#include "../chatstatefilter.h"
#include "../connectionlistener.h"
#include "../disco.h"
#include "../stanza.h"
#include "../gloox.h"
#include "../lastactivity.h"
#include "../loghandler.h"
#include "../logsink.h"
#include "../connectiontcpclient.h"
#include "../connectionsocks5proxy.h"
#include "../messagehandler.h"
#include "../connectionbosh.h"
// #include "../connectionhttpproxy.h"
#include "../connectiontls.h"

using namespace gloox;

#include <unistd.h>
#include <stdio.h>
#include <string>

#if defined( WIN32 ) || defined( _WIN32 )
# include <windows.h>
#endif

// Below you can change the current profile (you will need to recompile the example with the 'make' command to test)
// OPENFIRE - Connects to an Openfire server, running on localhost, port 8081
// PYTHON - Connects to a python CM (you can download here: http://blog.bluendo.com/ff/bosh-connection-manager-update ) on localhost:8080
// EJABBERD - Connects to an ejabberd server (currently doesn't work)
// SSL - Connects to an Openfire server on the SSL port (currently doesn't work)

#define PYTHON

// Change the below to the JID and password to log into your server
#define JID "mattj2@localhost/gloox/bosh"
#define PASSWORD "pumpkin"

class BoshTest : public MessageSessionHandler, ConnectionListener, LogHandler,
                    MessageEventHandler, MessageHandler, ChatStateHandler
{
  public:
    BoshTest() : m_session( 0 ), m_messageEventFilter( 0 ), m_chatStateFilter( 0 ) {}

    virtual ~BoshTest() {}

    void start()
    {

      JID jid( JID );
      j = new Client( jid, PASSWORD );
      j->registerConnectionListener( this );
      j->registerMessageSessionHandler( this, 0 );
      j->disco()->setVersion( "BoshTest", GLOOX_VERSION, "Linux" );
      j->disco()->setIdentity( "client", "bot" );
      j->disco()->addFeature( XMLNS_CHAT_STATES );
      j->setCompression(false);
      //StringList ca;
      //ca.push_back( "/path/to/cacert.crt" );
      //j->setCACerts( ca );

      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

#if  defined(EJABBERD)
       ConnectionTCPClient* conn0 = new ConnectionTCPClient( j->logInstance(), "localhost", 5280 );
	ConnectionBOSH* conn1 = new ConnectionBOSH( j, conn0, j->logInstance(), "localhost", "localhost");
#elif defined(OPENFIRE)
	ConnectionTCPClient* conn0 = new ConnectionTCPClient( j->logInstance(), "localhost", 8081 );
	ConnectionBOSH* conn1 = new ConnectionBOSH( j, conn0, j->logInstance(), "localhost", "localhost");
	j->setForceNonSasl();
#elif defined(SSL)
	    ConnectionTCPClient* conn0 = new ConnectionTCPClient( j->logInstance(), "localhost", 8483 );
	    ConnectionTLS* conn1 = new ConnectionTLS(conn0, NULL, j->logInstance());
	    ConnectionBOSH* conn2 = new ConnectionBOSH( j, conn1, j->logInstance(), "localhost", "localhost");
	    j->setConnectionImpl( conn2 );
	    j->setForceNonSasl();
	    conn2->setPath("/http-bind/");
	    conn2->setMode(ConnectionBOSH::ModeLegacyHTTP);
#elif defined(PYTHON)
	ConnectionTCPClient* conn0 = new ConnectionTCPClient( j->logInstance(), "localhost", 8080 );
	ConnectionBOSH* conn1 = new ConnectionBOSH( j, conn0, j->logInstance(), "localhost", "localhost");
#endif
	    
#ifndef SSL
       j->setConnectionImpl( conn1 );
       conn1->setPath("/http-bind/");
       conn1->setMode(ConnectionBOSH::ModeLegacyHTTP);    
#endif
       
       j->setForceNonSasl(); // Needed for non XEP-0206 compliant connection managers (such as Openfire 3.3.2)
       

      if( j->connect( false ) )
      {
        ConnectionError ce = ConnNoError;
        while( ce == ConnNoError )
        {
          ce = j->recv(200);
        }
        printf( "ce: %d\n", ce );
      }

      delete( j );
    }

    virtual void onConnect()
    {
      printf( "connected!!!\n" );
    }

    virtual void onDisconnect( ConnectionError e )
    {
      printf( "message_test: disconnected: %d\n", e );
      if( e == ConnAuthenticationFailed )
        printf( "auth failed. reason: %d\n", j->authError() );
    }

    virtual bool onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n"
              "from: %s\nto: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str(), ctime( (const time_t*)&info.date_from ),
              ctime( (const time_t*)&info.date_to ) );
      return true;
    }

    virtual void handleMessage( Stanza *stanza, MessageSession * /*session*/ )
    {
      printf( "type: %d, subject: %s, message: %s, thread id: %s\n", stanza->subtype(),
              stanza->subject().c_str(), stanza->body().c_str(), stanza->thread().c_str() );

      std::string msg = "You said:\n> " + stanza->body() + "\nI like that statement.";
      std::string sub;
      if( !stanza->subject().empty() )
        sub = "Re: " +  stanza->subject();

      m_messageEventFilter->raiseMessageEvent( MessageEventDisplayed );
      m_messageEventFilter->raiseMessageEvent( MessageEventComposing );
      m_chatStateFilter->setChatState( ChatStateComposing );
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
      printf( "got new session\n");
      // this example can handle only one session. so we get rid of the old session
      j->disposeMessageSession( m_session );
      m_session = session;
      m_session->registerMessageHandler( this );
      m_messageEventFilter = new MessageEventFilter( m_session );
      m_messageEventFilter->registerMessageEventHandler( this );
      m_chatStateFilter = new ChatStateFilter( m_session );
      m_chatStateFilter->registerChatStateHandler( this );
    }

    virtual void handleLog( LogLevel level, LogArea area, const std::string& message )
    {
	printf("%d: ", time(NULL));
	switch(area)
	{
          case LogAreaXmlIncoming:
           printf("Received XML: ");
	   break;
	  case LogAreaXmlOutgoing:
	   printf("Sent XML: ");
	   break;
	  case LogAreaClassConnectionBOSH:
	   printf("BOSH: ");
	   break;
	  default:
           printf("log: level: %d, area: %d, ", level, area);
	}
      printf("%s\n", message.c_str() );
    }

  private:
    Client *j;
    MessageSession *m_session;
    MessageEventFilter *m_messageEventFilter;
    ChatStateFilter *m_chatStateFilter;
};

int main( int /*argc*/, char** /*argv*/ )
{
  BoshTest *r = new BoshTest();
  r->start();
  delete( r );
  return 0;
}

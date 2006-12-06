#include "../client.h"
#include "../connectionlistener.h"
#include "../mucroomlistener.h"
#include "../mucroom.h"
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

#ifdef WIN32
#include <windows.h>
#endif

class MessageTest : public ConnectionListener, LogHandler, MUCRoomListener
{
  public:
    MessageTest() {};

    virtual ~MessageTest() {};

    void start()
    {
      JID jid( "hurkhurk@example.net/gloox" );
      j = new Client( jid, "hurkhurks" );
      j->registerConnectionListener( this );
      j->disco()->setVersion( "gloox muc_example", GLOOX_VERSION, "Linux" );
      j->disco()->setIdentity( "client", "bot" );
      StringList ca;
      ca.push_back( "/path/to/cacert.crt" );
      j->setCACerts( ca );

      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

      JID nick( "test@conference.jabber.org/glooxmuctest" );
      m_room = new MUCRoom( j, nick, this );

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
      delete m_room;
      delete j;
    }

    virtual void onConnect()
    {
      printf( "connected!!!\n" );
      m_room->join();
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

    virtual void handleLog( LogLevel level, LogArea area, const std::string& message )
    {
      printf("log: level: %d, area: %d, %s\n", level, area, message.c_str() );
    };

    virtual void handleMUCParticipantPresence( MUCRoom *room, const MUCRoomParticipant participant,
                                            Presence presence )
    {
      if( presence == PresenceAvailable )
        printf( "!!!!!!!!!!!!!!!! %s is in the room, too\n", participant.nick->resource().c_str() );
      else if( presence == PresenceUnavailable )
        printf( "!!!!!!!!!!!!!!!! %s left the room\n", participant.nick->resource().c_str() );
      else
        printf( "Presence is %d of %s\n", presence, participant.nick->resource().c_str() );
    };

    virtual void handleMUCMessage( MUCRoom *room, const std::string& nick, const std::string& message,
                                   bool history, const std::string& when )
    {
      printf( "%s said: '%s' (history: %s)\n", nick.c_str(), message.c_str(), history ? "yes" : "no" );
      if( nick != "glooxmuctest" && !history )
        m_room->send( message + " yourself" );
    };

    virtual void handleMUCSubject( MUCRoom *room, const std::string& nick, const std::string& subject )
    {
      if( nick.empty() )
        printf( "Subject: %s\n", subject.c_str() );
      else
        printf( "%s has set the subject to: '%s'\n", nick.c_str(), subject.c_str() );
    };

    virtual void handleMUCError( MUCRoom *room, StanzaError error )
    {
      printf( "!!!!!!!!got an error: %d", error );
    };


  private:
    Client *j;
    MUCRoom *m_room;
};

int main( int /*argc*/, char* /*argv[]*/ )
{
  MessageTest *r = new MessageTest();
  r->start();
  delete( r );
  return 0;
  return 0;
}

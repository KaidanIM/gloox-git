#ifndef CLIENTBASE_H__
#define CLIENTBASE_H__

#include "../iq.h"
#include "../presence.h"
#include "../message.h"
#include "../subscription.h"
#include "../iqhandler.h"
#include "../jid.h"
#include "../messagesession.h"

#include <string>

namespace gloox
{
  class Disco;
  class PresenceHandler;
  class SubscriptionHandler;

  class ClientBase
  {
    public:
      ClientBase() : m_disco( 0 )  {}
      virtual ~ClientBase() {}
      Disco* disco() { return m_disco; }
      const JID& jid() const { return m_jid; }
      const std::string getID();
      virtual void send( const IQ& ) {}
      virtual void send( const Presence& ) {}
      virtual void send( const Message& ) {}
      virtual void send( const Subscription& ) {}
      virtual void send( const IQ&, IqHandler*, int, bool = false ) = 0;
      virtual void trackID( IqHandler *ih, const std::string& id, int context ) = 0;
      void removeIqHandler( IqHandler* ih, int exttype );
      void removeIDHandler( IqHandler* ih );
      void registerIqHandler( IqHandler* ih, int exttype );
      void registerPresenceHandler( PresenceHandler* ) {}
      void registerPresenceHandler( const JID&, PresenceHandler* ) {}
      void removePresenceHandler( PresenceHandler* ) {}
      void removePresenceHandler( const JID&, PresenceHandler* ) {}
      void registerSubscriptionHandler( SubscriptionHandler* ) {}
      void removeSubscriptionHandler( SubscriptionHandler* ) {}
      void registerStanzaExtension( StanzaExtension* ext );
      void removeStanzaExtension( int ext );
      void registerMessageSession( MessageSession* ) {}
      void disposeMessageSession( MessageSession* session ) { delete session; }
      void rosterFilled() {}
    protected:
      Disco* m_disco;
    private:
      JID m_jid;
  };
  void ClientBase::removeIqHandler( IqHandler*, int ) {}
  void ClientBase::removeIDHandler( IqHandler* ) {}
  void ClientBase::registerIqHandler( IqHandler*, int ) {}
  void ClientBase::registerStanzaExtension( StanzaExtension* se ) { delete se; }
  void ClientBase::removeStanzaExtension( int ) {}
  const std::string ClientBase::getID() { return "id"; }
}

#endif // CLIENTBASE_H__

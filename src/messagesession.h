/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef MESSAGESESSION_H__
#define MESSAGESESSION_H__

#include "messagehandler.h"
#include "session.h"
#include "jid.h"

#include <string>

namespace gloox
{

  class ClientBase;
  class Tag;

  /**
   * @brief An abstraction of a message session between any two entities.
   *
   * This is an alternative interface to XMPP messaging. The original interface, using the simple
   * MessageHandler-derived interface, is based on an all-or-nothing approach. Once registered with
   * ClientBase, a handler receives all message stanzas sent to this client and has to do any filtering
   * on its own.
   *
   * MessageSession adds an abstraction to a chat conversation. A MessageSession is responsible for
   * communicating with exactly one (full) JID. It is extensible by arbitrary decorators which,
   * for instance, offer Message Events support for messages sent through it. Message Events are
   * implemented in the MessageEventDecorator.
   *
   * You can still use the old MessageHandler in parallel, but messages will not be relayed to both
   * the generic MessageHandler and a MessageSession established for the sender's JID. The MessageSession
   * takes precedence.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_EXPORT MessageSession : public Session, MessageHandler
  {
    public:
      /**
       * Constructs a new MessageSession for the given JID.
       * It is recommended to supply a full JID, in other words, it should have set a resource.
       * No resource can lead to unexpected behavior. A thread ID is generated and sent along
       * with every message sent through this session.
       * @param parent The ClientBase to use for communication.
       * @param JID The remote contact's full JID.
       */
      MessageSession( ClientBase *parent, const JID& jid );

      /**
       * Virtual destructor.
       */
      virtual ~MessageSession();

      /**
       * Use this function to associate a MessageHandler with this MessageSession.
       * The MessageHandler will receive all messages sent from this MessageSession's
       * remote contact.
       * @param mh The MessageHandler to register.
       */
      void registerMessageHandler( MessageHandler *mh );

      /**
       * This function clears the internal pointer to the MessageHandler and therefore
       * disables message delivery.
       */
      void removeMessageHandler();

      /**
       * Use this function to find out where is this session pointed at.
       * @return The receipient's JID.
       */
      const JID& target() const { return m_target; };

      /**
       * By default, a thread ID is sent with every message to identify
       * messages belonging together.
       * @returns The thread ID for this session.
       */
      const std::string& threadID() const { return m_thread; };

      // reimplemented from Session
      virtual void send( Tag *Tag );

      // reimplemented from MessageHandler
      virtual void handleMessage( Stanza *stanza );

    private:
      ClientBase *m_parent;
      JID m_target;
      MessageHandler *m_messageHandler;
      std::string m_thread;

  };

}

#endif // MESSAGESESSION_H__

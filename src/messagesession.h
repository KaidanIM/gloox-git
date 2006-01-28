/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MESSAGESESSION_H__
#define MESSAGESESSION_H__

#include "messagehandler.h"
#include "jid.h"

#include <string>

namespace gloox
{

  class ClientBase;
  class Tag;
  class MessageEventFilter;
  class MessageEventHandler;
  class ChatStateFilter;
  class ChatStateHandler;

  /**
   * @brief An abstraction of a message session between any two entities.
   *
   * This is an alternative interface to XMPP messaging. The original interface, using the simple
   * MessageHandler-derived interface, is based on an all-or-nothing approach. Once registered with
   * ClientBase, a handler receives all message stanzas sent to this client and has to do any filtering
   * on its own.
   *
   * MessageSession adds an abstraction to a chat conversation. A MessageSession is responsible for
   * communicating with exactly one (full) JID. By default it offers Message Events and Chat State
   * Notifications support for messages sent through it.
   *
   * You can still use the old MessageHandler in parallel, but messages will not be relayed to both
   * the generic MessageHandler and a MessageSession established for the sender's JID. The MessageSession
   * takes precedence.
   *
   * Using MessageSessions has the following adavantages over the old plain MessageHandler:
   * @li automatic creation of MessageSessions
   * @li filtering by JID
   * @li automatic handling of threading
   * @li simpler sending of messages
   * @li support for Message Events and Chat State Notifications
   *
   * Usage:<br>
   * Derive an object from MessageSessionHandler and reimplement handleMessageSession() to store your
   * shiny new sessions somewhere, or to create a new chat window, or whatever. Register your
   * object with a ClientBase instance using setAutoMessageSession(). In code:
   * @code
   * void MyClass::myFunc()
   * {
   *   JID jid( "abc@example.org/gloox" );
   *   j = new Client( jid, "password" );
   *   [...]
   *   j->setAutoMessageSession( true, this );
   * }
   * @endcode
   * MyClass is a MessageSessionHandler here.
   *
   * MyClass is MessageHandler, MessageEventHandler and ChatStateHandler, too. The handlers
   * are registered with the session to receive the respective events.
   * @code
   * virtual void MyClass::handleMessageSession( MessageSession *session )
   * {
   *   m_session = session;
   *   m_session->registerMessageHandler( this );
   *   m_session->registerMessageEventHandler( this );
   *   m_session->registerChatStateHandler( this );
   * }
   * @endcode
   *
   * MessageEventHandler::handleMessageEvent() and ChatStateHandler::handleChatState() are called
   * for incoming Message Events and Chat States, respectively.
   * @code
   * virtual void MyClass::handleMessageEvent( const JID& from, MessageEventType event )
   * {
   *   // display contact's Message Event
   * }
   *
   * virtual void MyClass::handleChatState( const JID& from, ChatStateType state )
   * {
   *   // display contact's Chat State
   * }
   * @endcode
   *
   * To let the chat partner now that the user is typing a message or has closed the chat window, use
   * raiseMessageEvent() and setChatState(), respectively. For example:
   * @code
   * // user is typing a message
   * m_session->raiseMessageEvent( MESSAGE_EVENT_COMPOSING );
   *
   * // acknowledge receiving of a message
   * m_session->raiseMessageEvent( MESSAGE_EVENT_DELIVERED );
   *
   * // user is not actively paying attention to the chat
   * m_session->setChatState( CHAT_STATE_INACTIVE );
   *
   * // user has closed the chat window
   * m_session->setChatState( CHAT_STATE_GONE );
   * @endcode
   *
   * To send a message to the chat partner of the session, use
   * @ref send( const std::string& message, const std::string& subject ). You don't have to care about
   * receipient, thread id, etc., they are added automatically.
   *
   * @code
   * m_session->send( "Hello World!", "No Subject" );
   * @endcode
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API MessageSession : public MessageHandler
  {

    friend class MessageEventFilter;
    friend class ChatStateFilter;

    public:
      /**
       * Constructs a new MessageSession for the given JID.
       * It is recommended to supply a full JID, in other words, it should have a resource set.
       * No resource can lead to unexpected behavior. A thread ID is generated and sent along
       * with every message sent through this session.
       * @param parent The ClientBase to use for communication.
       * @param jid The remote contact's full JID.
       */
      MessageSession( ClientBase *parent, const JID& jid );

      /**
       * Virtual destructor.
       *
       * @note This destructor de-registers with the ClientBase provided to the constructor. So make
       * sure you have it still around when you delete your last MessageSession.
       */
      virtual ~MessageSession();

      /**
       * This function clears the internal pointer to the MessageHandler and therefore
       * disables message delivery.
       */
      void removeMessageHandler();

      /**
       * Use this function to find out where is this session points at.
       * @return The receipient's JID.
       */
      const JID& target() const { return m_target; };

      /**
       * By default, a thread ID is sent with every message to identify
       * messages belonging together.
       * @returns The thread ID for this session.
       */
      const std::string& threadID() const { return m_thread; };

      /**
       * Use this function to associate a MessageHandler with this MessageSession.
       * The MessageHandler will receive all messages sent from this MessageSession's
       * remote contact.
       * @param mh The MessageHandler to register.
       */
      virtual void registerMessageHandler( MessageHandler *mh );

       /**
       * The MessageEventHandler registered here will receive Message Events according
       * to JEP-0022.
       * @param meh The MessageEventHandler to register.
        */
      void registerMessageEventHandler( MessageEventHandler *meh );

      /**
       * This function clears the internal pointer to the MessageEventHandler.
       * Message Events will not be delivered anymore after calling this function until another
       * MessageEventHandler is registered.
       */
      void removeMessageEventHandler();

      /**
       * The ChatStateHandler registered here will receive Chat States according
       * to JEP-0085.
       * @param csh The ChatStateHandler to register.
       */
      void registerChatStateHandler( ChatStateHandler *csh );

      /**
       * This function clears the internal pointer to the ChatStateHandler.
       * Chat States will not be delivered anymore after calling this function until another
       * ChatStateHandler is registered.
       */
      void removeChatStateHandler();

      /**
       * A convenience function to quickly send a message (optionally with subject). This is
       * the preferred way to send a message from a MessageSession.
       * @param message The message to send.
       * @param subject The optional subject to send.
       */
      void send( const std::string& message, const std::string& subject );

      /**
       * Use this function to raise an event as defined in JEP-0022.
       * @note The Spec states that Message Events shall not be sent to an entity
       * which did not request them. Reasonable effort is taken in this function to
       * avoid spurious event sending. You should be safe to call this even if Message
       * Events were not requested by the remote entity. However,
       * calling raiseEvent( MESSAGE_EVENT_COMPOSING ) for every keystroke still is
       * discouraged. ;)
       * @param event The event to raise.
       */
      void raiseMessageEvent( MessageEventType event );

      /**
       * Use this function to set a chat state as defined in JEP-0085.
       * @note The Spec states that Chat States shall not be sent to an entity
       * which did not request them. Reasonable effort is taken in this function to
       * avoid spurious state sending. You should be safe to call this even if Message
       * Events were not requested by the remote entity. However,
       * calling setChatState( CHAT_STATE_COMPOSING ) for every keystroke still is
       * discouraged. ;)
       * @param state The state to set.
       */
      void setChatState( ChatStateType state );

      // reimplemented from MessageHandler
      virtual void handleMessage( Stanza *stanza );

    protected:

      /**
      * A wrapper around ClientBase::send(). You should @b not use this function to send a
      * chat message because the Tag is not prepared accordingly (neither Thread ID nor Message
      * Event requests are added).
      * @param tag A Tag to send.
      */
      virtual void send( Tag *tag );

    private:
      ClientBase *m_parent;
      MessageEventFilter *m_eventFilter;
      ChatStateFilter *m_stateFilter;
      JID m_target;
      MessageHandler *m_messageHandler;
      std::string m_thread;

  };

}

#endif // MESSAGESESSION_H__

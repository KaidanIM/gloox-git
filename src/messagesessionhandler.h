/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MESSAGESESSIONHANDLER_H__
#define MESSAGESESSIONHANDLER_H__

#include "stanza.h"
#include "messagesession.h"

namespace gloox
{

  /**
   * @brief A virtual interface which can be reimplemented to receive incoming message sessions.
   *
   * Derived classes can be registered as MessageSessionHandlers with the Client.
   * If you have enabled automatic MessageSession creation by calling Client::setAutoMessageSession(),
   * handleMessageSession() will be called if a message stanza arrives for which there is no
   * MessageSession yet.
   *
   * @author Jakob Schroeter <js@camaya.net>
   */
  class GLOOX_EXPORT MessageSessionHandler
  {
    public:
      /**
       * Reimplement this function if you want to be notified about
       * incoming messages by means of automatically created MessageSessions.
       * You receive ownership of the supplied session (@b not the stanza) and
       * are responsible for deleting it at the end of its life.
       *
       * @note Make sure to read the note in ClientBase::setAutoMessageSession()
       * regarding the feeding of decorators.
       *
       * @param session The new MessageSession.
       * @param stanza The complete Stanza.
       */
      virtual void handleMessageSession( MessageSession *session, Stanza *stanza ) = 0;
  };

}

#endif // MESSAGESESSIONHANDLER_H__

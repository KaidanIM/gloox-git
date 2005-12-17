/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef MESSAGEEVENTHANDLER_H__
#define MESSAGEEVENTHANDLER_H__

#include "jid.h"

namespace gloox
{

  /**
   * @brief A virtual interface that enables an object to be notified about
   * Message Events (JEP-0022).
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_EXPORT MessageEventHandler
  {
    public:
      /**
       * Notifies the MessageEventHandler that the message sent lastly has
       * been stored in offline storage for later delivery.
       * @param from The originator of the Event.
       */
      virtual void handleOfflineEvent( const JID& from ) = 0;

      /**
       * Notifies the MessageEventHandler that the message sent lastly
       * has been delivered to the recipients client.
       * @param from The originator of the Event.
       */
      virtual void handleDeliveredEvent( const JID& from ) = 0;

      /**
       * Notifies the MessageEventHandler that the message sent lastly has
       * been displayed by the recipients client.
       * @param from The originator of the Event.
       */
      virtual void handleDisplayedEvent( const JID& from) = 0;

      /**
       * Notifies the MessageEventHandler that the receiver of message the
       * last message is composing a reply.
       * @param from The originator of the Event.
       */
      virtual void handleComposingEvent( const JID& from ) = 0;

  };

}

#endif // MESSAGEEVENTHANDLER_H__

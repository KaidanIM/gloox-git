/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PSSUBSCRIPTIONHANDLER_H__
#define PSSUBSCRIPTIONHANDLER_H__

#include "jid.h"
#include <string>

namespace gloox
{
  /**
   * @brief A virtual interface for receiving (un)subscription result.
   *
   * Derive from this interface and register with the PubSubManagerManager.
   *
   * @author Jakob Schroeter <js@camaya.net>
   */
  class PSSubscriptionHandler
  {
    public:
      /**
      * Receives the subscription results.
      * 
      */
      virtual void handleSubscriptionResult( const JID& jid,
                                             const std::string& node,
                                             const std::string& sid,
                                             const SubscriptionType subType,
                                             const SubscriptionError se ) = 0;

      /**
       * Receives the Subscription map for a specific service.
       * @param jid The pubsub service.
       * @param node The error node.
       */
      virtual void handleSubscriptionError( const std::string& jid, const std::string& node ) = 0;

      /**
       * 
       */
      virtual void handleUnsubscriptionResult( const JID& jid,
                                               const JID& service,
                                               const UnsubscriptionError se ) = 0;

      /**
       * Virtual destructor.
       */
      virtual ~PSSubscriptionHandler() {}

  };

}

#endif /* PSSUBSCRIPTIONHANDLER_H__ */

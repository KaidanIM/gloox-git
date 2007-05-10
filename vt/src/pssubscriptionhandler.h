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
#include "pubsub.h"

namespace gloox
{

  namespace PubSub
  {
    /**
     * @brief A virtual interface for receiving (un)subscription result.
     *
     * Derive from this interface and register with the Manager.
     *
     * @author Jakob Schroeter <js@camaya.net>
     */
    class SubscriptionHandler
    {
      public:
        /**
         * Receives the subscription results. In case a problem occured, the
         * SubscriptionError is set accordingly and the Subscription ID and
         * SubscriptionType becomes irrelevant.
         *
         * @param service PubSub service asked for subscription..
         * @param node Node asked for subscription.
         * @param sid Subscription ID.
         * @param subType Type of the subscription.
         * @param se Subscription error.
         */
        virtual void handleSubscriptionResult( const std::string& service,
                                               const std::string& node,
                                               const std::string& sid,
                                               const SubscriptionType subType,
                                               const SubscriptionError se ) = 0;

        /**
         * Receives the unsubscription results. In case a problem occured, the
         * UnsubscriptionError is set accordingly.
         *
         * @param service PubSub service asked for subscription.
         * @param node Node asked for subscription.
         * @param se Unsubscription error.
         */
        virtual void handleUnsubscriptionResult( const std::string& service,
                                                 const JID& service,
                                                 const UnsubscriptionError se ) = 0;

        /**
         * Virtual destructor.
         */
        virtual ~SubscriptionHandler() {}

    };

  }

}

#endif /* PSSUBSCRIPTIONHANDLER_H__ */

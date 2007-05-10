/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PSSUBSCRIPTIONLISTHANDLER_H__
#define PSSUBSCRIPTIONLISTHANDLER_H__

#include "jid.h"
#include <string>
#include <map>
#include "pubsub.h"

namespace gloox
{

  namespace PubSub
  {

    /**
     * @brief A virtual interface for receiving subscription list result.
     *
     * Derive from this interface and register with the PubSubManagerManager.
     *
     * @author Jakob Schroeter <js@camaya.net>
     */
    class SubscriptionListHandler
    {
      public:
        /**
         * Receives the Subscription map for a specific service.
         * @param jid The queried service.
         * @param subMap The map of each node's subscription.
         */
        virtual void handleSubscriptionListResult( const JID& jid, const SubscriptionMap& subMap ) = 0;

        /**
         * Receives the subscription listing error from a service (ie unsupported).
         * @param jid The queried service.
         */
        virtual void handleSubscriptionListError( const std::string& jid ) = 0;
/**
         * Receives the affiliation 
         * @param jid The queried service.
         */
        /**
         * Virtual destructor.
         */
        virtual ~SubscriptionListHandler() {}

    };

  }

}

#endif /* PSSUBSCRIPTIONLISTHANDLER_H__ */

/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PSSERVICEHANDLER_H__
#define PSSERVICEHANDLER_H__

#include "jid.h"
#include "pubsub.h"

#include <string>
#include <map>

namespace gloox
{

  namespace PubSub
  {

    /**
     * @brief A virtual interface for receiving service related requests result.
     *
     * Derive from this interface and pass over to Manager methods.
     *
     * @author Vincent Thomasset
     */
    class ServiceHandler
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~ServiceHandler() {}

        /**
         * Receives the Subscription map for a specific service.
         * @param service The queried service.
         * @param subMap The map of node's subscription. Check error if null.
         * @param error Error describing the resolution of the request.
         * @see Manager::requestSubscriptionList
         */
        virtual void handleSubscriptionList( const JID& service,
                                             const SubscriptionMap * subMap,
                                             const Error * error = 0) = 0;

        /**
         * Receives the Affiliation map for a specific service.
         * @param service The queried service.
         * @param subMap The map of node's affiliation. Check error if null.
         * @see Manager::requestAffiliationList
         */
        virtual void handleAffiliationList( const JID& service,
                                            const AffiliationMap * affMap,
                                            const Error * error = 0 ) = 0;

        /**
         * Receives the default configuration for a specific node type.
         * @param service The queried service.
         * @param type The type of the NodeType requested.
         * @param config Configuration form for the node type.
         * @see Manager::getDefaultNodeConfig
         */
        virtual void handleDefaultNodeConfig( const JID& service,
                                              const DataForm * config,
                                              const Error * error = 0 ) = 0;

    };

  }

}

#endif /* PSSERVICEHANDLER_H__ */


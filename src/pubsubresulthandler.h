/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBRESULTHANDLER_H__
#define PUBSUBRESULTHANDLER_H__

#include "macros.h"
#include "pubsub.h"
#include "error.h"
#include "jid.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  class Tag;
  class Error;
//  class DiscoNodeItem;
  class DataForm;

  typedef std::list<Tag*> TagList;
//  typedef std::list<DiscoNodeItem> DiscoNodeItemList;

  namespace PubSub
  {
    /**
     * @brief A virtual interface to receive item related requests results.
     *
     * Derive from this interface and pass it to item related requests.
     *
     * @author Vincent Thomasset
     */
    class GLOOX_API ResultHandler
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~ResultHandler() {}

        /**
         * 
         */
        virtual void handleItem( const JID& service,
                                 const std::string& node,
                                 const Tag* entry ) = 0;

        /**
         * Receives the list of Items for a node. Either one of the item list or the
         * error will be null.
         * @param service Service hosting the queried node.
         * @param nodeid ID of the queried node. If empty, the root node has been queried.
         * @param itemList List of contained items.
         * @param error Describes the error case if the request failed.
         */
        virtual void handleItemList( const JID& service,
                                     const std::string& node,
                                     const TagList* itemList,
                                     const Error* error = 0 ) = 0;


        virtual void handleItemPublication( const JID& service,
                                            const std::string& node,
                                            const std::string& item,
                                            const Error* error = 0 ) = 0;


        virtual void handleItemDeletation(  const JID& service,
                                            const std::string& node,
                                            const std::string& item,
                                            const Error* error = 0 ) = 0;

//        virtual void handleResult( int context, const TrackedItem& item, const Error* error ) = 0;

        /**
         * Receives the subscription results. In case a problem occured, the
         * SubscriptionError is set accordingly and the Subscription ID and
         * SubscriptionType becomes irrelevant.
         *
         * @param service PubSub service asked for subscription.
         * @param node Node asked for subscription.
         * @param sid Subscription ID.
         * @param subType Type of the subscription.
         * @param se Subscription error.
         */
        virtual void handleSubscriptionResult( const JID& service,
                                               const std::string& node,
                                               const std::string& sid,
                                               const JID& jid,
                                               const SubscriptionType subType,
                                               const Error* error = 0 ) = 0;

        /**
         * Receives the subscription results. In case a problem occured, the
         * SubscriptionError is set accordingly and the Subscription ID and
         * SubscriptionType becomes irrelevant.
         *
         * @param service PubSub service asked for subscription.
         * @param node Node asked for subscription.
         * @param sid Subscription ID.
         * @param subType Type of the subscription.
         * @param se Subscription error.
         */
        virtual void handleUnsubscriptionResult( const JID& service,
                                                 const std::string& node,
                                                 const std::string& sid,
                                                 const JID& jid,
                                                 const Error* error = 0 ) = 0;

        /**
         * Receives the configuration form of a node.
         * @param service Service hosting the queried node.
         * @param jid Subscribed entity.
         * @param node ID of the queried node.
         * @param options Options DataForm.
         */
        virtual void handleSubscriptionOptions( const JID& service,
                                                const JID& jid,
                                                const std::string& node,
                                                const DataForm* options,
                                                const Error* error = 0 ) = 0;

        /**
         * Receives the configuration form of a node.
         * @param service Service hosting the queried node.
         * @param jid Subscribed entity.
         * @param node ID of the queried node.
         * @param options Options DataForm.
         */
        virtual void handleSubscriptionOptionsResult( const JID& service,
                                                      const JID& jid,
                                                      const std::string& node,
                                                      const Error* error = 0 ) = 0;


        /**
         * Receives the list of subscribers to a node.
         * @param service Service hosting the node.
         * @param node ID of the queried node.
         * @param list Subscriber list.
         */
        virtual void handleSubscriberList( const JID& service,
                                           const std::string& node,
                                           const SubscriberList* list,
                                           const Error* error = 0 ) = 0;

        /**
         * Receives the list of subscribers to a node.
         * @param service Service hosting the node.
         * @param node ID of the queried node.
         * @param list Subscriber list.
         */
        virtual void handleSubscriberListResult( const JID& service,
                                                 const std::string& node,
                                                 const Error* error = 0 ) = 0;


        /**
         * Handle the affiliate list for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param list Affiliation list.
         */
        virtual void handleAffiliateList( const JID& service,
                                          const std::string& node,
                                          const AffiliateList* list,
                                          const Error* error = 0 ) = 0;

        /**
         * Handle the affiliate list for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param list Affiliation list.
         */
        virtual void handleAffiliateListResult( const JID& service,
                                                const std::string& node,
                                                const Error* error = 0 ) = 0;


        /**
         * Handle the configuration for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodeConfig( const JID& service,
                                       const std::string& node,
                                       const DataForm* config,
                                       const Error* error = 0 ) = 0;

        /**
         * Handle the configuration for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodeConfigResult( const JID& service,
                                             const std::string& node,
                                             const Error* error = 0 ) = 0;

        /**
         * Handle the configuration for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodeCreationResult( const JID& service,
                                               const std::string& node,
                                               const Error* error = 0 ) = 0;

        /**
         * Handle the configuration for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodeDeletationResult( const JID& service,
                                                 const std::string& node,
                                                 const Error* error = 0 ) = 0;


        /**
         * Handle the configuration for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodePurgeResult( const JID& service,
                                            const std::string& node,
                                            const Error* error = 0 ) = 0;

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

#endif /* PUBSUBRESULTHANDLER_H__ */


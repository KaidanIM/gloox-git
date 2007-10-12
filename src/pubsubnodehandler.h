/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBNODEHANDLER_H__
#define PUBSUBNODEHANDLER_H__

#include <string>

#include "pubsub.h"
#include "error.h"
#include "macros.h"
#include "jid.h"

namespace gloox
{

  class JID;
  class DataForm;

  namespace PubSub
  {

    /**
     * Describes a subscribed entity.
     */
    struct Subscriber
    {
      Subscriber( const std::string& _jid, SubscriptionType _type,
                                           const std::string& _subid = "")
        : jid( _jid ), type( _type ), subid( _subid ) {}
      const JID jid;
      SubscriptionType type;
      const std::string subid;
    };

    /**
     *
     */
    struct Affiliate
    {
      Affiliate( const std::string& _jid, AffiliationType _type )
        : jid( _jid ), type( _type ) {}
      const JID jid;
      AffiliationType type;
    };

    typedef std::list< Subscriber > SubscriberList;
    typedef std::list<  Affiliate > AffiliateList;

    /**
     * @brief A virtual interface for receiving node related requests results.
     *
     * Derive from this interface and pass it to item related requests.
     *
     * @author Vincent Thomasset
     */
    class GLOOX_API NodeHandler
    {
      public:

        /**
         * Default virtual destructor.
         */
        virtual ~NodeHandler() {}

        /**
         * Receives the result of a node creation request.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodeCreationResult( const JID& service,
                                               const std::string& node,
                                               const Error* e = 0 ) = 0;

        /**
         * Receives the result of a node removal request.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodeDeletationResult( const JID& service,
                                                 const std::string& node,
                                                 const Error* e = 0 ) = 0;

        /**
         * Receives the result of a node purge request.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodePurgeResult( const JID& service,
                                            const std::string& node,
                                            const Error* e = 0 ) = 0;

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
         * Receives the unsubscription results. In case a problem occured, the
         * error if not null.
         *
         * @param service PubSub service asked for subscription.
         * @param node Node asked for subscription.
         * @param sid Subscription ID.
         * @param error Error describing the problem that occured (may be null).
         */
        virtual void handleUnsubscriptionResult( const JID& service,
                                                 const std::string& nodeID,
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
         * Handle the configuration modification for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodeConfigResult( const JID& service,
                                             const std::string& node,
                                             const Error* error = 0 ) = 0;

    };

  }

}

#endif /* PUBSUBNODEHANDLER_H__ */

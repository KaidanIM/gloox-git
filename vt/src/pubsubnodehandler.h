/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBNODEHANDLER_H_
#define PUBSUBNODEHANDLER_H_

#include <string>

namespace gloox
{  

  class JID;
  class DataForm;

  namespace PubSub
  {

    struct Subscriber
    {
      Subscriber( const std::string& _jid, SubscriptionType _type,
                                            const std::string& _subid = "")
        : jid( _jid ), type( _type ), subid( _subid ) {}
      const JID jid;
      SubscriptionType type;
      const std::string subid;
    };

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
     * @brief A virtual interface for receiving node related results.
     *
     * Derive from this interface.
     *
     * @author Jakob Schroeter <js@camaya.net>
     */
    class NodeHandler
    {
      public:

        /**
         * Receives the result of a node creation request.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodeCreationResult( const JID& service,
                                               const std::string& node/*,
                                               const Error& e*/ ) = 0;

        /**
         * Receives the result of a node removal request.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodeDeletationResult( const JID& service,
                                                 const std::string& node/*,
                                                 const Error& e*/ ) = 0;

        /**
         * Receives the result of a node purge request.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodePurgeResult( const JID& service,
                                            const std::string& node/*,
                                            const Error& e*/ ) = 0;


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
                                                const DataForm& options ) = 0;

        /**
         * Receives the result of a subscription's options modification.
         * @param service Service hosting the node.
         * @param jid Subscribed entity.
         * @param node ID of the node.
         */
        virtual void handleSubscriptionOptionsResult( const JID& service,
                                                      //const JID& jid,
                                                      const std::string& node/*,
                                                      const Error& e*/ ) = 0;

        /**
         * Receives the list of subscribers to a node.
         * @param service Service hosting the node.
         * @param node ID of the queried node.
         * @param list Subscriber list.
         */
        virtual void handleSubscriberList( const JID& service,
                                           const std::string& node,
                                           const SubscriberList& list ) = 0;

        /**
         * Receives the result of a subscription's options modification.
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleSubscriberListResult( const JID& service,
                                                 const std::string& node/*,
                                                 const Error& e*/ ) = 0;

        /**
         * Handle the affiliate list for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param list Affiliation list.
         */
        virtual void handleAffiliateList( const JID& service,
                                          const std::string& node,
                                          const AffiliateList& list ) = 0;

        /**
         * 
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param itemList List of contained items.
         */
        virtual void handleAffiliateListResult( const JID& service,
                                                const std::string& node/*,
                                                 const Error& e*/ ) = 0;

        /**
         * Handle the configuration for a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param config Configuration DataForm.
         */
        virtual void handleNodeConfig( const JID& service,
                                       const std::string& node,
                                       const DataForm& config ) = 0;

        /**
         * 
         * @param service Service hosting the node.
         * @param node ID of the node.
         */
        virtual void handleNodeConfigResult( const JID& service,
                                             const std::string& node/*,
                                             const Error& e*/ ) = 0;

        /**
         * Default virtual destructor.
         */
        virtual ~NodeHandler() {}
    };

  }

}

#endif /* PUBSUBNODEHANDLER_H_ */

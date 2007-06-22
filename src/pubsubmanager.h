/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBMANAGER_H__
#define PUBSUBMANAGER_H__

#include "pubsub.h"
#include "iqhandler.h"
#include "discohandler.h"
#include "messagehandler.h"
#include "pubsubnodehandler.h"

#include <map>
#include <string>

namespace gloox
{

  class ClientBase;
  class DataForm;

  namespace PubSub
  {

    class SubscriptionHandler;
    class SubscriptionListHandler;
    class AffiliationListHandler;
    class NodeHandler;
    class ItemHandler;
    class DiscoHandler;
    class EventHandler;

    /**
     * @brief This manager is used to interact with PubSub services.
     *
     *
     * @code
     * class MyClient : public Client
     * {
     *   public:
     *     MyClient( ... ) : Client( ... )
     *     {
     *       m_psManager->registerSubscriptionHandler( new MySubscriptionHandler() );
     *       m_psManager->registerSubscriptionListHandler( new MySubscriptionListHandler() );
     *       m_psManager->registerAffiliationListHandler( new MySubscriptionHandler() );
     *       m_psManager->registerNodeHandler( new MyNodeHandler() );
     *       m_psManager->registerItemHandler( new MyItemHandler() );
     *     }
     *
     *     void requestItemList( const std::string& service, const std::string nodeid )
     *       { m_psManager->requestItemList( service, nodeid ); }
     *
     *   private:
     *     PubSub::Manager * m_pubsubManager;
     * };
     *
     * @endcode
     *
     * @author Vincent Thomasset
     *
     * XEP Version: 1.9
     */
    class Manager : public IqHandler, public gloox::DiscoHandler, public MessageHandler
    {
      public:

        /**
         * Initialize the manager.
         * @param parent Client to which this manager belongs to.
         */
        Manager( ClientBase* parent );

        /**
         * Default virtual destructor.
         */
        virtual ~Manager() {}

        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param nodeid ID of the node to query. If empty, the root node will be queried.
         */
        void discoverInfos( const JID& service, const std::string& node, PubSub::DiscoHandler *handler );

        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param nodeid ID of the node to query. If empty, the root node will be queried.
         * @param handler DiscoHandler to notify when receiving a response
         */
        void discoverServiceInfos( const JID& service, PubSub::DiscoHandler *handler )
          { discoverInfos( service, "", handler ); }

        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param nodeid ID of the node to query. If empty, the root node will be queried.
         * @param handler DiscoHandler to notify when receiving a response
         */
        void discoverNodeInfos( const JID& service, const std::string& node, PubSub::DiscoHandler *handler )
          { discoverInfos( service, node, handler ); }

        /**
         * Ask for the list children of a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node to ask for subnodes. If empty, the root node
         * will be queried.
         * @param handler DiscoHandler to notify when receiving a response
         */
        void discoverNodeItems( const JID& service, const std::string& nodeid,
                                PubSub::DiscoHandler *handler );

        /**
         * Subscribe to a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node to subscribe to.
         * @param jid JID to subscribe. If empty, the client's JID will be used
         *            (ie self subscription).
         * @param type SibscriptionType of the subscription (Collections only!).
         * @param depth Subscription depth. For 'all', use 0 (Collections only!).
         */
        void subscribe( const JID& service, const std::string& nodeid,
                        const JID& jid = JID(), SubscriptionObject type = SubscriptionNodes,
                        int depth = 1 );

        /**
         * Unsubscribe from a node.
         * @param service Service hosting the node.
         * @param node ID of the node to unsubscribe from.
         */
        void unsubscribe( const JID& service, const std::string& node );

        /**
         * Requests the subscription list from a service.
         * @param service Service to query.
         * @param slh The SubscriptionListHandler to handle the result.
         */
        void requestSubscriptionList( const JID& service, SubscriptionListHandler *slh  );

        /**
         * Requests the affiliation list from a service.
         * @param service Service to query.
         * @param alh The AffiliationListHandler to handle the result.
         */
        void requestAffiliationList( const JID& service, AffiliationListHandler *alh );

        /**
         * Requests subscription options.
         * @param service Service to query.
         * @param jid Subscribed entity.
         * @param node Node ID of the node.
         * @param handler Node ID of the node.
         * @param slh The SubscriptionListHandler to handle the result.
         */
        void requestSubscriptionOptions( const JID& service, const JID& jid,
                                         const std::string& node, NodeHandler *slh  );

        /**
         * Modifies subscription options.
         * @param service Service to query.
         * @param jid Subscribed entity.
         * @param node Node ID of the node.
         * @param df New configuration.
         */
        void setSubscriptionOptions( const JID& service, const JID& jid,
                                     const std::string& node, const DataForm& df );

        /**
         * Requests the affiliation list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param alh The AffiliationListHandler to handle the result.
         */
        void requestAffiliationList( const JID& service, const std::string& node,
                                     AffiliationListHandler *alh );

        /**
         * Publish an item to a node.
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param item The item to publish.
         */
        void publishItem( const JID& service, const std::string& node, const Tag& item );

        /**
         * Delete an item from a node.
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param item ID of the item in the node.
         */
        void deleteItem( const JID& service,
                         const std::string& node,
                         const std::string& item );

        /**
         * Ask for the item list of a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler ItemHandler to send the result to.
         */
        void requestItems( const JID& service, const std::string& nodeid, ItemHandler *handler );

        /**
         * Creates a new node.
         * @param type The type of the new node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be located at the root of
         * the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys of the map should be in
         * the form of 'pubsub#name', where 'name' is a valid pubsub option. It is not necessary to
         * include an access model config option. Use the @c access parameter instead.
         */
        void createNode( NodeType type, const JID& service,
                                        const std::string& node,
                                        const std::string& name,
                                        const std::string& parent = "",
                                        AccessModel access = AccessDefault,
                                        const StringMap * config = 0 );

        /**
         * Creates a new leaf node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be located at the root of
         * the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys of the map should be in
         * the form of 'pubsub#name', where 'name' is a valid pubsub option. It is not necessary to
         * include an access model config option. Use the @c access parameter instead.
         */
        void createLeafNode( const JID& service,
                             const std::string& node,
                             const std::string& name,
                             const std::string& parent = "",
                             AccessModel access = AccessDefault,
                             const StringMap * config = 0 )
          { createNode( NodeLeaf, service, node, name, parent, access, config ); }

        /**
         * Creates a new collection node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be located at the root of
         * the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys of the map should be in
         * the form of 'pubsub#name', where 'name' is a valid pubsub option. It is not necessary to
         * include an access model config option. Use the @c access parameter instead.
         */
        void createCollectionNode( const JID& service,
                                   const std::string& node,
                                   const std::string& name,
                                   const std::string& parent = "",
                                   AccessModel access = AccessDefault,
                                   const StringMap * config = 0 )
          { createNode( NodeCollection, service, node, name, parent, access, config ); }

        /**
         * Deletes a node.
         * @param service Service where to create the new node.
         * @param node Node ID of the new node.
         */
        void deleteNode( const JID& service,
                         const std::string& node );

/*
        void associateNode( const JID& service,
                            const std::string& node,
                            const std::string& collection );

        void disassociateNode( const JID& service,
                               const std::string& node,
                               const std::string& collection );

        void disassociateNode();

        void getDefaultNodeConfig( NodeType = NodeTypeLeaf );
*/

        /**
         * Requests the subscriber list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler .
         */
        void purgeNodeItems( const JID& service, const std::string& node, NodeHandler * handler );

        /**
         * Requests the subscriber list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler.
         */
        void requestSubscriberList( const JID& service, const std::string& node, NodeHandler * handler )
          { subscriberList( service, node, 0, handler ); }

        /**
         * Modifies the subscriber list for a node. This function SHOULD only set the
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param list NodeHandler.
         */
        void setSubscriberList( const JID& service,
                                const std::string& node,
                                const SubscriberList& list,
                                NodeHandler * handler )
          { subscriberList( service, node, &list, handler ); }

        /**
         * Requests the affiliate list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler.
         */
        void requestAffiliateList( const JID& service,
                                   const std::string& node,
                                   NodeHandler * handler )
          { affiliateList( service, node, 0, handler ); }

        /**
         * Modifies the affiliate list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param list NodeHandler.
         */
        void setAffiliateList( const JID& service,
                               const std::string& node,
                               const AffiliateList& list,
                               NodeHandler * handler )
          { affiliateList( service, node, &list, handler ); }

        /**
         * Retrieve the configuration (options) of a node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void requestNodeConfig( const JID& service,
                                const std::string& node,
                                NodeHandler * handler )
          { nodeConfig( service, node, 0, handler ); }

        /**
         * Changes a node's configuration (options).
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void setNodeConfig( const JID& service,
                            const std::string& node,
                            const DataForm& config,
                            NodeHandler * handler  )
          { nodeConfig( service, node, &config, handler ); }

        /**
         * Registers an handler to receive notification of (un)subscription events.
         * @param handler SubscriptionHandler to register.
         */
        void registerItemHandler( ItemHandler * handler )
          { m_itemHandlerList.push_back( handler ); }

        /**
         * Removes an handler from the list of objects listening to (un)subscription events.
         * @param handler SubscriptionHandler to remove.
         */
        void removeItemHandler( ItemHandler * handler )
          { m_itemHandlerList.remove( handler ); }

        /**
         * Registers an handler to receive notification of events.
         * @param handler EventHandler to register.
         */
        void registerEventHandler( EventHandler * handler )
          { m_eventHandlerList.push_back( handler ); }

        /**
         * Removes an handler from the list of event handlers.
         * @param handler EventHandler to remove.
         */
        void removeEventHandler( EventHandler * handler )
          { m_eventHandlerList.remove( handler ); }

        /**
         * Registers an handler to receive notification of (un)subscription events.
         * @param handler SubscriptionHandler to register.
         */
        void registerSubscriptionHandler( SubscriptionHandler * handler )
          { m_subscriptionTrackList.push_back( handler ); }

        /**
         * Removes an handler from the list of objects listening to (un)subscription events.
         * @param handler SubscriptionHandler to remove.
         */
        void removeSubscriptionHandler( SubscriptionHandler * handler )
          { m_subscriptionTrackList.remove( handler ); }

        // reimplemented from DiscoHandler
        virtual void handleDiscoInfoResult( Stanza *stanza, int context );
        virtual void handleDiscoItemsResult( Stanza *stanza, int context );
        virtual void handleDiscoError( Stanza *stanza, int context );
        virtual bool handleDiscoSet( Stanza * ) { return 0; }

        // reimplemented from MessageHandler
        virtual void handleMessage( Message* msg, MessageSession * );

        // reimplemented from IqHandler
        virtual bool handleIq  ( IQ* iq );
        virtual void handleIqID( IQ* iq, int context );

      private:

        /**
         * This function sets or requests a node's configuration form
         * (depending on arguments). Does the actual work for requestNodeConfig
         * and setNodeConfig.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void nodeConfig( const JID& service, const std::string& node,
                         const DataForm * config, NodeHandler * handler );

        /**
         * This function sets or requests a node's subscribers list form
         * (depending on arguments). Does the actual work for
         * requestSubscriberList and setSubscriberList.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void subscriberList( const JID& service, const std::string& node,
                             const SubscriberList * config, NodeHandler * handler );

        /**
         * This function sets or requests a node's affiliates list
         * (depending on arguments). Does the actual work for
         * requestAffiliateList and setAffiliateList.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void affiliateList( const JID& service, const std::string& node,
                            const AffiliateList * config, NodeHandler * handler );

        typedef std::list< SubscriptionHandler * > SubscriptionTrackList;
        typedef std::map < std::string, AffiliationListHandler * > AffiliationListTrackMap;
        typedef std::map < std::string, SubscriptionListHandler * > SubscriptionListTrackMap;
        typedef std::list</*std::map < std::string,*/ ItemHandler * > ItemHandlerList;
        typedef std::pair< std::string, std::string > TrackedItem;
        typedef std::map < std::string, TrackedItem > ItemOperationTrackMap;
        typedef std::map < std::string, std::string > NodeOperationTrackMap;
        typedef std::map < std::string, PubSub::DiscoHandler* > DiscoHandlerTrackMap;
        typedef std::list< EventHandler* > EventHandlerList;
        typedef std::map< std::string, NodeHandler* > NodeHandlerTrackMap;

        ClientBase *m_parent;

        SubscriptionTrackList m_subscriptionTrackList;
        AffiliationListTrackMap m_affListTrackMap;
        SubscriptionListTrackMap m_subListTrackMap;
        ItemHandlerList m_itemHandlerList;
        ItemOperationTrackMap m_iopTrackMap;
        NodeOperationTrackMap m_nopTrackMap;
        DiscoHandlerTrackMap m_discoHandlerTrackMap;
        EventHandlerList m_eventHandlerList;
        NodeHandlerTrackMap m_nodeHandlerTrackMap;
    };

  }

}

#endif /* PUBSUBMANAGER_H__ */

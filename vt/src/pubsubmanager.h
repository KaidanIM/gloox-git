/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBMANAGER_H__
#define PUBSUBMANAGER_H__

#include <map>
#include <string>
#include "iqhandler.h"
#include "pubsub.h"
#include "discohandler.h"

namespace gloox
{

  class ClientBase;
  class DataForm;

  namespace PubSub
  {

    class SubscriptionHandler;
    class SubscriptionListHandler;
    class AffiliationListHandler;
    class ItemHandler;
    class Item;

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
     * @author Jakob Schroeter <js@camaya.net>
     *
     * XEP Version: 1.9
     */
    class Manager : public IqHandler, public DiscoHandler
    {
      public:

        /**
         * Initialize the manager.
         * @param parent Client to which this manager belongs to.
         */
        Manager( ClientBase* parent );

        /**
         * Virtual Destructor.
         */
        virtual ~Manager() {}

        // reimplemented from DiscoHandler
        void handleDiscoInfoResult( Stanza *stanza, int context );
        void handleDiscoItemsResult( Stanza *stanza, int context );
        void handleDiscoError( Stanza *stanza, int context );
        bool handleDiscoSet( Stanza * stanza ) { return 0; }

        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param nodeid ID of the node to query. If empty, the root node will be queried.
         */
        void discoverInfos( const JID& service, const std::string& node = "");

        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param nodeid ID of the node to query. If empty, the root node will be queried.
         */
        void discoverServiceInfos( const JID& service )
          { discoverInfos( service ); }

        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param nodeid ID of the node to query. If empty, the root node will be queried.
         */
        void discoverNodeInfos( const JID& service, const std::string& node )
          { discoverInfos( service, node ); }

        /**
         * Ask for the list children of a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node to ask for subnodes. If empty, the root node
         *               will be queried.
         */
        void discoverNodeItems( const JID& service, const std::string& nodeid = "" );

        /**
         * Subscribe to a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node to subscribe to.
         * @param jid JID to subscribe. If empty, the client's JID will be used (ie
	 self subscription).
         */
        void subscribe( const JID& service, const std::string& nodeid, const
	std::string& jid = "" );

        /**
         * Unsubscribe from a node.
         * @param service Service hosting the node.
         * @param node ID of the node to unsubscribe from.
         */
        void unsubscribe( const JID& service, const std::string& nodeid );

        /**
         * Requests the subscription list from a service.
         * @param jid Service to query.
         */
        void requestSubscriptionList( const JID& jid, SubscriptionListHandler * slh  );

        /**
         * Requests the affiliation list from a service.
         * @param service Service to query.
         */
        void requestAffiliationList( const JID& jid, AffiliationListHandler * alh );

        /**
         * Publish an item to a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node to delete the item from.
         * @param item Item to publish.
         */
        void publishItem( const JID& jid, const std::string& nodeid, const Item& item );

        /**
         * Delete an item from a node.
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param itemid ID of the item in the node.
         */
        void deleteItem( const JID& service,
                         const std::string& nodeid,
                         const std::string& itemid );

        /**
         * Ask for the item list of a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler ItemHandler to send the result to.
         */
        void requestItems( const JID& service,
                           const std::string& nodeid,
                           ItemHandler * handler );

        /**
         * Creates a new node.
         * @param type NodeType of the new node.
         * @param service Service where to create the new node.
         * @param nodeid ID of the new node.
         * @param name Name of the new node.
         * @param parentid ID of the parent node. If empty, the node will
         *                 be located at the root of the service.
         */
        void createNode( NodeType type, const JID& service,
                                        const std::string& nodeid,
                                        const std::string& name,
                                        const std::string& parentid = ""  );

        /**
         * Creates a new leaf node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         * @param name Name of the new node.
         * @param parentid ID of the parent node. If empty, the node will
         *               be located at the root of the service.
         */
        void createLeafNode( const JID& service,
                             const std::string& nodeid,
                             const std::string& name,
                             const std::string& parentid = "" )
          { createNode( NodeLeaf, service, nodeid, name, parentid ); }

        /**
         * Creates a new collection node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         * @param name Name of the new node.
         * @param parentid ID of the parent node. If empty, the node will
         *               be located at the root of the service.
         */
        void createCollectionNode( const JID& service,
                                   const std::string& nodeid,
                                   const std::string& name,
                                   const std::string& parentid = "" )
          { createNode( NodeCollection, service, nodeid, name, parentid ); }

        /**
         * Deletes a node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         */
        void deleteNode( const JID& service,
                         const std::string& nodeid );

/*
        void createNode( Node::NodeType type, const std::string& service,
                                              const std::string& nodeid,
                                              const std::string& name,
                                              const Dataform& config );

        void createNode( NodeType type, const JID& node,
                                        const std::string& name,
                                        const Dataform& config );
          { createNode( node.bare(), node.resource(), config ); }

        void associateNode( const std::string& service,
                            const std::string& nodeid,
                            const std::string& collectionid );

        void disassociateNode( const std::string& service,
                               const std::string& nodeid,
                               const std::string& collectionid );

        void disassociateNode()

        void getDefaultNodeConfig( NodeType = NodeTypeLeaf );

        void getNodeConfig( const std::string& service, const std::string nodeid );

        void getNodeConfig( const JID& node )
          { getNodeConfig( node.bare(), node.resource ); }

        void handleNodeConfigError( const std::string& service, const std::string& nodeid ) = 0;

        void handleNodeConfigRequestError( const std::string& service, const std::string& nodeid ) = 0;
*/
        void purgeNodeItems( const JID& service, const std::string& nodeid );

/*
        void modifySubscriptions( const std::string& service,
                                  const std::string& nodeid,
                                  const std::string& jid,
                                  const SubscriptionMap& subMap );

        void modifySubscriptions( const JID& node,
                                  const std::string& jid,
                                  const SubscriptionMap& subMap )
          { modifySubscriptions( node.bare(), node.resource(), jid, subMap ); }

        void handleSubscriptionModificationError( const std::string& service,
                                                  const std::string& nodeid,
                                                  const SubscriptionMap& subMap,
                                                  SubscriptionModificationError error) = 0;
*/

        /**
         * Retrieve the configuration of a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node.
         */
        void requestNodeConfig( const JID& service, const std::string& nodeid );

        /**
         * Retrieve the configuration of a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node.
         * @param dataform Configuration of the node.
         * @param e Error of the configuration request.
         */
        /*virtual void handleNodeConfig( const JID& service,
                                       const std::string& nodeid,
                                       const DataForm& dataForm,
                                       const OptionRequestError e ) = 0;
*/

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

        bool handleIq  ( Stanza *stanza );
        bool handleIqID( Stanza *stanza, int context );

      private:
        typedef std::list< SubscriptionHandler * > SubscriptionTrackList;
        typedef std::map < std::string, AffiliationListHandler * > AffiliationListTrackMap;
        typedef std::map < std::string, SubscriptionListHandler * > SubscriptionListTrackMap;
        typedef std::list</*std::map < std::string,*/ ItemHandler * > ItemHandlerList;
        typedef std::pair< std::string, std::string > TrackedItem;
        typedef std::map < std::string, TrackedItem > ItemOperationTrackMap;
        typedef std::map < std::string, std::string > NodeOperationTrackMap;
        

        ClientBase* m_parent;

        SubscriptionTrackList m_subscriptionTrackList;
        AffiliationListTrackMap m_affListTrackMap;
        SubscriptionListTrackMap m_subListTrackMap;
        ItemHandlerList m_itemHandlerList;
        ItemOperationTrackMap m_iopTrackMap;
        NodeOperationTrackMap m_nopTrackMap;
    };

  }
}

#endif /* PUBSUBMANAGER_H__ */

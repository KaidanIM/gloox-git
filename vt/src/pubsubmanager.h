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
    class Manager : public IqHandler
    {
      public:

        /**
         * Initialize the manager.
         * @param parent Client to which this manager belongs to.
         */
        Manager( ClientBase* parent ) : m_parent(parent) {}

        /**
         * Virtual Destructor.
         */
        virtual ~Manager() {}

        /**
         * Subscribe to a node.
         * @param jid Service hosting the node.
         * @param node ID of the node to subscribe to.
         */
        void subscribe( const std::string& jid, const std::string& node );

        /**
         * Subscribe to a node.
         * @param jid Node location (service/node).
         */
        void subscribe( const JID& jid )
          { subscribe( jid.bare(), jid.resource() ); }

        /**
         * Unsubscribe from a node.
         * @param jid Service hosting the node.
         * @param node ID of the node to unsubscribe from.
         */
        void unsubscribe( const std::string& jid, const std::string& node );

        /**
         * Unsubscribe from a node.
         * @param jid Node location (service/node).
         */
        void unsubscribe( const JID& jid )
          { unsubscribe( jid.bare(), jid.resource() ); }

        /**
         * Requests the subscription list from a service.
         * @param jid Service to query.
         */
        void requestSubscriptionList( const std::string& jid, SubscriptionListHandler * slh  );

        /**
         * Requests the affiliation list from a service.
         * @param jid Service to query.
         */
        void requestAffiliationList( const std::string& jid, AffiliationListHandler * alh );

        /**
         * Publish an item to a node.
         * @param jid Service hosting the node.
         * @param node Name of the node to delete the item from.
         * @param item Item to publish.
         */
        void publishItem( const std::string& jid, const std::string& node, const Item& item );

        /**
         * Delete an item from a node.
         * @param jid Service hosting the node.
         * @param node Name of the node to delete the item from.
         * @param itemID ID of the item in the node.
         */
        void deleteItem( const std::string& jid, const std::string& node, const std::string& itemID );

        /**
         * Ask for the item list of a specific node.
         * @param node Node location (service/node).
         * @param handler ItemHandler to send the result to.
         */
        void requestItems( const JID& node, ItemHandler * handler );

        /**
         * Creates a new node.
         * @param type NodeType of the new node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         * @param name Name of the new node.
         * @param parent Node containing this node. If empty, the node will be located at the
         *               root of the service.
         */
        void createNode( NodeType type, const std::string& service,
                                        const std::string& nodeid,
                                        const std::string& name,
                                        const std::string& parent = "" );

        /**
         * Creates a new node.
         * @param type NodeType of the new node.
         * @param node Location (service/nodeid) of the new node.
         * @param name Name of the new node.
         * @param parent Node containing this node. If empty, the node will be located at the
         *               root of the service.
         */
        void createNode( NodeType type, const JID& node,
                                        const std::string& name,
                                        const std::string& parent = ""  )
          { createNode( type, node.bare(), node.resource(), name, parent ); }

        /**
         * Creates a new leaf node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         * @param name Name of the new node.
         * @param parent Node containing this node. If empty, the node will be located at the
         *               root of the service.
         */
        void createLeafNode( const std::string& service,
                             const std::string& nodeid,
                             const std::string& name,
                             const std::string& parent = "" )
          { createNode( NodeLeaf, service, nodeid, name, parent ); }

        /**
         * Creates a new collection node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         * @param name Name of the new node.
         * @param parent Node containing this node. If empty, the node will be located at the
         *               root of the service.
         */
        void createCollectionNode( const std::string& service,
                                   const std::string& nodeid,
                                   const std::string& name,
                                   const std::string& parent = "" )
          { createNode( NodeCollection, service, nodeid, name, parent ); }

        /**
         * Deletes a node.
         * @param service Service where to create the new node.
         * @param nodeid Node ID of the new node.
         */
        void deleteNode( const std::string& service,
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
        void purgeNodeItems( const std::string& service, const std::string& nodeid );

        void purgeNodeItems( const JID& node )
          { purgeNodeItems( node.bare(), node.resource() ); }

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

/*
  implement centralized error parsing w/ a TagName/ErrorType array
  to reduce size overhead ?
  PubSub::ErrorType errorType( Tag * error )
  {
    if( !tag || tag->name() != "error" )
      return ErrorNone;
    ...
  }
*/

        /**
         * Retrieve the configuration of a node.
         */
        void requestNodeConfig( const std::string& jid, const std::string& node );

        /**
         * 
         */
        void requestNodeConfig( const JID& jid )
          { requestNodeConfig( jid.bare(), jid.resource() ); }

        /**
         * 
         */
        virtual void handleNodeConfig( const std::string& service,
                                       const std::string& nodeid,
                                       const DataForm& dataForm,
                                       const OptionRequestError e ) = 0;

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
        typedef std::pair< std::string, std::string > TrackedItem;
        typedef std::map < std::string, TrackedItem > ItemOperationTrackMap;
        typedef std::map < std::string, std::string > NodeOperationTrackMap;
        

        ClientBase* m_parent;

        SubscriptionTrackList m_subscriptionTrackList;
        AffiliationListTrackMap m_affListTrackMap;
        SubscriptionListTrackMap m_subListTrackMap;
        ItemOperationTrackMap m_iopTrackMap;
        NodeOperationTrackMap m_nopTrackMap;
    };

  }
}

#endif /* PUBSUBMANAGER_H__ */

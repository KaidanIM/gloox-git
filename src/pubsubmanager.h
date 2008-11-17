/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
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

#include <map>
#include <string>

namespace gloox
{

  class ClientBase;
  class DataForm;

  namespace PubSub
  {

    class EventHandler;
    class ResultHandler;

    /**
     * @brief This manager is used to interact with PubSub services (XEP-0060).
     *
     * \note PubSub support in gloox is still relatively young and you are most
     *       welcome to ask questions, criticize the API and so on. For contact
     *       informations, see below.
     *
     * This manager acts in concert with 2 different handlers:
     *
     * - EventHandler is responsible for receiving the PubSub event
     *   notifications. Register as many as you need with the Manager.
     *
     * - ResultHandler is used to receive a request's result. Depending on the
     *   context, this can be a notification that an item has been succesfully
     *   deleted (or not), or the default node configuration for a service.
     *
     * Note that many PubSub queries will both trigger a notification from
     * registered EventHandlers and from the ResultHandler specific to the
     * query.
     *
     * To get started with PubSub in gloox, create a Manager, implement the
     * PubSub::EventHandler virtuals and register an instance with the Manager.
     * This will get you notified of PubSub events sent to you.
     *
     * Next, to be able to interact with PubSub services, you will need to
     * implement the ResultHandler virtual interfaces to be notified of the
     * result of requests and pass these along to these requests (null handlers
     * are not allowed).
     *
     * \note A null ResultHandler to a query is not allowed and is a no-op.
     *
     * Here's an example.
     *
     * EventHandler::handleItemPublication() can be called with or without
     * the actual tag, depending on if the notification actually includes
     * the payload. From there you could only record the event and be done
     * with it, or decide to retrieve the full payload. Eg:
     *
     * @code
     *
     * class MyEventHandler : public gloox::PubSub::EventHandler
     * {
     *   // ...
     * };
     *
     * void MyEventHandler::handleItemPublication( const JID& service,
     *                                             const std::string& node,
     *                                             const std::string& item,
     *                                             const Tag* entry )
     * {
     *   // we want to retrieve the payload everytime
     *   if( !entry )
     *   {
     *     m_manager->requestItem( service, node, item, this );
     *   }
     *   else
     *   {
     *     do_something_useful( entry );
     *   }
     * }
     *
     * @endcode
     *
     * In response to this request, MyResultHandler::handleItem() will be called.
     *
     * XEP Version: 1.9
     *
     * @author Vincent Thomasset <vthomasset@gmail.com>
     * @author Jakob Schroeter <js@camaya.net>
     *
     * @since 1.0
     *
     * @todo
     * @li Implement Subscription request management.
     * @li Update to XEP version 1.12.
     * @li Write tests and examples.
     * @li Check for possible ways to have generic handleResult method(s) in
     *   ResultHandler.
     * @li Implement unsubscription result notification (aka sub id tracking)
     */
    class GLOOX_API Manager : public IqHandler
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
         * Subscribe to a node.
         *
         * @param service Service hosting the node.
         * @param node ID of the node to subscribe to.
         * @param jid JID to subscribe. If empty, the client's JID will be used
         *        (ie self subscription).
         * @param type SubscriptionType of the subscription (Collections only).
         * @param depth Subscription depth. For 'all', use 0 (Collections only!).
         * @return The IQ ID used in the request.
         *
         * @see ResultHandler::handleSubscriptionResult
         */
        const std::string& subscribe( const JID& service, const std::string& node,
                                      ResultHandler* handler, const JID& jid = JID(),
                                      SubscriptionObject type = SubscriptionNodes,
                                      int depth = 1 );

        /**
         * Unsubscribe from a node.
         *
         * @param service Service hosting the node.
         * @param node ID of the node to unsubscribe from.
         * @param subid An optional, additional subscription ID.
         * @param handler ResultHandler receiving the result notification.
         * @param jid JID to unsubscribe. If empty, the client's JID will be
         * used (ie self unsubscription).
         * @return The IQ ID used in the request.
         *
         * @see ResultHandler::handleUnsubscriptionResult
         */
        const std::string& unsubscribe( const JID& service,
                                        const std::string& node,
                                        const std::string& subid,
                                        ResultHandler* handler,
                                        const JID& jid = JID() );

        /**
         * Requests the subscription list from a service.
         *
         * @param service Service to query.
         * @param handler The ResultHandler to handle the result.
         * @return The IQ ID used in the request.
         *
         * @see ResultHandler::handleSubscriptions
         */
        const std::string& getSubscriptions( const JID& service,
                                             ResultHandler* handler )
        {
          return getSubscriptionsOrAffiliations( service,
                                                 handler,
                                                 GetSubscriptionList );
        }

        /**
         * Requests the affiliation list from a service.
         *
         * @param service Service to query.
         * @param handler The ResultHandler to handle the result.
         * @return The IQ ID used in the request.
         *
         * @see ResultHandler::handleAffiliations
         */
        const std::string& getAffiliations( const JID& service,
                                            ResultHandler* handler )
        {
          return getSubscriptionsOrAffiliations( service,
                                                 handler,
                                                 GetAffiliationList );
        }

        /**
         * Requests subscription options.
         *
         * @param service Service to query.
         * @param jid Subscribed entity.
         * @param node Node ID of the node.
         * @param handler Node ID of the node.
         * @param slh The SubscriptionListHandler to handle the result.
         * @return The IQ ID used in the request.
         *
         * @see ResultHandler::handleSubscriptionOptions
         */
        const std::string& getSubscriptionOptions( const JID& service,
                                                   const JID& jid,
                                                   const std::string& node,
                                                   ResultHandler* handler)
          { return subscriptionOptions( GetSubscriptionOptions, service, jid, node, handler, 0 ); }

        /**
         * Modifies subscription options.
         *
         * @param service Service to query.
         * @param jid Subscribed entity.
         * @param node Node ID of the node.
         * @param df New configuration. The DataForm will be owned and deleted by the Manager.
         * @return The IQ ID used in the request.
         *
         * @see ResultHandler::handleSubscriptionOptionsResult
         */
        const std::string& setSubscriptionOptions( const JID& service,
                                                   const JID& jid,
                                                   const std::string& node,
                                                   DataForm* df,
                                                   ResultHandler* handler )
          { return subscriptionOptions( SetSubscriptionOptions, service, jid, node, handler, df ); }

        /**
         * Requests the affiliation list for a node.
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler The AffiliationListHandler to handle the result.
         *
         * @see ResultHandler::handleAffiliations
         */
        void getAffiliations( const JID& service,
                              const std::string& node,
                              ResultHandler* handler );

        /**
         * Publish an item to a node. The Tag to publish is destroyed
         * by the function before returning.
         *
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param item The item to publish.
         *
         * @see ResultHandler::handleItemPublication
         */
        void publishItem( const JID& service,
                          const std::string& node,
                          Tag* item,
                          ResultHandler* handler );

        /**
         * Delete an item from a node.
         *
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param item ID of the item in the node.
         *
         * @see ResultHandler::handleItemDeletation
         */
        void deleteItem( const JID& service,
                         const std::string& node,
                         const std::string& item,
                         ResultHandler* handler );

        /**
         * Ask for the item list of a specific node.
         *
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler ResultHandler to send the result to.
         *
         * @see ResultHandler::handleItems
         */
        void getItems( const JID& service,
                       const std::string& node,
                       ResultHandler* handler );

        /**
         * Creates a new node.
         *
         * @param type The type of the new node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be
         * located at the root of the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys
         * of the map should be in the form of 'pubsub#name', where 'name' is a
         * valid pubsub option. Do not use this map to include an access model
         * config option, use the @c access parameter instead. See
         * <a href='http://www.xmpp.org/registrar/formtypes.html#http:--jabber.org-protocol-pubsubnode_config'>XEP-0068 Sect. 8</a> for
         * valid options.
         *
         * @see ResultHandler::handleNodeCreation
         */
        void createNode( NodeType type, const JID& service,
                                        const std::string& node,
                                        ResultHandler* handler,
                                        const std::string& name = EmptyString,
                                        const std::string& parent = EmptyString,
                                        AccessModel access = AccessDefault,
                                        const StringMap* config = 0 );

        /**
         * Creates a new leaf node.
         *
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be
         * located at the root of the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys
         * of the map must be in the form of 'pubsub#name', where 'name' is
         * a valid pubsub option. Do not use this map to include an access
         * model config option, use the @c access parameter instead. See
         * <a href='http://www.xmpp.org/registrar/formtypes.html#http:--jabber.org-protocol-pubsubnode_config'>XEP-0068 Sect. 8</a> for
         * valid options.
         *
         * @see ResultHandler::handleNodeCreation
         */
        void createLeafNode( const JID& service,
                             const std::string& node,
                             ResultHandler* handler,
                             const std::string& name,
                             const std::string& parent = EmptyString,
                             AccessModel access = AccessDefault,
                             const StringMap* config = 0 )
          { createNode( NodeLeaf, service, node, handler, name, parent, access, config ); }

        /**
         * Creates a new collection node.
         *
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be
         * located at the root of the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys
         * of the map must be in the form of 'pubsub#name', where 'name' is
         * a valid pubsub option. It is not necessary to include an access
         * model config option. Use the @c access parameter instead. See
         * <a href='http://www.xmpp.org/registrar/formtypes.html#http:--jabber.org-protocol-pubsubnode_config'>XEP-0068 Sect. 8</a> for
         * valid options.
         *
         * @see ResultHandler::handleNodeCreation
         */
        void createCollectionNode( const JID& service,
                                   const std::string& node,
                                   ResultHandler* handler,
                                   const std::string& name,
                                   const std::string& parent = EmptyString,
                                   AccessModel access = AccessDefault,
                                   const StringMap* config = 0 )
          { createNode( NodeCollection, service, node, handler, name, parent, access, config ); }

        /**
         * Deletes a node.
         *
         * @param service Service where to create the new node.
         * @param node Node ID of the new node.
         *
         * @see ResultHandler::handleNodeDeletation
         */
        void deleteNode( const JID& service,
                         const std::string& node,
                         ResultHandler* handler );

/*
        void associateNode( const JID& service,
                            const std::string& node,
                            const std::string& collection );

        void disassociateNode( const JID& service,
                               const std::string& node,
                               const std::string& collection );
*/
        /**
         * Retrieves the default configuration for a specific NodeType.
         *
         * @param service The queried service.
         * @param type NodeType to get default configuration for.
         * @param handler ResultHandler.
         *
         * @see ResultHandler::handleDefaultNodeConfig
         */
        void getDefaultNodeConfig( const JID& service,
                                   NodeType type,
                                   ResultHandler* handler );

        /**
         * Removes all the items from a node.
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler ResultHandler.
         *
         * @see ResultHandler::handleNodePurge
         */
        void purgeNode( const JID& service,
                        const std::string& node,
                        ResultHandler* handler );

        /**
         * Requests the subscriber list for a node.
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler ResultHandler.
         *
         * @see ResultHandler::handleSubscribers
         */
        void getSubscribers( const JID& service,
                             const std::string& node,
                             ResultHandler* handler )
          { subscriberList( service, node, 0, handler ); }

        /**
         * Modifies the subscriber list for a node. This function SHOULD only set the
         * subscriber list to those which needs modification.
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param list ResultHandler.
         *
         * @see ResultHandler::handleSubscribers
         */
        void setSubscribers( const JID& service,
                             const std::string& node,
                             const SubscriberList& list,
                             ResultHandler* handler )
          { subscriberList( service, node, &list, handler ); }

        /**
         * Requests the affiliate list for a node.
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler ResultHandler.
         *
         * @see ResultHandler::handleAffiliates
         */
        void getAffiliates( const JID& service,
                            const std::string& node,
                            ResultHandler* handler )
          { affiliateList( service, node, 0, handler ); }

        /**
         * Modifies the affiliate list for a node.
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param list ResultHandler.
         *
         * @see ResultHandler::handleAffiliatesResult
         */
        void setAffiliates( const JID& service,
                            const std::string& node,
                            const AffiliateList& list,
                            ResultHandler* handler )
          { affiliateList( service, node, &list, handler ); }

        /**
         * Retrieve the configuration (options) of a node.
         *
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler ResultHandler responsible to handle the request result.
         *
         * @see ResultHandler::handleNodeConfig
         */
        void getNodeConfig( const JID& service,
                            const std::string& node,
                            ResultHandler* handler )
          { nodeConfig( service, node, 0, handler ); }

        /**
         * Changes a node's configuration (options).
         *
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler ResultHandler responsible to handle the request result.
         *
         * @see ResultHandler::handleNodeConfigResult
         */
        void setNodeConfig( const JID& service,
                            const std::string& node,
                            const DataForm& config,
                            ResultHandler* handler  )
          { nodeConfig( service, node, &config, handler ); }

        /**
         * Registers an handler to receive notification of events.
         *
         * @param handler EventHandler to register.
         */
        void registerEventHandler( EventHandler* handler )
          { m_eventHandlerList.push_back( handler ); }

        /**
         * Removes an handler from the list of event handlers.
         * @param handler EventHandler to remove.
         */
        void removeEventHandler( EventHandler* handler )
          { m_eventHandlerList.remove( handler ); }

        // reimplemented from DiscoHandler
        void handleDiscoInfoResult( IQ* iq, int context );
        void handleDiscoItemsResult( IQ* iq, int context );
        void handleDiscoError( IQ* iq, int context );
        bool handleDiscoSet( IQ* ) { return 0; }

        // reimplemented from IqHandler.
        virtual bool handleIq( const IQ& iq ) { (void)iq; return false; }

        // reimplemented from IqHandler.
        virtual void handleIqID( const IQ& iq, int context );

      private:
#ifdef PUBSUBMANAGER_TEST
      public:
#endif

        enum TrackContext
        {
          Subscription,
          Unsubscription,
          GetSubscriptionOptions,
          SetSubscriptionOptions,
          GetSubscriptionList,
          GetSubscriberList,
          SetSubscriberList,
          GetAffiliationList,
          GetAffiliateList,
          SetAffiliateList,
          GetNodeConfig,
          SetNodeConfig,
          DefaultNodeConfig,
          GetItemList,
          PublishItem,
          DeleteItem,
          CreateNode,
          DeleteNode,
          PurgeNodeItems,
          NodeAssociation,
          NodeDisassociation,
          GetFeatureList,
          DiscoServiceInfos,
          DiscoNodeInfos,
          DiscoNodeItems,
          InvalidContext
        };

        class PubSub : public StanzaExtension
        {
          public:
            /**
             * Creates a new PubSub object that can be used to request the given type.
             * @param context The requets type.
             */
            PubSub( TrackContext context = InvalidContext );

            /**
             * Creates a new PubSub object by parsing the given Tag.
             * @param tag The Tag to parse.
             */
            PubSub( const Tag* tag );

            /**
             * Virtual destructor.
             */
            virtual ~PubSub();

            /**
             * Returns the list of subscriptions.
             * @return The list of subscriptions.
             */
            const SubscriptionMap& subscriptionMap() const
              { return m_subscriptionMap; }

            /**
             * Returns the list of subscriptions.
             * @return The list of subscriptions.
             */
            const AffiliationMap& affiliationMap() const
             { return m_affiliationMap; }

            /**
             * Sets the JID to use in e.g. subscription requests.
             * @param jid The JID to use.
             */
            void setJID( const JID& jid ) { m_jid = jid; }

            /**
             * Returns the pubsub JID (not the service JID).
             * @return The pubsub JID.
             */
            const JID& jid() const { return m_jid; }

            /**
             * Sets the node to use in e.g. subscription requests.
             * @param node The node to use.
             */
            void setNode( const std::string& node ) { m_node = node; }

            /**
             * Returns the pubsub node.
             * @return The pubsub node.
             */
            const std::string& node() const { return m_node; }

            /**
             * Sets the Subscription ID to use.
             * @param subid The Subscription ID to use.
             */
            void setSubscriptionID( const std::string& subid )
              { m_subid = subid; }

            /**
             * Sets the subscription options.
             * @param jid The JID to set the options for.
             * @param node The node to set the options for.
             * @param df The DataForm holding the subscription options.
             * Will be owned and deleted by the PubSub object
             */
            void setOptions( const JID& jid, const std::string& node, DataForm* df )
            {
              m_options.jid = jid;
              m_options.node = node;
              m_options.df = df;
            }

            // re-implemented from StanzaExtension
            virtual const std::string& filterString() const;

            // re-implemented from StanzaExtension
            virtual StanzaExtension* newInstance( const Tag* tag ) const
            {
              return new PubSub( tag );
            }

            // re-implemented from StanzaExtension
            virtual Tag* tag() const;

          private:
            AffiliationMap m_affiliationMap;
            SubscriptionMap m_subscriptionMap;
            TrackContext m_ctx;

            struct Options
            {
              JID jid;
              std::string node;
              DataForm* df;
            };
            Options m_options;
            JID m_jid;
            std::string m_node;
            std::string m_subid;
        };

        /**
         * This function sets or requests a node's configuration form
         * (depending on arguments). Does the actual work for requestNodeConfig
         * and setNodeConfig.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler ResultHandler responsible to handle the request result.
         */
        void nodeConfig( const JID& service, const std::string& node,
                         const DataForm* config, ResultHandler* handler );

        /**
         * This function sets or requests a node's subscribers list form
         * (depending on arguments). Does the actual work for
         * requestSubscriberList and setSubscriberList.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler ResultHandler responsible to handle the request result.
         */
        void subscriberList( const JID& service,
                             const std::string& node,
                             const SubscriberList* config,
                             ResultHandler* handler );

        /**
         * This function sets or requests a node's affiliates list
         * (depending on arguments). Does the actual work for
         * requestAffiliateList and setAffiliateList.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler ResultHandler responsible to handle the request result.
         */

        void affiliateList( const JID& service,
                            const std::string& node,
                            const AffiliateList* config,
                            ResultHandler* handler );

        const std::string& subscriptionOptions( TrackContext context,
                                                const JID& service,
                                                const JID& jid,
                                                const std::string& node,
                                                ResultHandler* handler,
                                                DataForm* df );

        const std::string& getSubscriptionsOrAffiliations( const JID& service,
            ResultHandler* handler,
            TrackContext context );
        typedef std::pair< std::string, std::string > TrackedItem;
        typedef std::map < std::string, TrackedItem > ItemOperationTrackMap;
        typedef std::map < std::string, std::string > NodeOperationTrackMap;

        typedef std::map < std::string, ResultHandler* > ResultHandlerTrackMap;
        typedef std::list< EventHandler* > EventHandlerList;

        ClientBase* m_parent;

        ItemOperationTrackMap  m_iopTrackMap;
        NodeOperationTrackMap  m_nopTrackMap;

        ResultHandlerTrackMap  m_resultHandlerTrackMap;
        EventHandlerList       m_eventHandlerList;
    };

  }

}

#endif // PUBSUBMANAGER_H__

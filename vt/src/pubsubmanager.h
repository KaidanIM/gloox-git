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
     * 
     * \bug No 'from' field to iq (use m_parent->BareJID() ?),
     *      same for the jid field of the subscription tag in subscribe.
     * \bug Tracking...
     * \bug HandleOptions is incomplete
     * \bug conflicting AffiliationType w/ MUCXXX
     */
    class Manager : public IqHandler
    {
      public:

        /**
         * Initialize the manager.
         */
        Manager( ClientBase* parent ) : m_parent(parent) {}

        /**
         * Virtual Destructor.
         */
        virtual ~Manager() {}

        /**
         * Subscribe to a node.
         * @param jid Service hosting the node.
         * @param node Name of the node to subscribe to.
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
         * @param node Name of the node to unsubscribe from.
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
         * 
         */
        void requestOptions( const std::string& jid, const std::string& node );

        /**
         * 
         */
        void requestOptions( const JID& jid )
          { requestOptions( jid.bare(), jid.resource() ); }

        /**
         * Ask for the item list of a specific node.
         * @param node Node location (service/node).
         * @param handler ItemHandler to send the result to.
         */
        void requestItems( const JID& node, ItemHandler * handler );

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

        /**
         * 
         */
        virtual void handleOptions( const JID& jid,
                                    const std::string& node,
                                    const DataForm& dataForm,
                                    const OptionRequestError e ) = 0;

        bool handleIq  ( Stanza *stanza );
        bool handleIqID( Stanza *stanza, int context );

      private:
        typedef std::list< SubscriptionHandler * > SubscriptionTrackList;
        typedef std::map< std::string, AffiliationListHandler * > AffiliationListTrackMap;
        typedef std::map< std::string, SubscriptionListHandler * > SubscriptionListTrackMap;

        ClientBase* m_parent;

        SubscriptionTrackList m_subscriptionTrackList;
        AffiliationListTrackMap m_affListTrackMap;
        SubscriptionListTrackMap m_subListTrackMap;

    };

  }
}

#endif /* PUBSUBMANAGER_H__ */

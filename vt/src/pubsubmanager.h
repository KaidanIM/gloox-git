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
    class Node;

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


        //typedef std::list< Node * > NodeList;
        //typedef std::list< Item * > ItemList;

        /**
         * Initialize the manager.
         */
        Manager( ClientBase* parent ) : m_parent(parent) {}

        /**
         * Virtual Destructor.
         */
        virtual ~Manager() {}

        /**
         * 
         */
        void subscribe( const std::string& jid, const std::string& node );

        /**
         * 
         */
        void unsubscribe( const std::string& jid, const std::string& node );

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
         * 
         */
        void requestOptions( const std::string& jid, const std::string& node );

        /**
         * 
         */
        void requestItems( const JID& node, ItemHandler * handler );

        /**
         * Registers an handler to receive notification of (un)subscription events.
         */
        void registerSubscriptionHandler( SubscriptionHandler * handler )
          { m_subscriptionTrackList.push_back( handler ); }

        /**
         * Removes an handler from the list of objects listening to (un)subscription events.
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

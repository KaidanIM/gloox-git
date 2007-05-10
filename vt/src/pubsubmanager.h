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
  class PSSubscriptionHandler;
  class PSSubscriptionListHandler;
  class PSAffiliationListHandler;

  /**
   * \bug No 'from' field to iq (use m_parent->BareJID() ?),
   *      same for the jid field of the subscription tag in subscribe.
   * \bug Tracking...
   * \bug HandleOptions is incomplete
   * \bug conflicting AffiliationType w/ MUCXXX
   */
  class PubSubManager : public IqHandler
  {
    public:


      class Node;
      class Item;

      typedef std::list< Node * > NodeList;
      typedef std::list< Item * > ItemList;

      struct Item
      {
        public:
          Item( const std::string& _id )
            : id( _id )
          {}
          std::string id;
          
          
      };

      struct Node
      {
        public:
          /**
           * Describes the different node types.
           */
          enum NodeType {
            NodeInvalid,     /**< Invalid node type */
            NodeLeaf,        /**< A node that contains published items only. It is NOT a container for other nodes. */
            NodeCollection   /**< A node that contains nodes and/or other collections but no published items.
                              *   Collections make it possible to represent hierarchial node structures. */
          };

          /**
           * Constructs a Node from a type, a JID (XEP-0060 Sect 4.6.1) and a name.
           */
          Node( NodeType _type, const std::string& _jid, const std::string& _name )
            : type( _type ), jid( _jid ), name( _name) {}

          /**
           * Constructs a Node from a type, a JID+NodeID (XEP-0060 Sect 4.6.2) and a name.
           */
          Node( NodeType _type, const std::string& _jid,
                                const std::string& _node,
                                const std::string& _name )
            : type( _type ), jid( _jid ), name( _name) { jid.setResource( _node ); }

          NodeType type;
          JID jid;
          std::string name;
          /*
          union {
            ItemList items;
            NodeList nodes;
          };
          */
      };

      struct LeafNode : public Node
      {
        LeafNode( const std::string& _service, const std::string& _name )
          : Node( NodeLeaf, _service, _name )
        {}
        ItemList m_itemList;
      };

      struct CollectionNode : public Node
      {
        CollectionNode( const std::string& _service, const std::string& _name )
          : Node( NodeCollection, _service, _name )
        {}
        NodeList m_nodeList;
      };

      /**
       * Initialize the manager.
       */
      PubSubManager( ClientBase* parent ) : m_parent(parent) {}

      /**
       * Virtual Destructor.
       */
      virtual ~PubSubManager() {}

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
      void requestSubscriptionList( const std::string& jid, PSSubscriptionListHandler * slh  );

      /**
       * Requests the affiliation list from a service.
       * @param jid Service to query.
       */
      void requestAffiliationList( const std::string& jid, PSAffiliationListHandler * alh );

      /**
       * 
       */
      void requestOptions( const std::string& jid, const std::string& node );

      /**
       * 
       */
      virtual void handleOptions( const JID& jid,
                                  const std::string& node,
                                  const DataForm& dataForm,
                                  const OptionRequestError e ) = 0;

      bool handleIq (Stanza *stanza);
      bool handleIqID (Stanza *stanza, int context);

    private:
      typedef std::map<std::string, PSSubscriptionHandler * > SubscriptionTrackMap;
      typedef std::map<std::string, PSAffiliationListHandler * > AffiliationListTrackMap;
      typedef std::map<std::string, PSSubscriptionListHandler * > SubscriptionListTrackMap;
      ClientBase* m_parent;
      SubscriptionTrackMap m_subscriptionTrackMap;
      AffiliationListTrackMap m_affListTrackMap;
      SubscriptionListTrackMap m_subListTrackMap;
  };

}

#endif /* PUBSUBMANAGER_H__ */

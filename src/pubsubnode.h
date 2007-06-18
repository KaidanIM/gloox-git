/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBNODE_H__
#define PUBSUBNODE_H__

#include <string>

namespace gloox
{

  namespace PubSub
  {

    /**
     * Describes a PubSub node.
     */
    class Node
    {
      public:

        /**
         * Constructs a Node from a type, a JID (XEP-0060 Sect 4.6.1) and a name.
         * @param type
         * @param jid
         * @param id
         * @param name
         */
        Node( NodeType type, const std::string& jid,
              const std::string& id, const std::string& name )
          : m_type( type ), m_service( jid ), m_id( id), m_name( name )
        {}

        /**
         * Constructs a Node from a type, a JID (XEP-0060 Sect 4.6.1)
         * and a name.
         * @param type
         * @param jid
         * @param name
         */
        Node( NodeType type, const JID& jid, const std::string& name )
          : m_type( type ), m_service( jid.bare() ), m_id( jid.resource() ), m_name( name)
        {}

        /**
         * Constructs a Node from a type, a JID+NodeID (XEP-0060 Sect 4.6.2)
         * and a name.
         * @param type
         * @param jid
         * @param id
         */
        Node( NodeType type, const std::string& jid, const std::string& id,
              const std::string& sid, const std::string& name )
          : m_type( type ), m_service( jid ), m_id( id ), m_sid( sid ), m_name( name)
        {}

        /**
         * Constructs a Node from a type, a JID+NodeID (XEP-0060 Sect 4.6.2)
         * and a name.
         * @param type
         * @param jid
         * @param sid
         * @param name
         */
        Node( NodeType type, const JID& jid, const std::string& sid, const std::string& name )
          : m_type( type ), m_service( jid.bare() ), m_id( jid.resource() ), m_sid( sid ), m_name( name)
        {}

      private:

        NodeType m_type;
        std::string m_service;
        std::string m_id;
        std::string m_name;
        std::string m_sid;
        SubscriptionType m_subscription;
        /*
        union {
          ItemList items;
          NodeList nodes;
        };
        */
    };

    /**
     * A leaf node.
     */
    class LeafNode : public Node
    {
      public:
        LeafNode( const std::string& service, const std::string& name )
          : Node( NodeLeaf, service, name )
        {}
      private:
        ItemList m_itemList;
    };

    /**
     * A collection node.
     */
    class CollectionNode : public Node
    {
      public:
        CollectionNode( const std::string& service, const std::string& name )
          : Node( NodeCollection, service, name )
        {}
      private:
        NodeList m_nodeList;
    };

  }

}

#endif // PUBSUBNODE_H__

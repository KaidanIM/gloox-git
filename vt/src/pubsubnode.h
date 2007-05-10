/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef __PUBSUBITEM_H__
#define __PUBSUBITEM_H__

#include <string>

namespace gloox
{

  namespace PubSub
  {

    /**
     *
     */
    struct Node
    {
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
      std::string sid;
      SubscriptionType subscription;
      /*
      union {
        ItemList items;
        NodeList nodes;
      };
      */
    };

    /**
     *
     */
    struct LeafNode : public PSNode
    {
      LeafNode( const std::string& _service, const std::string& _name )
        : Node( NodeLeaf, _service, _name ) {}
      ItemList m_itemList;
    };

    /**
     *
     */
    struct CollectionNode : public PSNode
    {
      CollectionNode( const std::string& _service, const std::string& _name )
        : Node( NodeCollection, _service, _name ) {}
      NodeList m_nodeList;
    };

  }

}

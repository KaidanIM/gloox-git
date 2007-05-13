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
     * Describes a PubSub node.
     */
    class Node
    {
      public:

        /**
         * Constructs a Node from a type, a JID (XEP-0060 Sect 4.6.1) and a name.
         * @param _type
         * @param _jid
         * @param _id
         * @param _name
         */
        Node( NodeType _type, const std::string& _jid,
                              const std::string& _id,
                              const std::string& _name )
          : type( _type ), service( _jid ),
                id( _id), name( _name )
        {}

        /**
         * Constructs a Node from a type, a JID (XEP-0060 Sect 4.6.1)
         * and a name.
         * @param
         * @param
         * @param
         */
        Node( NodeType _type, const JID& _jid,
                              const std::string& _name )
          : type( _type ), service( _jid.bare() ),
                id( jid.resource() ), name( _name)
        {}

        /**
         * Constructs a Node from a type, a JID+NodeID (XEP-0060 Sect 4.6.2)
         * and a name.
         * @param
         * @param
         * @param
         */
        Node( NodeType _type, const std::string& _jid,
                              const std::string& _id,
                              const std::string& _sid, 
                              const std::string& _name )
          : type( _type ), service( _jid ),
                id( id ), sid( _sid ), name( _name)
        {}

        /**
         * Constructs a Node from a type, a JID+NodeID (XEP-0060 Sect 4.6.2)
         * and a name.
         * @param 
         * @param 
         * @param 
         */
        Node( NodeType _type, const JID& _jid,
                              const std::string& _sid,
                              const std::string& _name )
          : type( _type ), service( _jid.bare() ),
                id( _jid.resource() ), sid( _sid ), name( _name)
        {}

      private:

        NodeType type;
        std::string service;
        std::string id;
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
    class LeafNode : public Node
    {
      public:
        LeafNode( const std::string& _service, const std::string& _name )
          : Node( NodeLeaf, _service, _name ) {}
      private:
        ItemList m_itemList;
    };

    /**
     *
     */
    class CollectionNode : public Node
    {
      public:
        CollectionNode( const std::string& _service, const std::string& _name )
          : Node( NodeCollection, _service, _name ) {}
      private:
        NodeList m_nodeList;
    };

  }

}

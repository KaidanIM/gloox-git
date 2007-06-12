/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef __PUBSUBITEMHANDLER_H__
#define __PUBSUBITEMHANDLER_H__

#include <list>

namespace gloox
{  

  class JID;
  class DiscoNodeItem;

  typedef std::list<DiscoNodeItem> DiscoNodeItemList;

  namespace PubSub
  {

    //class Item;
    //typedef std::list< const Tag * > ItemList;

    /**
     * @brief A virtual interface for receiving item (list) result.
     *
     * Derive from this interface and register with the PubSub::Manager.
     *
     * @author Jakob Schroeter <js@camaya.net>
     */
    class ItemHandler
    {
      public:
        /**
         * 
         */
        virtual void handleItem( const JID& service, const std::string& nodeid, const Item * item ) = 0;

        /**
         * Receives the list of Items for a node.
         * @param service Service hosting the queried node.
         * @param nodeid ID of the queried node. If empty, the root node has been queried.
         * @param itemList List of contained items.
         */
        virtual void handleItemList( const JID& service, const std::string& nodeid, const Tag::TagList& itemList ) = 0;

        /**
         * Virtual destructor.
         */
        virtual ~ItemHandler() {}
    };

  }

}



#endif /* __PUBSUBITEMHANDLER_H__ */

/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBITEMHANDLER_H__
#define PUBSUBITEMHANDLER_H__

#include <list>

namespace gloox
{

  class JID;
  class Tag;
  class DiscoNodeItem;

  typedef std::list<DiscoNodeItem> DiscoNodeItemList;

  namespace PubSub
  {

    /**
     * @brief A virtual interface to receive item related requests results.
     *
     * Derive from this interface and pass it to item related requests.
     *
     * @author Vincent Thomasset
     */
    class ItemHandler
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~ItemHandler() {}

        /**
         * 
         */
        virtual void handleItem( const JID& service, const std::string& node, const Tag * entry ) = 0;

        /**
         * Receives the list of Items for a node.
         * @param service Service hosting the queried node.
         * @param nodeid ID of the queried node. If empty, the root node has been queried.
         * @param itemList List of contained items.
         */
        virtual void handleItemList( const JID& service,
                                     const std::string& node,
                                     const Tag::TagList& itemList ) = 0;

    };

  }

}



#endif /* PUBSUBITEMHANDLER_H__ */

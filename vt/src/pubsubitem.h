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
#include <list>

namespace gloox
{

  class Tag;

  namespace PubSub
  {
    /**
     *
     */
    class Item
    {
      public:

        /**
         * Constructs an Item from a tag.
         */
        Item( Tag * tag );

        /**
         * Construct an Item
         */
        Item( const std::string& _id,
              const std::string& _title,
              const std::string& _sum,
              const std::string& _link = "" )
          : id( _id ), title( _title ), summary( _sum ), link( _link ) {}

      //private:

        std::string id;      /**< Unique item ID. */
        std::string title;   /**< Item title. */
        std::string summary; /**< Item summary. */
        std::string link;    /**< Item link. */
        //time_t publishDate;
        //time_t updateDate;
    };

    typedef std::list< Item * > ItemList;

  }

}


#endif /* __PUBSUBITEM_H__ */

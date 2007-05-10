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

  class Tag;

  namespace PubSub
  {
    /**
     *
     */
    struct Item
    {
      public:
        /**
         *
         */
        Item( Tag * tag );

        /**
         *
         */
        Item( const std::string& _id,
              const std::string& _title,
              const std::string& _sum,
              const std::string& _link = "" )
          : id( _id ), title( _title ), summary( _sum ), link( _link ) {}

        std::string id;
        std::string title;
        std::string summary;
        std::string link;
        //time_t publishDate;
        //time_t updateDate;
    };
  }
}


#endif /* __PUBSUBITEM_H__ */

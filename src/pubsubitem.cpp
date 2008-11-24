/*
  Copyright (c) 2005-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "pubsubitem.h"
#include "tag.h"

namespace gloox
{

  namespace PubSub
  {

    Item::Item()
      : m_child( 0 )
    {
    }

    Item::Item( const Tag* tag )
      : m_child( 0 )
    {
      if( !tag || tag->name() != "item" )
        return;

      m_id = tag->findAttribute( "id" );

      if( tag->children().size() )
        m_child = tag->children().front();
    }

    Item::~Item()
    {
      delete m_child;
    }

    Tag* Item::tag() const
    {
      Tag* t = new Tag( "item" );
      t->addAttribute( "id", m_id );
      if( m_child )
        t->addChild( m_child->clone() );

      return t;
    }

  }

}

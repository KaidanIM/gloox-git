 /*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "dataformitem.h"

#include "tag.h"

namespace gloox
{

  namespace DataForm
  {

    Item::Item()
    {
    }

    Item::Item( Tag* tag )
    {
      if( tag->name() != "item" )
        return;

      TagList &l = tag->children();
      TagList::const_iterator it = l.begin();
      for( ; it != l.end(); ++it )
      {
        Field* f = new Field( (*it) );
        m_fields.push_back( f );
      }
    }

    Item::~Item()
    {
    }

    Tag* Item::tag() const
    {
      Tag* i = new Tag ( "item" );
      FieldContainer::FieldList::const_iterator it = m_fields.begin();
      for( ; it != m_fields.end(); ++it )
      {
        i->addChild( (*it)->tag() );
      }
      return i;
    }

  }

}

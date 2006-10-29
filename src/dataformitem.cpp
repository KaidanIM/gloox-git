 /*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
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

  DataFormItem::DataFormItem()
    : DataFormField( FIELD_TYPE_ITEM )
  {
  }

  DataFormItem::DataFormItem( Tag* tag )
    : DataFormField( FIELD_TYPE_ITEM )
  {
    if( tag->name() != "item" )
      return;

    Tag::TagList &l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      DataFormField *f = new DataFormField( (*it) );
      m_fields.push_back( f );
    }
  }

  DataFormItem::~DataFormItem()
  {
  }

  Tag* DataFormItem::tag()
  {
    Tag *i = new Tag ( "item" );
    DataFormBase::FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
      i->addChild( (*it)->tag() );
    }
    return i;
  }

}

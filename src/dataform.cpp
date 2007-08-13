/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "dataform.h"
#include "dataformfield.h"
#include "dataformreported.h"
#include "dataformitem.h"
#include "util.h"
#include "tag.h"

namespace gloox
{

  DataForm::DataForm( DataFormType type, const StringList& instructions, const std::string& title )
    : m_instructions( instructions ), m_type( type ), m_title( title )
  {
  }

  DataForm::DataForm( DataFormType type, const std::string& title )
    : m_type( type ), m_title( title )
  {
  }

  DataForm::DataForm( const Tag* tag )
    : m_type( FormTypeInvalid )
  {
    parse( tag );
  }

  DataForm::DataForm()
  : m_type( FormTypeInvalid )
  {
  }

  DataForm::~DataForm()
  {
  }

  static const char * dfTypeValues[] =
  {
    "form", "submit", "cancel", "result"
  };

  bool DataForm::parse( const Tag* tag )
  {
    if( !tag || !tag->hasAttribute( XMLNS, XMLNS_X_DATA ) || tag->name() != "x" )
      return false;

    m_type = (DataFormType)util::lookup(tag->findAttribute( TYPE ), dfTypeValues );
    if( m_type == FormTypeInvalid )
      return false;

    const Tag::TagList& l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      if( (*it)->name() == "title" )
        m_title = (*it)->cdata();
      else if( (*it)->name() == "instructions" )
        m_instructions.push_back( (*it)->cdata() );
      else if( (*it)->name() == "field" )
        m_fields.push_back( new DataFormField( (*it) ) );
      else if( (*it)->name() == "reported" )
        m_fields.push_back( new DataFormReported( (*it) ) );
      else if( (*it)->name() == "item" )
        m_fields.push_back( new DataFormItem( (*it) ) );
    }

    return true;
  }

  Tag* DataForm::tag() const
  {
    if( m_type == FormTypeInvalid )
      return 0;

    Tag* x = new Tag( "x", XMLNS, XMLNS_X_DATA );
    x->addAttribute( TYPE, util::lookup( m_type, dfTypeValues ) );
    if( !m_title.empty() )
      new Tag( x, "title", m_title );

    StringList::const_iterator it_i = m_instructions.begin();
    for( ; it_i != m_instructions.end(); ++it_i )
      new Tag( x, "instructions", (*it_i) );

    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
      DataFormItem* i = dynamic_cast<DataFormItem*>( (*it) );
      if( i )
      {
        x->addChild( i->tag() );
        continue;
      }

      DataFormReported* r = dynamic_cast<DataFormReported*>( (*it) );
      if( r )
      {
        x->addChild( r->tag() );
        continue;
      }

      x->addChild( (*it)->tag() );
    }

    return x;
  }

}

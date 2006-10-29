/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
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
#include "tag.h"

namespace gloox
{

  DataForm::DataForm( DataFormType type, const StringList& instructions, const std::string& title )
    : m_instructions( instructions ), m_type( type ), m_title( title )
  {
  }

  DataForm::DataForm( DataFormType type )
    : m_type( type )
  {
  }

  DataForm::DataForm( Tag *tag )
    : m_type( FORM_TYPE_INVALID )
  {
    if( !tag || !tag->hasAttribute( "xmlns", XMLNS_X_DATA ) || tag->name() != "x" )
      return;

    if( tag->hasAttribute( "type", "form" ) )
      m_type = FORM_TYPE_FORM;
    else if( tag->hasAttribute( "type", "submit" ) )
      m_type = FORM_TYPE_SUBMIT;
    else if( tag->hasAttribute( "type", "cancel" ) )
      m_type = FORM_TYPE_CANCEL;
    else if( tag->hasAttribute( "type", "result" ) )
      m_type = FORM_TYPE_RESULT;
    else
      return;

    Tag::TagList l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      if( (*it)->name() == "title" )
        m_title = (*it)->cdata();
      else if( (*it)->name() == "instructions" )
        m_instructions.push_back( (*it)->cdata() );
      else if( (*it)->name() == "field" )
      {
        DataFormField *f = new DataFormField( (*it) );
        m_fields.push_back( f );
      }
      else if( (*it)->name() == "reported" )
      {
        DataFormReported *r = new DataFormReported( (*it) );
        m_fields.push_back( r );
      }
      else if( (*it)->name() == "item" )
      {
        DataFormItem *r = new DataFormItem( (*it) );
        m_fields.push_back( r );
      }
    }
  }

  DataForm::~DataForm()
  {
    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
      DataFormField *f = dynamic_cast<DataFormField*>( (*it) );
      if( f )
      {
        delete f;
        continue;
      }

      DataFormItem *i = dynamic_cast<DataFormItem*>( (*it) );
      if( i )
      {
        delete i;
        continue;
      }

      DataFormReported *r = dynamic_cast<DataFormReported*>( (*it) );
      if( r )
      {
        delete r;
        continue;
      }
    }
  }

  Tag* DataForm::tag() const
  {
    if( m_type == FORM_TYPE_INVALID )
      return 0;

    Tag *x = new Tag( "x" );
    x->addAttribute( "xmlns", XMLNS_X_DATA );
    if( !m_title.empty() )
      new Tag( x, "title", m_title );

    StringList::const_iterator it_i = m_instructions.begin();
    for( ; it_i != m_instructions.end(); ++it_i )
      new Tag( x, "instructions", (*it_i) );

    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
      DataFormField *f = dynamic_cast<DataFormField*>( (*it) );
      if( f )
      {
        x->addChild( f->tag() );
        continue;
      }

      DataFormItem *i = dynamic_cast<DataFormItem*>( (*it) );
      if( i )
      {
        x->addChild( i->tag() );
        continue;
      }

      DataFormReported *r = dynamic_cast<DataFormReported*>( (*it) );
      if( r )
      {
        x->addChild( r->tag() );
        continue;
      }
    }

    switch( m_type )
    {
      case FORM_TYPE_FORM:
        x->addAttribute( "type", "form" );
        break;
      case FORM_TYPE_SUBMIT:
        x->addAttribute( "type", "submit" );
        break;
      case FORM_TYPE_CANCEL:
        x->addAttribute( "type", "cancel" );
        break;
      case FORM_TYPE_RESULT:
        x->addAttribute( "type", "result" );
        break;
      default:
        break;
    }

    return x;
  }

}

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "dataform.h"
#include "dataformfield.h"
#include "tag.h"

namespace gloox
{

  DataForm::DataForm( DataFormType type, const std::string& title, const std::string& instructions )
    : m_type( type ), m_title( title ), m_instructions( instructions )
  {
  }

  DataForm::DataForm( Tag *tag )
    : m_type( FORM_TYPE_INVALID )
  {
    if( !tag->hasAttribute( "xmlns", XMLNS_DATA_FORMS ) || tag->name() != "x" )
      return;

    if( tag->hasAttribute( "type", "form" ) )
      m_type == FORM_TYPE_FORM;
    else if( tag->hasAttribute( "type", "submit" ) )
      m_type == FORM_TYPE_SUBMIT;
    else if( tag->hasAttribute( "type", "cancel" ) )
      m_type == FORM_TYPE_CANCEL;
    else if( tag->hasAttribute( "type", "result" ) )
      m_type == FORM_TYPE_RESULT;
    else
      return;

    Tag::TagList l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( it; it != l.end(); ++it )
    {
      if( (*it)->name() == "title" )
        m_title = (*it)->cdata();
      else if( (*it)->name() == "instructions" )
        m_instructions = (*it)->cdata();
      else if( (*it)->name() == "field" )
      {
        DataFormField f( (*it) );
        m_fields.push_back( f );
      }
    }
  }

  DataForm::~DataForm()
  {
  }

  Tag* DataForm::tag()
  {
    if( m_type == FORM_TYPE_INVALID )
      return 0;

    Tag *x = new Tag( "x" );
    x->addAttrib( "xmlns", XMLNS_DATA_FORMS );
    if( !m_title.empty() )
      x->addChild( new Tag( "title", m_title ) );
    if( !m_instructions.empty() )
      x->addChild( new Tag( "instructions", m_instructions ) );

    FieldList::const_iterator it = m_fields.begin();
    for( it; it != m_fields.end(); ++it )
    {
      x->addChild( (*it).tag() );
    }

    switch( m_type )
    {
      case FORM_TYPE_FORM:
        x->addAttrib( "type", "form" );
        break;
      case FORM_TYPE_SUBMIT:
        x->addAttrib( "type", "submit" );
        break;
      case FORM_TYPE_CANCEL:
        x->addAttrib( "type", "cancel" );
        break;
      case FORM_TYPE_RESULT:
        x->addAttrib( "type", "result" );
        break;
    }

    return x;
  }

  bool DataForm::hasField( const std::string& field )
  {
    FieldList::const_iterator it = m_fields.begin();
    for( it; it != m_fields.end(); ++it )
    {
      if( (*it).name() == field )
        return true;
    }

    return false;
  }

  DataFormField DataForm::field( const std::string& field )
  {
    FieldList::const_iterator it = m_fields.begin();
    for( it; it != m_fields.end(); ++it )
    {
      if( (*it).name() == field )
        return (*it);
    }

    return DataFormField();
  }

};

/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/

#include "dataformfield.h"
#include "tag.h"

namespace gloox
{

  DataFormField::DataFormField( DataFormFieldType type )
    : m_type( type ), m_required( false )
  {
  }

  DataFormField::DataFormField( Tag *tag )
    : m_type( FIELD_TYPE_INVALID ), m_required( false )
  {
    if( tag->hasAttribute( "type", "boolean" ) )
      m_type = FIELD_TYPE_BOOLEAN;
    else if( tag->hasAttribute( "type", "fixed" ) )
      m_type = FIELD_TYPE_FIXED;
    else if( tag->hasAttribute( "type", "hidden" ) )
      m_type = FIELD_TYPE_HIDDEN;
    else if( tag->hasAttribute( "type", "jid-multi" ) )
      m_type = FIELD_TYPE_JID_MULTI;
    else if( tag->hasAttribute( "type", "jid-single" ) )
      m_type = FIELD_TYPE_JID_SINGLE;
    else if( tag->hasAttribute( "type", "list-multi" ) )
      m_type = FIELD_TYPE_LIST_MULTI;
    else if( tag->hasAttribute( "type", "list-single" ) )
      m_type = FIELD_TYPE_LIST_SINGLE;
    else if( tag->hasAttribute( "type", "text-multi" ) )
      m_type = FIELD_TYPE_TEXT_MULTI;
    else if( tag->hasAttribute( "type", "text-private" ) )
      m_type = FIELD_TYPE_TEXT_PRIVATE;
    else if( tag->hasAttribute( "type", "text-single" ) )
      m_type = FIELD_TYPE_TEXT_SINGLE;

    Tag::TagList l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      if( (*it)->name() == "desc" )
        m_desc = (*it)->cdata();
      else if( (*it)->name() == "required" )
        m_required = true;
      else if( (*it)->name() == "value" )
      {
        if( m_type == FIELD_TYPE_TEXT_MULTI )
          m_values.push_back( (*it)->cdata() );
        else
          m_value = (*it)->cdata();
      }
      else if( (*it)->name() == "option" )
        m_options[(*it)->findAttribute( "label" )] = (*it)->findChild( "value" )->cdata();
    }

    if( tag->hasAttribute( "var" ) )
      m_name = tag->findAttribute( "var" );

    if( tag->hasAttribute( "label" ) )
      m_label = tag->findAttribute( "label" );
  }

  DataFormField::~DataFormField()
  {
  }

  Tag* DataFormField::tag() const
  {
    if( m_type == FIELD_TYPE_INVALID )
      return 0;

    Tag *field = new Tag( "field" );
    field->addAttrib( "var", m_name );
    field->addAttrib( "label", m_label );
    if( m_required )
      field->addChild( new Tag( "required" ) );
    if( !m_desc.empty() )
      field->addChild( new Tag( "desc", m_desc ) );

    switch( m_type )
    {
      case FIELD_TYPE_BOOLEAN:
        field->addAttrib( "type", "boolean" );
        break;
      case FIELD_TYPE_FIXED:
        field->addAttrib( "type", "fixed" );
        break;
      case FIELD_TYPE_HIDDEN:
        field->addAttrib( "type", "hidden" );
        break;
      case FIELD_TYPE_JID_MULTI:
        field->addAttrib( "type", "jid-multi" );
        break;
      case FIELD_TYPE_JID_SINGLE:
        field->addAttrib( "type", "jid-single" );
        break;
      case FIELD_TYPE_LIST_MULTI:
        field->addAttrib( "type", "list-multi" );
        break;
      case FIELD_TYPE_LIST_SINGLE:
        field->addAttrib( "type", "list-single" );
        break;
      case FIELD_TYPE_TEXT_MULTI:
        field->addAttrib( "type", "text-multi" );
        break;
      case FIELD_TYPE_TEXT_PRIVATE:
        field->addAttrib( "type", "text-private" );
        break;
      case FIELD_TYPE_TEXT_SINGLE:
        field->addAttrib( "type", "text-single" );
        break;
      default:
        break;
    }

    if( ( m_type == FIELD_TYPE_LIST_SINGLE ) || ( m_type == FIELD_TYPE_LIST_MULTI ) )
    {
      StringMap::const_iterator it = m_options.begin();
      for( ; it != m_options.end(); ++it )
      {
        Tag *option = new Tag( field, "option" );
        option->addAttrib( "label", (*it).first );
        option->addChild( new Tag( "value", (*it).second ) );
      }
    }
    else if( m_type == FIELD_TYPE_BOOLEAN )
    {
      if( m_value.empty() || m_value == "false" || m_value == "0" )
        field->addChild( new Tag( "value", "0" ) );
      else
        field->addChild( new Tag( "value", "1" ) );
    }
    else if( m_type == FIELD_TYPE_TEXT_MULTI )
    {
      StringList::const_iterator it = m_values.begin();
      for( ; it != m_values.end() ; ++it )
      {
        field->addChild( new Tag( "value", (*it) ) );
      }
    }
    else if( !m_value.empty() )
      field->addChild( new Tag( "value", m_value ) );

    return field;
  }

}

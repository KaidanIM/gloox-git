/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "dataformfield.h"
#include "dataformbase.h"
#include "util.h"
#include "tag.h"

namespace gloox
{

  static const char * fieldTypeValues[] =
  {
    "boolean", "fixed", "hidden", "jid-multi", "jid-single",
    "list-multi", "list-single", "text-multi", "text-private", "text-single"
  };

  DataFormField::DataFormField( DataFormFieldType type )
    : m_type( type ), m_required( false )
  {
  }

  DataFormField::DataFormField( const std::string& name, const std::string& value,
                                const std::string& label, DataFormFieldType type )
    : m_name( name ), m_label( label ), m_type( type ), m_required( false )
  {
    m_values.push_back( value );
  }

  DataFormField::DataFormField( Tag *tag )
    : m_type( FieldTypeInvalid ), m_required( false )
  {
    if( !tag )
      return;

    const std::string& type = tag->findAttribute( "type" );
    if( type.empty() )
    {
      if( !tag->name().empty() )
        m_type = FieldTypeNone;
    }
    else
      m_type = (DataFormFieldType)util::lookup( type, fieldTypeValues );

    if( tag->hasAttribute( "var" ) )
      m_name = tag->findAttribute( "var" );

    if( tag->hasAttribute( "label" ) )
      m_label = tag->findAttribute( "label" );

    const Tag::TagList& l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      if( (*it)->name() == "desc" )
        m_desc = (*it)->cdata();
      else if( (*it)->name() == "required" )
        m_required = true;
      else if( (*it)->name() == "value" )
      {
        if( m_type == FieldTypeTextMulti || m_type == FieldTypeListMulti || m_type == FieldTypeJidMulti )
          addValue( (*it)->cdata() );
        else
          setValue( (*it)->cdata() );
      }
      else if( (*it)->name() == "option" )
      {
        Tag *v = (*it)->findChild( "value" );
        if( v )
          m_options[(*it)->findAttribute( "label" )] = v->cdata();
      }
    }

  }

  DataFormField::~DataFormField()
  {
  }

  Tag* DataFormField::tag() const
  {
    if( m_type == FieldTypeInvalid )
      return 0;

    Tag *field = new Tag( "field" );
    field->addAttribute( "type", util::lookup( m_type, fieldTypeValues ) );
    field->addAttribute( "var", m_name );
    field->addAttribute( "label", m_label );
    if( m_required )
      new Tag( field, "required" );

    if( !m_desc.empty() )
      new Tag( field, "desc", m_desc );

    if( m_type == FieldTypeListSingle || m_type == FieldTypeListMulti )
    {
      StringMap::const_iterator it = m_options.begin();
      for( ; it != m_options.end(); ++it )
      {
        Tag *option = new Tag( field, "option" );
        option->addAttribute( "label", (*it).first );
        new Tag( option, "value", (*it).second );
      }
    }
    else if( m_type == FieldTypeBoolean )
    {
      if( m_values.size() == 0 || m_values.front() == "false" || m_values.front() == "0" )
        new Tag( field, "value", "0" );
      else
        new Tag( field, "value", "1" );
    }

    if( m_type == FieldTypeTextMulti || m_type == FieldTypeListMulti || m_type == FieldTypeJidMulti )
    {
      StringList::const_iterator it = m_values.begin();
      for( ; it != m_values.end() ; ++it )
        new Tag( field, "value", (*it) );
    }

    if( m_values.size() && !( m_type == FieldTypeTextMulti || m_type == FieldTypeListMulti
                               || m_type == FieldTypeBoolean || m_type == FieldTypeListSingle
                               || m_type == FieldTypeJidMulti ) )
      new Tag( field, "value", m_values.front() );

    return field;
  }

}

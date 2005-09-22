/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/

#include "dataformfield.h"

namespace gloox
{

  DataFormField::DataFormField( DataFormFieldType type )
  : m_type( type )
  {

  }

  DataFormField::~DataFormField()
  {

  }

  void DataFormField::addOptions( const StringMap& options )
  {
    StringMap::const_iterator it = options.begin();
    for( it; it != options.end(); ++it )
    {
      m_options[(*it).first] = (*it).second;
    }
  }

  Tag* DataFormField::tag()
  {
    Tag *field = new Tag( "field" );
    field->addAttrib( "xmlns", XMLNS_DATA_FORMS );
    field->addAttrib( "var", m_fieldName );
    field->addAttrib( "label", m_label );
    if( m_required )
      field->addChild( new Tag( "required" ) );
    if( !m_desc.empty() )
      field->addChild( new Tag( "desc", m_desc ) );
    if( !m_value.empty() )
      field->addChild( new Tag( "value", m_value ) );

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
    }
  }

};

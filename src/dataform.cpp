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

  DataForm::DataForm( DataFormType type, const std::string& element, const std::string& title,
                      const std::string& instructions )
    : m_formType( type ), m_element( element ), m_title( title ), m_instructions( instructions )
  {
  }

  DataForm::~DataForm()
  {
  }

  Tag* DataForm::tag()
  {
    Tag *x = new Tag( m_element );
    if( !m_title.empty() )
      x->addChild( new Tag( "title", m_title ) );
    if( !m_instructions.empty() )
      x->addChild( new Tag( "instructions", m_instructions ) );

    FieldList::const_iterator it = m_fields.begin();
    for( it; it != m_fields.end(); ++it )
    {
      x->addChild( (*it)->tag() );
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

};

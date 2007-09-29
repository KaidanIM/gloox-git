/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "dataformfieldcontainer.h"
#include "util.h"


namespace gloox
{

  namespace DataForm
  {

    FieldContainer::FieldContainer()
    {
    }

    FieldContainer::~FieldContainer()
    {
      util::clear( m_fields );
    }

    Field* FieldContainer::field( const std::string& field ) const
    {
      FieldList::const_iterator it = m_fields.begin();
      for( ; it != m_fields.end() && (*it)->name() != field; ++it )
        ;
      return it != m_fields.end() ? (*it) : 0;
    }

  }

}

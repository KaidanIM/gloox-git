/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "dataformbase.h"
#include "dataformitem.h"
#include "dataformreported.h"


namespace gloox
{

  DataFormBase::DataFormBase()
  {
  }

  DataFormBase::~DataFormBase()
  {
    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
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

      delete (*it);
    }
  }

  bool DataFormBase::hasField( const std::string& field )
  {
    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
      if( (*it)->name() == field )
        return true;
    }

    return false;
  }

  DataFormField* DataFormBase::field( const std::string& field )
  {
    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end(); ++it )
    {
      if( (*it)->name() == field )
        return (*it);
    }

    return 0;
  }

}

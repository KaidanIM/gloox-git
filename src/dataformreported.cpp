 /*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "dataformreported.h"

#include "tag.h"

namespace gloox
{

  DataFormReported::DataFormReported()
    : DataFormField( FIELD_TYPE_REPORTED )
  {
  }

  DataFormReported::DataFormReported( Tag* tag )
    : DataFormField( FIELD_TYPE_REPORTED )
  {
    if( tag->name() != "reported" )
      return;

    Tag::TagList &l = tag->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      DataFormField f( (*it) );
      m_fields.push_back( f );
    }
  }

  DataFormReported::~DataFormReported()
  {
  }

}

/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "oob.h"
#include "tag.h"

namespace gloox
{

  OOB::OOB()
  {
  }

  OOB::OOB( Tag *tag )
  {
    if( tag && tag->name() == "x" && tag->hasAttribute( "xmlns", XMLNS_X_OOB ) )
    {
      if( tag->hasChild( "url" ) )
        m_url = tag->findChild( "url" )->cdata();
      if( tag->hasChild( "desc" ) )
        m_url = tag->findChild( "desc" )->cdata();
    }
  }

  OOB::~OOB()
  {
  }

}

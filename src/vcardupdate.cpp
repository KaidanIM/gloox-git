/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "vcardupdate.h"
#include "tag.h"

namespace gloox
{

  VCardUpdate::VCardUpdate()
    : StanzaExtension( ExtVCardUpdate ),
      m_notReady( false ), m_noImage( false )
  {
  }

  VCardUpdate::VCardUpdate( Tag *tag )
    : StanzaExtension( ExtVCardUpdate )
  {
    if( tag && tag->name() == "x" && tag->hasAttribute( "xmlns", XMLNS_X_VCARD_UPDATE ) )
    {
      if( tag->hasChild( "photo" ) )
      {
        m_hash = tag->findChild( "photo" )->cdata();
        if( m_hash.empty() )
          m_noImage = true;
      }
      else
        m_notReady = true;
    }
  }

  VCardUpdate::~VCardUpdate()
  {
  }

  Tag* VCardUpdate::tag() const
  {
    Tag *x = new Tag( "x" );
    x->addAttribute( "xmlns", XMLNS_X_VCARD_UPDATE );
    if( m_notReady )
      return x;

    Tag *p = new Tag( x, "photo" );
    if( m_noImage )
      return x;

    p->setCData( m_hash );

    return x;
  }

}

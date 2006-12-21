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

  OOB::OOB( const std::string& xmlns )
    : StanzaExtension( ExtOOB ), m_xmlns( xmlns )
  {
  }

  OOB::OOB( Tag *tag )
    : StanzaExtension( ExtOOB )
  {
    if( tag &&
        ( ( tag->name() == "x" && tag->hasAttribute( "xmlns", XMLNS_X_OOB ) ) ||
        ( tag->name() == "query" && tag->hasAttribute( "xmlns", XMLNS_IQ_OOB ) ) ) )
    {
      if( tag->hasChild( "url" ) )
        m_url = tag->findChild( "url" )->cdata();
      if( tag->hasChild( "desc" ) )
        m_desc = tag->findChild( "desc" )->cdata();
      m_xmlns = tag->findAttribute( "xmlns" );
    }
  }

  OOB::~OOB()
  {
  }

  Tag* OOB::tag() const
  {
    Tag *t = 0;

    if( m_xmlns == XMLNS_X_OOB && !m_url.empty() )
    {
      t = new Tag( "x" );
      t->addAttribute( "xmlns", XMLNS_X_OOB );
    }
    else if( m_xmlns == XMLNS_IQ_OOB && !m_url.empty() )
    {
      t = new Tag( "query" );
      t->addAttribute( "xmlns", XMLNS_IQ_OOB );
    }
    else
      return 0;

    new Tag( t, "url", m_url );
    if( !m_desc.empty() )
      new Tag( t, "desc", m_desc );

    return t;
  }

}

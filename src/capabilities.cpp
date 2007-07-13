/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "capabilities.h"

#include "tag.h"

namespace gloox
{

  Capabilities::Capabilities( const std::string& node, const std::string& version, const std::string& ext )
    : StanzaExtension( ExtCaps ), m_node( node ), m_version( version ), m_ext( ext ), m_valid( false )
  {
    if( !m_node.empty() && !m_version.empty() )
      m_valid = true;
  }


  Capabilities::Capabilities( Tag *tag )
    : StanzaExtension( ExtCaps ), m_valid( false )
  {
    if( !tag || tag->name() != "c" || !tag->hasAttribute( XMLNS, XMLNS_CAPS )
        || !tag->hasAttribute( "node" ) || !tag->hasAttribute( "ver" ) )
      return;

    m_node = tag->findAttribute( "node" );
    m_version = tag->findAttribute( "ver" );
    m_ext = tag->findAttribute( "ext" );
    m_valid = true;
  }

  Capabilities::~Capabilities()
  {
  }

  Tag* Capabilities::tag() const
  {
    if( !m_valid || m_node.empty() || m_version.empty() )
      return 0;

    Tag *t = new Tag( "c" );
    t->addAttribute( XMLNS, XMLNS_CAPS );
    t->addAttribute( "node", m_node );
    t->addAttribute( "ver", m_version );
    t->addAttribute( "ext", m_ext );
    return t;
  }

}

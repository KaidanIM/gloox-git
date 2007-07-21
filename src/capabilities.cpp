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

#include "base64.h"
#include "disco.h"
#include "sha.h"
#include "tag.h"

namespace gloox
{

  Capabilities::Capabilities( Disco* disco, const std::string& node )
    : StanzaExtension( ExtCaps ), m_disco( disco ), m_node( node ), m_valid( false )
  {
    if( !m_node.empty() && !m_ver.empty() )
      m_valid = true;
  }


  Capabilities::Capabilities( Tag *tag )
    : StanzaExtension( ExtCaps ), m_disco( 0 ), m_valid( false )
  {
    if( !tag || tag->name() != "c" || !tag->hasAttribute( XMLNS, XMLNS_CAPS )
        || !tag->hasAttribute( "node" ) || !tag->hasAttribute( "ver" ) )
      return;

    m_node = tag->findAttribute( "node" );
    m_ver = tag->findAttribute( "ver" );
    m_valid = true;
  }

  Capabilities::~Capabilities()
  {
  }

  const std::string Capabilities::ver() const
  {
    if( m_disco )
    {
      std::string s;
      s += m_disco->category();
      s += '/';
      s += m_disco->type();
      s += '<';
      StringList f = m_disco->features();
      f.sort();
      StringList::const_iterator it = f.begin();
      for( ; it != f.end(); ++it )
      {
        s += (*it);
        s += '<';
      }
      SHA sha;
      sha.feed( s );
      return Base64::encode64( sha.binary() );
    }
    else
      return m_ver;
  }

  Tag* Capabilities::tag() const
  {
    if( !m_valid || m_node.empty() )
      return 0;

    Tag *t = new Tag( "c" );
    t->addAttribute( XMLNS, XMLNS_CAPS );
    t->addAttribute( "node", m_node );
    t->addAttribute( "ver", ver() );
    return t;
  }

}

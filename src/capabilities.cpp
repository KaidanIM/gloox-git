/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
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

  Capabilities::Capabilities( Disco* disco )
    : StanzaExtension( ExtCaps ), m_disco( disco ), m_node( GLOOX_CAPS_NODE ), m_valid( false )
  {
    if( m_disco )
      m_valid = true;
  }

  Capabilities::Capabilities( const Tag* tag )
    : StanzaExtension( ExtCaps ), m_disco( 0 ), m_valid( false )
  {
    if( !tag || tag->name() != "c" || !tag->hasAttribute( XMLNS, XMLNS_CAPS )
        || !tag->hasAttribute( "node" ) || !tag->hasAttribute( "ver" ) )
      return;

    m_node = tag->findAttribute( "node" );
    m_ver = tag->findAttribute( "ver" );
    m_n = tag->findAttribute( "n" );
    m_v = tag->findAttribute( "v" );
    m_os = tag->findAttribute( "os" );
    m_valid = true;
  }

  Capabilities::~Capabilities()
  {
    if( m_disco )
      m_disco->removeNodeHandlers( const_cast<Capabilities*>( this ) );
  }

  const std::string Capabilities::ver() const
  {
    if( !m_disco )
      return m_ver;

    StringList sl;
    const Disco::IdentityList& il = m_disco->identities();
    Disco::IdentityList::const_iterator it = il.begin();
    for( ; it != il.end(); ++it )
      sl.push_back( (*it)->category() + '/' + (*it)->type() );
    sl.sort();

    std::string s;
    StringList::const_iterator it2 = sl.begin();
    for( ; it2 != sl.end(); ++it2 )
    {
      s += (*it2);
      s += '<';
    }

    StringList f = m_disco->features();
    f.sort();
    it2 = f.begin();
    for( ; it2 != f.end(); ++it2 )
    {
      s += (*it2);
      s += '<';
    }
    SHA sha;
    sha.feed( s );
    const std::string& hash = Base64::encode64( sha.binary() );
    m_disco->registerNodeHandler( const_cast<Capabilities*>( this ), m_node + '#' + hash );
    return hash;
  }
  const std::string& Capabilities::n() const
  {
    if( m_disco )
      return m_disco->name();
    else
      return m_n;
  }

  const std::string& Capabilities::v() const
  {
    if( m_disco )
      return m_disco->version();
    else
      return m_v;
  }

  const std::string& Capabilities::os() const
  {
    if( m_disco )
      return m_disco->os();
    else
      return m_os;
  }

  const std::string& Capabilities::filterString() const
  {
    static const std::string filter = "/presence/c[@xmlns='" + XMLNS_CAPS + "']";
    return filter;
  }

  Tag* Capabilities::tag() const
  {
    if( !m_valid || m_node.empty() )
      return 0;

    Tag* t = new Tag( "c" );
    t->setXmlns( XMLNS_CAPS );
    t->addAttribute( "hash", "sha-1" );
    t->addAttribute( "node", m_node );
    t->addAttribute( "ver", ver() );
    if( m_disco )
    {
      t->addAttribute( "n", m_disco->name() );
      t->addAttribute( "v", m_disco->version() );
      t->addAttribute( "os", m_disco->os() );
    }
    return t;
  }

  StringList Capabilities::handleDiscoNodeFeatures( const JID&, const std::string& )
  {
    return m_disco->features();
  }

  Disco::IdentityList Capabilities::handleDiscoNodeIdentities( const JID&, const std::string& )
  {
    return m_disco->identities();
  }

  Disco::ItemList Capabilities::handleDiscoNodeItems( const JID&, const std::string& )
  {
    return Disco::ItemList();
  }

}

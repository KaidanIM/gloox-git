/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "stanza.h"
#include "error.h"
#include "jid.h"
#include "util.h"
#include "stanzaextension.h"
#include "stanzaextensionfactory.h"

#include <cstdlib>

namespace gloox
{

  Stanza::Stanza( const std::string& name, const JID& to, const JID& from )
    : Tag( name ), m_xmllang( "default" ), m_from( from ), m_to( to )
  {
    if( m_to )
      addAttribute( "to", m_to.full() );
    if( m_from )
      addAttribute( "from", m_from.full() );
  }

  Stanza::Stanza( Tag* tag )
    : Tag( tag ), m_xmllang( "default" )
  {
    m_from.setJID( findAttribute( "from" ) );
    m_to.setJID( findAttribute( "to" ) );
    m_id = findAttribute( "id" );
  }

  Stanza::~Stanza()
  {
    util::clearList( m_extensionList );
  }

  const Error* Stanza::error() const
  {
    return static_cast< const Error* >( findExtension( ExtError ) );
  }

  void Stanza::addExtension( StanzaExtension* se )
  {
    m_extensionList.push_back( se );
//     addChild( se->tag() );
  }

  const StanzaExtension* Stanza::findExtension( int type ) const
  {
    StanzaExtensionList::const_iterator it = m_extensionList.begin();
    for( ; it != m_extensionList.end() && (*it)->extensionType() != type; ++it ) ;
    return it != m_extensionList.end() ? (*it) : 0;
  }

  void Stanza::setLang( StringMap& map, const Tag* tag )
  {
    const std::string& lang = tag->findAttribute( "xml:lang" );
    map[ lang.empty() ? "default" : lang ] = tag->cdata();
  }

  const std::string Stanza::findLang( const StringMap& map, const std::string& lang )
  {
    StringMap::const_iterator it = map.find( lang );
    return ( it != map.end() ) ? (*it).second : std::string();
  }

}

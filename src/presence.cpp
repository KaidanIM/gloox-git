/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "presence.h"
#include "util.h"

#include <cmath>
#include <algorithm>
#include <sstream>

namespace gloox
{

  static const char* msgTypeStringValues[] =
  {
    "available", "", "", "", "", "unavailable", "probe", "error"
  };

  static inline const std::string typeString( Presence::PresenceType type )
  {
    return util::lookup( type, msgTypeStringValues );
  }

  static const char* msgShowStringValues[] =
  {
    "", "chat", "away", "dnd", "xa", "", "", ""
  };

  static inline const std::string showString( Presence::PresenceType type )
  {
    return util::lookup( type, msgShowStringValues );
  }

  Presence::Presence( Tag* tag )
    : Stanza( tag ), m_subtype( Invalid ), m_stati( 0 ), m_priority( 0 )
  {
    if( !tag || tag->name() != "presence" )
      return;

    const std::string& type = tag->findAttribute( TYPE );
    if( type.empty() )
      m_subtype = Available;
    else
      m_subtype = (PresenceType)util::lookup( type, msgTypeStringValues );

    if( m_subtype == Available )
    {
      Tag* t = tag->findChild( "show" );
      if( t )
        m_subtype = (PresenceType)util::lookup( t->cdata(), msgShowStringValues );
    }

    const TagList& c = tag->children();
    TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "status" )
        setLang( &m_stati, m_status, (*it) );
      else if( (*it)->name() == "priority" )
        m_priority = atoi( (*it)->cdata().c_str() );
    }
  }

  Presence::Presence( PresenceType type, const JID& to, const std::string& status,
                      int priority, const std::string& xmllang, const JID& from )
    : Stanza( to, from ), m_subtype( type ), m_stati( 0 )
  {
    setLang( &m_stati, m_status, status, xmllang );

    if( priority < -128 )
      m_priority = -128;
    else if( priority > 127 )
      m_priority = 127;
    else
      m_priority = priority;
  }

  Presence::~Presence()
  {
    delete m_stati;
  }

  Tag* Presence::tag() const
  {
    if( m_subtype == Invalid )
      return 0;

    Tag* t = new Tag( "presence" );
    if( m_to )
      t->addAttribute( "to", m_to.full() );
    if( m_from )
      t->addAttribute( "from", m_from.full() );

    const std::string& type = typeString( m_subtype );
    if( !type.empty() )
    {
      if( type != "available" )
        t->addAttribute( "type", type );
    }
    else
    {
      const std::string& show = showString( m_subtype );
      if( !show.empty() )
        new Tag( t, "show", show );
    }

//     const int len = 4 + (int)std::log10( m_priority ? m_priority : 1 ) + 1;
//     char* tmp = new char[len];
//     sprintf( tmp, "%d", m_priority );
//     std::string ret( tmp, len );
//     new Tag( t, "priority", ret );
//     delete[] tmp;

    std::ostringstream oss;
    oss << m_priority;
    new Tag( t, "priority", oss.str() );

    getLangs( m_stati, m_status, "status", t );

    StanzaExtensionList::const_iterator it = m_extensionList.begin();
    for( ; it != m_extensionList.end(); ++it )
      t->addChild( (*it)->tag() );

    return t;
  }

}

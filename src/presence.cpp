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

namespace gloox
{

  static const char * msgTypeStringValues[] =
  {
    "available", "", "", "", "", "unavailable", "probe", "error"
  };

  static inline const char * typeString( Presence::PresenceType type )
    { return msgTypeStringValues[type]; }

  static const char * msgShowStringValues[] =
  {
    "", "chat", "away", "dnd", "xa", "", "", ""
  };

  static inline const char * showString( Presence::PresenceType type )
  { return msgShowStringValues[type]; }

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
#warning CHECKME write unit test for the Available types
      Tag* t = tag->findChild( "show" );
      if( t )
        m_subtype = (PresenceType)util::lookup( t->cdata(), msgShowStringValues );
    }

    const TagList& c = tag->children();
    TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "status" )
        setLang( m_stati, m_status, (*it) );
      else if( (*it)->name() == "priority" )
        m_priority = atoi( (*it)->cdata().c_str() );
    }
  }

  Presence::Presence( PresenceType type, const JID& to, const std::string& status,
                      int priority, const std::string& xmllang, const JID& from )
    : Stanza( to, from ), m_subtype( type ), m_stati( 0 )
  {
    setLang( m_stati, m_status, status, xmllang );

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
#warning FIXME implement!
    Tag* t = new Tag( "presence" );
    if( m_to )
      t->addAttribute( "to", m_to.full() );
    if( m_from )
      t->addAttribute( "from", m_from.full() );
    return t;
  }

}

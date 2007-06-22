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

  static inline const std::string& showString( Presence::PresenceType type )
  { return msgShowStringValues[type]; }

  Presence::Presence( Tag *tag, bool rip )
    : Stanza( tag, rip ), m_subtype( PresenceInvalid ), m_priority( 0 )
  {
    if( !tag || tag->name() != "presence" )
    {
      m_name = "";
      return;
    }

    m_type = StanzaPresence;
    const std::string& type = findAttribute( "type" );
    if( type.empty() )
      m_subtype = PresenceAvailable;
    else
      m_subtype = (PresenceType)util::lookup( type, msgTypeStringValues,
                               sizeof( msgTypeStringValues ) / sizeof(char*) );
    printf("parsed '%s'\n", msgTypeStringValues[m_subtype]);

    if( m_subtype == PresenceAvailable )
    {
      Tag* t = findChild( "show" );
      if( t )
        m_subtype = (PresenceType)util::lookup( t->cdata(), msgShowStringValues,
                              sizeof( msgShowStringValues ) / sizeof( char * ) );
      printf("parsed '%s'\n", msgShowStringValues[m_subtype]);
    }

    const TagList& c = children();
    TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "status" )
      {
        setLang( m_status, (*it) );
      }
      else if( (*it)->name() == "priority" )
      {
        m_priority = atoi( (*it)->cdata().c_str() );
      }
    }
  }

  Presence::Presence( PresenceType type, const JID& to, const std::string& status,
                      int priority, const std::string& xmllang, const JID& from )
    : Stanza( "presence", to, from ), m_subtype( PresenceInvalid )
  {
    addAttribute( "type", typeString( type ) );

    const std::string& show = showString( type );
    if( !show.empty() )
      new Tag( this, "show", show );

    if( !status.empty() )
    {
      Tag *t = new Tag( this, "status", status );
      t->addAttribute( "xml:lang", xmllang );
    }

    if( priority < -128 )
      m_priority = -128;
    else if( priority > 127 )
      m_priority = 127;
    else
      m_priority = priority;

    if( type != PresenceUnavailable )
    {
      char tmp[5];
      tmp[4] = '\0';
      sprintf( tmp, "%d", m_priority );
      new Tag( this, "priority", tmp );
    }
  }

  Presence::~Presence()
  {
  }

}

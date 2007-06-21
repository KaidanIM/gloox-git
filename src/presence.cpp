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

namespace gloox
{

  static const std::string msgTypeStringValues[] =
  {
    "available", "available", "available", "available", "available", "unavailable", "probe", "error"
  };

  static inline const std::string& typeString( Presence::PresenceType type )
    { return msgTypeStringValues[type-1]; }

  static const std::string msgShowStringValues[] =
  {
    "", "chat", "away", "dnd", "xa", "", "", ""
  };

  static inline const std::string& showString( Presence::PresenceType type )
  { return msgShowStringValues[type-1]; }

  Presence::Presence( Tag *tag, bool rip )
    : Stanza( tag, rip ), m_subtype( PresenceInvalid ), m_priority( 0 )
  {
    if( !tag || tag->name() != "presence" )
    {
      m_name = "";
      return;
    }

    m_type = StanzaPresence;

//     m_subtype = lookup( findAttribute( "type" ), msgTypeStringValues, sizeof( msgTypeStringValues ) );
    const std::string& type = findAttribute( "type" );
    if( type.empty() || type == "available" )
    {
      m_subtype = PresenceAvailable;
      Tag* t = findChild( "show" );
      if( t )
      {
//       m_subtype = _lookup( show, msgShowStringValues, sizeof( msgShowStringValues );
        const std::string& show = t->cdata();
        if( show == "chat" )
          m_subtype = PresenceChat;
        else if( show == "away" )
          m_subtype = PresenceAway;
        else if( show == "dnd" )
          m_subtype = PresenceDnd;
        else if( show == "xa" )
          m_subtype = PresenceXa;
      }
    }
    else if( type == "unavailable" )
      m_subtype = PresenceUnavailable;
    else if( type == "error" )
      m_subtype = PresenceError;
    else if( type == "probe" )
      m_subtype = PresenceProbe;

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

  Presence::Presence( PresenceType type, const std::string& to, const std::string& status,
                      int priority, const std::string& xmllang, const std::string& from )
    : Stanza( "presence" ), m_subtype( PresenceInvalid )
  {
    addAttribute( "type", typeString( type ) );

    addAttribute( "to", to );
    m_to.setJID( to );
    if( !from.empty() )
    {
      addAttribute( "from", from );
      m_from.setJID( from );
    }

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

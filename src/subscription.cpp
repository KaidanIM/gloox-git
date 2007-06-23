/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "subscription.h"
#include "util.h"

namespace gloox
{

  static const char* msgTypeStringValues[] =
  {
    "subscribe", "subscribed", "unsubscribe", "unsubscribed"
  };

  static inline const char* typeString( Subscription::S10nType type )
    { return msgTypeStringValues[type]; }

  Subscription::Subscription( Tag *tag, bool rip )
    : Stanza( tag, rip ), m_subtype( Invalid )
  {
    if( !tag || tag->name() != "presence" )
    {
      m_name = "";
      return;
    }

    m_type = StanzaS10n;
    m_subtype = (S10nType)util::lookup( findAttribute( "type" ), msgTypeStringValues );

    const TagList& c = children();
    TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "status" )
      {
        setLang( m_status, (*it) );
      }
    }
  }

  Subscription::Subscription( S10nType type, const JID& to, const std::string& status,
                              const std::string& xmllang, const JID& from )
    : Stanza( "presence", to, from ), m_subtype( type )
  {
    addAttribute( "type", typeString( type ) );

    if( !status.empty() )
    {
      Tag *t = new Tag( this, "status", status );
      t->addAttribute( "xml:lang", xmllang );
    }

  }

  Subscription::~Subscription()
  {
  }

}

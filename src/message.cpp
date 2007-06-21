/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "message.h"
#include "stanzaextension.h"
#include "stanzaextensionfactory.h"

namespace gloox
{

  static const std::string msgTypeStringValues[] =
  {
    "chat", "error", "groupchat", "headline", "normal"
  };

  static inline const std::string& typeString( Message::MessageType type )
    { return msgTypeStringValues[type-1]; }

  Message::Message( Tag *tag )
    : Stanza( tag ), m_subtype( MessageInvalid )
  {
    if( !tag || tag->name() != "message" )
    {
      m_name = "";
      return;
    }

    m_type = StanzaMessage;

    if( hasAttribute( "type", "chat" ) )
      m_subtype = MessageChat;
    else if( hasAttribute( "type", "error" ) )
      m_subtype = MessageError;
    else if( hasAttribute( "type", "headline" ) )
      m_subtype = MessageHeadline;
    else if( hasAttribute( "type", "groupchat" ) )
      m_subtype = MessageGroupchat;
    else
      m_subtype = MessageNormal;

    const TagList& c = children();
    TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "body" )
      {
        setLang( m_body, (*it) );
      }
      else if( (*it)->name() == "subject" )
      {
        setLang( m_subject, (*it) );
      }
      else if( (*it)->name() == "thread" )
      {
        m_thread = (*it)->cdata();
      }
      else
      {
        StanzaExtension *se = StanzaExtensionFactory::create( (*it) );
        if( se )
          m_extensionList.push_back( se );
      }
    }
  }

  Message::Message( MessageType type, const std::string& id, const std::string& to,
                    const std::string& body, const std::string& thread,
                    const std::string& xmllang, const std::string& subject, const std::string& from )
    : Stanza( "message" ), m_subtype( MessageInvalid )
  {
    addAttribute( "type", typeString( type ) );
    addAttribute( "id", id );
    m_id = id;
    addAttribute( "to", to );
    m_to.setJID( to );
    if( !from.empty() )
    {
      addAttribute( "from", from );
      m_from.setJID( from );
    }

    if( !body.empty() )
    {
      Tag *t = new Tag( this, "body", body );
      t->addAttribute( "xml:lang", xmllang );
    }

    if( !subject.empty() )
    {
      Tag *t = new Tag( this, "subject", subject );
      t->addAttribute( "xml:lang", xmllang );
    }

    if( !thread.empty() )
      new Tag( this, "thread", thread );
  }

  Message::~Message()
  {
  }

}

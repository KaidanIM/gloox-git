/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "util.h"
#include "message.h"

namespace gloox
{

  static const char * msgTypeStringValues[] =
  {
    "chat", "error", "groupchat", "headline", "normal"
  };

  Message::Message( Tag* tag )
    : Stanza( tag ), m_subtype( Invalid ), m_bodies( 0 ), m_subjects( 0 )
  {
    if( !tag || tag->name() != "message" )
    {
      m_name = EmptyString;
      return;
    }

    const std::string& typestring = findAttribute( TYPE );
    if( typestring.empty() )
      m_subtype = Normal;
    else
      m_subtype = (MessageType)util::lookup2( typestring , msgTypeStringValues );

    const TagList& c = children();
    TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "body" )
      {
        const std::string& lang = (*it)->findAttribute( "xml:lang" );
        if( lang.empty() )
        {
          m_body = (*it)->cdata();
        }
        else
        {
          if( !m_bodies )
            m_bodies = new StringMap();
          (*m_bodies)[lang] = (*it)->cdata();
        }
      }
      else if( (*it)->name() == "subject" )
      {
        const std::string& lang = (*it)->findAttribute( "xml:lang" );
        if( lang.empty() )
        {
          m_subject = (*it)->cdata();
        }
        else
        {
          if( !m_subjects )
            m_subjects = new StringMap();
          (*m_subjects)[lang] = (*it)->cdata();
        }
      }
      else if( (*it)->name() == "thread" )
      {
        m_thread = (*it)->cdata();
      }
    }
  }

  Message::Message( MessageType type, const JID& to,
                    const std::string& body, const std::string& subject,
                    const std::string& thread, const std::string& xmllang, const JID& from )
    : Stanza( "message", to, from ), m_subtype( type ), m_bodies( 0 ), m_subjects( 0 ), m_thread( thread )
  {
    addAttribute( TYPE, util::lookup2( type, msgTypeStringValues ) );

    if( !body.empty() )
    {
      Tag* t = new Tag( this, "body", body );
      t->addAttribute( "xml:lang", xmllang );
      if( xmllang.empty() )
        m_body = body;
      else
      {
        m_bodies = new StringMap();
        (*m_bodies)[xmllang] = body;
      }
    }

    if( !subject.empty() )
    {
      Tag* t = new Tag( this, "subject", subject );
      t->addAttribute( "xml:lang", xmllang );
      if( xmllang.empty() )
        m_subject = subject;
      else
      {
        m_subjects = new StringMap();
        (*m_subjects)[xmllang] = subject;
      }
    }

    if( !thread.empty() )
      new Tag( this, "thread", thread );
  }

  Message::~Message()
  {
    delete m_bodies;
    delete m_subjects;
  }

  const std::string Message::body( const std::string& lang ) const
  {
    if( m_bodies && lang != "default" )
    {
      StringMap::const_iterator it = m_bodies->find( lang );
      if( it != m_bodies->end() )
        return (*it).second;
    }
    return m_body;
  }

  const std::string Message::subject( const std::string& lang ) const
  {
    if( m_subjects && lang != "default" )
    {
      StringMap::const_iterator it = m_subjects->find( lang );
      if( it != m_subjects->end() )
        return (*it).second;
    }
    return m_subject;
  }
}

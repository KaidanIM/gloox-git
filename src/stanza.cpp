/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This file is distributed under a license. The exact licensing terms
  can be found in the file LICENSE in the distribution.
*/


#include "stanza.h"
#include "jid.h"

namespace gloox
{

  Stanza::Stanza( const std::string& name, const std::string& cdata )
  : Tag( name, cdata )
  {
  }

  Stanza::Stanza( Tag *tag )
    : Tag( tag->name() ),
      m_show( PRESENCE_UNKNOWN )
  {
    m_name = tag->name();
    m_attribs = tag->attributes();
    m_cdata = tag->cdata();
    m_children = tag->children();
//     m_parent = tag->parent();
    init();
  }

  void Stanza::init()
  {
    m_from.setJID( findAttribute( "from" ) );
    m_to.setJID( findAttribute( "to" ) );
    m_id = findAttribute( "id" );

    if( m_name == "iq" )
    {
      m_type = STANZA_IQ;
      if( hasAttribute( "type", "get" ) )
        m_subtype = STANZA_IQ_GET;
      else if( hasAttribute( "type", "set" ) )
        m_subtype = STANZA_IQ_SET;
      else if( hasAttribute( "type", "result" ) )
        m_subtype = STANZA_IQ_RESULT;
      else if( hasAttribute( "type", "error" ) )
        m_subtype = STANZA_IQ_ERROR;
      else
        m_subtype = STANZA_SUB_UNDEFINED;

      Tag *t = findChildWithAttrib( "xmlns" );
      if( t )
        m_xmlns = t->findAttribute( "xmlns" );
    }
    else if( m_name == "message" )
    {
      m_type = STANZA_MESSAGE;
      if( hasAttribute( "type", "chat" ) )
        m_subtype = STANZA_MESSAGE_CHAT;
      else if( hasAttribute( "type", "error" ) )
        m_subtype = STANZA_MESSAGE_ERROR;
      else if( hasAttribute( "type", "headline" ) )
        m_subtype = STANZA_MESSAGE_HEADLINE;
      else if( hasAttribute( "type", "groupchat" ) )
        m_subtype = STANZA_MESSAGE_GROUPCHAT;
      else if( hasAttribute( "type", "normal" ) )
        m_subtype = STANZA_MESSAGE_NORMAL;
      else
        m_subtype = STANZA_SUB_UNDEFINED;

      Tag *t = findChild( "body" );
      if( t )
        m_message = t->cdata();
      t = findChild( "subject" );
      if( t )
        m_subject = t->cdata();
      t = findChild( "thread" );
      if( t )
        m_thread = t->cdata();
    }
    else if( m_name == "presence" )
    {
      if( hasAttribute( "type", "subscribe" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_SUBSCRIBE;
      }
      else if( hasAttribute( "type", "subscribed" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_SUBSCRIBED;
      }
      else if( hasAttribute( "type", "unsubscribe" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_UNSUBSCRIBE;
      }
      else if( hasAttribute( "type", "unsubscribed" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_UNSUBSCRIBED;
      }
      else if( hasAttribute( "type", "unavailable" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_UNAVAILABLE;
      }
      else if( hasAttribute( "type", "probe" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_PROBE;
      }
      else if( hasAttribute( "type", "error" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_ERROR;
      }
      else if( !hasAttribute( "type" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_AVAILABLE;
      }
      else
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_SUB_UNDEFINED;
      }
    }
    else
    {
      m_type = STANZA_UNDEFINED;
      m_subtype = STANZA_SUB_UNDEFINED;
    }

    if( m_type == STANZA_PRESENCE )
    {
      if( !hasChild( "show" ) )
        m_show = PRESENCE_AVAILABLE;
      else if( hasChildWithCData( "show", "chat" ) )
        m_show = PRESENCE_CHAT;
      else if( hasChildWithCData( "show", "away" ) )
        m_show = PRESENCE_AWAY;
      else if( hasChildWithCData( "show", "dnd" ) )
        m_show = PRESENCE_DND;
      else if( hasChildWithCData( "show", "xa" ) )
        m_show = PRESENCE_XA;
      else if( hasAttribute( "type", "unavailable" ) )
        m_show = PRESENCE_UNAVAILABLE;
      else
        m_show = PRESENCE_UNKNOWN;

      if( hasChild( "status" ) )
        m_status = findChild( "status" )->cdata();

      if( hasChild( "priority" ) )
        m_priority = atoi( findChild( "priority" )->cdata().c_str() );
    }
  }

};

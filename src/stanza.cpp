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

  Stanza::Stanza( const Tag *tag )
    : m_show( PRESENCE_UNKNOWN )
  {
    m_name = tag->name();
    m_attribs = ( const_cast<Tag*>( tag ) )->attributes();
    m_cdata = tag->cdata();
    m_children = ( const_cast<Tag*>( tag ) )->children();

    m_from.setJID( tag->findAttribute( "from" ) );
    m_to.setJID( tag->findAttribute( "to" ) );
    m_id = tag->findAttribute( "id" );

    if( m_name == "iq" )
    {
      m_type = STANZA_IQ;
      if( tag->hasAttribute( "type", "get" ) )
        m_subtype = STANZA_IQ_GET;
      else if( tag->hasAttribute( "type", "set" ) )
        m_subtype = STANZA_IQ_SET;
      else if( tag->hasAttribute( "type", "result" ) )
        m_subtype = STANZA_IQ_RESULT;
      else if( tag->hasAttribute( "type", "error" ) )
        m_subtype = STANZA_IQ_ERROR;
      else
        m_subtype = STANZA_SUB_UNDEFINED;

      Tag *t = findChildWithAttrib( "xmlns" );
      if( t )
        m_xmlns = findAttribute( "xmlns" );
    }
    else if( m_name == "message" )
    {
      m_type = STANZA_MESSAGE;
      if( tag->hasAttribute( "type", "chat" ) )
        m_subtype = STANZA_MESSAGE_CHAT;
      else if( tag->hasAttribute( "type", "error" ) )
        m_subtype = STANZA_MESSAGE_ERROR;
      else if( tag->hasAttribute( "type", "headline" ) )
        m_subtype = STANZA_MESSAGE_HEADLINE;
      else if( tag->hasAttribute( "type", "groupchat" ) )
        m_subtype = STANZA_MESSAGE_GROUPCHAT;
      else if( tag->hasAttribute( "type", "normal" ) )
        m_subtype = STANZA_MESSAGE_NORMAL;
      else
        m_subtype = STANZA_SUB_UNDEFINED;
    }
    else if( m_name == "presence" )
    {
      if( tag->hasAttribute( "type", "subscribe" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_SUBSCRIBE;
      }
      else if( tag->hasAttribute( "type", "subscribed" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_SUBSCRIBED;
      }
      else if( tag->hasAttribute( "type", "unsubscribe" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_UNSUBSCRIBE;
      }
      else if( tag->hasAttribute( "type", "unsubscribed" ) )
      {
        m_type = STANZA_S10N;
        m_subtype = STANZA_S10N_UNSUBSCRIBED;
      }
      else if( tag->hasAttribute( "type", "unavailable" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_UNAVAILABLE;
        m_show = PRESENCE_UNAVAILABLE;
      }
      else if( tag->hasAttribute( "type", "probe" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_PROBE;
      }
      else if( tag->hasAttribute( "type", "error" ) )
      {
        m_type = STANZA_PRESENCE;
        m_subtype = STANZA_PRES_ERROR;
      }
      else if( !tag->hasAttribute( "type" ) )
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
      else
        m_show = PRESENCE_UNKNOWN;

      if( hasChild( "status" ) )
        m_status = findChild( "status" )->cdata();

      if( hasChild( "priority" ) )
        m_priority = atoi( findChild( "priority" )->cdata().c_str() );
    }
  }

}

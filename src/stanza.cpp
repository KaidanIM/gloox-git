/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "stanza.h"
#include "jid.h"

namespace gloox
{

  Stanza::Stanza( const std::string& name, const std::string& cdata )
  : Tag( name, cdata ), m_xmllang( "default" ), m_stanzaError( ST_ERROR_UNDEFINED )
  {
  }

  Stanza::Stanza( Tag *tag )
    : Tag( tag->name() ),
      m_show( PRESENCE_UNKNOWN ), m_xmllang( "en" )
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

      TagList& c = children();
      TagList::const_iterator it = c.begin();
      for( it; it != c.end(); ++it )
      {
        if( (*it)->name() == "body" )
        {
          const std::string lang = (*it)->findAttribute( "xml:lang" );
          if( !lang.empty() )
            m_body[lang] = (*it)->cdata();
          else
            m_body["default"] = (*it)->cdata();
        }
        else if( (*it)->name() == "subject" )
        {
          const std::string lang = (*it)->findAttribute( "xml:lang" );
          if( !lang.empty() )
            m_subject[lang] = (*it)->cdata();
          else
            m_subject["default"] = (*it)->cdata();
        }
        else if( (*it)->name() == "thread" )
          m_thread = (*it)->cdata();
      }
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
      if( !hasAttribute( "type" ) )
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

      TagList& c = children();
      TagList::const_iterator it = c.begin();
      for( it; it != c.end(); ++it )
      {
        if( (*it)->name() == "status" )
        {
          const std::string lang = (*it)->findAttribute( "xml:lang" );
          if( !lang.empty() )
            m_status[lang] = (*it)->cdata();
          else
            m_status["default"] = (*it)->cdata();
        }
      }

      if( hasChild( "priority" ) )
        m_priority = atoi( findChild( "priority" )->cdata().c_str() );
    }

    m_xmllang = findAttribute( "xml:lang" );

    if( hasAttribute( "type", "error" ) )
    {
      Tag *e = findChild( "error" );

      if( e->hasAttribute( "type", "cancel" ) )
        m_stanzaErrorType = ST_TYPE_CANCEL;
      else if( e->hasAttribute( "type", "continue" ) )
        m_stanzaErrorType = ST_TYPE_CONTINUE;
      else if( e->hasAttribute( "type", "modify" ) )
        m_stanzaErrorType = ST_TYPE_MODIFY;
      else if( e->hasAttribute( "type", "auth" ) )
        m_stanzaErrorType = ST_TYPE_AUTH;
      else if( e->hasAttribute( "type", "wait" ) )
        m_stanzaErrorType = ST_TYPE_WAIT;

      if( e->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_BAD_REQUEST;
      else if( e->hasChild( "conflict", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_CONFLICT;
      else if( e->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_FEATURE_NOT_IMPLEMENTED;
      else if( e->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_FORBIDDEN;
      else if( e->hasChild( "gone", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_GONE;
      else if( e->hasChild( "internal-server-error", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_INTERNAL_SERVER_ERROR;
      else if( e->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_ITEM_NOT_FOUND;
      else if( e->hasChild( "jid-malformed", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_JID_MALFORMED;
      else if( e->hasChild( "not-acceptable", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_NOT_ACCEPTABLE;
      else if( e->hasChild( "not-allowed", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_NOT_ALLOWED;
      else if( e->hasChild( "not-authorized", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_NOT_AUTHORIZED;
      else if( e->hasChild( "recipient-unavailable", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_RECIPIENT_UNAVAILABLE;
      else if( e->hasChild( "redirect", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_REDIRECT;
      else if( e->hasChild( "registration-required", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_REGISTRATION_REQUIRED;
      else if( e->hasChild( "remote-server-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_REMOTE_SERVER_NOT_FOUND;
      else if( e->hasChild( "remote-server-timeout", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_REMOTE_SERVER_TIMEOUT;
      else if( e->hasChild( "resource-constraint", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_RESOURCE_CONSTRAINT;
      else if( e->hasChild( "service-unavailable", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_SERVICE_UNAVAILABLE;
      else if( e->hasChild( "subscription-required", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_SUBSCRIBTION_REQUIRED;
      else if( e->hasChild( "undefined-condition", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_UNDEFINED_CONDITION;
      else if( e->hasChild( "unexpected-request", "xmlns", XMLNS_XMPP_STANZAS ) )
        m_stanzaError = ST_ERROR_UNEXPECTED_REQUEST;

      TagList& c = e->children();
      TagList::const_iterator it = c.begin();
      for( it; it != c.end(); ++it )
      {
        if( (*it)->name() == "text" )
        {
          const std::string lang = (*it)->findAttribute( "xml:lang" );
          if( !lang.empty() )
            m_errorText[lang] = (*it)->cdata();
          else
            m_errorText["default"] = (*it)->cdata();
        }
      }

    }
  }

  const std::string Stanza::body( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_body.find( lang );
    if( it != m_body.end() )
      return (*it).second;
    else
      return "";
  }

  const std::string Stanza::subject( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_subject.find( lang );
    if( it != m_subject.end() )
      return (*it).second;
    else
      return "";
  }

  const std::string Stanza::status( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_status.find( lang );
    if( it != m_status.end() )
      return (*it).second;
    else
      return "";
  }

  const std::string Stanza::errorText( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_errorText.find( lang );
    if( it != m_errorText.end() )
      return (*it).second;
    else
      return "";
  }

};

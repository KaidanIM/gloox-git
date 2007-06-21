/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "iq.h"

namespace gloox
{

  static const std::string iqTypeStringValues[] =
  {
    "get", "set", "result", "error"
  };

  static inline const std::string& typeString( IQ::IqType type )
    { return iqTypeStringValues[type-1]; }

  IQ::IQ( Tag *tag, bool rip )
    : Stanza( tag, rip ), m_query( 0 ), m_subtype( IqTypeInvalid )
  {
    if( !tag || tag->name() != "iq" )
    {
      m_name = "";
      return;
    }

    m_type = StanzaIq;

    if( hasAttribute( "type", "get" ) )
      m_subtype = IqTypeGet;
    else if( hasAttribute( "type", "set" ) )
      m_subtype = IqTypeSet;
    else if( hasAttribute( "type", "result" ) )
      m_subtype = IqTypeResult;
    else if( hasAttribute( "type", "error" ) )
      m_subtype = IqTypeError;

    Tag *m_query = findChildWithAttrib( "xmlns" );
    if( m_query )
      m_xmlns = m_query->findAttribute( "xmlns" );
  }

  IQ::IQ( IqType type, const std::string& id, const std::string& to, const std::string& xmlns,
          const std::string& childtag, const std::string& from )
    : Stanza( "iq" ), m_query( 0 ), m_subtype( type )
  {
    m_type = StanzaIq;

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

    if( !xmlns.empty() )
    {
      m_xmlns = xmlns;
      m_query = new Tag( this, childtag.empty() ? "query" : childtag );
      m_query->addAttribute( "xmlns", xmlns );
    }
  }

  IQ::~IQ()
  {
  }

}

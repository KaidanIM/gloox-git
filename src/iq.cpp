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
#include "util.h"

namespace gloox
{

  static const char * iqTypeStringValues[] =
  {
    "get", "set", "result", "error"
  };

  static inline const char * typeString( IQ::IqType type )
    { return iqTypeStringValues[type]; }

  IQ::IQ( Tag* tag )
    : Stanza( tag ), m_query( 0 ), m_subtype( Invalid )
  {
    if( !tag || tag->name() != "iq" )
    {
      m_name = "";
      return;
    }

    m_subtype = ( IQ::IqType )util::lookup( findAttribute( TYPE ), iqTypeStringValues );

    m_query = findChildWithAttrib( XMLNS );
    if( m_query )
      m_xmlns = m_query->findAttribute( XMLNS );
  }

  IQ::IQ( IqType type, const JID& to, const std::string& id, const std::string& xmlns,
          const std::string& childtag, const JID& from )
    : Stanza( "iq", to, from ), m_query( 0 ), m_subtype( type )
  {
    addAttribute( TYPE, typeString( type ) );
    addAttribute( "id", id );
    m_id = id;

    if( !xmlns.empty() )
    {
      m_xmlns = xmlns;
      m_query = new Tag( this, childtag.empty() ? "query" : childtag,  XMLNS, xmlns );
    }
  }

  IQ::~IQ()
  {
  }

}

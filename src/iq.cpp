/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
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

  static inline const char* typeString( IQ::IqType type )
  {
    return iqTypeStringValues[type];
  }

  IQ::IQ( Tag* tag )
    : Stanza( tag ), m_query( 0 ), m_subtype( Invalid )
  {
    if( !tag || tag->name() != "iq" )
      return;

    m_subtype = (IQ::IqType)util::lookup( tag->findAttribute( TYPE ), iqTypeStringValues );

    m_query = tag->findChildWithAttrib( XMLNS );

    if( m_query )
      m_query = m_query->clone(); // FIXME this needs to be removed once all code is
                                  // ported to Tag-less Stanzas

    if( m_query )
      m_xmlns = m_query->findAttribute( XMLNS );
  }

  IQ::IQ( IqType type, const JID& to, const std::string& id )
    : Stanza( to ), m_query( 0 ), m_subtype( type )
  {
    m_id = id;
  }

  IQ::IQ( IqType type, const JID& to, const std::string& id, const std::string& xmlns,
          const std::string& childtag )
    : Stanza( to ), m_query( 0 ), m_subtype( type )
  {
    m_id = id;

    if( !xmlns.empty() ) // FIXME needs to be removed
    {
      m_xmlns = xmlns;
      m_query = new Tag( childtag.empty() ? "query" : childtag,  XMLNS, xmlns );
    }
  }

  IQ::~IQ()
  {
    delete m_query;
  }

  Tag* IQ::tag() const
  {
    if( m_subtype == Invalid )
      return 0;

    Tag* t = new Tag( "iq" );
    t->setXmlns( XMLNS_CLIENT );
    if( m_to )
      t->addAttribute( "to", m_to.full() );
    if( m_from )
      t->addAttribute( "from", m_from.full() );
    if( !m_id.empty() )
      t->addAttribute( "id", m_id );
    t->addAttribute( TYPE, typeString( m_subtype ) );

    if( m_query ) // FIXME this should be temporary
      t->addChild( m_query->clone() );

    StanzaExtensionList::const_iterator it = m_extensionList.begin();
    for( ; it != m_extensionList.end(); ++it )
      t->addChild( (*it)->tag() );

    return t;
  }

}

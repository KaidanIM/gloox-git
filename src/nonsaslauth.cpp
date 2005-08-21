/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/


#include "nonsaslauth.h"
#include "jclient.h"

#include <string>

#include <iksemel.h>

namespace gloox
{

  NonSaslAuth::NonSaslAuth( JClient *parent, const std::string& sid )
    : m_parent( parent ), m_sid( sid )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_AUTH );
  }

  NonSaslAuth::~NonSaslAuth()
  {
    if( m_parent )
      m_parent->removeIqHandler( XMLNS_AUTH );
  }

  void NonSaslAuth::doAuth()
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "to", m_parent->server() );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_AUTH );
    q.addChild( Tag( "username", m_parent->username() ) );
    iq.addChild( q );

    m_parent->send( iq );
  }

  void NonSaslAuth::handleIq( const Stanza& stanza )
  {
    switch( stanza.subtype() )
    {
      case STANZA_IQ_RESULT:
      {
        Tag iq( "iq" );
        std::string id = m_parent->getID();
        iq.addAttrib( "id", id );
        iq.addAttrib( "type", "set" );
        Tag query( "query" );
        query.addAttrib( "xmlns", XMLNS_AUTH );
        Tag u( "username", m_parent->jid().username() );
        query.addChild( u );

        Tag q = stanza.findChild( "query" );
        if( ( q.hasChild( "digest" ) ) && !m_sid.empty() )
        {
          const std::string pwd = m_parent->password();
          char buf[41];
          iksha *sha;
          sha = iks_sha_new();
          iks_sha_hash( sha, (const unsigned char*)m_sid.c_str(), m_sid.length(), 0 );
          iks_sha_hash( sha, (const unsigned char*)pwd.c_str(), pwd.length(), 1 );
          iks_sha_print( sha, buf );
          iks_sha_delete( sha );
          Tag d( "digest", buf );
          query.addChild( d );
        }
        else
        {
          Tag p( "password", m_parent->password() );
          query.addChild( p );
        }

        iq.addChild( query );
        m_parent->trackID( this, id, 0 );
        m_parent->send( iq );
        break;
      }

      case STANZA_IQ_ERROR:
      {
        m_parent->setState( STATE_AUTHENTICATION_FAILED );
#warning FIXME: More detail necessary?

  //       iks *ft = iks_child( iks_find( pak->x, "error" ) );
  //
  //       if( iks_strncmp( iks_name( ft ), "conflict", 8 ) == 0 )
  //         m_parent->setClientState( JClient::STATE_AUTHENTICATION_FAILED );
  //       else if( iks_strncmp( iks_name( ft ), "not-acceptable", 14 ) == 0 )
  //         m_parent->setClientState( JClient::STATE_AUTHENTICATION_FAILED );
  //       else if( iks_strncmp( iks_name( ft ), "not-authorized", 14 ) == 0 )
  //         m_parent->setClientState( JClient::STATE_AUTHENTICATION_FAILED );

        break;
      }
    }
  }

  void NonSaslAuth::handleIqID( const Stanza& stanza, int context )
  {
    // this needs fixing! NonSaslAuth shouldn't be a friend of JClient.
    m_parent->setState( STATE_AUTHENTICATED );
    m_parent->notifyOnConnect();
  }

};

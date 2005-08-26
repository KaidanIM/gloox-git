/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "nonsaslauth.h"
#include "client.h"

#include <string>

#include <iksemel.h>

namespace gloox
{

  NonSaslAuth::NonSaslAuth( Client *parent, const std::string& sid )
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

    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "to", m_parent->server() );
    iq->addAttrib( "id", id );
    iq->addAttrib( "type", "get" );
    Tag *q = new Tag( "query" );
    q->addAttrib( "xmlns", XMLNS_AUTH );
    q->addChild( new Tag( "username", m_parent->username() ) );
    iq->addChild( q );

    m_parent->send( iq );
  }

  bool NonSaslAuth::handleIq( Stanza *stanza )
  {
    switch( stanza->subtype() )
    {
      case STANZA_IQ_RESULT:
      {
        std::string id = m_parent->getID();

        Tag *iq = new Tag( "iq" );
        iq->addAttrib( "id", id );
        iq->addAttrib( "type", "set" );
        Tag *query = new Tag( "query" );
        query->addAttrib( "xmlns", XMLNS_AUTH );
        query->addChild( new Tag( "username", m_parent->jid().username() ) );
        query->addChild( new Tag( "resource", m_parent->jid().resource() ) );

        Tag *q = stanza->findChild( "query" );
        if( ( q->hasChild( "digest" ) ) && !m_sid.empty() )
        {
          const std::string pwd = m_parent->password();
          char buf[41];
          iksha *sha;
          sha = iks_sha_new();
          iks_sha_hash( sha, (const unsigned char*)m_sid.c_str(), m_sid.length(), 0 );
          iks_sha_hash( sha, (const unsigned char*)pwd.c_str(), pwd.length(), 1 );
          iks_sha_print( sha, buf );
          iks_sha_delete( sha );
          query->addChild( new Tag( "digest", buf ) );
        }
        else
        {
          query->addChild( new Tag( "password", m_parent->password() ) );
        }

        iq->addChild( query );
        m_parent->trackID( this, id, 0 );
        m_parent->send( iq );
        break;
      }

      case STANZA_IQ_ERROR:
      {
        m_parent->setAuthed( false );
#warning FIXME: More detail necessary?

  //       iks *ft = iks_child( iks_find( pak->x, "error" ) );
  //
  //       if( iks_strncmp( iks_name( ft ), "conflict", 8 ) == 0 )
  //         m_parent->setClientState( Client::STATE_AUTHENTICATION_FAILED );
  //       else if( iks_strncmp( iks_name( ft ), "not-acceptable", 14 ) == 0 )
  //         m_parent->setClientState( Client::STATE_AUTHENTICATION_FAILED );
  //       else if( iks_strncmp( iks_name( ft ), "not-authorized", 14 ) == 0 )
  //         m_parent->setClientState( Client::STATE_AUTHENTICATION_FAILED );

        break;
      }
    }
    return false;
  }

  bool NonSaslAuth::handleIqID( Stanza *stanza, int context )
  {
    switch( stanza->subtype() )
    {
      case STANZA_IQ_ERROR:
        m_parent->setAuthed( false );
        break;
      case STANZA_IQ_RESULT:
        m_parent->setAuthed( true );
        m_parent->notifyOnConnect();
      break;
    }
    return false;
  }

};

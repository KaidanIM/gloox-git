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

#include <iksemel.h>

namespace gloox
{

  NonSaslAuth::NonSaslAuth( JClient *parent, const string& sid )
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
    string id = m_parent->getID();
    iks *x = iks_make_iq( IKS_TYPE_GET, XMLNS_AUTH );
    iks_insert_attrib( x, "to", m_parent->server().c_str() );
    iks_insert_attrib( x, "id", id.c_str() );
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "username" ),
                      m_parent->username().c_str(), m_parent->username().length() );

    m_parent->send( x );
  }

  void NonSaslAuth::handleIq( const char *tag, const char *xmlns, ikspak *pak )
  {
    bool digest = false;
    switch( pak->subtype )
    {
      case IKS_TYPE_RESULT:
      {
        iks *x;

        if( iks_find( pak->query, "digest" ) && !m_sid.empty() )
        {
          //FIXME: write own 'iks_make'auth' to get rid of cast
          x = iks_make_auth( (iksid*)m_parent->parsedJid(), m_parent->password().c_str(), m_sid.c_str() );
        }
        else if( iks_find( pak->query, "password" ) )
        {
          //FIXME: write own 'iks_make'auth' to get rid of cast
          x = iks_make_auth( (iksid*)m_parent->parsedJid(), m_parent->password().c_str(), NULL );
        }

        string id = m_parent->getID();
        iks_insert_attrib( x, "id", id.c_str() );
        m_parent->trackID( this, id.c_str(), 0 );
        m_parent->send( x );

        break;
      }

      case IKS_TYPE_ERROR:
      {
        m_parent->setState( STATE_AUTHENTICATION_FAILED );
  #warning FIXME: More detail?

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

  void NonSaslAuth::handleIqID( const char *id, ikspak *pak, int context )
  {
    // this needs fixing! NonSaslAuth shouldn't be a friend of JClient.
    m_parent->setState( STATE_AUTHENTICATED );
    m_parent->notifyOnConnect();
  }

};

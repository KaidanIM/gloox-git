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



#include "component.h"

#include "disco.h"
#include "tag.h"

namespace gloox
{

  Component::Component( const std::string& ns, const std::string& server,
                          const std::string& component, const std::string& password, int port )
    : ClientBase( ns, password, server, port ),
      m_to( component ), m_disco( 0 )
  {
//     m_disco = new Disco( this );
//     m_disco->setVersion( "based on gloox", GLOOX_VERSION );
//     m_disco->setIdentity( "component", "generic" );
  }

  Component::~Component()
  {
//     delete m_disco;
  }

  void Component::handleStartNode()
  {
    printf( "in handleStartNode\n" );
    if( m_sid.empty() )
      return;

    const std::string data = m_sid + m_password;
    char *hash = (char*)calloc( 41, sizeof( char ) );
    iks_sha( data.c_str(), hash );

    Tag *h = new Tag( "handshake", hash );
    send( h );

    free( hash );
  }

  bool Component::handleNormalNode( const Tag *tag )
  {
    printf( "in handleNormalNode\n" );
    if( tag->name() == "handshake" )
      notifyOnConnect();
    else
      return false;

    return true;
  }

};

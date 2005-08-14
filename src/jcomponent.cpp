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



#include "jcomponent.h"


namespace gloox
{

  JComponent::JComponent( const std::string& ns, const std::string& server,
                          const std::string& component, const std::string& password, int port )
    : ClientBase( ns, password, server, port ),
    m_to( component ), m_disco( 0 )
  {
    m_disco = new Disco( this );
    m_disco->setVersion( "based on gloox", GLOOX_VERSION );
    m_disco->setIdentity( "component", "generic" );
  }

  JComponent::~JComponent()
  {
    delete m_disco;
  }

  void JComponent::on_stream( int type, iks* node )
  {
    if( !node )
      return;

    ikspak *pak = iks_packet( node );

    switch( type )
    {
      case( IKS_NODE_START ):
        login( iks_find_attrib( node, "id" ) );
        break;
      case IKS_NODE_NORMAL:
        if( iks_strncmp( iks_name( node ), "handshake", 9 ) == 0 )
          notifyOnConnect();
        else
          iks_filter_packet( m_filter, pak );
        break;
      case IKS_NODE_ERROR:
        disconnect();
        break;
      case IKS_NODE_STOP:
        disconnect();
        break;
    }

    iks_delete( node );
  }

  void JComponent::login( const char* sid )
  {
    if( !sid )
      return;

    std::string data = sid + m_password;
    char *hash = (char*)calloc( 41, sizeof( char ) );
    iks_sha( data.c_str(), hash );

    iks *x = iks_new( "handshake" );
    iks_insert_cdata( x, hash, iks_strlen( hash ) );
    send( x );

    free( hash );
  }

};

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


#include "jid.h"

#include "prep.h"

using namespace std;

namespace gloox
{

  JID::JID()
  {
  }

  JID::JID( const std::string& jid )
  {
    setJID( jid );
  }

  JID::~JID()
  {
  }

  void JID::setJID( const std::string& jid )
  {
    if( jid.empty() )
      return;

    int at = jid.find( "@", 0 );
    int slash = jid.find( "/", 0 );

    if( ( at == std::string::npos ) && ( slash == std::string::npos ) )
    {
      m_serverRaw = jid;
      m_server = Prep::nameprep( m_serverRaw );
    }

    if( ( at != std::string::npos ) && ( slash != std::string::npos ) )
    {
      m_username = Prep::nodeprep( jid.substr( 0, at ) );
      m_serverRaw = jid.substr( at + 1, slash - at - 1 );
      m_server = Prep::nameprep( m_serverRaw );
      m_resource = Prep::resourceprep( jid.substr( slash + 1 ) );
    }

    if( ( at == std::string::npos ) && ( slash != std::string::npos ) )
    {
      m_serverRaw = jid.substr( 0, slash );
      m_server = Prep::nameprep( m_serverRaw );
      m_resource = Prep::resourceprep( jid.substr( slash + 1 ) );
    }

    if( ( at != std::string::npos ) && ( slash == std::string::npos ) )
    {
      m_username = Prep::nodeprep( jid.substr( 0, at ) );
      m_serverRaw = jid.substr( at + 1 );
      m_server = Prep::nameprep( m_serverRaw );
    }
  }

  void JID::setUsername( const std::string& username )
  {
    m_username = Prep::nodeprep( username );
  }

  void JID::setServer( const std::string& server )
  {
    m_serverRaw = server;
    m_server = Prep::nameprep( m_serverRaw );
  }

  void JID::setResource( const std::string& resource )
  {
    m_resource = Prep::resourceprep( resource );
  }

  std::string JID::full() const
  {
    if( m_server.empty() )
      return "";
    else if( m_username.empty() )
      if( m_resource.empty() )
        return m_server;
      else
        return ( m_server + "/" + m_resource );
    else
      if( m_resource.empty() )
        return ( m_username + "@" + m_server );
      else
        return ( m_username + "@" + m_server + "/" + m_resource );
  }

  std::string JID::bare() const
  {
    if( m_server.empty() )
      return "";
    else if( m_username.empty() )
      return m_server;
    else
      return m_username + "@" + m_server;
  }

  int JID::operator==( const JID& right )
  {
    return ( ( m_resource == right.m_resource )
        && ( m_server == right.m_server )
        && ( m_username == right.m_username ) );
  }

};

/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "jid.h"
#include "prep.h"
#include "gloox.h"

namespace gloox
{

  bool JID::setJID( const std::string& jid )
  {
    if ( jid.empty() )
    {
      m_bare = m_full = m_server = m_username = m_serverRaw = m_resource = EmptyString;
      m_valid = false;
      return false;
    }

    const std::string::size_type at = jid.find( '@' );
    const std::string::size_type slash = jid.find( '/', at == std::string::npos ? 0 : at );

    if( at != std::string::npos && !( m_valid = prep::nodeprep( jid.substr( 0, at ), m_username ) ) )
      return false;

    m_serverRaw = jid.substr( at == std::string::npos ? 0 : at + 1, slash - at - 1 );
    if( !( m_valid = prep::nameprep( m_serverRaw, m_server ) ) )
      return false;

    if( slash != std::string::npos
         && !( m_valid = prep::resourceprep( jid.substr( slash + 1 ), m_resource ) ) )
      return false;

    setStrings();

    return m_valid;
  }

  bool JID::setUsername( const std::string& uname )
  {
    m_valid = prep::nodeprep( uname, m_username );
    setStrings();
    return m_valid;
  }

  bool JID::setServer( const std::string& serv )
  {
    m_serverRaw = serv;
    m_valid = prep::nameprep( m_serverRaw, m_server );
    setStrings();
    return m_valid;
  }

  bool JID::setResource( const std::string& res )
  {
    m_valid = prep::resourceprep( res, m_resource );
    setFull();
    return m_valid;
  }

  void JID::setFull()
  {
    m_full = bare();
    if( !m_resource.empty() )
      m_full += '/' + m_resource;
  }

  void JID::setBare()
  {
    if( !m_username.empty() )
      m_bare = m_username + '@';
    else
      m_bare = EmptyString;
    m_bare += m_server;
  }

}

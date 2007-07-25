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

namespace gloox
{

  void JID::setJID( const std::string& jid )
  {
    if ( jid.empty() )
    {
      m_bare = m_full = m_server = m_username = m_serverRaw = m_resource = "";
      return;
    }

    const size_t at = jid.find( '@' );
    const size_t slash = jid.rfind( '/' );

    if( at != std::string::npos )
      m_username = prep::nodeprep( jid.substr( 0, at ) );

    m_serverRaw = jid.substr( at == std::string::npos ? 0 : at + 1, slash - at - 1 );
    m_server = prep::nameprep( m_serverRaw );

    if( slash != std::string::npos )
      m_resource = prep::resourceprep( jid.substr( slash + 1 ) );

    setStrings();
  }

  void JID::setUsername( const std::string& uname )
  {
    m_username = prep::nodeprep( uname );
    setStrings();
  }

  void JID::setServer( const std::string& serv )
  {
    m_serverRaw = serv;
    m_server = prep::nameprep( m_serverRaw );
    setStrings();
  }

  void JID::setResource( const std::string& res )
  {
    m_resource = prep::resourceprep( res );
    setFull();
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
      m_bare = "";
    m_bare += m_server;
  }

}

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifdef WIN32
#include "../config.h.win"
#else
#include "config.h"
#endif

#include "client.h"
#include "rostermanager.h"
#include "disco.h"
#include "logsink.h"
#include "connection.h"
#include "tag.h"
#include "stanza.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>

namespace gloox
{

  Client::Client( const std::string& server )
    : BareClient( XMLNS_CLIENT, server ),
      m_rosterManager( 0 ), m_disco( 0 ),
      m_manageRoster( true ), m_handleDisco( true )
  {
    m_jid.setServer( server );
    init();
  }

  Client::Client( const JID& jid, const std::string& password, int port )
    : BareClient( XMLNS_CLIENT, password, "", port ),
      m_rosterManager( 0 ), m_disco( 0 ),
      m_manageRoster( true ), m_handleDisco( true )
  {
    m_jid = jid;
    m_server = m_jid.serverRaw();
    init();
  }

  Client::Client( const std::string& username, const std::string& password,
                    const std::string& server, const std::string& resource, int port )
    : BareClient( XMLNS_CLIENT, password, server, port ),
      m_rosterManager( 0 ), m_disco( 0 ),
      m_manageRoster( true ), m_handleDisco( true )
  {
    m_jid.setUsername( username );
    m_jid.setServer( server );
    m_jid.setResource( resource );

    init();
  }

  Client::~Client()
  {
    delete m_disco;
    delete m_rosterManager;
  }

  void Client::init()
  {
    m_disco = new Disco( this );
    m_rosterManager = new RosterManager( this, true );
    m_disco->setVersion( "based on gloox", GLOOX_VERSION );
    m_disco->setIdentity( "client", "bot" );
  }

  void Client::disableDisco()
  {
    m_handleDisco = false;
    delete m_disco;
    m_disco = 0;
  }

  void Client::disableRoster()
  {
    m_manageRoster = false;
    delete m_rosterManager;
    m_rosterManager = 0;
  }

  RosterManager* Client::rosterManager()
  {
    return m_rosterManager;
  }

  Disco* Client::disco()
  {
    return m_disco;
  }

  void Client::connected()
  {
    if( m_authed )
    {
      if( m_manageRoster )
        m_rosterManager->fill();

      if( m_autoPresence )
        sendInitialPresence();
    }

    notifyOnConnect();
  }

}

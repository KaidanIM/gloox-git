/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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


#include "jclient.h"
#include "rostermanager.h"
#include "disco.h"
#include "nonsaslauth.h"

#include <iksemel.h>

#include <unistd.h>
#include <iostream>


namespace gloox
{

  JClient::JClient()
    : ClientBase( XMLNS_CLIENT ),
    m_priority( -1 ),
    m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_authorized( false )
  {
    init();
  }

  JClient::JClient( const std::string& id, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, port ),
    m_priority( -1 ), m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_authorized( false )
  {
    iksid *tmp = iks_id_new( get_stack(), id.c_str() );
    if( tmp->user )
      m_username = tmp->user;
    if( tmp->resource )
      m_resource = tmp->resource;
    if( tmp->server )
      m_server = tmp->server;

    init();
  }

  JClient::JClient( const std::string& username, const std::string& password,
                    const std::string& server, const std::string& resource, int port )
    : ClientBase( XMLNS_CLIENT, password, server, port ),
    m_username( username ), m_resource( resource ),
    m_priority( -1 ), m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_authorized( false )
  {
    init();
  }

  JClient::~JClient()
  {
  }

  void JClient::init()
  {
    registerConnectionListener( this );

    iks_filter_add_rule( m_filter, (iksFilterHook*) sessionHook, this,
                        IKS_RULE_TYPE, IKS_PAK_IQ,
                        IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                        IKS_RULE_ID, "session",
                        IKS_RULE_DONE );
    iks_filter_add_rule( m_filter, (iksFilterHook*) bindHook, this,
                        IKS_RULE_TYPE, IKS_PAK_IQ,
                        IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                        IKS_RULE_ID, "bind",
                        IKS_RULE_DONE );

    m_disco = new Disco( this );
    m_rosterManager = new RosterManager( this );
    m_disco->setVersion( "based on gloox", GLOOX_VERSION );
    m_disco->setIdentity( "client", "bot" );
  }

  void JClient::cleanUp()
  {
    delete m_disco;
    delete m_rosterManager;
    ClientBase::cleanUp();
  }

  std::string JClient::jid()
  {
    if( server().empty() )
      return "";
    else if( username().empty() )
      if( resource().empty() )
        return server();
      else
        return ( server() + "/" + resource() );
    else
      if( resource().empty() )
        return ( username() + "@" + server() );
      else
        return ( username() + "@" + server() + "/" + resource() );
  }

  iksid* JClient::parsedJid()
  {
    return iks_id_new( get_stack(), jid().c_str() );
  }

  void JClient::on_stream( int type, iks* node )
  {
    if( !node )
      return;

    if( m_debug ) printf("in on_stream\n");
    ikspak* pak = iks_packet( node );

    switch( type )
    {
      case IKS_NODE_START:      // <stream:stream>
        m_sid = iks_find_attrib( node, "id" );
        break;
      case IKS_NODE_NORMAL:     // first level child of stream
        if( strncmp( "stream:features", iks_name( node ), 15 ) == 0 )
        {
          m_streamFeatures = getStreamFeatures( node );

          if( tls() && !is_secure() && ( m_streamFeatures & STREAM_FEATURE_STARTTLS ) )
          {
            start_tls();
            if( m_debug ) printf("after starttls\n");
            break;
          }

          if( sasl() )
          {
            if( tls() && !sasl() && !is_secure() )
              disconnect();

            if( m_authorized )
            {
              if( m_streamFeatures & STREAM_FEATURE_BIND )
              {
                bindResource();
              }
            }
            else if( !username().empty() && !password().empty() )
            {
              std::string user = username();
              std::string pwd = password();
              if( m_streamFeatures & STREAM_FEATURE_SASL_DIGESTMD5 )
                start_sasl( IKS_SASL_DIGEST_MD5, (char*)user.c_str(), (char*)pwd.c_str() );

              else if( is_secure() && ( m_streamFeatures & STREAM_FEATURE_SASL_PLAIN ) )
                start_sasl( IKS_SASL_PLAIN, (char*)user.c_str(), (char*)pwd.c_str() );

              else if( m_streamFeatures & STREAM_FEATURE_IQAUTH )
                nonSaslLogin();

              else
              {
                m_state = STATE_NO_SUPPORTED_AUTH;
                disconnect();
              }
            }
            else
            {
              notifyOnConnect();
            }
          }
        }
        else if( iks_strncmp( "failure", iks_name ( node ), 7 ) == 0 )
        {
          if( m_debug ) printf("sasl authentication failed...\n");
          m_state = STATE_AUTHENTICATION_FAILED;
          disconnect();
        }
        else if( iks_strncmp( "success", iks_name ( node ), 7 ) == 0 )
        {
          if( m_debug ) printf( "sasl initialisation successful...\n" );
          m_state = STATE_AUTHENTICATED;
          m_authorized = true;
          header( server() );
        }
        else
        {
          iks_filter_packet ( m_filter, pak );
        }
        break;
      case IKS_NODE_ERROR:      // <stream:error>
        m_state = STATE_ERROR;
        if( m_debug ) printf( "stream error. quitting\n");
        disconnect();
        break;
      case IKS_NODE_STOP:       // </stream:stream>
        disconnect();
        break;
    }

    iks_delete( node );
  }

  int JClient::getStreamFeatures( iks *x )
  {
    if( iks_strncmp( iks_name( x ), "stream:features", 15 ) != 0 )
      return 0;

    int features = 0;

    iks *y = iks_first_tag( x );
    while( y )
    {
      if( !iks_strncmp( iks_name( y ), "starttls", 8 )
                && !iks_strncmp( iks_find_attrib( y, "xmlns" ),
                                XMLNS_STREAM_TLS, iks_strlen( XMLNS_STREAM_TLS ) ) )
        features |= STREAM_FEATURE_STARTTLS;

      else if( !iks_strncmp( iks_name( y ), "mechanisms", 10 )
                && !iks_strncmp( iks_find_attrib( y, "xmlns" ),
                                XMLNS_STREAM_SASL, iks_strlen( XMLNS_STREAM_SASL ) ) )
        features |= getSaslMechs( iks_child( y ) );

      else if( !iks_strncmp( iks_name( y ), "bind", 4 )
                && !iks_strncmp( iks_find_attrib( y, "xmlns" ),
                                XMLNS_STREAM_BIND, iks_strlen( XMLNS_STREAM_BIND ) ) )
        features |= STREAM_FEATURE_BIND;

      else if( !iks_strncmp( iks_name( y ), "session", 7 )
                && !iks_strncmp( iks_find_attrib( y, "xmlns" ),
                                XMLNS_STREAM_SESSION, iks_strlen( XMLNS_STREAM_SESSION ) ) )
        features |= STREAM_FEATURE_SESSION;

      else if( !iks_strncmp( iks_name( y ), "auth", 4 )
                && !iks_strncmp( iks_find_attrib( y, "xmlns" ),
                                XMLNS_STREAM_IQAUTH, iks_strlen( XMLNS_STREAM_IQAUTH ) ) )
        features |= STREAM_FEATURE_IQAUTH;

      else if( !iks_strncmp( iks_name( y ), "register", 8 )
                && !iks_strncmp( iks_find_attrib( y, "xmlns" ),
                                XMLNS_STREAM_IQREGISTER, iks_strlen( XMLNS_STREAM_IQREGISTER ) ) )
        features |= STREAM_FEATURE_IQREGISTER;

      y = iks_next_tag( y );
    }

    if( features == 0 )
      features = STREAM_FEATURE_IQAUTH;

    return features;
  }

  int JClient::getSaslMechs( iks* x )
  {
    int mechs = 0;

    while( x )
    {
      if( iks_strncmp( iks_cdata( iks_child( x ) ), "DIGEST-MD5", 10 ) == 0 )
        mechs |= STREAM_FEATURE_SASL_DIGESTMD5;
      else if( iks_strncmp( iks_cdata( iks_child( x ) ), "PLAIN", 5 ) == 0 )
        mechs |= STREAM_FEATURE_SASL_PLAIN;

      x = iks_next_tag( x );
    }
    return mechs;
  }

  void JClient::bindResource()
  {
    iks *x = iks_new( "iq" );
    iks_insert_attrib( x, "type", "set" );
    iks_insert_attrib( x, "id", "bind" );
    iks *y = iks_insert( x, "bind" );
    iks_insert_attrib( y, "xmlns", XMLNS_STREAM_BIND );
    if( !resource().empty() && resource().length() )
      iks_insert_cdata( iks_insert( y, "resource" ), resource().c_str(), 0 );

    send( x );
  }

  void JClient::createSession()
  {
    if( m_streamFeatures & STREAM_FEATURE_SESSION )
    {
      iks *x = iks_new( "iq" );
      iks_insert_attrib( x, "type", "set" );
      iks_insert_attrib( x, "id", "session" );
      iks *y = iks_insert( x, "session" );
      iks_insert_attrib( y, "xmlns", XMLNS_STREAM_SESSION );

      send( x );
    }
  }

  void JClient::disableDisco()
  {
    m_handleDisco = false;
    delete m_disco;
    m_disco = 0;
  }

  void JClient::disableRoster()
  {
    m_manageRoster = false;
    delete m_rosterManager;
    m_rosterManager = 0;
  }

  void JClient::nonSaslLogin( const char* sid )
  {
    if( m_debug ) printf("in login()\n");
    NonSaslAuth *auth = new NonSaslAuth( this, m_sid );
    auth->doAuth();
  }

  void JClient::sendInitialPresence()
  {
    char prio[5];
    sprintf( prio, "%d", m_priority );
    iks* x = iks_make_pres( IKS_SHOW_AVAILABLE, "online" );
    iks_insert_cdata( iks_insert( x, "priority" ), prio, iks_strlen( prio ) );
    send( x );
  }

  void JClient::sendPresence( int priority, ikshowtype type, const std::string& msg )
  {
    if( priority < -128 )
      priority = -128;
    if( priority > 127 )
      priority = 127;

    char prio[5];
    sprintf( prio, "%d", priority );
    iks* x = iks_make_pres( type, msg.c_str() );
    iks_insert_cdata( iks_insert( x, "priority" ), prio, iks_strlen( prio ) );
    send( x );
  }

  void JClient::setInitialPriority( int priority )
  {
    if( priority < -128 )
      priority = -128;
    if( priority > 127 )
      priority = 127;

    m_priority = priority;
  }

  RosterManager* JClient::rosterManager()
  {
    return m_rosterManager;
  }

  Disco* JClient::disco()
  {
    return m_disco;
  }

  void JClient::onConnect()
  {
    if( m_manageRoster )
      m_rosterManager->fill();

    if( m_autoPresence )
      sendInitialPresence();
  }

  // FIXME!!!
  bool notified = false;

  int bindHook( JClient* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("bindHook\n");
    iks* x = iks_child( iks_child( pak->x ) );
    if( iks_strncmp( iks_name( x ), "jid", 3 ) == 0 )
    {
      iksid* id = iks_id_new( stream->get_stack(), iks_cdata( iks_child( x ) ) );
      stream->setResource( id->resource );
    }

    stream->createSession();

    return IKS_FILTER_EAT;
  }

  int sessionHook( JClient* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("sessionHook\n");
    stream->notifyOnConnect();

    return IKS_FILTER_EAT;
  }

};

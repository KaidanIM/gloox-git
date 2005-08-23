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

#include "config.h"

#include "client.h"
#include "rostermanager.h"
#include "disco.h"
#include "nonsaslauth.h"
#include "connection.h"
#include "tag.h"
#include "stanza.h"

#include <iksemel.h>

#include <unistd.h>
#include <iostream>


namespace gloox
{

  Client::Client()
    : ClientBase( XMLNS_CLIENT ),
    m_priority( -1 ),
    m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_auth( 0 ), m_authorized( false ), m_resourceBound( false )
  {
    init();
  }

  Client::Client( const std::string& id, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, port ),
    m_priority( -1 ), m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_auth( 0 ), m_authorized( false ), m_resourceBound( false )
  {
    m_jid.setJID( id );

    init();
  }

  Client::Client( const std::string& username, const std::string& password,
                    const std::string& server, const std::string& resource, int port )
    : ClientBase( XMLNS_CLIENT, password, server, port ),
    m_username( username ), m_resource( resource ),
    m_priority( -1 ), m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_auth( 0 ), m_authorized( false ), m_resourceBound( false )
  {
    init();
  }

  Client::~Client()
  {
    delete m_disco;
    delete m_rosterManager;
    delete m_auth;
  }

  void Client::init()
  {
    m_disco = new Disco( this );
    m_rosterManager = new RosterManager( this );
    m_disco->setVersion( "based on gloox", GLOOX_VERSION );
    m_disco->setIdentity( "client", "bot" );
  }

  bool Client::handleNormalNode( Tag *tag )
  {
    if( tag->name() == "stream:features" )
    {
      m_streamFeatures = getStreamFeatures( tag );
      printf( "stream features: %d\n", m_streamFeatures );

#ifdef HAVE_GNUTLS
      if( tls() && hasTls() && !m_connection->isSecure() && ( m_streamFeatures & STREAM_FEATURE_STARTTLS ) )
      {
        startTls();
        return true;
      }
#endif

      if( sasl() )
      {
        if( m_authorized )
        {
          if( m_streamFeatures & STREAM_FEATURE_BIND )
          {
            bindResource();
          }
        }
        else if( !username().empty() && !password().empty() )
        {
          if( m_streamFeatures & STREAM_FEATURE_SASL_DIGESTMD5 )
          {
            startSASL( SASL_DIGEST_MD5 );
          }
          else if( m_streamFeatures & STREAM_FEATURE_SASL_PLAIN )
          {
            startSASL( SASL_PLAIN );
          }
          else if( m_streamFeatures & STREAM_FEATURE_IQAUTH )
          {
            nonSaslLogin();
          }
          else
          {
#ifdef DEBUG
            printf( "the server doesn't support any auth mechanisms we know about\n" );
#endif
            disconnect( STATE_NO_SUPPORTED_AUTH );
          }
        }
        else
        {
          connected();
        }
      }
      else if( m_streamFeatures & STREAM_FEATURE_IQAUTH )
      {
        nonSaslLogin();
      }
      else
      {
#ifdef DEBUG
        printf( "the server doesn't support any auth mechanisms we know about\n" );
#endif
        disconnect( STATE_NO_SUPPORTED_AUTH );
      }
    }
#ifdef HAVE_GNUTLS
    else if( ( tag->name() == "proceed" ) && tag->hasAttribute( "xmlns", XMLNS_STREAM_TLS ) )
    {
#ifdef DEBUG
      printf( "starting TLS handshake...\n" );
#endif
      if( m_connection->tlsHandshake() )
      {
        header();
        if( !notifyOnTLSConnect( m_connection->fetchTLSInfo() ) )
          disconnect( STATE_DISCONNECTED );
      }
#ifdef DEBUG
      printf( "connection security is now %d\n", m_connection->isSecure() );
#endif
    }
#endif
    else if( ( tag->name() == "challenge" ) && tag->hasAttribute( "xmlns", XMLNS_STREAM_SASL ) )
    {
#ifdef DEBUG
      printf( "processing sasl challenge\n" );
#endif
      processSASLChallenge( tag->cdata() );
    }
    else if( tag->name() == "failure" )
    {
#ifdef DEBUG
      printf( "sasl authentication failed...\n" );
#endif
      disconnect( STATE_AUTHENTICATION_FAILED );
    }
    else if( tag->name() == "success" )
    {
#ifdef DEBUG
      printf( "sasl auth successful...\n" );
#endif
      setState( STATE_AUTHENTICATED );
      m_authorized = true;
      header();
    }
    else
    {
      if( ( tag->name() == "iq" ) && tag->hasAttribute( "id", "bind" ) )
      {
        processResourceBind( tag );
      }
      else if( ( tag->name() == "iq" ) && tag->hasAttribute( "id", "session" ) )
        processCreateSession( tag );
      else
        return false;
    }

    return true;
  }

  int Client::getStreamFeatures( Tag *tag )
  {
    if( tag->name() != "stream:features" )
      return 0;

    int features = 0;

    if( tag->hasChild( "starttls", "xmlns", XMLNS_STREAM_TLS ) )
      features |= STREAM_FEATURE_STARTTLS;

    if( tag->hasChild( "mechanisms", "xmlns", XMLNS_STREAM_SASL ) )
      features |= getSaslMechs( tag->findChild( "mechanisms" ) );

    if( tag->hasChild( "bind", "xmlns", XMLNS_STREAM_BIND ) )
      features |= STREAM_FEATURE_BIND;

    if( tag->hasChild( "session", "xmlns", XMLNS_STREAM_SESSION ) )
      features |= STREAM_FEATURE_SESSION;

    if( tag->hasChild( "auth", "xmlns", XMLNS_STREAM_IQAUTH ) )
      features |= STREAM_FEATURE_IQAUTH;

    if( tag->hasChild( "register", "xmlns", XMLNS_STREAM_IQREGISTER ) )
      features |= STREAM_FEATURE_IQREGISTER;


    if( features == 0 )
      features = STREAM_FEATURE_IQAUTH;

    return features;
  }

  int Client::getSaslMechs( Tag *tag )
  {
    int mechs = 0;

    if( tag->hasChildWithCData( "mechanism", "DIGEST-MD5" ) )
      mechs |= STREAM_FEATURE_SASL_DIGESTMD5;

    if( tag->hasChildWithCData( "mechanism", "PLAIN" ) )
        mechs |= STREAM_FEATURE_SASL_PLAIN;

    return mechs;
  }

  void Client::bindResource()
  {
    if( !m_resourceBound )
    {
      Tag *iq = new Tag( "iq" );
      iq->addAttrib( "type", "set" );
      iq->addAttrib( "id", "bind" );
      Tag *b = new Tag( "bind" );
      b->addAttrib( "xmlns", XMLNS_STREAM_BIND );
      if( !resource().empty() )
      {
        Tag *r = new Tag( "resource", resource() );
        b->addChild( r );
      }
      iq->addChild( b );

      send( iq );
    }
  }

  void Client::processResourceBind( Tag *tag )
  {
    Stanza *stanza= new Stanza( tag );
    switch( stanza->subtype() )
    {
      case STANZA_IQ_RESULT:
      {
        Tag *bind = stanza->findChild( "bind" );
        Tag *jid = bind->findChild( "jid" );
        m_jid.setJID( jid->cdata() );
        m_resourceBound = true;

        if( m_streamFeatures & STREAM_FEATURE_SESSION )
          createSession();
        else
          connected();
        break;
      }
      case STANZA_IQ_ERROR:
      {
        Tag *error = stanza->findChild( "error" );
        if( stanza->hasChild( "error", "type", "modify" )
            && error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnResourceBindError( ConnectionListener::RB_BAD_REQUEST );
        }
        else if( stanza->hasChild( "error", "type", "cancel" ) )
        {
          if( error->hasChild( "not-allowed", "xmlns", XMLNS_XMPP_STANZAS ) )
            notifyOnResourceBindError( ConnectionListener::RB_NOT_ALLOWED );
          else if( error->hasChild( "conflict", "xmlns", XMLNS_XMPP_STANZAS ) )
            notifyOnResourceBindError( ConnectionListener::RB_CONFLICT );
          else
            notifyOnResourceBindError( ConnectionListener::RB_UNKNOWN_ERROR );
        }
        else
          notifyOnResourceBindError( ConnectionListener::RB_UNKNOWN_ERROR );
        break;
      }
    }
  }

  void Client::createSession()
  {
    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "type", "set" );
    iq->addAttrib( "id", "session" );
    Tag *s = new Tag( "session" );
    s->addAttrib( "xmlns", XMLNS_STREAM_SESSION );
    iq->addChild( s );

    send( iq );
  }

  void Client::processCreateSession( Tag *tag )
  {
    Stanza *stanza = new Stanza( tag );
    switch( stanza->subtype() )
    {
      case STANZA_IQ_RESULT:
      {
        connected();
        break;
      }
      case STANZA_IQ_ERROR:
      {
        Tag *error = stanza->findChild( "error" );
        if( stanza->hasChild( "error", "type", "wait" )
            && error->hasChild( "internal-server-error", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ConnectionListener::SC_INTERNAL_SERVER_ERROR );
        }
        else if( stanza->hasChild( "error", "type", "auth" )
                 && error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ConnectionListener::SC_FORBIDDEN );
        }
        else if( stanza->hasChild( "error", "type", "cancel" )
                 && error->hasChild( "conflict", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ConnectionListener::SC_CONFLICT );
        }
        else
          notifyOnSessionCreateError( ConnectionListener::SC_UNKNOWN_ERROR );
        break;
      }
    }
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

  void Client::nonSaslLogin()
  {
    m_auth = new NonSaslAuth( this, m_sid );
    m_auth->doAuth();
  }

  void Client::sendInitialPresence()
  {
    Tag *p = new Tag( "presence" );
    char priority[5];
    sprintf( priority, "%d", m_priority );
    Tag *prio= new Tag( "priority", priority );
    p->addChild( prio );
    send( p );
  }

//   void Client::sendPresence( int priority, ikshowtype type, const std::string& msg )
//   {
//     if( priority < -128 )
//       priority = -128;
//     if( priority > 127 )
//       priority = 127;
//
//     char prio[5];
//     sprintf( prio, "%d", priority );
//     iks* x = iks_make_pres( type, msg.c_str() );
//     iks_insert_cdata( iks_insert( x, "priority" ), prio, iks_strlen( prio ) );
//     send( x );
//   }

  void Client::setInitialPriority( int priority )
  {
    if( priority < -128 )
      priority = -128;
    if( priority > 127 )
      priority = 127;

    m_priority = priority;
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
    notifyOnConnect();

    if( m_manageRoster )
      m_rosterManager->fill();

    if( m_autoPresence )
      sendInitialPresence();
  }

};

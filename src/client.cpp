/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
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

  Client::Client( const std::string& server )
    : ClientBase( XMLNS_CLIENT, server ),
    m_priority( -1 ),
    m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_auth( 0 ), m_authorized( false ), m_resourceBound( false )
  {
    init();
  }

  Client::Client( const JID& jid, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, "", port ),
    m_priority( -1 ), m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_auth( 0 ), m_authorized( false ), m_resourceBound( false )
  {
    m_jid = jid;
    m_server = m_jid.serverRaw();
    init();
  }

  Client::Client( const std::string& username, const std::string& password,
                    const std::string& server, const std::string& resource, int port )
    : ClientBase( XMLNS_CLIENT, password, server, port ),
    m_priority( -1 ), m_autoPresence( false ), m_manageRoster( true ),
    m_handleDisco( true ), m_rosterManager( 0 ),
    m_disco( 0 ), m_auth( 0 ), m_authorized( false ), m_resourceBound( false )
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
    delete m_auth;
  }

  void Client::init()
  {
    m_disco = new Disco( this );
    m_rosterManager = new RosterManager( this );
    m_disco->setVersion( "based on gloox", GLOOX_VERSION );
    m_disco->setIdentity( "client", "bot" );
  }

  bool Client::handleNormalNode( Stanza *stanza )
  {
    if( stanza->name() == "stream:features" )
    {
      m_streamFeatures = getStreamFeatures( stanza );

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
            disconnect( CONN_NO_SUPPORTED_AUTH );
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
        disconnect( CONN_NO_SUPPORTED_AUTH );
      }
    }
#ifdef HAVE_GNUTLS
    else if( ( stanza->name() == "proceed" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_TLS ) )
    {
#ifdef DEBUG
      printf( "starting TLS handshake...\n" );
#endif
      if( m_connection->tlsHandshake() )
      {
        if( !notifyOnTLSConnect( m_connection->fetchTLSInfo() ) )
          disconnect( CONN_TLS_FAILED );
        else
        {
#ifdef DEBUG
          printf( "connection security is now %d\n", m_connection->isSecure() );
#endif
          header();
        }
      }
    }
    else if( ( stanza->name() == "failure" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_TLS ) )
    {
#ifdef DEBUG
      printf( "tls handshake failed...\n" );
#endif
      disconnect( CONN_TLS_FAILED );
    }
#endif
    else if( ( stanza->name() == "challenge" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_SASL ) )
    {
#ifdef DEBUG
      printf( "processing sasl challenge\n" );
#endif
      processSASLChallenge( stanza->cdata() );
    }
    else if( ( stanza->name() == "failure" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_SASL ) )
    {
#ifdef DEBUG
      printf( "sasl authentication failed...\n" );
#endif
      processSASLError( stanza );
      disconnect( CONN_AUTHENTICATION_FAILED );
    }
    else if( stanza->name() == "success" )
    {
#ifdef DEBUG
      printf( "sasl auth successful...\n" );
#endif
      setAuthed( true );
      m_authorized = true;
      header();
    }
    else
    {
      if( ( stanza->name() == "iq" ) && stanza->hasAttribute( "id", "bind" ) )
      {
        processResourceBind( stanza );
      }
      else if( ( stanza->name() == "iq" ) && stanza->hasAttribute( "id", "session" ) )
        processCreateSession( stanza );
      else
        return false;
    }

    return true;
  }

  int Client::getStreamFeatures( Stanza *stanza )
  {
    if( stanza->name() != "stream:features" )
      return 0;

    int features = 0;

    if( stanza->hasChild( "starttls", "xmlns", XMLNS_STREAM_TLS ) )
      features |= STREAM_FEATURE_STARTTLS;

    if( stanza->hasChild( "mechanisms", "xmlns", XMLNS_STREAM_SASL ) )
      features |= getSaslMechs( stanza->findChild( "mechanisms" ) );

    if( stanza->hasChild( "bind", "xmlns", XMLNS_STREAM_BIND ) )
      features |= STREAM_FEATURE_BIND;

    if( stanza->hasChild( "session", "xmlns", XMLNS_STREAM_SESSION ) )
      features |= STREAM_FEATURE_SESSION;

    if( stanza->hasChild( "auth", "xmlns", XMLNS_STREAM_IQAUTH ) )
      features |= STREAM_FEATURE_IQAUTH;

    if( stanza->hasChild( "register", "xmlns", XMLNS_STREAM_IQREGISTER ) )
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

  void Client::processResourceBind( Stanza *stanza )
  {
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

  void Client::processCreateSession( Stanza *stanza )
  {
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

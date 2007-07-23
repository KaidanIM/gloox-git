/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifdef WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#include "client.h"
#include "capabilities.h"
#include "rostermanager.h"
#include "disco.h"
#include "logsink.h"
#include "nonsaslauth.h"
#include "tag.h"
#include "stanzaextensionfactory.h"
#include "stanzaextension.h"
#include "tlsbase.h"
#include "util.h"

#if !defined( WIN32 ) && !defined( _WIN32_WCE )
# include <unistd.h>
#endif

#ifndef _WIN32_WCE
# include <iostream>
# include <sstream>
#else
# include <stdio.h>
#endif

namespace gloox
{

  Client::Client( const std::string& server )
    : ClientBase( XMLNS_CLIENT, server ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available ), m_capabilities( 0 ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ), m_doAuth( false ),
      m_streamFeatures( 0 ), m_priority( 0 )
  {
    m_jid.setServer( server );
    init();
  }

  Client::Client( const JID& jid, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, "", port ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available ), m_capabilities( 0 ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ), m_doAuth( true ),
      m_streamFeatures( 0 ), m_priority( 0 )
  {
    m_jid = jid;
    m_server = m_jid.serverRaw();
    init();
  }

  Client::Client( const std::string& username, const std::string& password,
                  const std::string& server, const std::string& resource, int port )
    : ClientBase( XMLNS_CLIENT, password, server, port ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available ), m_capabilities( 0 ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ), m_doAuth( true ),
      m_streamFeatures( 0 ), m_priority( 0 )
  {
    m_jid.setUsername( username );
    m_jid.setServer( server );
    m_jid.setResource( resource );

    init();
  }

  Client::~Client()
  {
    removePresenceExtensions();
    delete m_rosterManager;
    delete m_auth;
  }

  void Client::init()
  {
    m_rosterManager = new RosterManager( this );
    m_disco->setIdentity( "client", "bot" );
    m_capabilities = new Capabilities( m_disco );
    addPresenceExtension( m_capabilities );
  }

  void Client::setUsername( const std::string &username )
  {
    m_jid.setUsername( username );
    m_doAuth = true;
  }

  bool Client::handleNormalNode( Tag *tag )
  {
    if( tag->name() == "stream:features" )
    {
      m_streamFeatures = getStreamFeatures( tag );

      if( m_tls && m_encryption && !m_encryptionActive
          && ( m_streamFeatures & StreamFeatureStartTls ) )
      {
        notifyStreamEvent( StreamEventEncryption );
        startTls();
        return true;
      }

      if( m_compress && m_compression && !m_compressionActive
          && ( m_streamFeatures & StreamFeatureCompressZlib ) )
      {
        notifyStreamEvent( StreamEventCompression );
        negotiateCompression( StreamFeatureCompressZlib );
        return true;
      }
//       if( ( m_streamFeatures & StreamFeatureCompressDclz )
//               && m_connection->initCompression( StreamFeatureCompressDclz ) )
//       {
//         negotiateCompression( StreamFeatureCompressDclz );
//         return true;
//       }

      if( m_sasl )
      {
        if( m_authed )
        {
          if( m_streamFeatures & StreamFeatureBind )
          {
            notifyStreamEvent( StreamEventResourceBinding );
            bindResource();
          }
        }
        else if( m_doAuth && !username().empty() && !password().empty() )
        {
          if( m_streamFeatures & SaslMechDigestMd5 && m_availableSaslMechs & SaslMechDigestMd5
              && !m_forceNonSasl )
          {
            notifyStreamEvent( StreamEventAuthentication );
            startSASL( SaslMechDigestMd5 );
          }
          else if( m_streamFeatures & SaslMechPlain && m_availableSaslMechs & SaslMechPlain
                   && !m_forceNonSasl )
          {
            notifyStreamEvent( StreamEventAuthentication );
            startSASL( SaslMechPlain );
          }
          else if( m_streamFeatures & StreamFeatureIqAuth || m_forceNonSasl )
          {
            notifyStreamEvent( StreamEventAuthentication );
            nonSaslLogin();
          }
          else
          {
            logInstance().log( LogLevelError, LogAreaClassClient,
                                     "the server doesn't support any auth mechanisms we know about" );
            disconnect( ConnNoSupportedAuth );
          }
        }
        else if( m_doAuth && !m_clientCerts.empty() && !m_clientKey.empty()
                 && m_streamFeatures & SaslMechExternal && m_availableSaslMechs & SaslMechExternal )
        {
          notifyStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechExternal );
        }
#ifdef _WIN32
        else if( m_doAuth && m_streamFeatures & SaslMechGssapi && m_availableSaslMechs & SaslMechGssapi )
        {
          notifyStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechGssapi );
        }
#endif
        else if( m_doAuth && m_streamFeatures & SaslMechAnonymous
                 && m_availableSaslMechs & SaslMechAnonymous )
        {
          notifyStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechAnonymous );
        }
        else
        {
          notifyStreamEvent( StreamEventFinished );
          connected();
        }
      }
      else if( m_streamFeatures & StreamFeatureIqAuth )
      {
        notifyStreamEvent( StreamEventAuthentication );
        nonSaslLogin();
      }
      else
      {
        logInstance().log( LogLevelError, LogAreaClassClient,
                           "fallback: the server doesn't support any auth mechanisms we know about" );
        disconnect( ConnNoSupportedAuth );
      }
    }
    else if( ( tag->name() == "proceed" ) && tag->hasAttribute( XMLNS, XMLNS_STREAM_TLS ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "starting TLS handshake..." );

      if( m_encryption )
      {
        m_encryptionActive = true;
        m_encryption->handshake();
      }
    }
    else if( ( tag->name() == "failure" ) && tag->hasAttribute( XMLNS, XMLNS_STREAM_TLS ) )
    {
      logInstance().log( LogLevelError, LogAreaClassClient, "TLS handshake failed (server-side)!" );
      disconnect( ConnTlsFailed );
    }
    else if( ( tag->name() == "failure" ) && tag->hasAttribute( XMLNS, XMLNS_COMPRESSION ) )
    {
      logInstance().log( LogLevelError, LogAreaClassClient, "stream compression init failed!" );
      disconnect( ConnCompressionFailed );
    }
    else if( ( tag->name() == "compressed" ) && tag->hasAttribute( XMLNS, XMLNS_COMPRESSION ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "stream compression inited" );
      m_compressionActive = true;
      header();
    }
    else if( ( tag->name() == "challenge" ) && tag->hasAttribute( XMLNS, XMLNS_STREAM_SASL ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "processing SASL challenge" );
      processSASLChallenge( tag->cdata() );
    }
    else if( ( tag->name() == "failure" ) && tag->hasAttribute( XMLNS, XMLNS_STREAM_SASL ) )
    {
      logInstance().log( LogLevelError, LogAreaClassClient, "SASL authentication failed!" );
      processSASLError( tag );
      disconnect( ConnAuthenticationFailed );
    }
    else if( ( tag->name() == "success" ) && tag->hasAttribute( XMLNS, XMLNS_STREAM_SASL ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "SASL authentication successful" );
      setAuthed( true );
      header();
    }
    else
      return false;

    return true;
  }

  int Client::getStreamFeatures( Tag *tag )
  {
    if( tag->name() != "stream:features" )
      return 0;

    int features = 0;

    if( tag->hasChild( "starttls", XMLNS, XMLNS_STREAM_TLS ) )
      features |= StreamFeatureStartTls;

    if( tag->hasChild( "mechanisms", XMLNS, XMLNS_STREAM_SASL ) )
      features |= getSaslMechs( tag->findChild( "mechanisms" ) );

    if( tag->hasChild( "bind", XMLNS, XMLNS_STREAM_BIND ) )
      features |= StreamFeatureBind;

    if( tag->hasChild( "session", XMLNS, XMLNS_STREAM_SESSION ) )
      features |= StreamFeatureSession;

    if( tag->hasChild( "auth", XMLNS, XMLNS_STREAM_IQAUTH ) )
      features |= StreamFeatureIqAuth;

    if( tag->hasChild( "register", XMLNS, XMLNS_STREAM_IQREGISTER ) )
      features |= StreamFeatureIqRegister;

    if( tag->hasChild( "compression", XMLNS, XMLNS_STREAM_COMPRESS ) )
      features |= getCompressionMethods( tag->findChild( "compression" ) );

    if( features == 0 )
      features = StreamFeatureIqAuth;

    return features;
  }

  int Client::getSaslMechs( Tag *tag )
  {
    int mechs = SaslMechNone;

    if( tag->hasChildWithCData( "mechanism", "DIGEST-MD5" ) )
      mechs |= SaslMechDigestMd5;

    if( tag->hasChildWithCData( "mechanism", "PLAIN" ) )
      mechs |= SaslMechPlain;

    if( tag->hasChildWithCData( "mechanism", "ANONYMOUS" ) )
      mechs |= SaslMechAnonymous;

    if( tag->hasChildWithCData( "mechanism", "EXTERNAL" ) )
      mechs |= SaslMechExternal;

    if( tag->hasChildWithCData( "mechanism", "GSSAPI" ) )
      mechs |= SaslMechGssapi;

    return mechs;
  }

  int Client::getCompressionMethods( Tag *tag )
  {
    int meths = 0;

    if( tag->hasChildWithCData( "method", "zlib" ) )
      meths |= StreamFeatureCompressZlib;

    if( tag->hasChildWithCData( "method", "lzw" ) )
      meths |= StreamFeatureCompressDclz;

    return meths;
  }

  void Client::handleIqID( IQ *iq, int context )
  {
    switch( context )
    {
      case ResourceBind:
        processResourceBind( iq );
        break;
      case SessionEstablishment:
        processCreateSession( iq );
        break;
      default:
        break;
    }
  }

  void Client::bindResource()
  {
    if( !m_resourceBound )
    {
      const std::string& id = getID();
      IQ *iq = new IQ( IQ::Set, JID(), id, XMLNS_STREAM_BIND, "bind" );
      if( !resource().empty() )
        new Tag( iq->query(), "resource", resource() );

      trackID( this, id, ResourceBind );
      send( iq );
    }
  }

  void Client::processResourceBind( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Result:
      {
        Tag *bind = iq->query();
        if( !bind )
        {
          notifyOnResourceBindError( RbErrorUnknownError );
          break;
        }

        Tag *jid = bind->findChild( "jid" );
        if( !jid )
        {
          notifyOnResourceBindError( RbErrorUnknownError );
          break;
        }

        m_jid.setJID( jid->cdata() );
        m_resourceBound = true;

        if( m_streamFeatures & StreamFeatureSession )
          createSession();
        else
          connected();
        break;
      }
      case IQ::Error:
      {
        Tag *error = iq->findChild( "error" );
        if( iq->hasChild( "error", TYPE, "modify" )
            && error->hasChild( "bad-request", XMLNS, XMLNS_XMPP_STANZAS ) )
        {
          notifyOnResourceBindError( RbErrorBadRequest );
        }
        else if( iq->hasChild( "error", TYPE, "cancel" ) )
        {
          if( error->hasChild( "not-allowed", XMLNS, XMLNS_XMPP_STANZAS ) )
            notifyOnResourceBindError( RbErrorNotAllowed );
          else if( error->hasChild( "conflict", XMLNS, XMLNS_XMPP_STANZAS ) )
            notifyOnResourceBindError( RbErrorConflict );
          else
            notifyOnResourceBindError( RbErrorUnknownError );
        }
        else
          notifyOnResourceBindError( RbErrorUnknownError );
        break;
      }
      default:
        break;
    }
  }

  void Client::createSession()
  {
    notifyStreamEvent( StreamEventSessionCreation );
    const std::string& id = getID();
    IQ *iq = new IQ( IQ::Set, JID(), id, XMLNS_STREAM_SESSION, "session" );
    trackID( this, id, SessionEstablishment );
    send( iq );
  }

  void Client::processCreateSession( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Result:
      {
        connected();
        break;
      }
      case IQ::Error:
      {
        Tag *error = iq->findChild( "error" );
        if( iq->hasChild( "error", TYPE, "wait" )
            && error->hasChild( "internal-server-error", XMLNS, XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ScErrorInternalServerError );
        }
        else if( iq->hasChild( "error", TYPE, "auth" )
                 && error->hasChild( "forbidden", XMLNS, XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ScErrorForbidden );
        }
        else if( iq->hasChild( "error", TYPE, "cancel" )
                 && error->hasChild( "conflict", XMLNS, XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ScErrorConflict );
        }
        else
          notifyOnSessionCreateError( ScErrorUnknownError );
        break;
      }
      default:
        break;
    }
  }

  void Client::negotiateCompression( StreamFeature method )
  {
    Tag *t = new Tag( "compress" );
    t->addAttribute( XMLNS, XMLNS_COMPRESSION );

    if( method == StreamFeatureCompressZlib )
      new Tag( t, "method", "zlib" );

    if( method == StreamFeatureCompressDclz )
      new Tag( t, "method", "lzw" );

    send( t );
  }

  void Client::addPresenceExtension( StanzaExtension *se )
  {
    m_presenceExtensions.push_back( se );
  }

  void Client::removePresenceExtensions()
  {
    util::clear( m_presenceExtensions );
  }

  void Client::setPresence( Presence::PresenceType presence, int priority, const std::string& msg )
  {
    m_presence = presence;
    m_status = msg;

    if( priority < -128 )
      m_priority = -128;
    else if( priority > 127 )
      m_priority = 127;
    else
      m_priority = priority;

    if( state() >= StateConnected )
      sendPresence();
  }

  void Client::disableRoster()
  {
    m_manageRoster = false;
    delete m_rosterManager;
    m_rosterManager = 0;
  }

  void Client::nonSaslLogin()
  {
    if( !m_auth )
      m_auth = new NonSaslAuth( this );
    m_auth->doAuth( m_sid );
  }

  void Client::sendPresence()
  {
    if( m_presence != Presence::Invalid &&
        m_presence != Presence::Unavailable )
    {
      Presence* p = new Presence( m_presence, JID(), m_status, m_priority );

      StanzaExtensionList::const_iterator it = m_presenceExtensions.begin();
      for( ; it != m_presenceExtensions.end(); ++it )
      {
        p->addChild( (*it)->tag() );
      }

      send( p );
    }
  }

  void Client::connected()
  {
    if( m_authed )
    {
      if( m_manageRoster )
      {
        notifyStreamEvent( StreamEventRoster );
        m_rosterManager->fill();
      }
      else
        rosterFilled();
    }
    else
    {
      notifyStreamEvent( StreamEventFinished );
      notifyOnConnect();
    }
  }

  void Client::rosterFilled()
  {
    sendPresence();
    notifyStreamEvent( StreamEventFinished );
    notifyOnConnect();
  }

  void Client::disconnect()
  {
    disconnect( ConnUserDisconnected );
  }

  void Client::disconnect( ConnectionError reason )
  {
    m_resourceBound = false;
    m_authed = false;
    m_streamFeatures = 0;
    ClientBase::disconnect( reason );
  }

  void Client::cleanup()
  {
    m_authed = false;
    m_resourceBound = false;
    m_streamFeatures = 0;
  }

}

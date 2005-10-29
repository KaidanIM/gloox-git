/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef CLIENT_H__
#define CLIENT_H__

#include "clientbase.h"

#include "iqhandler.h"

#include <string>

namespace gloox
{

  class RosterManager;
  class Disco;
  class NonSaslAuth;
  class Stanza;

  /**
   * @brief This class implements a Jabber Client.
   *
   * It supports SASL (Authentication) as well as TLS (Encryption), which can be switched on/off
   * separately. They are used automatically if the server supports them.
   *
   * To use, create a new Client instance and feed it connection credentials, either in the Constructor or
   * afterwards using the setters. You should then register packet handlers implementing the corresponding
   * Interfaces (ConnectionListener, PresenceHandler, MessageHandler, IqHandler, SubscriptionHandler),
   * and call @ref connect() to establish the connection to the server.<br>
   *
   * Usage example:
   * @code
   * void Class::doIt()
   * {
   *   Client* j = new Client( "user@server/resource", "password" );
   *   j->registerPresenceHandler( this );
   *   j->setVersion( "TestProg", "1.0" );
   *   j->setIdentity( "client", "bot" );
   *   j->setAutoPresence( true );
   *   j->setInitialPriority( 5 );
   *   j->connect();
   * }
   *
   * virtual void Class::presenceHandler( Stanza *stanza )
   * {
   *   // handle incoming presence packets here
   * }
   * @endcode
   *
   * By default, the library handles a few (incoming) IQ namespaces on the application's behalf. These
   * include:
   * @li jabber:iq:roster: by default the server-side roster is fetched and handled. Use
   * @ref rosterManager() and @ref RosterManager to interact with the Roster.
   * @li JEP-0092 (Software Version): If no version is specified, a name of "based on gloox" with
   * gloox's current version is announced.
   * @li JEP-0030 (Service Discovery): All supported/available services are announced. No items are
   * returned.
   * @note By default a priority of -1 is sent along with the initial presence. That means no message
   * stanzas will be received (from compliant servers). Use @ref setInitialPriority() to set a different
   * value. Also, no initial presence is sent which is usually required for a client to show up as
   * 'online' in their contact's contact list.
   *
   * @author Jakob Schroeter <js@camaya.net>
   */
  class GLOOX_EXPORT Client : public ClientBase
  {
    public:

      friend class NonSaslAuth;
      friend class Parser;

      /**
       * Constructs a new Client which can be used for account registration.
       * SASL and TLS are on by default. The port will be determined by looking up SRV records.
       * Alternatively, you can set it be calling @ref setPort().
       * @param server The server to connect to.
       */
      Client( const std::string& server );

      /**
       * Constructs a new Client.
       * SASL and TLS are on by default. This should be the default constructor for most use cases.
       * The server address will be taken from the JID. The actual host will be resolved using SRV
       * records. The domain part of the JID is used as a fallback in case no SRV record is found, or
       * you can set the server address separately by calling @ref setServer().
       * @param jid A full Jabber ID used for connecting to the server.
       * @param password The password used for authentication.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      Client( const JID& jid, const std::string& password, int port = -1 );

      /**
       * Constructs a new Client.
       * SASL and TLS are on by default.
       * The actual host will be resolved using SRV records. The server value is used as a fallback
       * in case no SRV record is found.
       * @param username The username/local part of the JID.
       * @param resource The resource part of the JID.
       * @param password The password to use for authentication.
       * @param server The Jabber ID'S server part and the host name to connect to. If those are different
       * for your setup, use the second constructor instead.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      Client( const std::string& username, const std::string& password,
              const std::string& server, const std::string& resource,
              int port = -1 );

      /**
       * Virtual destructor.
       */
      virtual ~Client();

      /**
       * Use this function to @b re-try to bind a resource only in case you were notified about an
       * error by means of ConnectionListener::onResourceBindError().
       */
      void bindResource();

      /**
       * Returns the current prepped username.
       * @return The username used to connect.
       */
      virtual const std::string username() const { return m_jid.username(); };

      /**
       * Returns the current prepped resource.
       * @return The resource used to connect.
       */
      std::string const resource() const { return m_jid.resource(); };

      /**
       * Returns the current priority.
       * @return The priority of the current resource.
       */
      int priority() const { return m_priority; };

      /**
       * Sets the username to use to connect to the XMPP server.
       * @param username The username to authenticate with.
       */
      void setUsername( const std::string &username ) { m_jid.setUsername( username ); };

      /**
       * Sets the resource to use to connect to the XMPP server.
       * @param resource The resource to use to log into the server.
       */
      void setResource( const std::string &resource ) { m_jid.setResource( resource ); };

      /**
       * Set initial priority. Legal values: -128 <= priority <= 127
       * @param priority The initial priority value.
       */
      void setInitialPriority( int priority );

      /**
       * Enables/disables the automatic sending of a presence packet
       * upon successful authentication @em before the ConnectionListeners
       * are notified. Default: off
       * @param autoPresence Whether to switch AutoPresence on or off.
       */
      void setAutoPresence( bool autoPresence ) { m_autoPresence = autoPresence; };

      /**
       * This is a temporary hack to allow login to google talk. You must set this to true
       * to avoid SASL PLAIN login, which fails. Google talk does not announce availability
       * of non-SASL auth which is why it has to be enabled explicitely.
       * @param force Whether to force non-SASL auth. Default @b true.
       * @deprecated
       */
      void setForceNonSasl( bool force = true ) { m_forceNonSasl = force; };

      /**
       * Disables automatic handling of disco queries.
       * There is currently no way to re-enable disco query-handling.
       * @note This disables the browsing capabilities because
       * both use the same @c Disco object.
       */
      void disableDisco();

      /**
       * Disables the automatic roster management.
       * You have to keep track of incoming presence yourself if
       * you want to have a roster.
       */
      void disableRoster();

      /**
       * This function gives access to the @c RosterManager object.
       * @return A pointer to the RosterManager.
       */
      RosterManager* rosterManager();

      /**
       * This function gives access to the @c Disco object.
       * @return A pointer to the Disco object.
       */
      Disco* disco();

    protected:
      void nonSaslLogin();

    private:
      enum StreamFeaturesEnum
      {
        STREAM_FEATURE_BIND             =   1, /**< The server supports resource binding. */
        STREAM_FEATURE_SESSION          =   2, /**< The server supports sessions. */
        STREAM_FEATURE_SASL_DIGESTMD5   =   4, /**< The server supports SASL DIGEST-MD5 mechanism. */
        STREAM_FEATURE_SASL_PLAIN       =   8, /**< The server supports SASL PLAIN mechanism. */
        STREAM_FEATURE_SASL_ANONYMOUS   =  16, /**< The server supports SASL ANONYMOUS mechanism. */
        STREAM_FEATURE_STARTTLS         =  32, /**< The server supports &lt;starttls&gt;. */
        STREAM_FEATURE_IQREGISTER       =  64, /**< The server supports JEP-0077 (In-Band
                                                * Registration). */
        STREAM_FEATURE_IQAUTH           = 128, /**< The server supports JEP-0078 (Non-SASL
                                                * Authentication). */
        STREAM_FEATURE_ACK              = 256, /**< The server supports JEPAck (experimental). */
        STREAM_FEATURE_COMPRESS_ZLIB    = 512  /**< The server supports JEP-0138 (Stream
                                                * Compression) (Zlib)*/
      };

      virtual void handleStartNode() {};
      virtual bool handleNormalNode( Stanza *stanza );
      int getStreamFeatures( Stanza *stanza );
      int getSaslMechs( Tag *tag );
      int getCompressionMethods( Tag *tag );
      void processResourceBind( Stanza *stanza );
      void processCreateSession( Stanza *stanza );
      void sendInitialPresence();
      void createSession();
      void negotiateCompression( StreamFeaturesEnum method );
      void connected();

      void init();

      RosterManager *m_rosterManager;
      NonSaslAuth *m_auth;
      Disco *m_disco;

      bool m_resourceBound;
      bool m_autoPresence;
      bool m_forceNonSasl;
      bool m_manageRoster;
      bool m_handleDisco;

      int m_streamFeatures;
      int m_priority;

  };

}

#endif // CLIENT_H__

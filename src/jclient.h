/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
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


#ifndef JCLIENT_H__
#define JCLIENT_H__

#include "clientbase.h"

#include "iqhandler.h"

#include <string>
using namespace std;

namespace gloox
{

  class RosterManager;
  class Disco;
  class NonSaslAuth;
  class ConnectionListener;

  /**
   * This class implements a Jabber Client.
   * It supports SASL (Authentication) as well as TLS (Encryption), which can be switched on/off separately.
   * It uses a thread to poll for new data arriving over the network which is then fed into the XML stream
   * parser.
   *
   * To use, create a new JClient instance and feed it connection credentials, either in the Constructor or
   * afterwards using the setters. You should then register packet handlers implementing the corresponding
   * Interfaces (ConnectionListener, PresenceHandler, MessageHandler, IqHandler, SubscriptionHandler),
   * and call @ref connect() to establish the connection to the server.<br>
   *
   * Usage example:
   * @code
   * void Class::doIt()
   * {
   *   JClient* j = new JClient( "user", "resource", "password", "server" );
   *   j->registerPresenceHandler( this );
   *   j->setVersion( "TestProg", "1.0" );
   *   j->setIdentity( "client", "bot" );
   *   j->connect();
   * }
   *
   * virtual void Class::presenceHandler( iksid* from, iksubtype type, ikshowtype show, const char* msg )
   * {
   *   // handle incoming presence packets here
   * }
   * @endcode
   *
   * For debugging purposes you might want to @ref setDebug() and set_log_hook(). <br>
   * By default, the library handles a few (incoming) IQ namespaces on the application's behalf. These
   * include:
   * @li jabber:iq:roster: by default the server-side roster is fetched and handled. Use @ref rosterManager()
   * and @ref RosterManager to interact with the Roster.
   * @li JEP-0092 (Software Version): If no version is specified, a name of "based on gloox" with
   * gloox's current version is announced.
   * @li JEP-0030 (Service Discovery): All supported/available services are announced. No items are returned.
   * @note By default a priority of -1 is sent along with the initial presence. That means no message stanzas
   * will be received (from compliant servers). Use @ref setInitialPriority() to set a different value.
   *
   * @author Jakob Schroeter <js@camaya.net>
   */
  class JClient : public ClientBase, ConnectionListener
  {
    public:

      friend class NonSaslAuth;

      /**
       * Describes the possible error conditions on XMPP level.
       * @todo Add missing values.
       */
      enum ErrorEnum
      {
        FEATURE_NOT_IMPLEMENTED,      /**< The requested feature is either not availabale or not
                                       * implemented. */
        ITEM_NOT_FOUND,               /**< The requested item does not exist. Privacy is a non-issue. */
        SERVICE_UNAVAILABLE           /**< The requested protocol is not supported. Privacy is an issue. */
      };

      enum StreamFeaturesEnum
      {
        STREAM_FEATURE_BIND             =  1,  /**< The server supports resource binding. */
        STREAM_FEATURE_SESSION          =  2,  /**< The server supports sessions. */
        STREAM_FEATURE_SASL_DIGESTMD5   =  4,  /**< The server supports SASL mechanism Digest-MD5. */
        STREAM_FEATURE_SASL_PLAIN       =  8,  /**< The server supports SASL mechanism Plain. */
        STREAM_FEATURE_STARTTLS         = 16,  /**< The server supports &lt;startls&gt;. */
        STREAM_FEATURE_IQREGISTER       = 32,  /**< The server supports JEP-0077 (In-Band Registration). */
        STREAM_FEATURE_IQAUTH           = 64   /**< The server supports JEP-0078 (Non-SASL Authentication). */
      };

      /**
       * Constructs a new JClient.
       * SASL and TLS are on by default. No further initialisations are made. Don't forget to
       * set  at least the server (and probably username, password and resource) using the corresponding
       * setters, else @ref connect() will do nothing.
       */
      JClient();

      /**
       * Constructs a new JClient.
       * SASL and TLS are on by default.
       * The server address will be taken from the JID. The actual host will be resolved using SRV
       * records. The domain part of the JID is used as a fallback in case no SRV record is found.
       * @param id A full Jabber ID used for connecting to the server.
       * @param password The password used for authentication.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      JClient( const std::string& id, const std::string& password, int port = -1 );

      /**
       * Constructs a new JClient.
       * SASL and TLS are on by default.
       * The actual host will be resolved using SRV records. The server value is used as a fallback
       * in case no SRV record is found.
       * @param username The username/local part of the JID.
       * @param resource The resource part of the JID.
       * @param password The password to use for authentication.
       * @param server The jabber server's address or host name to connect to.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      JClient( const std::string& username, const std::string& password,
              const std::string& server, const std::string& resource,
              int port = -1 );

      /**
       * Destructor.
       */
      virtual ~JClient();

      /**
       * Returns the current prepped username.
       * @return The username used to connect.
       */
      virtual std::string username() { return Prep::nodeprep( m_username ); };

      /**
       * Returns the current prepped resource.
       * @return The resource used to connect.
       */
      std::string resource() { return Prep::resourceprep( m_resource ); };

      /**
       * Returns the current priority.
       * @return The priority of the current resource.
       */
      int priority() { return m_priority; };

      /**
       * Returns the current complete jabber id.
       * @return The complete jabber id, composed of username, server and resource.
       */
      std::string jid();

      /**
       * Returns the current complete jabber id.
       * @return The complete jabber id, composed of username, server and resource.
       */
      iksid* parsedJid();

      // FIXME: setters have to update each other, e.g. username, server, resource --> jid
      /**
       * Sets the username to use to connect to the XMPP server.
       * @param username The username to authenticate with.
       */
      void setUsername( const std::string &username ) { m_username = username; };

      /**
       * Sets the resource to use to connect to the XMPP server.
       * @param resource The resource to use to log into the server.
       */
      void setResource( const std::string &resource ) { m_resource = resource; };

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
       * Send the initial Presence. This can be done only once after
       * a connection is established.
       * @todo Enhance to allow for all presence types to be sent.
       */
      void sendPresence( int priority, ikshowtype type, const std::string& msg );

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

      // reimplemented from ConnectionListener
      virtual void onConnect();

    private:
      int getStreamFeatures( iks* x );
      int getSaslMechs( iks* x );
      void sendInitialPresence();
      void bindResource();
      void createSession();

      virtual void on_stream( int type, iks* node );

      void nonSaslLogin( const char* sid = 0L );
      void init();

      RosterManager* m_rosterManager;
      Disco* m_disco;
      NonSaslAuth *m_auth;

      bool m_authorized;
      std::string m_username;
      std::string m_resource;
      std::string m_jid;
      std::string m_sid;
      bool m_autoPresence;
      bool m_handleDisco;
      bool m_manageRoster;
      int m_priority;

      int m_streamFeatures;

      friend int sessionHook( JClient* stream, ikspak* pak );
      friend int bindHook( JClient* stream, ikspak* pak );
  };

};

#endif // JCLIENT_H__

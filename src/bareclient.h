/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef BARECLIENT_H__
#define BARECLIENT_H__

#include "macros.h"
#include "clientbase.h"
#include "nonsaslauth.h"


namespace gloox
{
  class NonSaslAuth;

  class GLOOX_API BareClient : public ClientBase
  {
    public:

      friend class NonSaslAuth;

      /**
       *
       */
      BareClient( const JID& jid, const std::string& password, int port = -1 );

      /**
       * Virtual Destructor.
       */
      virtual ~BareClient();

        /**
       * Use this function to @b re-try to bind a resource only in case you were notified about an
       * error by means of ConnectionListener::onResourceBindError().
       * You may (or should) use setResource() before.
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
      GLOOX_DEPRECATED void setForceNonSasl( bool force = true ) { m_forceNonSasl = force; };

    protected:
      BareClient( const std::string& ns, const std::string& password,
                  const std::string& server, int port = -1 );
      BareClient( const std::string& ns, const std::string& server, int port = -1 );

      virtual void connected();

      /**
       * Initiates non-SASL login.
       */
      void nonSaslLogin();

      void sendInitialPresence();

      bool m_autoPresence;

    private:
      enum StreamFeaturesEnum
      {
        STREAM_FEATURE_BIND             =    1, /**< The server supports resource binding. */
        STREAM_FEATURE_SESSION          =    2, /**< The server supports sessions. */
        STREAM_FEATURE_SASL_DIGESTMD5   =    4, /**< The server supports SASL DIGEST-MD5 mechanism. */
        STREAM_FEATURE_SASL_PLAIN       =    8, /**< The server supports SASL PLAIN mechanism. */
        STREAM_FEATURE_SASL_ANONYMOUS   =   16, /**< The server supports SASL ANONYMOUS mechanism. */
        STREAM_FEATURE_SASL_EXTERNAL    =   32, /**< The server supports SASL EXTERNAL mechanism. */
        STREAM_FEATURE_STARTTLS         =   64, /**< The server supports &lt;starttls&gt;. */
        STREAM_FEATURE_IQREGISTER       =  128, /**< The server supports JEP-0077 (In-Band
                                                 * Registration). */
        STREAM_FEATURE_IQAUTH           =  256, /**< The server supports JEP-0078 (Non-SASL
                                                 * Authentication). */
        STREAM_FEATURE_ACK              =  512, /**< The server supports JEPAck (experimental). */
        STREAM_FEATURE_COMPRESS_ZLIB    = 1024, /**< The server supports JEP-0138 (Stream
                                                 * Compression) (Zlib)*/
        STREAM_FEATURE_COMPRESS_DCLZ    = 2048  /**< The server supports JEP-0138 (Stream
                                                 * Compression) (LZW/DCLZ)*/
      };

      virtual void handleStartNode() {};
      virtual bool handleNormalNode( Stanza *stanza );
      int getStreamFeatures( Stanza *stanza );
      int getSaslMechs( Tag *tag );
      int getCompressionMethods( Tag *tag );
      void processResourceBind( Stanza *stanza );
      void processCreateSession( Stanza *stanza );
      void createSession();
      void negotiateCompression( StreamFeaturesEnum method );

      NonSaslAuth *m_auth;

      bool m_resourceBound;
      bool m_forceNonSasl;
      bool m_doAuth;
      int m_streamFeatures;
      int m_priority;

  };

}

#endif // BARECLIENT_H__

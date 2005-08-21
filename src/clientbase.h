/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
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



#ifndef CLIENTBASE_H__
#define CLIENTBASE_H__

#include "gloox.h"

#include "connectionlistener.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "rosterlistener.h"
#include "subscriptionhandler.h"
#include "prep.h"
#include "jid.h"


namespace gloox
{

  class string;
  class map;
  class list;
  class Connection;
  class Packet;
  class Tag;
  class Stanza;

  /**
   * This is a common base class for a jabber client and a jabber component. It manages connection
   * establishing, authentication, filter registrationa and invocation.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class ClientBase
  {

    friend class Parser;

    public:

      /**
       * Constructs a new ClientBase.
       * You should not need to use this class directly. Use JClient or JComponent instead.
       * @param ns The namespace which qualifies the stream. Either jabber:client or jabber:component:*
       */
      ClientBase( const std::string& ns );

      /**
       * Constructs a new ClientBase.
       * You should not need to use this class directly. Use @ref JClient or @ref JComponent instead.
       * @param ns The namespace which qualifies the stream. Either jabber:client or jabber:component:*
       * @param password The password to use for further authentication.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      ClientBase( const std::string& ns, const std::string& password, int port = -1 );

      /**
       * Constructs a new ClientBase.
       * You should not need to use this class directly. Use @ref JClient or @ref JComponent instead.
       * @param ns The namespace which qualifies the stream. Either jabber:client or jabber:component:*
       * @param password The password to use for further authentication.
       * @param server The server to connect to.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      ClientBase( const std::string& ns, const std::string& password,
                  const std::string& server, int port = -1 );

      /**
       * Virtual destrcuctor.
       */
      virtual ~ClientBase();

      /**
       * Initiates the connection to a server. This function blocks as long as a connection is
       * established.
       * @return @b False if prerequisits are not met (server not set), @b true otherwise.
       */
      bool connect();

      /**
       * Disconnects from the server. A reason can be given which is broadcasted to
       * ConnectionListeners.
       * @param reason The reason for the disconnection.
       */
      void disconnect( ConnectionState reason );

      /**
       * Reimplement this function to provide a username for connection purposes.
       * @return The username.
       */
      virtual const std::string username() const {};

      /**
       * Returns the current jabber id.
       * @return The Jabber ID.
       */
      const JID& jid() const { return m_jid; };

      /**
       * Switches usage of SASL on/off (if available). Default: on
       * @param sasl Whether to switch SASL usage on or off.
       */
      void setSasl( bool sasl ) { m_sasl = sasl; };

      /**
       * Switches usage of TLS on/off (if available). Default: on
       * @param tls Whether to switch TLS usage on or off.
       */
      void setTls( bool tls ) { m_tls = tls; };

      /**
       * Sets the port to connect to. This is not necessary if either the default port (5222) is used
       * or SRV records exist which will be resolved.
       * @param port The port to connect to.
       */
      void setPort( int port ) { m_port = port; };

      /**
       * Sets the XMPP server to connect to.
       * @param server The server to connect to. Either IP or fully qualified domain name.
       */
      void setServer( const std::string &server ) { m_jid.setServer( server ); };

      /**
       * Sets the password to use to connect to the XMPP server.
       * @param password The password to use for authentication.
       */
      void setPassword( const std::string &password ) { m_password = password; };

      /**
       * Returns the current prepped server.
       * @return The server used to connect.
       */
      const std::string server() const { return m_jid.server(); };

      /**
       * Returns the current SASL status.
       * @return The current SASL status.
       */
      bool sasl() const { return m_sasl; };

      /**
       * Returns the current TLS status.
       * @return The current TLS status.
       */
      bool tls() const { return m_tls; };

      /**
       * Returns the current port.
       * @return The port used to connect.
       */
      int port() const { return m_port; };

      /**
       * Returns the current password.
       * @return The password used to connect.
       */
      virtual const std::string password() const { return m_password; };

      /**
       * Creates a std::string. This std::string is unique in the current instance and
       * can be used as an ID for queries.
       * @return A unique std::string suitable for query IDs.
       * @deprecated
       */
      const std::string getID() { printf( "don't use ClientBase's getID(), use Packet's instead!\n" ); return "abc"; };

      /**
       * Sends a given Tag over an steablished connection.
       * @param tag The Tag to send.
       */
      void send( const Tag& tag );

      /**
       * Sends a given string over an established connection.
       * @param xml The string to send.
       */
      void send( const std::string& xml );

      /**
       * Sends the given xml via the established connection.
       * @note x is automatically free()'ed.
       * @param x The xml data.
       * @deprecated
       */
      void send( iks *x );

      /**
       * Returns the current connection state.
       * @return The state of the connection.
       */
      ConnectionState state() const;

      /**
       * Sets the state of the connection. This can be used to indicate successful authentication.
       * A parameter of 'STATE_DISCONNECTED' will not disconnect.
       * @param state The new connection state.
       */
      void setState( ConnectionState state );

      /**
       * Registers @c cl as object that receives connection notifications.
       * @param cl The object to receive connection notifications.
       */
      void registerConnectionListener( ConnectionListener *cl );

      /**
       * Registers @c ih as object that receives Iq packet notifications for namespace
       * @c xmlns. Only one handler per namespace is possible.
       * @param ih The object to receive Iq packet notifications.
       * @param xmlns The namespace the object handles.
       */
      void registerIqHandler( IqHandler *ih, const std::string& xmlns );

      /**
       * Use this function to be notified of incoming IQ stanzas with the given value of the @b id
       * attribute.
       * Since IDs are supposed to be unique, this notification works only once.
       * @param ih The IqHandler to receive notifications.
       * @param id The id to track.
       * @param context A value that allows for restoring context.
       */
      void trackID( IqHandler *ih, const std::string& id, int context );

      /**
       * Registers @c mh as object that receives Message packet notifications.
       * @param mh The object to receive Message packet notifications.
       */
      void registerMessageHandler( MessageHandler *mh );

      /**
       * Registers @c ph as object that receives Presence packet notifications.
       * @param ph The object to receive Presence packet notifications.
       */
      void registerPresenceHandler( PresenceHandler *ph );

      /**
       * Registers @c sh as object that receives Subscription packet notifications.
       * @param sh The object to receive Subscription packet notifications.
       */
      void registerSubscriptionHandler( SubscriptionHandler *sh );

      /**
       * Removes the given object from the list of connection listeners.
       * @param cl The object to remove from the list.
       */
      void removeConnectionListener( ConnectionListener *cl );

      /**
       * Removes the handler for the given namespace from the list of Iq handlers.
       * @param xmlns The namespace to remove from the list.
       */
      void removeIqHandler( const std::string& xmlns );

      /**
       * Removes the given object from the list of message handlers.
       * @param mh The object to remove from the list.
       */
      void removeMessageHandler( MessageHandler *mh );

      /**
       * Removes the given object from the list of presence handlers.
       * @param ph The object to remove from the list.
       */
      void removePresenceHandler( PresenceHandler *ph );

      /**
       * Removes the given object from the list of subscription handlers.
       * @param sh The object to remove from the list.
       */
      void removeSubscriptionHandler( SubscriptionHandler *sh );

    protected:
      void notifyOnResourceBindError( ConnectionListener::ResourceBindError error );
      void notifyOnSessionCreateError( ConnectionListener::SessionCreateError error );
      void notifyOnConnect();
      void notifyOnDisconnect();
      void header();

      /**
       * Returns std::string which is used in the 'to' attribute of the initial stream opening tag.
       * This should be the server's hostname for the @b jabber:client namespace, and the
       * component's hostname for the jabber:component:* namespaces, respectively.
       * @return The host to name in the stream's 'to' attribute. Defaults to the destination server.
       */
      virtual const std::string streamTo() const { return server(); };

      JID m_jid;
      Connection *m_connection;

      std::string m_password;
      std::string m_namespace;
      std::string m_sid;
      bool m_sasl;
      bool m_tls;
      int m_port;

    private:
      virtual void handleStartNode() = 0;
      virtual bool handleNormalNode( const Tag& tag ) = 0;
      Stanza createStanza( const Tag& tag );
      void log( const Tag& tag, bool incoming );

      void notifyIqHandlers( const Stanza& stanza );
      void notifyMessageHandlers( const Stanza& stanza );
      void notifyPresenceHandlers( const Stanza& stanza );
      void notifySubscriptionHandlers( const Stanza& stanza );
      void filter( int type, const Tag& tag );
      void logEvent( const char *data, size_t size, int is_incoming );

      struct TrackStruct
      {
        IqHandler *ih;
        int context;
      };

      typedef std::list<ConnectionListener*>            ConnectionListenerList;
      typedef std::map<const std::string, IqHandler*>   IqHandlerMap;
      typedef std::map<const std::string, TrackStruct>  IqTrackMap;
      typedef std::list<MessageHandler*>                MessageHandlerList;
      typedef std::list<PresenceHandler*>               PresenceHandlerList;
      typedef std::list<SubscriptionHandler*>           SubscriptionHandlerList;

      ConnectionListenerList  m_connectionListeners;
      IqHandlerMap            m_iqNSHandlers;
      IqTrackMap              m_iqIDHandlers;
      MessageHandlerList      m_messageHandlers;
      PresenceHandlerList     m_presenceHandlers;
      SubscriptionHandlerList m_subscriptionHandlers;

      Parser *m_parser;

      int m_idCount;

      friend int presenceHook( ClientBase *cb, ikspak *pak );
      friend int msgHook( ClientBase *cb, ikspak *pak );
      friend int subscriptionHook( ClientBase *cb, ikspak *pak );
      friend int iqHook( ClientBase *cb, ikspak *pak );
      friend int bindHook( ClientBase *cb, ikspak* pak );
      friend int sessionHook( ClientBase *cb, ikspak* pak );
      friend int logHook( ClientBase *cb, const char *data, size_t size, int is_incoming);
  };

};

#endif // CLIENTBASE_H__

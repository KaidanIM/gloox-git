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

#include "connectionlistener.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "rosterlistener.h"
#include "subscriptionhandler.h"

#include "prep.h"

#include "wrapper/stream.h"
using namespace Iksemel;

#include <string>
#include <map>
#include <list>
using namespace std;

#define XMLNS_CLIENT            "jabber:client"
#define XMLNS_COMPONENT_ACCEPT  "jabber:component:accept"
#define XMLNS_COMPONENT_CONNECT "jabber:component:connect"

#define XMLNS_DISCO_INFO        "http://jabber.org/protocol/disco#info"
#define XMLNS_DISCO_ITEMS       "http://jabber.org/protocol/disco#items"
#define XMLNS_ADHOC_COMMANDS    "http://jabber.org/protocol/commands"
#define XMLNS_ROSTER            "jabber:iq:roster"
#define XMLNS_VERSION           "jabber:iq:version"
#define XMLNS_REGISTER          "jabber:iq:register"
#define XMLNS_PRIVACY           "jabber:iq:privacy"
#define XMLNS_AUTH              "jabber:iq:auth"
#define XMLNS_PRIVATE_XML       "jabber:iq:private"
#define XMLNS_BOOKMARKS         "storage:bookmarks"
#define XMLNS_ANNOTATIONS       "storage:rosternotes"

#define XMLNS_STREAM_TLS        "urn:ietf:params:xml:ns:xmpp-starttls"
#define XMLNS_STREAM_SASL       "urn:ietf:params:xml:ns:xmpp-sasl"
#define XMLNS_STREAM_BIND       "urn:ietf:params:xml:ns:xmpp-bind"
#define XMLNS_STREAM_SESSION    "urn:ietf:params:xml:ns:xmpp-session"
#define XMLNS_STREAM_IQAUTH     "http://jabber.org/features/iq-auth"
#define XMLNS_STREAM_IQREGISTER "http://jabber.org/features/iq-register"

#define XMPP_PORT            5222

#define GLOOX_VERSION "0.3"

/**
 * This is the namespace for the gloox library.
 */
namespace gloox
{

  class JThread;

 /**
  * This describes the possible states of a stream.
  */
  enum StateEnum
  {
    STATE_ERROR,                    /**< An error occured. The stream has been closed. */
    STATE_IO_ERROR,                 /**< An I/O error occured. */
    STATE_OUT_OF_MEMORY,            /**< Out of memory. Uhoh. */
    STATE_TLS_FAILED,               /**< TLS negotiation failed. */
    STATE_PARSE_ERROR,              /**< XML parse error. */
    STATE_NO_SUPPORTED_AUTH,        /**< The auth mechanisms the server offers are not supported. */
    STATE_AUTHENTICATION_FAILED,    /**< Authentication failed. Username/password wrong or account does
                                     * not exist. */
    STATE_DISCONNECTED,             /**< The client is in disconnected state. */
    STATE_CONNECTING,               /**< The client is currently trying to establish a connection. */
    STATE_CONNECTED,                /**< The client is connected to the server but authentication is not
                                     * (yet) done. */
    STATE_AUTHENTICATED             /**< The client has successfully authenticated itself to the server. */
  };


  /**
   * THis is a common base class for a jabber client and a jabber component. It includes connection
   * establishing, authentication, filter registrationa and invocation.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class ClientBase : public Stream
  {
    public:
      friend class JThread;

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
       */
      ClientBase( const std::string& ns, const std::string& password );

      /**
       * Constructs a new ClientBase.
       * You should not need to use this class directly. Use @ref JClient or @ref JComponent instead.
       * @param ns The namespace which qualifies the stream. Either jabber:client or jabber:component:*
       * @param password The password to use for further authentication.
       * @param server The server to connect to.
       * @param port The port to connect to. Default: 5222
       */
      ClientBase( const std::string& ns, const std::string& password,
                  const std::string& server, int port = XMPP_PORT );

      /**
       * Virtual destrcuctor.
       */
      virtual ~ClientBase();

      /**
       * Connects to the XMPP server, authenticates and gets the whole thing running.
       * Creates a new thread which receives arriving data and feeds the parser.
       * @param blocking Determines whether connect() blocks. If all your program does
       * is react to incoming events, you should be fine with the default, which is blocking
       * behaviour.
       */
      void connect( bool blocking = true );

      /**
       * Disconnects from the server by ending the receiver thread.
       */
      void disconnect();

      /**
       * Reimplement this function to provide a username for connection purposes.
       * @return The username.
       */
      virtual std::string username() {};

      /**
       * Reimplement this function to provide a JID.
       * @return The JID.
       */
      virtual std::string jid() {};

      /**
       * Returns the current client state.
       * @return The current client state.
       */
      StateEnum state() { return m_state; };

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
       * Sets the port to connect to.
       * @param port The port to connect to.
       */
      void setPort( int port ) { m_port = port; };

      /**
       * Sets the XMPP server to connect to.
       * @param server The server to connect to. Either IP or fully qualified domain name.
       */
      void setServer( const std::string &server ) { m_server = server; };

      /**
       * Sets the password to use to connect to the XMPP server.
       * @param password The password to use for authentication.
       */
      void setPassword( const std::string &password ) { m_password = password; };

      /**
       * Returns the current debug status.
       * @return the current debug status.
       */
      bool debug()           { return m_debug; };

      /**
       * Switches debug output on/off. Default: off
       * @param debug Whether to switch debug output on or off.
       */
      void setDebug( bool debug ) { m_debug = debug; };

      /**
       * Returns string which is used in the 'to' attribute of tzhe initial stream.
       * This should be the server's hostname for the @b jabber:client namespace, and the
       * component's hostname for the jabber:component:* namespaces.
       * @return The host to name in the stream's 'to' attribute.
       */
      virtual const std::string streamTo() { return server(); };

      /**
       * Returns the current prepped server.
       * @return The server used to connect.
       */
      std::string server() { return Prep::nameprep( m_server ); };

      /**
       * Returns the current server. IDNA rules are applied.
       * @return The server used to connect.
       */
      std::string serverIdn() { return Prep::idna( m_server ); };

      /**
       * Returns the current SASL status.
       * @return The current SASL status.
       */
      bool sasl() { return m_sasl; };

      /**
       * Returns the current TLS status.
       * @return The current TLS status.
       */
      bool tls() { return m_tls; };

      /**
       * Returns the current port.
       * @return The port used to connect.
       */
      int port() { return m_port; };

      /**
       * Returns the current password.
       * @return The password used to connect.
       */
      virtual std::string password() { return m_password; };

      /**
       * Creates a string. Thsi String is unique in the current instance and
       * can be used as an id for queries.
       * @return A unique string suitable for query IDs.
       */
      std::string getID();

      /**
       * Sends the given xml via the established connection.
       * @note x is automatically free()'ed.
       * @param x The xml data.
       */
      void send( iks* x );

      /**
       * Registers @c cl as object that receives connection notifications.
       * @param cl The object to receive connection notifications.
       */
      void registerConnectionListener( ConnectionListener* cl );

      /**
       * Registers @c ih as object that receives Iq packet notifications for namespace
       * @c xmlns. Only one handler per namespace is possible.
       * @param ih The object to receive Iq packet notifications.
       * @param xmlns The namespace the object handles.
       */
      void registerIqHandler( IqHandler* ih, const char* xmlns );

      /**
       * Use this function to be notified of incoming IQ stanzas with the given value of the @b id
       * attribute.
       * Since IDs are supposed to be unique, this notification works only once.
       * @param ih The IqHandler to receive notifications.
       * @param id The id to track.
       * @param context A value that allows for restoring context.
       */
      void trackID( IqHandler* ih, const std::string& id, int context );

      /**
       * Registers @c mh as object that receives Message packet notifications.
       * @param mh The object to receive Message packet notifications.
       */
      void registerMessageHandler( MessageHandler* mh );

      /**
       * Registers @c ph as object that receives Presence packet notifications.
       * @param ph The object to receive Presence packet notifications.
       */
      void registerPresenceHandler( PresenceHandler* ph );

      /**
       * Registers @c sh as object that receives Subscription packet notifications.
       * @param sh The object to receive Subscription packet notifications.
       */
      void registerSubscriptionHandler( SubscriptionHandler* sh );

      /**
       * Removes the given object from the list of connection listeners.
       * @param cl The object to remove from the list.
       */
      void removeConnectionListener( ConnectionListener* cl );

      /**
       * Removes the handler for the given namespace from the list of Iq handlers.
       * @param xmlns The namespace to remove from the list.
       */
      void removeIqHandler( const char* xmlns );

      /**
       * Removes the given object from the list of message handlers.
       * @param mh The object to remove from the list.
       */
      void removeMessageHandler( MessageHandler* mh );

      /**
       * Removes the given object from the list of presence handlers.
       * @param ph The object to remove from the list.
       */
      void removePresenceHandler( PresenceHandler* ph );

      /**
       * Removes the given object from the list of subscription handlers.
       * @param sh The object to remove from the list.
       */
      void removeSubscriptionHandler( SubscriptionHandler* sh );

    protected:
      void notifyOnConnect();
      void notifyOnDisconnect();

      void setupFilter();
      virtual void cleanUp();
      void setState( gloox::StateEnum s ) { m_state = s; };

      gloox::StateEnum m_state;
      JThread* m_thread;
      iksfilter* m_filter;

      std::string m_server;
      std::string m_password;
      bool m_sasl;
      bool m_tls;
      bool m_debug;
      int m_port;

    private:
      void notifyIqHandlers( const char* xmlns, ikspak* pak );
      void notifyMessageHandlers( iksid* from, iksubtype type, const char* msg );
      void notifyPresenceHandlers( iksid* from, iksubtype type, ikshowtype show, const char* msg );
      void notifySubscriptionHandlers( iksid* from, iksubtype type, const char* msg );
      virtual void on_log( const char* data, size_t size, int is_incoming );
      iksparser* parser();
      void init();

      virtual void on_stream( int type, iks* node ) {};

      struct TrackStruct
      {
        IqHandler *ih;
        int context;
      };

      typedef list<ConnectionListener*>           ConnectionListenerList;
      typedef map<const std::string, IqHandler*>  IqHandlerMap;
      typedef map<const std::string, TrackStruct> IqTrackMap;
      typedef list<IqHandler*>                    IqHandlerList;
      typedef list<MessageHandler*>               MessageHandlerList;
      typedef list<PresenceHandler*>              PresenceHandlerList;
      typedef list<SubscriptionHandler*>          SubscriptionHandlerList;

      ConnectionListenerList  m_connectionListeners;
      IqHandlerMap            m_iqNSHandlers;
      IqHandlerMap            m_iqFTHandlers;
      IqTrackMap              m_iqIDHandlers;
      IqHandlerList           m_iqHandlers;
      MessageHandlerList      m_messageHandlers;
      PresenceHandlerList     m_presenceHandlers;
      SubscriptionHandlerList m_subscriptionHandlers;

      bool m_blockingConnect;
      int m_idCount;

      friend int presenceHook( ClientBase* stream, ikspak* pak );
      friend int msgHook( ClientBase* stream, ikspak* pak );
      friend int subscriptionHook( ClientBase* stream, ikspak* pak );
      friend int iqHook( ClientBase* stream, ikspak* pak );

  };

};

#endif // CLIENTBASE_H__

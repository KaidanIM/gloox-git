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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#ifndef JCLIENT_H__
#define JCLIENT_H__

#include "connectionlistener.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "subscriptionhandler.h"

#include "wrapper/stream.h"

#include <list>
#include <map>
#include <string>

#define XMLNS_DISCO_INFO  "http://jabber.org/protocol/disco#info"
#define XMLNS_DISCO_ITEMS "http://jabber.org/protocol/disco#items"
#define XMLNS_ROSTER      "jabber:iq:roster"

using namespace std;
using namespace Iksemel;

class JThread;
class Roster;

/**
 * This class implements a Jabber Client.
 * It supports SASL (Authentication) as well as TLS (Encryption), which can be switched on/off separately.
 * It uses a thread to poll for new data arriving over the network which is then fed into the XML stream parser.
 *
 * To use, create a new JClient instance and feed it connection credentials, either in the Constructor or afterwards
 * using the setters. You should then register packet handlers implementing the corresponding
 * Interfaces (ConnectionListener, PresenceHandler, MessageHandler, IqHandler,
 * SubscriptionHandler), and call @ref connect() to establish the connection to the server.<br>
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
 * By default, the library handles a few (incoming) IQ namespaces on the application's behalf. These include:
 * @li JEP-0092 (Software Version) If no version is specified, a name of "based on Jlib" with Jlib's current version is announced.
 * @li JEP-0030 (Service Discovery) All supported/available services are announced. No items are returned.
 * @author Jakob Schroeter <js@camaya.net>
 */
class JClient : public Stream
{
  public:

    friend class JThread;
    /**
     * Describes the current connection status.
     * TODO: Make this private?
     */
    enum state
    {
      STATE_ERROR,                 /**< An error occured. The stream has been closed. */
      STATE_IO_ERROR,              /**< An I/O error occured. */
      STATE_AUTHENTICATION_FAILED, /**< Authentication failed. Username/password wrong or account does not exist. */
      STATE_DISCONNECTED,          /**< The client is in disconnected state. */
      STATE_CONNECTING,            /**< The client is currently trying to establish a connection. */
      STATE_CONNECTED,             /**< The client is conencted to the server but authentication is not yet complete. */
      STATE_AUTHENTICATED          /**< The client has successfully authenticated itself to the server. */
    };

    /**
     * Constructs a new JClient.
     * SASL and TLS are on by default. No further initialisations are made. It is absolutely necessary to
     * set server, username, password and resource using the corresponding setters.
     */
    JClient();

    /**
     * Constructs a new JClient.
     * SASL and TLS are on by default.
     * @param username The username/local part of the JID.
     * @param resource The resource part of the JID.
     * @param password The password to use for authentication.
     * @param server The jabber server's address or host name to connect to.
     * @param port The port to connect to. Default: 5222
     */
    JClient( const std::string username, const std::string resource,
             const std::string password, const std::string server,
             int port = 5222 );

    /**
     * Destructor
     */
    virtual ~JClient();

    /**
     * Returns the current username.
     * @return The username used to connect.
     */
    std::string username() { return m_username; };

    /**
     * Returns the current resource.
     * @return The resource used to connect.
     */
    std::string resource() { return m_resource; };

    /**
     * Returns the current password.
     * @return The password used to connect.
     */
    std::string password() { return m_password; };

    /**
     * Returns the current server.
     * @return The server used to connect.
     */
    std::string server()   { return m_server; };

    /**
     * Returns the current complete jabber id.
     * @return The complete jabber id, composed of username, server and resource.
     */
    std::string jid()      { return ( m_username + "@" + m_server + "/" + m_resource ); };

    /**
     * Returns the current debug status.
     * @return the current debug status.
     */
    bool debug()           { return m_debug; };

    /**
     * Returns the current SASL status.
     * @return The current SASL status.
     */
    bool sasl()            { return m_sasl; };

    /**
     * Returns the current TLS status.
     * @return The current TLS status.
     */
    bool tls()             { return m_tls; };

    /**
     * Returns the current port.
     * @return The port used to connect.
     */
    int port()             { return m_port; };

    /**
     * Returns the current status of AutoPresence.
     * @return The current AutoPresence status.
     */
    bool autoPresence()    { return m_autoPresence; };

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
     * Sets the password to use to connect to the XMPP server.
     * @param password The password to use for authentication.
     */
    void setPassword( const std::string &password ) { m_password = password; };

    /**
     * Sets the XMPP Cserver to connect to.
     * @param server The server to connect to. Either IP or fully qualified domain name.
     */
    void setServer( const std::string &server ) { m_server = server;     };

    /**
     * Switches debug output on/off. Default: off
     * @param debug Whether to switch debug output on or off.
     */
    void setDebug( bool debug ) { m_debug = debug; };

    /**
     * Switches usage of SASL on/off (if available). Default: on
     * @param sasl Whether to switch SASL usage on or off.
     */
    void setSasl( bool sasl ) { m_sasl = sasl;   };

    /**
     * Switches usage of TLS on/off (if available). Default: on
     * @param tls Whether to switch TLS usage on or off.
     */
    void setTls( bool tls ) { m_tls = tls;     };

    /**
     * Sets the port to connect to.
     * @param port The port to connect to.
     */
    void setPort( int port ) { m_port = port;   };

    /**
     * Enables/disables the automatic sending of a presence packet
     * upon successful authentication @em before the ConnectionListeners
     * are notified. Default: on
     * @param autoPresence Whether to switch AutoPresence on or off.
     */
    void setAutoPresence( bool autoPresence ) { m_autoPresence = autoPresence; };

    /**
     * Sets the version of the host application using this library.
     * The library takes care of jabber:iq:version requests. These
     * IQ packets will not be forwarded to the IqHandlers.
     * @param name The name to be returned to inquireing clients.
     * @param version The version to be returned to inquireing clients.
     */
    void setVersion( const char* name, const char* version );

    /**
     * Sets the identity of the this entity.
     * The library uses this information to answer disco#info requests
     * with a correct identity.
     * JEP-0030 requires an entity to have at least one identity. See JEP-0030
     * for more information on categories and types.
     * @param category The entity category of thsi client. Default: client
     * @param type The type of this entity. Default: bot
     */
    void setIdentity( const char* category, const char* type );

    /**
     * Adds a feature to the list of supported Jabber features.
     * The list will be posted as an answer to IQ queries in the
     * "http://jabber.org/protocol/disco#info" namespace.
     * These IQ packets will not be forwarded but answered on the
     * application's behalf, unless @ref disableDiscoInfo() is called. By
     * default, disco(very) queries are handled by the library.
     * By default, all supported, not disabled features are announced.
     * @param feature A feature (namespace) the host app supports.
     */
    void setFeature( const char* feature );

    /**
     * Disables automatic handling of disco#info.
     * All further disco#info IQ packets will be forwarded
     * to the IqHandlers. This is independent of @ref disableDiscoItems().
     * There is no way to re-enable disco#info-handling.
     * @note This does @em not influence the browsing capabilities,
     * but handling of incoming queries only.
     */
    void disableDiscoInfo();

    /**
     * Disables automatic handling of disco#items.
     * All further disco#items IQ packets will be forwarded
     * to the IqHandlers. This is independent of @ref disableDiscoInfo().
     * There is no way to re-enable disco#item-handling.
     * @note This does @em not influence the browsing capabilities,
     * but handling of incoming queries only.
     */
    void disableDiscoItems();

    /**
     * Queries the given JID for general infomation according to
     * JEP-0030 (Service Discovery).
     * @param to The destination-JID of the query.
     * @return A list of capabilities.
     */
    void getDiscoInfo( const char* to );

    /**
     * Queries the given JID for its items according to
     * JEP-0030 (Service Discovery).
     * @param to The destination-JID of the query.
     * @return A list of items.
     */
    void getDiscoItems( const char* to );

    /**
     * Disables the automatic roster management.
     * You have to keep track of incoming presence yourself if
     * you want to have a roster.
     */
    void disableRosterManagement();

    /**
     * Returns the current client state.
     * @return The current client state.
     */
    state clientState();

    /**
     * Sends the given xml via the established connection.
     * @note @ref x is automatically free()'ed.
     * @param x The xml data.
     */
    void send( iks* x );

    /**
     * Sends the given data to the given jid as a message.
     * @param jid The Jabber ID to send the data to.
     * @param data The data to send.
     */
    void send( const char* jid, const char* data );

    /**
     * Connects to the XMPP server, authenticates and gets the whole thing running.
     * Creates a new thread that receives arriving data and feeds the parser.
     * @note This function currently blocks.
     */
    void connect();

    /**
     * Disconnects from the server by ending the receiver thread.
     */
    void disconnect();

    /**
     * This function gives access to the @c Roster object.
     * @return A pointer to the Roster.
     */
    Roster* roster();
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
     * Registers @c ih as object that receives all Iq packet notifications.
     * @param ih The object to receive Iq packet notifications.
     */
    void registerIqHandler( IqHandler* ih );

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
     * Removes the generic IQ handler from the list of generic IQ handlers.
     * @param ih The iQHandler to remove.
     */
    void removeIqHandler( IqHandler* ih );

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

  private:
    void notifyOnConnect();
    void notifyOnDisconnect();
    void notifyIqHandlers( const char* xmlns, ikspak* pak );
    void notifyMessageHandlers( iksid* from, iksubtype type, const char* msg );
    void notifyPresenceHandlers( iksid* from, iksubtype type, ikshowtype show, const char* msg );
    void notifySubscriptionHandlers( iksid* from, iksubtype type, const char* msg );
    iksparser* parser();


    friend void authHook(JClient* stream, ikspak* pak);
    friend void registerHook(JClient* stream, ikspak* pak);
    friend void registeredHook(JClient* stream, ikspak* pak);
    friend void errorHook(JClient* stream, ikspak* pak);
    friend void presenceHook(JClient* stream, ikspak* pak);
    friend void msgHook(JClient* stream, ikspak* pak);
    friend void subscriptionHook(JClient* stream, ikspak* pak);
    friend void iqHook(JClient* stream, ikspak* pak);

    typedef list<ConnectionListener*>     ConnectionListenerList;
    typedef map<const char*, IqHandler*>  IqHandlerMap;
    typedef list<IqHandler*>              IqHandlerList;
    typedef list<MessageHandler*>         MessageHandlerList;
    typedef list<PresenceHandler*>        PresenceHandlerList;
    typedef list<SubscriptionHandler*>    SubscriptionHandlerList;
    typedef list<const char*>             CharList;
    typedef map<std::string, std::string> StringMap;

    virtual void on_log( const char* data, size_t size, int is_incoming );
    virtual void on_stream( int type, iks* node );

    void setupFilter();
    void login( char* sid = 0L );
    void setClientState( state s );
    void addQueryID( std::string jid, std::string id );
    std::string getID();
    void init();

    JThread* m_thread;
    Roster* m_roster;

    iksid* m_self;
    iksfilter* m_filter;

    bool m_authorized;
    std::string m_username;
    std::string m_resource;
    std::string m_password;
    std::string m_server;
    std::string m_jid;
    std::string m_versionName;
    std::string m_versionVersion;
    std::string m_identityCategory;
    std::string m_identityType;
    bool m_debug;
    bool m_sasl;
    bool m_tls;
    bool m_createAccount;
    bool m_autoPresence;
    bool m_handleDiscoInfo;
    bool m_handleDiscoItems;
    bool m_manageRoster;
    int m_port;
    state m_state;

    ConnectionListenerList  m_connectionListeners;
    IqHandlerMap            m_iqNSHandlers;
    IqHandlerList           m_iqHandlers;
    MessageHandlerList      m_messageHandlers;
    PresenceHandlerList     m_presenceHandlers;
    SubscriptionHandlerList m_subscriptionHandlers;
    CharList                m_discoCapabilities;
    StringMap               m_queryIDs;

    int m_streamFeatures;
    int m_idCount;
};

#endif // JCLIENT_H__

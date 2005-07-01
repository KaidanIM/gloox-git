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


#ifndef JCLIENT_H__
#define JCLIENT_H__

#include "connectionlistener.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "rosterlistener.h"
#include "subscriptionhandler.h"
#include "prep.h"

#include "wrapper/stream.h"

#include <list>
#include <map>
#include <string>

#define XMLNS_DISCO_INFO     "http://jabber.org/protocol/disco#info"
#define XMLNS_DISCO_ITEMS    "http://jabber.org/protocol/disco#items"
#define XMLNS_ADHOC_COMMANDS "http://jabber.org/protocol/commands"
#define XMLNS_ROSTER         "jabber:iq:roster"
#define XMLNS_VERSION        "jabber:iq:version"
#define XMLNS_REGISTER       "jabber:iq:register"
#define XMLNS_PRIVATE_XML    "jabber:iq:private"
#define XMLNS_BOOKMARKS      "storage:bookmarks"

#define XMPP_PORT            5222


using namespace std;
using namespace Iksemel;

class JThread;
class Roster;
class Disco;
class Adhoc;

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
 * By default, the library handles a few (incoming) IQ namespaces on the application's behalf. These include:
 * @li jabber:iq:roster is handled in that by default the server-side roster is fetched.
 * @li JEP-0092 (Software Version) If no version is specified, a name of "based on gloox" with gloox's current
 * version is announced.
 * @li JEP-0030 (Service Discovery) All supported/available services are announced. No items are returned.
 * @author Jakob Schroeter <js@camaya.net>
 */
class JClient : public Stream
{
  public:

    friend class JThread;
    /**
     * Describes the current connection status.
     * @todo Make this private?
     */
    enum StateEnum
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
     * Describes the possible error conditions on XMPP level.
     * @todo Add missing values.
     */
    enum ErrorEnum
    {
      FEATURE_NOT_IMPLEMENTED,      /**< The requested fetaure is not availabale or implemented. */
      ITEM_NOT_FOUND,              /**< The requested item does not exist. Privacy is a non-issue. */
      SERVICE_UNAVAILABLE          /**< The requested protocol is not supported. Privacy is an issue. */
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
     * @param id A full Jabber ID used for connecting to the server.
     * @param password The password used for authentication.
     * @param port The port to connect to. Default: 5222
     */
    JClient( const std::string& id, const std::string& password, int port = XMPP_PORT );

    /**
     * Constructs a new JClient.
     * SASL and TLS are on by default.
     * @param username The username/local part of the JID.
     * @param resource The resource part of the JID.
     * @param password The password to use for authentication.
     * @param server The jabber server's address or host name to connect to.
     * @param port The port to connect to. Default: 5222
     */
    JClient( const std::string& username, const std::string& password,
             const std::string& server, const std::string& resource,
             int port = XMPP_PORT );

    /**
     * Destructor
     */
    virtual ~JClient();

    /**
     * Returns the current username.
     * @return The username used to connect.
     */
    std::string username() { return Prep::nodeprep( m_username ); };

    /**
     * Returns the current resource.
     * @return The resource used to connect.
     */
    std::string resource() { return Prep::resourceprep( m_resource ); };

    /**
     * Returns the current password.
     * @return The password used to connect.
     */
    std::string password() { return m_password; };

    /**
     * Returns the current server.
     * @return The server used to connect.
     */
    std::string server()   { return Prep::nameprep( m_server ); };

    /**
     * Returns the current complete jabber id.
     * @return The complete jabber id, composed of username, server and resource.
     */
    std::string jid();

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
    void setServer( const std::string &server )     { m_server = server; };

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
     * Returns the current client state.
     * @return The current client state.
     */
    StateEnum clientState();

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
     * Send the initial Presence. This can be done only once after
     * a connection is established.
     * @todo Enhance to allow for all presence types to be sent.
     */
    void sendPresence();

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
     * This function gives access to the @c Disco object.
     * @return A pointer to the Disco object.
     */
    Disco* disco();

    /**
     * This function gives access to the @c Adhoc object.
     * @return A pointer to the Adhoc object.
     */
    Adhoc* adhoc();

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
     * Registers @c ih as object that receives Iq packet notifications for IQ packets
     * containing a tag with name @c tag. Only one handler per tag is possible.
     * @param ih The object to receive Iq packet notifications.
     * @param tag The tag name the object handles.
     */
    void registerIqFTHandler( IqHandler* ih, const char* tag );

    /**
     * Registers @c ih as object that receives all Iq packet notifications.
     * Exists for edge cases.
     * @param ih The object to receive Iq packet notifications.>
     * @deprecated Use @c registerIqHandler( IqHandler* ih, const char* xmlns ) instead.
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
    void removeIqNSHandler( const char* xmlns );

    /**
     * Removes the handler for the given first_tag from the list of Iq handlers.
     * @param xmlns The namespace to remove from the list.
     */
    void removeIqFTHandler( const char* tag );

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
    void cleanUp();
    void notifyOnConnect();
    void notifyOnDisconnect();
    void notifyIqHandlers( const char* xmlns, ikspak* pak );
    void notifyMessageHandlers( iksid* from, iksubtype type, const char* msg );
    void notifyPresenceHandlers( iksid* from, iksubtype type, ikshowtype show, const char* msg );
    void notifySubscriptionHandlers( iksid* from, iksubtype type, const char* msg );
    iksparser* parser();


    friend int authHook(JClient* stream, ikspak* pak);
    friend int registerHook(JClient* stream, ikspak* pak);
    friend int registeredHook(JClient* stream, ikspak* pak);
    friend int errorHook(JClient* stream, ikspak* pak);
    friend int presenceHook(JClient* stream, ikspak* pak);
    friend int msgHook(JClient* stream, ikspak* pak);
    friend int subscriptionHook(JClient* stream, ikspak* pak);
    friend int iqHook(JClient* stream, ikspak* pak);

    typedef list<ConnectionListener*>     ConnectionListenerList;
    typedef map<const char*, IqHandler*>  IqHandlerMap;
    typedef list<IqHandler*>              IqHandlerList;
    typedef list<MessageHandler*>         MessageHandlerList;
    typedef list<PresenceHandler*>        PresenceHandlerList;
    typedef list<SubscriptionHandler*>    SubscriptionHandlerList;

    virtual void on_log( const char* data, size_t size, int is_incoming );
    virtual void on_stream( int type, iks* node );

    void setupFilter();
    void login( const char* sid = 0L );
    void setClientState( StateEnum s );
    void init();

    JThread* m_thread;
    Roster* m_roster;
    Disco* m_disco;
    Adhoc* m_adhoc;

    iksid* m_self;
    iksfilter* m_filter;

    bool m_authorized;
    std::string m_username;
    std::string m_resource;
    std::string m_password;
    std::string m_server;
    std::string m_jid;
    bool m_debug;
    bool m_sasl;
    bool m_tls;
    bool m_createAccount;
    bool m_autoPresence;
    bool m_handleDisco;
    bool m_manageRoster;
    int m_port;
    StateEnum m_state;

    ConnectionListenerList  m_connectionListeners;
    IqHandlerMap            m_iqNSHandlers;
    IqHandlerMap            m_iqFTHandlers;
    IqHandlerList           m_iqHandlers;
    MessageHandlerList      m_messageHandlers;
    PresenceHandlerList     m_presenceHandlers;
    SubscriptionHandlerList m_subscriptionHandlers;

    int m_streamFeatures;
    int m_idCount;
};

#endif // JCLIENT_H__

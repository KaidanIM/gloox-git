/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#ifndef JCLIENT_H__
#define JCLIENT_H__

#include "connectionlistener.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "subscriptionhandler.h"

#include <iksemelmm.hh>

#include <list>
#include <string>


using namespace std;
using namespace Iksemel;

class JThread;

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
 *   JClient* j = new JClient( "user", "resource", "password", "resource" );
 *   j->registerPresenceHandler( this );
 *   j->setVersion( "TestProg", "1.0" );
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
 * By default, the library handles a few IQ namespaces on the application's behalf. These include:
 * @li JEP-0092 (Software Version) If no version is specified, a name of "based on Jlib" with Jlib's current version is announced.
 * @li JEP-0030 (Service Discovery) All supported/available services are announced. No items are returned.
 * @author Jakob Schroeter <js@camaya.net>
 */
class JClient : public Stream
{
  public:

    friend class JThread;
    /**
     * describes the current connection status
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
     * @param username The username/local part of the JID
     * @param resource The resource part of the JID
     * @param password The password to use for authentication
     * @param server The jabber server's address or host name to connect to
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
     * Get the current username.
     * @return The username used to connect.
     */
    std::string username() { return m_username; };

    /**
     * Get the current resource.
     * @return The resource used to connect.
     */
    std::string resource() { return m_resource; };

    /**
     * Get the current password.
     * @return The password used to connect.
     */
    std::string password() { return m_password; };

    /**
     * Get the current server.
     * @return The server used to connect.
     */
    std::string server()   { return m_server; };

    /**
     * Get the current complete jabber id.
     * @return The complete jabber id, composed of username, server and resource.
     */
    std::string jid()      { return ( m_username + "@" + m_server + "/" + m_resource ); };

    /**
     * Get the current debug status.
     * @return the current debug status.
     */
    bool debug()           { return m_debug; };

    /**
     * Get the current SASL status.
     * @return The current SASL status.
     */
    bool sasl()            { return m_sasl; };

    /**
     * Get the current TLS status.
     * @return The current TLS status.
     */
    bool tls()             { return m_tls; };

    /**
     * Get the current port.
     * @return The port used to connect.
     */
    int port()             { return m_port; };

    /**
     * Get the current status of AutoPresence
     * @return The current AutoPresence status
     */
    bool autoPresence()    { return m_autoPresence; };

    // FIXME: setters have to update each other, e.g. username, server, resource --> jid
    /**
     * set the username to use to connect to the XMPP server
     * @param username The username to authenticate with
     */
    void setUsername( const std::string &username ) { m_username = username; };

    /**
     * set the resource to use to connect to the XMPP server
     * @param resource The resource to use to log into the server
     */
    void setResource( const std::string &resource ) { m_resource = resource; };

    /**
     * set the password to use to connect to the XMPP server
     * @param password The password to use for authentication
     */
    void setPassword( const std::string &password ) { m_password = password; };

    /**
     * set the XMPP Cserver to connect to
     * @param server The server to connect to. Either IP or fully qualified domain name
     */
    void setServer( const std::string &server ) { m_server = server;     };

    /**
     * switch debug output on/off
     * @param debug Whether to switch debug output on or off
     */
    void setDebug( bool debug ) { m_debug = debug; };

    /**
     * switch usage of SASL on/off (if available). Default: on
     * @param sasl Whether to switch SASL usage on or off
     */
    void setSasl( bool sasl ) { m_sasl = sasl;   };

    /**
     * switch usage of TLS on/off (if available). Default: on
     * @param tls Whether to switch TLS usage on or off
     */
    void setTls( bool tls ) { m_tls = tls;     };

    /**
     * set the port to connect to
     * @param port The port to connect to
     */
    void setPort( int port ) { m_port = port;   };

    /**
     * enables/disables the automatic sending of a presence packet
     * upon successful authentication @em before the ConnectionListeners
     * are notified. Default: on
     * @param autoPresence Whether to switch AutoPresence on or off
     */
    void setAutoPresence( bool autoPresence ) { m_autoPresence = autoPresence; };

    /**
      * Sets the version of the host application using this library.
      * If this is called, the library will talke care of any jabber:iq:version
      * requests. These IQ packets will not be forwarded to the IqHandlers.
      * @param name The name to be returned to inquireing clients
      * @param version The version to be returned to inquireing clients
      */
    void setVersion( const char* name, const char* version );

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
     * All disco#info IQ packets will be forwarded
     * to the IqHandlers. This is independent of @ref disableDiscoItems().
     * There is no way to re-enable disco#info-handling.
     */
    void disableDiscoInfo();

    /**
     * Disables automatic handling of disco#items.
     * All disco#items IQ packets will be forwarded
     * to the IqHandlers. This is independent of @ref disableDiscoInfo().
     * There is no way to re-enable disco#item-handling.
     */
    void disableDiscoItems();

    /**
     * returns the current client state
     * @return The current client state
     */
    state clientState();

    /**
     * sends the given xml via the established connection
     * @param x The xml data
     */
    void send( iks* x );

    /**
     * sends the given data to the given jid as a message
     * @param jid The Jabber ID to send the data to
     * @param data The data to send
     */
    void send( const char* jid, const char* data );

    /**
     * connects to the XMPP server, authenticates and gets the whole thing running.
     * creates a new thread that receives arrivin data and feeds the parser.
     */
    void connect();

    /**
     * disconnects from the server by ending the receiver thread
     */
    void disconnect();

    /**
     * registers @c cl as object that receives connection notifications
     * @param cl The object to receive connection notifications
     */
    void registerConnectionListener( ConnectionListener* cl );

    /**
     * registers @c ih as object that receives Iq packet notifications
     * @param ih The object to receive Iq packet notifications
     */
    void registerIqHandler( IqHandler* ih );

    /**
     * registers @c mh as object that receives Message packet notifications
     * @param mh The object to receive Message packet notifications
     */
    void registerMessageHandler( MessageHandler* mh );

    /**
     * registers @c ph as object that receives Presence packet notifications
     * @param ph The object to receive Presence packet notifications
     */
    void registerPresenceHandler( PresenceHandler* ph );

    /**
     * registers @c sh as object that receives Subscription packet notifications
     * @param sh The object to receive Subscription packet notifications
     */
    void registerSubscriptionHandler( SubscriptionHandler* sh );


    /**
     * removes the given object from the list of connection listeners
     * @param cl The object to remove from the list
     */
    void removeConnectionListener( ConnectionListener* cl );

    /**
     * removes the given object from the list of Iq handlers
     * @param ih The object to remove from the list
     */
    void removeIqHandler( IqHandler* ih );

    /**
     * removes the given object from the list of message handlers
     * @param mh The object to remove from the list
     */
    void removeMessageHandler( MessageHandler* mh );

    /**
     * removes the given object from the list of presence handlers
     * @param ph The object to remove from the list
     */
    void removePresenceHandler( PresenceHandler* ph );

    /**
     * removes the given object from the list of subscription handlers
     * @param sh The object to remove from the list
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

    typedef list<ConnectionListener*>  ConnectionListenerList;
    typedef list<IqHandler*>           IqHandlerList;
    typedef list<MessageHandler*>      MessageHandlerList;
    typedef list<PresenceHandler*>     PresenceHandlerList;
    typedef list<SubscriptionHandler*> SubscriptionHandlerList;
    typedef list<const char*>          DiscoList;

    virtual void on_log( const char* data, size_t size, int is_incoming );
    virtual void on_stream( int type, iks* node );

    void setupFilter();
    void login( char* sid = 0L );
    void setClientState( state s );
    void init();

    JThread* m_thread;

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
    bool m_debug;
    bool m_sasl;
    bool m_tls;
    bool m_createAccount;
    bool m_autoPresence;
    bool m_handleVersion;
    bool m_handleDiscoInfo;
    bool m_handleDiscoItems;
    int m_port;
    state m_state;

    ConnectionListenerList  m_connectionListeners;
    IqHandlerList           m_iqHandlers;
    MessageHandlerList      m_messageHandlers;
    PresenceHandlerList     m_presenceHandlers;
    SubscriptionHandlerList m_subscriptionHandlers;
    DiscoList               m_discoList;

    int m_streamFeatures;
};

#endif // JCLIENT_H__

/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#ifndef FEEDER_H__
#define FEEDER_H__

#include "../jlib/jclient.h"
#include "../jlib/connectionlistener.h"
#include "../jlib/messagehandler.h"
#include "../jlib/presencehandler.h"
#include "pollhandler.h"

#include <iksemelmm.hh>

#include <map>
#include <string>

using namespace std;

/**
 * This is the main class of the Feeder.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Feeder : public ConnectionListener, PresenceHandler, MessageHandler
{
  public:
    /**
     * Constructor
     * @param username The username/local part of the JID
     * @param resource The resource part of the JID
     * @param password The password to use for authentication
     * @param server The jabber server's address or host name to connect to
     * @param port The port to connect to. Default: 5222
     * @param debug Turn debug of the jabber client on. Default: true
     */
    Feeder( const string username, const string resource,
            const string password, const string server,
            int port = 5222, bool debug = true );

    /**
     * virtual Destructor
     */
    virtual ~Feeder();

    /**
     * use this function to push data. If a worker is available, @param data is passed to it for processing
     */
    bool push( const char* data );

    /**
     * called for incoming presence notifications
     * @param from The sender's jid
     * @param type The presence type
     * @param show The presence's status
     * @param msg The status message
     */
    virtual void handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg );

    /**
     * called for incoming messages
     * @param from The sender's jid
     * @param type The packets type
     * @param msg The actual message content
     */
    virtual void handleMessage( iksid* from, iksubtype type, const char* msg );

    /**
     * called upon successful connection
     */
    virtual void onConnect();

    /**
     * called upon disconnection
     */
    virtual void onDisconnect();

    /**
     * Using this method you can register an object as poll handler. This object is
     * polled for data to be sent to an available worker.
     */
    void registerPollHandler( PollHandler* ph );

  protected:
    /**
     * Holds JID/status pairs
     */
    typedef map<const char*, char*> PresenceList;

  private:
    JClient* c;
    PresenceList m_presence;
    PollHandler* m_pollHandler;

    bool m_poll;

};

#endif // FEEDER_H__

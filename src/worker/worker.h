/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#ifndef WORKER_H__
#define WORKER_H__

#include "../jlib/messagehandler.h"
#include "../jlib/subscriptionhandler.h"
#include "../jlib/jclient.h"


#include <string>

using namespace std;


/**
 * This class implements a Worker.
 * You should reimplement the DataHandler interface to receive to-be-processed
 * data from the Feeder.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Worker : public MessageHandler, SubscriptionHandler
{
  public:
    /**
     * Constructor
     * Creates a new Worker, registers Message- and SubscriptionHandler and connects
     * to the XMPP server.
     * @param username The username/local part of the JID
     * @param resource The resource part of the JID
     * @param password The password to use for authentication
     * @param server The jabber server's address or host name to connect to
     * @param port The port to connect to. Default: 5222
     * @param debug Turn debug of the jabber client on. Default: true
     */
    Worker( const string username, const string resource,
            const string password, const string server,
            int port = 5222, bool debug = true );

    /**
     * Destructor
     */
    virtual ~Worker();

    /**
     * reimplement this function if you want to be notified about 
     * incoming messages
     * @param from The sender's jid
     * @param type The packets type
     * @param msg The actual message content
     */
    virtual void handleMessage( iksid* from, iksubtype type, const char* msg );

    /**
     * reimplement this function if you want to be notified about 
     * incoming subscriptions
     * @param from The sender's jid
     * @param type The packet type
     * @param msg The subscription message (reason)
     */
    virtual void handleSubscription( iksid* from, iksubtype type, const char* msg );

  private:
    JClient* c;

};

#endif // WORKER_H__


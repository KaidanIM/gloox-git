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


#ifndef FEEDER_H__
#define FEEDER_H__

#include "pollhandler.h"
#include "infohandler.h"

#include "../jlib/jclient.h"
#include "../jlib/connectionlistener.h"
#include "../jlib/iqhandler.h"
#include "../jlib/presencehandler.h"

#include "../common/common.h"

#include <iksemel.h>

#include <map>
#include <string>

using namespace std;

/**
 * This is the main class of the Feeder.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Feeder : public ConnectionListener, SubscriptionHandler, PresenceHandler, IqHandler
{
  public:
    /**
     * Constructor.
     * @param username The username/local part of the JID.
     * @param resource The resource part of the JID.
     * @param password The password to use for authentication.
     * @param server The jabber server's address or host name to connect to.
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
     * Initiates the connection to the XMPP server.
     */
    void connect();

    /**
     * Use this function to push data. If a Worker is available, data is pushed.
     * @param data is passed to it for processing
     * @return Returns false if no Worker is available. True otherwise.
     */
    bool push( const char* data );

    /**
     * Called for incoming presence notifications.
     * @param from The sender's jid.
     * @param type The presence type.
     * @param show The presence's status.
     * @param msg The status message.
     */
    virtual void handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg );

    /**
     * Called for incoming messages.
     * @param from The sender's jid.
     * @param type The packets type.
     * @param msg The actual message content.
     */
    virtual void handleIq( const char* xmlns, ikspak* pak );

    /**
     * Called upon successful connection.
     */
    virtual void onConnect();

    /**
     * Called upon disconnection.
     */
    virtual void onDisconnect();

    /**
     * Using this method you can register an object as poll handler. This object is
     * polled for data to be sent to an available worker.
     * @param ph The object derived from PollHandler.
     */
    void registerPollHandler( PollHandler* ph );

    /**
     * Using this method you can register an object as info handler. The methods
     * reimplemented from get called on various events. See the documentation of 
     * the InfoHandler interface for more information.
     * @param ih The object derived from InfoHandlerFeeder.
     */
    void registerInfoHandler( InfoHandlerFeeder* ih );

  protected:
    /**
     * Holds JID/status pairs.
     */
    typedef map<const char*, int> PresenceList;

  private:
    JClient* c;
    PresenceList m_presence;
    PollHandler* m_pollHandler;
    InfoHandlerFeeder* m_infoHandler;

    bool m_poll;

};

#endif // FEEDER_H__

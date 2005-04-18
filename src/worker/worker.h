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


#ifndef WORKER_H__
#define WORKER_H__

#include "datahandler.h"
#include "infohandler.h"

#include "../jlib/connectionlistener.h"
#include "../jlib/iqhandler.h"
#include "../jlib/rosterlistener.h"

#include "../common/common.h"

#include <string>

using namespace std;

class JClient;

/**
 * This class implements a Worker.
 * You should reimplement the DataHandler interface to receive to-be-processed
 * data from the Feeder.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Worker : public ConnectionListener, IqHandler, RosterListener
{
  public:
    /**
     * Constructor
     * Creates a new Worker, registers IqHandler, ConnectionListener and
     * RosterListener and connects to the XMPP server.
     * @param username The username/local part of the JID
     * @param resource The resource part of the JID
     * @param password The password to use for authentication
     * @param server The jabber server's address or host name to connect to
     * @param debug Turn debug of the jabber client on. Default: true
     * @param port The port to connect to. Default: 5222
     */
    Worker( const string username, const string resource,
            const string password, const string server,
            bool debug = true, int port = 5222 );

    /**
     * Destructor
     */
    virtual ~Worker();

    /**
     * Initiates the connection to the XMPP server.
     */
    void connect();

    /**
     * Allows to set the address of the Feeder.
     * @param jid The Feeder's Jabber ID.
     */
    void setFeeder( const string& jid );

    /**
     * Reimplemented from IqHandler.
     */
    virtual void handleIq( const char* xmlns, ikspak* pak );

    /**
     * reimplemented from ConnectionListener.
     */
    virtual void onConnect();

    /**
     * reimplemented from ConnectionListener.
     */
    virtual void onDisconnect();

    /**
     * Using this method you can register an object as data handler. This object gets
     * data received from the Feeder.
     * @param dh The object derived from DataHandler.
     */
    void registerDataHandler( DataHandler* dh );

    /**
     * Using this method you can register an object as info handler. The methods
     * reimplemented from get called on various events. See the documentation of 
     * the InfoHandler interface for more information.
     * @param ih The object derived from InfoHandlerWorker.
     */
    void registerInfoHandler( InfoHandlerWorker* ih );

    /**
     * Call this function when calculation is finished and a result shall
     * be sent to the Feeder.
     * @param code ResultCode indicating general success or failure of the processing
     * @param result The result of the calculation.
     */
    void result( ResultCode code, const char* result );

    /**
     * Reimplemented from RosterListener
     */
    virtual bool subscriptionRequest( const string& jid, const char* msg );

  private:
    JClient* c;
    DataHandler* m_dataHandler;
    InfoHandlerWorker* m_infoHandler;
    iksid* m_feederID;

    bool m_working;
    bool m_debug;

};

#endif // WORKER_H__

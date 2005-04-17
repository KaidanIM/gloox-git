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


#ifndef ROSTER_H__
#define ROSTER_H__

#include "subscriptionhandler.h"
#include "rosterhelper.h"
#include "rosterlistener.h"

#include <map>
#include <string>
using namespace std;

class JClient;
class IqHandler;
// class RosterListener;
class PresenceHandler;
// class SubscriptionHandler;

/**
 * A Jabber/XMPP Roster.
 * This class implements a roster. It takes care of changing presence, subscriptions, etc.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Roster : public IqHandler, PresenceHandler, SubscriptionHandler
{
  public:
    /**
     * Constructor.
     * Creates an empty Roster.
     * @param parent The @c JClient which is used for communication.
     */
    Roster( JClient* parent );

    /**
     * Virtual destructor.
     */
    virtual ~Roster();

    /**
     * This function does the initial filling of the roster with
     * the current server-side roster.
     */
    void fill();

    /**
     * This function returns the roster.
     * @return Returns a map of JIDs with their current presence.
     */
    RosterHelper::RosterMap listRoster();

    /**
     * Reimplemented from IqHandler.
     */
    virtual void handleIq( const char* xmlns, ikspak* pak );

    /**
     * Reimplemented from PresenceHandler.
     */
    virtual void handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg );

    /**
     * Reimplemented from SubscriptionHandler.
     */
    virtual void handleSubscription( iksid* from, iksubtype type, const char* msg );

    /**
     * Use this function to subscribe to a new JID.
     * @param jid The address to subscribe to.
     * @param msg The reason sent along with the subscription request.
     */
    void subscribe( const string& jid, const string& msg );

    /**
     * Use this function to unsubscribe from a JID in the roster.
     * @param jid The address to unsubscribe from.
     * @param msg The reason sent along with the unsubscription request.
     */
    void unsubscribe( const string& jid, const string& msg );

    /**
     * Register @c rl as object that receives updates on roster operations.
     * @param rl The object that receives roster updates.
     */
    void registerRosterListener( RosterListener* rl );

  private:
    void add( const string& jid, int status);

    RosterListener* m_rosterListener;
    RosterHelper::RosterMap m_roster;
    JClient* m_parent;

};

#endif // ROSTER_H__

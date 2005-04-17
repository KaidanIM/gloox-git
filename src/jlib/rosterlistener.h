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


#ifndef ROSTERLISTENER_H__
#define ROSTERLISTENER_H__

#include "rosterhelper.h"

#include <string>
using namespace std;

/**
 * A virtual interface.
 * A class implementing this interface and being registered as RosterListener with the Roster
 * object receives notifications about all the changes in the server-side roster.
 * Only one RosterListener at a time per Roster is possible.
 * @author Jakob Schroeter <js@camaya.net>
 */
class RosterListener
{
  public:
    /**
     * Reimplement this function if you want to be notified about new items
     * on the server-side roster.
     * @param jid The new item's full address.
     */
    virtual void itemAdded( const string& jid ) {};

    /**
     * Reimplement this function if you want to be notified about items that
     * were removed from the server-side roster.
     * @param jid The removed item's full address.
     */
    virtual void itemRemoved( const string& jid ) {};

    /**
     * Reimplement this function if you want to receive the whole server-side roster
     * on the initial roster push. The roster item status is probably wrong.
     * @param roster The full roster.
     */
    virtual void roster( RosterHelper::RosterMap roster ) {};

    /**
     * This function is called on every status change of an item in the roster.
     * @param jid The item's address.
     * @param status The item's new status.
     */
    virtual void itemChanged( const string& jid, int status ) {};

    /**
     * This function is called when an entity wishes to subscribe to this entities presence.
     * @param jid The item's address.
     * @param msg The message sent along with the request.
     */
    virtual bool subscriptionRequest( const string& jid, const string& msg ) {};

};

#endif // ROSTERLISTENER_H__

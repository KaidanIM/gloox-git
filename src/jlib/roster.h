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

#include "presencehandler.h"
#include "iqhandler.h"
#include "jclient.h"

#include <map>
#include <string>
using namespace std;

class Roster : public IqHandler, PresenceHandler
{
  public:
    typedef map<const string, int> RosterMap;

    /**
     * Constructor.
     * Creates an empty Roster.
     * @param parent The @c JClient that is used for communication.
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
    Roster::RosterMap listRoster();

    /**
     * Reimplemented from IqHandler.
     */
    virtual void handleIq( const char* xmlns, ikspak* pak );

    /**
     * Reimplemented from PresenceHandler.
     */
    virtual void handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg );

    /**
     * Use this function to subscribe to a new JID.
     * @param jid The address to subscribe to.
     */
    void add( const string& jid );

  private:
    void add( const string& jid, int status);

    RosterMap m_roster;
    JClient* m_parent;

};

#endif // ROSTER_H__

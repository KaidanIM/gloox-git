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



#ifndef PRESENCEHANDLER_H__
#define PRESENCEHANDLER_H__

#include <iksemel.h>
#include <string>

using namespace std;


/**
 * A virtual interface.
 * Derived classes can be registered as IqHandlers with the JClient.
 * Upon an incoming Presence packet @ref handlePresence() will be called.
 * @author Jakob Schroeter <js@camaya.net>
 */
class PresenceHandler
{
  public:
    /**
     * Reimplement this function if you want to be updated on
     * incoming presence notifications.
     * @param from The sender's jid
     * @param type The presence type
     * @param show The presence's status
     * @param msg The status message
     */
    virtual void handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg ) {};
};

#endif // PRESENCEHANDLER_H__

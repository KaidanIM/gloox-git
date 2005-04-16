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


#ifndef INFOHANDLER_H__
#define INFOHANDLER_H__

#include <iksemel.h>

/**
 * Vitual interface.
 * This interface acts as a forwarder for other interfaces, e.g. ConnectionListener.
 * It is an generic interface that can be enhanced for more special need.
 */
class InfoHandler
{
  public:
    /**
     * This function is called whenever a connection has been successfully established.
     */
    virtual void connected() {};

    /**
     * This function is called whenever a connection has ended.
     */
    virtual void disconnected() {};

    /**
     * This function is called upon incoming presence information.
     * @param from The node that changed status.
     * @param show The new status of the node.
     */
    virtual void rosterChanged( iksid* from, ikshowtype show ) {};

};

#endif // INFOHANDLER_H__

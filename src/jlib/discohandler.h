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


#ifndef DiscoHandler_H__
#define DiscoHandler_H__

#include "jclient.h"

#include <string>
using namespace std;

/**
 * A virtual interface.
 * A class implementing this interface and being registered as DiscoHandler with the Disco
 * object receives the results of sent disco queries.
 * Only one DiscoHandler at a time per Disco object is possible.
 * @author Jakob Schroeter <js@camaya.net>
 */
class DiscoHandler
{
  public:
    /**
     * Reimplement this function if you want to be notified about the result
     * of an disco#info query.
     * @param id The id of the result. Corresponds to the id of the sent query.
     * @param pak The full packet.
     * @todo Replace the ikspak with decoded values.
     */
    virtual void discoInfoResult( const string& id, const ikspak* pak ) {};

    /**
     * Reimplement this function if you want to be notified about the result
     * of an disco#items query.
     * @param id The id of the result. Corresponds to the id of the sent query.
     * @param pak The full packet.
     * @todo Replace the ikspak with decoded values.
     */
    virtual void discoItemsResult( const string& id, const ikspak* pak ) {};

    /**
     * Reimplement this function to receive disco error notifications.
     * @param id The id of the result. Corresponds to the id of the sent query.
     * @param errno The type of the error as returned by the server.
     */
    virtual void discoError( const string& id, const string& error ) {};

    /**
     * Reimplement this function to receive notifications about incoming IQ
     * packets in the disco namespace of type 'set'.
     * @param id The id of the incoming query.
     * @param pak The full packet.
     * @todo Replace the ikspak with decoded values.
     */
    virtual void discoSet( const string& id, const ikspak* pak ) {};

};

#endif // DiscoHandler_H__

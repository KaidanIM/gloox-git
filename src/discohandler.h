/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/


#ifndef DISCOHANDLER_H__
#define DISCOHANDLER_H__

#include <string>
using namespace std;

namespace gloox
{

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
       * @param stanza The full Stanza.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoInfoResult( const std::string& id, const Stanza& stanza ) {};

      /**
       * Reimplement this function if you want to be notified about the result
       * of an disco#items query.
       * @param id The id of the result. Corresponds to the id of the sent query.
       * @param stanza The full Stanza.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoItemsResult( const std::string& id, const Stanza& stanza ) {};

      /**
       * Reimplement this function to receive disco error notifications.
       * @param id The id of the result. Corresponds to the id of the sent query.
       * @param error The type of the error as returned by the server.
       */
      virtual void handleDiscoError( const std::string& id, const std::string& error ) {};

      /**
       * Reimplement this function to receive notifications about incoming IQ
       * stanzas of type 'set' in the disco namespace.
       * @param id The id of the incoming query.
       * @param stanza The full Stanza.
       * @todo Replace the stanza with decoded values.
       */
      virtual bool handleDiscoSet( const std::string& id, const Stanza& stanza ) { return false; };

  };

};

#endif // DISCOHANDLER_H__

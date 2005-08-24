/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef DISCOHANDLER_H__
#define DISCOHANDLER_H__

#include "stanza.h"

#include <string>

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
      virtual void handleDiscoInfoResult( const std::string& id, Stanza *stanza ) {};

      /**
       * Reimplement this function if you want to be notified about the result
       * of an disco#items query.
       * @param id The id of the result. Corresponds to the id of the sent query.
       * @param stanza The full Stanza.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoItemsResult( const std::string& id, Stanza *stanza ) {};

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
      virtual bool handleDiscoSet( const std::string& id, Stanza *stanza ) { return false; };

  };

};

#endif // DISCOHANDLER_H__

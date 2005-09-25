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
   * @brief A virtual interface that enables objects to receive Service Discovery (JEP-0030) events.
   * A class implementing this interface can receives the results of sent disco queries.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class DiscoHandler
  {
    public:
      /**
       * Reimplement this function if you want to be notified about the result
       * of an disco#info query.
       * @param stanza The full Stanza.
       * @param context A context identifier.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoInfoResult( Stanza *stanza, int context ) {};

      /**
       * Reimplement this function if you want to be notified about the result
       * of an disco#items query.
       * @param stanza The full Stanza.
       * @param context A context identifier.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoItemsResult( Stanza *stanza, int context ) {};

      /**
       * Reimplement this function to receive disco error notifications.
       * @param stanza The full Stanza.
       * @param context A context identifier.
       */
      virtual void handleDiscoError( Stanza *stanza, int context ) {};

      /**
       * Reimplement this function to receive notifications about incoming IQ
       * stanzas of type 'set' in the disco namespace.
       * @param stanza The full Stanza.
       * @todo Replace the stanza with decoded values.
       */
      virtual bool handleDiscoSet( Stanza *stanza ) { return false; };

  };

};

#endif // DISCOHANDLER_H__

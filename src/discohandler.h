/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef DISCOHANDLER_H__
#define DISCOHANDLER_H__

#include "macros.h"

#include <string>

namespace gloox
{

  class IQ;

  /**
   * @brief A virtual interface that enables objects to receive Service Discovery (XEP-0030) events.
   *
   * A class implementing this interface can receive the results of sent disco queries.
   *
   * @author Jakob Schroeter <js@camaya.net>
   */
  class GLOOX_API DiscoHandler
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~DiscoHandler() {}

      /**
       * Reimplement this function if you want to be notified about the result
       * of an disco#info query.
       * @param iq The full IQ.
       * @param context A context identifier.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoInfoResult( IQ* iq, int context ) = 0;

      /**
       * Reimplement this function if you want to be notified about the result
       * of an disco#items query.
       * @param iq The full IQ.
       * @param context A context identifier.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoItemsResult( IQ* iq, int context ) = 0;

      /**
       * Reimplement this function to receive disco error notifications.
       * @param iq The full IQ.
       * @param context A context identifier.
       * @todo Replace the stanza with decoded values.
       */
      virtual void handleDiscoError( IQ* iq, int context ) = 0;

      /**
       * Reimplement this function to receive notifications about incoming IQ
       * stanzas of type 'set' in the disco namespace.
       * @param iq The full IQ.
       * @return Returns @b true if the stanza was handled and answered, @b false otherwise.
       * @todo Replace the stanza with decoded values.
       */
      virtual bool handleDiscoSet( IQ* iq ) { (void) iq; return false; }

  };

}

#endif // DISCOHANDLER_H__

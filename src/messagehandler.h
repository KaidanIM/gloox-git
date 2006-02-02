/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MESSAGEHANDLER_H__
#define MESSAGEHANDLER_H__

#include "stanza.h"

namespace gloox
{

  /**
   * @brief A virtual interface which can be reimplemented to receive incoming message stanzas.
   *
   * Derived classes can be registered as MessageHandlers with the Client.
   * Upon an incoming Message packet @ref handleMessage() will be called.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class GLOOX_API MessageHandler
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~MessageHandler() {};

      /**
       * Reimplement this function if you want to be notified about
       * incoming messages.
       * @param stanza The complete Stanza.
       */
      virtual void handleMessage( Stanza *stanza ) = 0;
  };

}

#endif // MESSAGEHANDLER_H__

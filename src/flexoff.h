/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef FLEXOFF_H__
#define FLEXOFF_H__

#include "disco.h"
#include "discohandler.h"

namespace gloox
{

  /**
   * An implementation of JEP-0013 (Flexible Offline Message Retrieval).
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.7
   */
  class FlexibleOffline : public DiscoHandler
  {
    public:
      /**
       * Creates a new FlexibleOffline object that manages retrieval of offline messages.
       * @param parent The ClientBase to use for communication.
       * @param disco The disco object to use.
       */
      FlexibleOffline();

      /**
       * Virtual Destructor.
       */
      virtual ~FlexibleOffline();

      /**
       * Initiates querying the server for Flexible Offline Message Retrieval-support.
       * The result is announced through the FlexibleOfflineHandler.
       */
      void checkSupport();

      /**
       * Initiates asking the server for the number of stored offline messages.
       * The result is announced through the FlexibleOfflineHandler.
       */
      void getMsgCount();

      /**
       * Initiates fetching the offline message headers.
       * The result is announced through the FlexibleOfflineHandler.
       */
      void fetchHeaders();

      /**
       * Initiates fetching of one or more specific messages, or all messages.
       * The result is announced through the FlexibleOfflineHandler.
       */
      void fetchMessages();

      /**
       * Initiates removing of one or more specific messages, or all messages.
       * The result is announced through the FlexibleOfflineHandler.
       */
      void removeMessages();

    private:

  };

};

#endif // FLEXOFF_H__

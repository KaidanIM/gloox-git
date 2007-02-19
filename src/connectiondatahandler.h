/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef CONNECTIONDATAHANDLER_H__
#define CONNECTIONDATAHANDLER_H__

#include <string>

namespace gloox
{

  /**
   * @brief This is an abstract base class to receive events from a ConnectionBase-derived object.
   *
   * You should not need to use this class directly.
   *
   * @author Jakob Schröter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API ConnectionDataHandler
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~ConnectionDataHandler() {};

      /**
       *
       */
      virtual void handleReceivedData( const std::string& data ) = 0;

  };

}

#endif // CONNECTIONDATAHANDLER_H__

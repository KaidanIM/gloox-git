/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef LOGHANDLER_H__
#define LOGHANDLER_H__

#include <string>

namespace gloox
{

  /**
   * @brief A virtual interface which can be reimplemented to receive data sent and received over
   * the network.
   *
   * Upon an incoming packet @ref handleLog() is be called.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.5
   */
  class GLOOX_EXPORT LogHandler
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~LogHandler() {};

      /**
       * Reimplement this function if you want to receive the chunks of the conversation
       * between gloox and server.
       * @param xml The sent or received XML.
       * @param incoming @b True if the string was received, @b false if it was sent.
       */
      virtual void handleLog( const std::string& xml, bool incoming ) = 0;
  };

}

#endif // PRESENCEHANDLER_H__

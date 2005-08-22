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


#ifndef MESSAGEHANDLER_H__
#define MESSAGEHANDLER_H__

#include "stanza.h"

namespace gloox
{

  /**
   * A virtual interface.
   * Derived classes can be registered as MessageHandlers with the Client.
   * Upon an incoming Message packet @ref handleMessage() will be called.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class MessageHandler
  {
    public:
      /**
       * Reimplement this function if you want to be notified about
       * incoming messages.
       * @param stanza The complete Stanza.
       */
      virtual void handleMessage( const Stanza& stanza ) = 0;
  };

};

#endif // MESSAGEHANDLER_H__

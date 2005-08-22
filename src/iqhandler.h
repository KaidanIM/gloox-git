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



#ifndef IQHANDLER_H__
#define IQHANDLER_H__

#include "stanza.h"

namespace gloox
{

  /**
   * A virtual interface.
   * Derived classes can be registered as IqHandlers with the Client.
   * Upon an incoming IQ packet @ref handleIq() will be called.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class IqHandler
  {
    public:
      /**
       * Reimplement this function if you want to be notified about incoming IQs.
       * @param stanza The complete Stanza.
       * @return Indicates whether a request of type 'get' or 'set' has been handled. This includes
       * the obligatory 'result' answer. If you return @b false, a 'error' will be sent.
       */
      virtual bool handleIq( const Stanza& stanza ) = 0;

      /**
       * Reimplement this function if you want to be notified about
       * incoming IQs with a specific value of the @c id attribute. You
       * have to enable tracking of those IDs using @c Client::trackID().
       * This is usually useful for IDs that generate a positive reply, i.e.
       * &lt;iq type='result' id='reg'/&gt; where a namespace filter wouldn't
       * work.
       * @param stanza The complete Stanza.
       * @param context A value to restore context, stored with @ref ClientBase::trackID().
       * @return Indicates whether a request of type 'get' or 'set' has been handled. This includes
       * the obligatory 'result' answer. If you return @b false, a 'error' will be sent.
       */
      virtual bool handleIqID( const Stanza& stanza, int context ) = 0;
  };

};

#endif // IQHANDLER_H__

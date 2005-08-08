/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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


#ifndef JTHREAD_H__
#define JTHREAD_H__

#include "thread.h"

#include <iksemel.h>

namespace gloox
{

  class ClientBase;

  /**
   * This is the JClient-specific implementation of Thread.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class JThread : public Thread
  {
    public:
      /**
       * Constructor.
       * @param parent An initialised JClient instance.
       */
      JThread( ClientBase* parent );

      /**
       * Virtual destructor
       */
      virtual ~JThread();

      /**
       * Contains the actual thread implementation
       */
      virtual void run();

      /**
       * Call this function to end the thread
       */
      void cancel();

      /**
       * Call this function to indicate that the point of authentication has passed
       */
      void over();

    private:
      bool m_cancel;
      bool m_over;
      ClientBase* m_parent;
      iksparser* m_parser;
  };

};

#endif // JTHREAD_H__

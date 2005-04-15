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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#ifndef JTHREAD_H__
#define JTHREAD_H__

#include "thread.h"

#include <iksemel.h>

class JClient;


/**
 * This is the JClient-specific implementation of Thread.
 * @author Jakob Schroeter <js@camaya.net>
 */
class JThread : public Thread
{
  public:
    /**
     * Constructor
     * @param parent An initialised JClient instance.
     */
    JThread( JClient* parent );

    /**
     * Destructor
     */
    virtual ~JThread();

    /**
     * reimplemented from Thread
     * contains the actual thread implementation
     */
    virtual void run();

    /**
     * call this function to end the thread
     */
    void cancel();

    /**
     * call this function to indicate that the point of authentication has passed
     */
    void over();

  private:
    bool m_cancel;
    bool m_over;
    JClient* m_parent;
    iksparser* m_parser;
};

#endif // JTHREAD_H__

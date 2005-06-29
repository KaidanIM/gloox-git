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


#ifndef THREAD_H__
#define THREAD_H__

#include <pthread.h>   /* Posix 1003.1c threads */

/**
 * A generic thread class based on pthreads.
 * @author Jakob Schroeter <js@camaya.net>
 */
class Thread
{
  public:
    /**
     * Constructor
     */
    Thread();

    /**
     * Destrcutor
     */
    virtual ~Thread();

    /** Starts the Thread.
     * This method must be called to start the Thread.
     */
    void start();

    /** Blocks until the Thread terminated.
     * This method can be called to wait for termination of a Thread.
     */
    void join();

    /** Returns true if the calling Thread is this Thread.
     */
    bool isCurrent();

    /** Call this method to wait for a certain amount of time.
     * \param msec Time to wait in microseconds
     */
    static void sleep( int msec );

    /** Get Current Thread ID.
     * \return
     *         Returns the Thread ID of the calling Thread.
     */
    static pthread_t self();

    /** Abstract method which actually is the Thread.
     * Override this method to implement a Thread. When calling start()
     * this method is executed as a Thread.
     */
    virtual void run() = 0;

  protected:
    /**
     * non class member called by the pthread
     */
    friend  void* thread_runner(void* arg);

  private:
    pthread_t m_thread;
};


#endif // THREAD_H__

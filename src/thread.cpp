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



#include "thread.h"

#include <unistd.h>


namespace gloox
{

  Thread::Thread()
  {
  }

  Thread::~Thread()
  {
  }

  void Thread::start()
  {
    pthread_create( &m_thread, NULL, thread_runner, (void *)this );
  }

  void Thread::join() {
    void* value;
    pthread_join( m_thread, &value );
  }

  bool Thread::isCurrent() {
    return ( m_thread == self() );
  }

  void Thread::sleep( int msec ) {
    if( msec < 1000 ) {
      usleep( msec * 100 );
    } else {
      int s = msec/1000;
        ::sleep( s );
        usleep( ( msec%1000 )*100 );
    }
  }

  pthread_t Thread::self()
  {
    return pthread_self();
  }

  void* thread_runner( void* arg ) {
    Thread* t = (Thread*)arg;
    t->run();
    return NULL;
  }

};

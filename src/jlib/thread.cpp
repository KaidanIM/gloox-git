/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */




#include "thread.h"

#include <unistd.h>


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

/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MUTEXGUARD_H__
#define MUTEXGUARD_H__

#include "mutex.h"

namespace gloox
{

  /**
   * @brief A simple implementation of mutex guard.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class MutexGuard
  {
    public:
      /**
       * Contructs a new simple mutex gaurdc and locks the supplied Mutex.
       * @param mutex The Mutex to guard.
       */
      MutexGuard( Mutex* mutex ) : m_mutex( mutex ) { if( m_mutex ) m_mutex->lock(); }

      /**
       * Destructor. Releases the guarded Mutex.
       */
      ~MutexGuard() { if( m_mutex ) m_mutex->unlock(); }

      /**
       * Locks the guarded mutex.
       */
      void lock() { if( m_mutex ) m_mutex->lock(); }

      /**
       * Releases the guarded mutex.
       */
      void unlock() { if( m_mutex ) m_mutex->unlock(); }

    private:
      Mutex* m_mutex;

  };
}

#endif // MUTEXGUARD_H__

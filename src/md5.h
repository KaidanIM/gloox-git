/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MD5_H__
#define MD5_H__

#include "gloox.h"

#include <iksemel.h>

#include <string>

namespace gloox
{

  /**
   * @brief An implementation of the MD5 Message-Digest Algorithm (RFC 1321)
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API MD5
  {
    public:
      /**
       * Contructs a new MD5 state object.
       */
      MD5();

      /**
       * Virtual destructor.
       */
      virtual ~MD5();

      /**
       * Feeds the algorithm.
       */
      void feed( const std::string& text, bool finalize );

      /**
       * Returns the binary hash.
       */
      const std::string hash();

      /**
       * Returns a prettyfied (hex) version of the hash.
       */
      const std::string pretty();

      /**
       * Resets the MD5 object.
       */
      void reset();

    private:
      iksmd5 *m_md5;
      int funcF( int x, int y, int z );
      int funcG( int x, int y, int z );
      int funcH( int x, int y, int z );
      int funcI( int x, int y, int z );

      int m_buffer[4];
  };

}

#endif // MD5_H__

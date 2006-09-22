/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef SHA_H__
#define SHA_H__

#include "gloox.h"

#include <iksemel.h>

#include <string>

namespace gloox
{

  /**
   * @brief An implementation of the SHA Message-Digest Algorithm (RFC 1321)
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API SHA
  {
    public:
      /**
       * Contructs a new SHA state object.
       */
      SHA();

      /**
       * Virtual destructor.
       */
      virtual ~SHA();

      /**
       * Feeds the algorithm.
       */
      void feed( const std::string& text, bool finalize );

      /**
       * Returns a prettyfied (hex) version of the hash.
       */
      const std::string pretty();

      /**
       * Resets the SHA object.
       */
      void reset();

    private:
      iksha *m_sha;

  };

}

#endif // SHA_H__

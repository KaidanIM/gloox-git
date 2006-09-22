/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "sha.h"

#define STEP_ONE_LENGTH 448

namespace gloox
{

  SHA::SHA()
  {
// //     setp 3: Initialize MD Buffer
//     m_buffer[0] = 0x67452301;
//     m_buffer[1] = 0xefcdab89;
//     m_buffer[2] = 0x98badcfe;
//     m_buffer[3] = 0x10325476;
    m_sha = iks_sha_new();
  }

  SHA::~SHA()
  {
    iks_sha_delete( m_sha );
  }

  void SHA::feed( const std::string& text, bool finalize )
  {
    iks_sha_hash( m_sha, (const unsigned char*)text.c_str(), text.length(), finalize );
  }

  const std::string SHA::pretty()
  {
    char t[40];
    iks_sha_print( m_sha, (char*)&t );
    std::string m;
    m.assign( t, 40 );
    return m;
  }

  void SHA::reset()
  {
    iks_sha_reset( m_sha );
  }

}

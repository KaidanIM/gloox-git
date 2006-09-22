/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "md5.h"

#define STEP_ONE_LENGTH 448

namespace gloox
{

  MD5::MD5()
  {
// //     setp 3: Initialize MD Buffer
//     m_buffer[0] = 0x67452301;
//     m_buffer[1] = 0xefcdab89;
//     m_buffer[2] = 0x98badcfe;
//     m_buffer[3] = 0x10325476;
    m_md5 = iks_md5_new();
  }

  MD5::~MD5()
  {
    iks_md5_delete( m_md5 );
  }

  void MD5::feed( const std::string& text, bool finalize )
  {
    iks_md5_hash( m_md5, (const unsigned char*)text.c_str(), text.length(), finalize );
  }

  const std::string MD5::hash()
  {
    char t[16];
    iks_md5_digest( m_md5, (unsigned char*)t );
    std::string m;
    m.assign( t, 16 );
    return m;
  }

  const std::string MD5::pretty()
  {
    char t[32];
    iks_md5_print( m_md5, (char*)t );
    std::string m;
    m.assign( t, 32 );
    return m;
  }

  void MD5::reset()
  {
    iks_md5_reset( m_md5 );
  }

  inline int MD5::funcF( int x, int y, int z )
  {
    return ( (x) & (y) | (~x) & (z) );
  }

  inline int MD5::funcG( int x, int y, int z )
  {
    return ( (x) & (z) | (y) & (~z) );
  }

  inline int MD5::funcH( int x, int y, int z )
  {
    return ( (x) ^ (y) ^ (z) );
  }

  inline int MD5::funcI( int x, int y, int z )
  {
    return ( (y) ^ ( (x) | (~z) ) );
  }

}

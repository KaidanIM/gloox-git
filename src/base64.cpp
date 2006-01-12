/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "base64.h"

namespace gloox
{

  const std::string Base64::alphabet64( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );
  const std::string pad = "=";

  const std::string Base64::encode64( const std::string& input )
  {
    std::string encoded;
    char c;
    size_t length = input.length();

    encoded.reserve( length * 2 );

    for( size_t i = 0; i < length; ++i )
    {
      c = input[i];
      c = ( c >> 2 ) & 0x3f;
      encoded.append( 1, alphabet64[c] );
      printf( "ENCODED bit 1: %s\n", encoded.c_str() );

      c = input[i];
      c = ( c << 4 ) & 0x3f;
      if( ++i < length )
        c |= ( ( input[i] >> 4 ) & 0x0f );
      encoded.append( 1, alphabet64[c] );
      printf( "ENCODED bit 2: %s\n", encoded.c_str() );

      if( i < length )
      {
        c = input[i];
        c = ( c << 2 ) & 0x3c;
        if( ++i < length )
          c |= ( input[i] >> 6 ) & 0x03;
        encoded.append( 1, alphabet64[c] );
        printf( "ENCODED bit 3: %s\n", encoded.c_str() );
      }
      else
      {
        ++i;
        encoded.append( pad );
        printf( "ENCODED bit 3 (pad): %s\n", encoded.c_str() );
      }

      if( i < length )
      {
        c = input[i];
        c = c & 0x3f;
        encoded.append( 1, alphabet64[c] );
        printf( "ENCODED bit 4: %s\n", encoded.c_str() );
      }
      else
      {
        encoded.append( pad );
        printf( "ENCODED bit 4(pad): %s\n", encoded.c_str() );
      }
    }

    printf( "ENCODED XYZXYZ: %s\n", encoded.c_str() );
    return encoded;
  }

  const std::string Base64::decode64( const std::string& input )
  {

    return ""/*decoded*/;
  }

}

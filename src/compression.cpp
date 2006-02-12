/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "compression.h"

namespace gloox
{

  Compression::Compression( StreamFeature method )
    : m_valid( false ), m_method( method ), m_compCount( 0 ), m_decompCount( 0 ), m_dataOutCount( 0 ),
      m_dataInCount( 0 )
  {
    switch( method )
    {
      case StreamFeatureCompressZlib:
      {
#ifdef HAVE_ZLIB
        int ret = Z_OK;
        m_zinflate.zalloc = Z_NULL;
        m_zinflate.zfree = Z_NULL;
        m_zinflate.opaque = Z_NULL;
        m_zinflate.avail_in = 0;
        m_zinflate.next_in = Z_NULL;
        ret = inflateInit( &m_zinflate );

        if( ret == Z_OK )
        {
          m_zdeflate.zalloc = Z_NULL;
          m_zdeflate.zfree = Z_NULL;
          m_zdeflate.opaque = Z_NULL;
          m_zinflate.avail_in = 0;
          m_zinflate.next_in = Z_NULL;
          ret = deflateInit( &m_zdeflate, Z_BEST_COMPRESSION/*Z_DEFAULT_COMPRESSION*/ );

          if( ret == Z_OK )
            m_valid = true;
        }
#endif
        break;
      }
      case StreamFeatureCompressDclz:
      {
#ifdef HAVE_LZW
        // add lzw initialisation
#endif
        break;
      }
      default:
        break;
    }
  }

  Compression::~Compression()
  {
#ifdef HAVE_ZLIB
    inflateEnd( &m_zinflate );
    deflateEnd( &m_zdeflate );
#endif
#ifdef HAVE_LZW
    // add lzw destruction
#endif
  }

  const std::string Compression::compress( const std::string& data )
  {
    if( !m_valid )
      return data;

    if( data.empty() )
      return "";

#ifdef HAVE_ZLIB
    int CHUNK = data.length() + ( data.length() / 100 ) + 13;
    Bytef *out = new Bytef[CHUNK];
    char *in = const_cast<char*>( data.c_str() );

    m_zdeflate.avail_in = data.length();
    m_zdeflate.next_in = (Bytef*)in;

    int ret;
    std::string result, tmp;
    do {
      m_zdeflate.avail_out = CHUNK;
      m_zdeflate.next_out = (Bytef*)out;

      ret = deflate( &m_zdeflate, Z_SYNC_FLUSH );
      printf( "deflate returns: %d\n", ret );
      tmp.assign( (char*)out, CHUNK - m_zdeflate.avail_out );
      result += tmp;
    } while( m_zdeflate.avail_out == 0 );

//     ::compress( out, (uLongf*)&CHUNK, (Bytef*)in, data.length() );
//     std::string result;
//     result.assign( (char*)out, CHUNK );
    m_compCount += result.length();
    m_dataOutCount += data.length();
    delete[] out;

//     printf( "about to send RAW data (%d), uncompressed: '%s' (%d)\n", result.length(), data.c_str(), data.length() );
    return result;
#else
    return data;
#endif
  }

  const std::string Compression::decompress( const std::string& data )
  {
    if( !m_valid )
      return data;

//     if( data.empty() )
//       return "";
#ifdef HAVE_ZLIB
    m_inflateBuffer += data;

    int CHUNK = /*m_inflateBuffer.length() **/ 50;
    char *out = new char[CHUNK];
    char *in = const_cast<char*>( m_inflateBuffer.c_str() );

    m_zinflate.avail_in = m_inflateBuffer.length();
    m_zinflate.next_in = (Bytef*)in;

    int ret = Z_OK;
    std::string result, tmp;
    do
    {
      m_zinflate.avail_out = CHUNK;
      m_zinflate.next_out = (Bytef*)out;

      printf( "av_in: %d, ne_in: %c, av_out: %d\n",
              m_zinflate.avail_in, m_zinflate.next_in,
              m_zinflate.avail_out );

      ret = inflate( &m_zinflate, Z_SYNC_FLUSH );
      tmp.assign( out, CHUNK - m_zinflate.avail_out );
      printf( "inflate: %d, produced: %s\n", ret, tmp.c_str() );
      result += tmp;
    } while( m_zinflate.avail_out == 0 );

//     if( result.empty() || ret < 0 )
//     {
//       printf( "raw data received, error: %s, recv: %s\n", m_zinflate.msg, data.c_str() );
//       inflateReset( &m_zinflate );
//       return "";
//     }

//     } while( m_zinflate.avail_out == 0 );

    m_decompCount += result.length();
    m_dataInCount += m_inflateBuffer.length();
    delete[] out;

    printf( "received RAW data (%d) uncompressed to: '%s' (%d)\n", m_inflateBuffer.length(),
            result.c_str(), result.length() );
    m_inflateBuffer.clear();
    return result;
#else
    return data;
#endif
  }

}

/*
  Copyright (c) 2006-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "util.h"
#include "gloox.h"
#include <cmath>

namespace gloox
{

  namespace util
  {

    unsigned _lookup( const std::string& str, const char* values[], unsigned size, int def )
    {
      unsigned i = 0;
      for( ; i < size && str != values[i]; ++i )
        ;
      return ( i == size && def >= 0 ) ? (unsigned)def : i;
    }

    const std::string _lookup( unsigned code, const char* values[], unsigned size, const std::string& def )
    {
      return code < size ? std::string(values[code]) : def;
    }

    unsigned _lookup2( const std::string& str, const char* values[],
                       unsigned size, int def )
    {
      return 1 << _lookup( str, values, size, def <= 0 ? def : (int)log2( def ) );
    }

    const std::string _lookup2( unsigned code, const char* values[], unsigned size, const std::string& def )
    {
      const unsigned i = (unsigned)log2( code );
      return i < size ? std::string( values[i] ) : def;
    }

    void escape( const std::string& what, std::string& buf )
    {
      const std::string::size_type len = what.length();
      for( std::string::size_type i = 0; i < len; ++i )
      {
        switch( what[i] )
        {
          case '\'': buf.append( "&apos;", 6 ); break;
          case '"':  buf.append( "&quot;", 6 ); break;
          case '&':  buf.append( "&amp;",  5 ); break;
          case '<':  buf.append( "&lt;",   4 ); break;
          case '>':  buf.append( "&gt;",   4 ); break;
          default:
            buf += what[i];
        }
      }
    }

    bool checkValidXMLChars( const std::string& data )
    {
      if( data.empty() )
        return true;

      std::string::const_iterator it = data.begin();
      for( ; it != data.end()
             && ( (unsigned char)(*it) == 0x09
                  || (unsigned char)(*it) == 0x0a
                  || (unsigned char)(*it) == 0x0d
                  || ( (unsigned char)(*it) >= 0x20
                     && (unsigned char)(*it) != 0xc0
                     && (unsigned char)(*it) != 0xc1
                     && (unsigned char)(*it) < 0xf5 ) ); ++it );

      return ( it == data.end() );
    }

  }

}


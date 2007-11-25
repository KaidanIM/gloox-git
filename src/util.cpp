/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
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

    unsigned _lookup( const std::string& str, const char * values[], unsigned size )
    {
      unsigned i = 0;
      for( ; i < size && str != values[i]; ++i ) ;
      return i;
    }

    const std::string _lookup( unsigned code, const char * values[], unsigned size )
    {
      return code < size ? std::string(values[code]) : EmptyString;
    }

    const std::string _lookup2( unsigned code, const char * values[], unsigned size )
    {
      const unsigned i = (unsigned)log2(code);
      return i < size ? std::string(values[i]) : EmptyString;
    }

    static const char escape_chars[] = { '&', '<', '>', '\'', '"' };

    static const std::string escape_seqs[] = { "amp;", "lt;", "gt;", "apos;", "quot;" };

    static const unsigned nb_escape = sizeof( escape_chars ) / sizeof( char );
    static const unsigned escape_size = 5;

    const std::string escape( std::string what )
    {
      for( unsigned val, i = 0; i < what.length(); ++i )
      {
        for( val = 0; val < escape_size; ++val )
        {
          if( what[i] == escape_chars[val] )
          {
            what[i] = '&';
            what.insert( i+1, escape_seqs[val] );
            i += escape_seqs[val].length();
            break;
          }
        }
      }
      return what;
    }

    bool checkValidXMLChars( const std::string& data )
    {
      if( data.empty() )
        return true;

      std::string::const_iterator it = data.begin();
      for( ; it != data.end() && (*it) != 0xc0 && (*it) != 0xc1 && (*it) < 0xf5; ++it );

      return ( it == data.end() );
    }

  }

}


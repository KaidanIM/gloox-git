/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <math.h>

namespace gloox
{

  namespace util
  {  

    #define lookup( a, b ) _lookup( a, b, sizeof(b)/sizeof(char*) )
    #define lookup2( a, b ) _lookup2( a, b, sizeof(b)/sizeof(char*) )

    /**
     * Finds the enumerated value associated with a string value.
     * The enumerated type must have a default (invalid/unknown) value type with
     * a value of 0. eg: enum X { XInvalid = 0, ... };
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    unsigned _lookup( const std::string& str, const char * values[], unsigned size );

    /**
     * Finds the enumerated value associated with a string value.
     * The enumerated type must have a default (invalid/unknown) value type with
     * a value of 0. eg: enum X { XInvalid = 0, ... };
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    inline const char * _lookup( unsigned code, const char * values[], unsigned size )
      {  return code < size ? values[code] : 0;  }

    /**
     * Finds the enumerated value associated with a string value.
     * The enumerated type must have a default (invalid/unknown) value type with
     * a value of 0. eg: enum X { XInvalid = 0, ... };
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    inline unsigned _lookup2( const std::string& str, const char * values[], unsigned size )
      {  return 1 << _lookup( str, values, size ); }

    /**
     * Finds the string associated with an enumerated type.
     * @param code Feature string to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated string (or 0 in case there's no match).
     */
    inline const char * _lookup2( unsigned code, const char * values[], unsigned size )
    {
      long i = log2(code);
      return i < size ? values[i] : 0;
    }

  }

}

#endif /* UTIL_H_ */

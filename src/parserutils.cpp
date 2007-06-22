/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "parserutils.h"

namespace gloox
{

    /**
     * Finds the enumerated value associated with a string value.
     * The enumerated type must have a default (invalid/unknown) value type with
     * a value of 0. eg: enum X { XInvalid = 0, ... };
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    int lookup( const std::string& str, const LookupPair values[], unsigned size )
    {
      unsigned i = 0;
      for( ; i < size && str != values[i].first;  ++i ) ;
      return i != size ? values[i].second : 0;
    }

    /**
     * Finds the enumerated value associated with a string value.
     * The enumerated type must have a default (invalid/unknown) value type with
     * a value of 0. eg: enum X { XInvalid = 0, ... };
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    int lookup( const std::string& str, const char * values[], unsigned size )
    {
      unsigned i = 0;
      for( ; ( i < size ) && ( str != values[i] ); ++i ) ;
      return i != size ? i : 0;
    }

    /**
     * Finds the string associated with an enumerated type.
     * @param code Feature string to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated string (or 0 in case there's no match).
     */
    const char * lookup( int code, const LookupPair values[], unsigned size )
    {
      unsigned i = 0;
      for( ; i < size && code != values[i].second; ++i ) ;
      return i != size ? values[i].first : 0;
    }

    /**
     * Finds the enumerated value associated with a string value.
     * The enumerated type must have a default (invalid/unknown) value type with
     * a value of 0. eg: enum X { XInvalid = 0, ... };
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    const char * lookup( int i, const char * values[], unsigned size )
    {
      return i < size ? values[i] : 0;
    }

}


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
#include <list>
#include <map>

namespace gloox
{

  namespace util
  {  

    #define lookup( a, b ) _lookup( a, b, sizeof(b)/sizeof(char*) )
    #define lookup2( a, b ) _lookup2( a, b, sizeof(b)/sizeof(char*) )

    /**
     * Finds the enumerated value associated with a string value.
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    unsigned _lookup( const std::string& str, const char * values[], unsigned size );

    /**
     * Finds the string associated with an enumerated type.
     * @param code Code of the string to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated string (empty in case there's no match).
     */
    std::string _lookup( unsigned code, const char * values[], unsigned size );

    /**
     * Finds the ORable enumerated value associated with a string value.
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated enum code.
     */
    inline unsigned _lookup2( const std::string& str, const char * values[], unsigned size )
      {  return 1 << _lookup( str, values, size ); }

    /**
     * Finds the string associated with an ORable enumerated type.
     * @param code Code of the string to search for.
     * @param values Array of String/Code pairs to look into.
     * @return The associated string (empty in case there's no match).
     */
    std::string _lookup2( unsigned code, const char * values[], unsigned size );

    /**
     * Delete all elements from a list of pointers.
     * @param L List of pointers to delete.
     */
    template< typename T >
    inline void clear( std::list< T* >& L )
    {
      typename std::list< T* >::iterator it = L.begin();
      for( ; it != L.end(); ++it ) delete (*it);
    }

    /**
     * Delete all associated values from a map (not the key elements).
     * @param M Map of pointer values to delete.
     */
    template< typename Key, typename T >
    inline void clear( std::map< Key, T* >& M )
    {
      typename std::map< Key, T* >::iterator it = M.begin();
      for( ; it != M.end(); ++it ) delete (*it).second;
    }

    /**
     * Delete all associated values from a map (not the key elements).
     * Const key type version.
     * @param M Map of pointer values to delete.
     */
    template< typename Key, typename T >
    inline void clear( std::map< const Key, T* >& M )
    {
      typename std::map< const Key, T* >::iterator it = M.begin();
      for( ; it != M.end(); ++it ) delete (*it).second;
    }

  }

}

#endif /* UTIL_H_ */

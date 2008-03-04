/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef UTIL_H__
#define UTIL_H__

#include "gloox.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  /**
   * @brief A namespace holding a couple utility functions.
   */
  namespace util
  {

    #define lookup( a, b ) _lookup( a, b, sizeof(b)/sizeof(char*) )
    #define lookup2( a, b ) _lookup2( a, b, sizeof(b)/sizeof(char*) )
    #define deflookup( a, b, c ) _lookup( a, b, sizeof(b)/sizeof(char*), c )
    #define deflookup2( a, b, c ) _lookup2( a, b, sizeof(b)/sizeof(char*), c )

    /**
     * Finds the enumerated value associated with a string value.
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @param size The array's size.
     * @return The associated enum code.
     */
    unsigned _lookup( const std::string& str, const char* values[],
                      unsigned size, int def = -1 );

    /**
     * Finds the string associated with an enumerated type.
     * @param code Code of the string to search for.
     * @param values Array of String/Code pairs to look into.
     * @param size The array's size.
     * @return The associated string (empty in case there's no match).
     */
    const std::string _lookup( unsigned code, const char* values[],
                               unsigned size, const std::string& def = EmptyString );

    /**
     * Finds the ORable enumerated value associated with a string value.
     * @param str String to search for.
     * @param values Array of String/Code pairs to look into.
     * @param size The array's size.
     * @return The associated enum code.
     */
    unsigned _lookup2( const std::string& str, const char* values[],
                              unsigned size, int def = -1 );

    /**
     * Finds the string associated with an ORable enumerated type.
     * @param code Code of the string to search for.
     * @param values Array of String/Code pairs to look into.
     * @param size The array's size.
     * @return The associated string (empty in case there's no match).
     */
    const std::string _lookup2( unsigned code, const char* values[],
                                unsigned size, const std::string& def = EmptyString );

    /**
     * A convenience function that executes the given function on each object in a given list.
     * @param t The object to execute the function on.
     * @param f The function to execute.
     */
    template< typename T, typename F >
    inline void ForEach( T& t, F f )
    {
      for( typename T::iterator it = t.begin(); it != t.end(); ++it )
        ( (*it)->*f )();
    }

    /**
     * A convenience function that executes the given function on each object in a given list,
     * passing the given argument.
     * @param t The object to execute the function on.
     * @param f The function to execute.
     * @param d An argument to pass to the function.
     */
    template< typename T, typename F, typename D >
    inline void ForEach( T& t, F f, D& d )
    {
      for( typename T::iterator it = t.begin(); it != t.end(); ++it )
        ( (*it)->*f )( d );
    }

    /**
     * A convenience function that executes the given function on each object in a given list,
     * passing the given arguments.
     * @param t The object to execute the function on.
     * @param f The function to execute.
     * @param d1 An argument to pass to the function.
     * @param d2 An argument to pass to the function.
     */
    template< typename T, typename F, typename D1, typename D2 >
    inline void ForEach( T& t, F f, D1& d1, D2& d2 )
    {
      for( typename T::iterator it = t.begin(); it != t.end(); ++it )
        ( (*it)->*f )( d1, d2 );
    }

    /**
     * A convenience function that executes the given function on each object in a given list,
     * passing the given arguments.
     * @param t The object to execute the function on.
     * @param f The function to execute.
     * @param d1 An argument to pass to the function.
     * @param d2 An argument to pass to the function.
     * @param d3 An argument to pass to the function.
     */
    template< typename T, typename F, typename D1, typename D2, typename D3 >
    inline void ForEach( T& t, F f, D1& d1, D2& d2, D3& d3 )
    {
      for( typename T::iterator it = t.begin(); it != t.end(); ++it )
        ( (*it)->*f )( d1, d2, d3 );
    }

    /**
     * Delete all elements from a list of pointers.
     * @param L List of pointers to delete.
     */
//     template< typename T >
//     inline void clearList( std::list< T* >& L )
//     {
//       typename std::list< T* >::iterator it = L.begin();
//       typename std::list< T* >::iterator it2;
//       while( it != L.end() )
//       {
//         it2 = it++;
//         delete (*it2);
//         L.erase( it2 );
//       }
//     }

    /**
     * Delete all associated values from a map (not the key elements).
     * @param M Map of pointer values to delete.
     */
//     template< typename Key, typename T >
//     inline void clearMap( std::map< Key, T* >& M )
//     {
//       typename std::map< Key, T* >::iterator it = M.begin();
//       typename std::map< Key, T* >::iterator it2;
//       while( it != M.end() )
//       {
//         it2 = it++;
//         delete (*it2).second;
//         M.erase( it2 );
//       }
//     }

    /**
     * Delete all associated values from a map (not the key elements).
     * Const key type version.
     * @param M Map of pointer values to delete.
     */
//     template< typename Key, typename T >
//     inline void clearMap( std::map< const Key, T* >& M )
//     {
//       typename std::map< const Key, T* >::iterator it = M.begin();
//       typename std::map< Key, T* >::iterator it2;
//       while( it != M.end() )
//       {
//         it2 = it++;
//         delete (*it2).second;
//         M.erase( it2 );
//       }
//     }

    /**
     * Does some fancy escaping. (& --> &amp;, etc).
     * @param what A string to escape.
     * @return The escaped string.
     */
    const std::string escape( std::string what );

    /**
     * Checks whether the given input is valid UTF-8.
     * @param data The data to check for validity.
     * @return @@b True if the input is valid UTF-8, @b false otherwise.
     */
    bool checkValidXMLChars( const std::string& data );

    /**
     * Custom log2() implementation.
     * @param n Figure to take the logarithm from.
     * @return The logarithm to the basis of 2.
     */
    int log2( unsigned int n );
  }

}

#endif // UTIL_H__

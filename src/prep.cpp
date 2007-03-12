/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifdef WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#include "prep.h"

#include <string>

#ifdef HAVE_LIBIDN
# include <stringprep.h>
# include <idna.h>
#endif

#define JID_PORTION_SIZE 1023

namespace gloox
{

#ifdef HAVE_LIBIDN
  /**
   * \brief Helper function to convert a string to UTF8.
   * @param buf preallocated (JID_PORTION_SIZE+1 sized) buffer receiving the prepped string.
   * @param s string to be prepped.
   * \todo avoid copying the string everytime from p to buf
   */
  static void toUTF8( char * buf, const std::string& s )
  {
    memset( buf, '\0', JID_PORTION_SIZE + 1 );
    strncpy( buf, s.c_str(), s.length() );
    char* p = stringprep_locale_to_utf8( buf );
    if( p )
    {
      strncpy( buf, p, JID_PORTION_SIZE + 1 );
      free( p );
    }
  }
#endif

  /**
   * \brief Wrapper around stringprep.
   * @param s UTF8 string to convert.
   * @param profile Stringprep_profile to use.
   */
  static std::string prepare( const std::string& s, const Stringprep_profile* profile )
  {
    if( s.empty() )
      return s;

    if( s.length() > JID_PORTION_SIZE )
      return "";

#ifdef HAVE_LIBIDN
    char buf[JID_PORTION_SIZE + 1];
    toUTF8( buf, s );
    return stringprep( buf, JID_PORTION_SIZE, (Stringprep_profile_flags)0, profile )
        == STRINGPREP_OK ? buf : std::string();
#endif
    return s;
  }

  std::string Prep::nodeprep( const std::string& node )
  {
    return prepare( node, stringprep_xmpp_nodeprep );
  }

  std::string Prep::nameprep( const std::string& domain )
  {
    return prepare( domain, stringprep_nameprep );
  }

  std::string Prep::resourceprep( const std::string& resource )
  {
    return prepare( resource, stringprep_xmpp_resourceprep );
  }

  std::string Prep::idna( const std::string& domain )
  {
    if( domain.empty() )
      return domain;

    if( domain.length() > JID_PORTION_SIZE )
      return "";

#ifdef HAVE_LIBIDN
    char *p = 0;
    char buf[JID_PORTION_SIZE + 1];
    toUTF8( buf, domain );
    int rc = idna_to_ascii_8z( buf, &p, (Idna_flags)0 );
    return rc == IDNA_SUCCESS ? p : "";
#else
    return domain;
#endif
  }

}

/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "prep.h"

#include <cstdlib>
#include <string>
#include <string.h>

#ifdef WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#ifdef HAVE_LIBIDN
# include <stringprep.h>
# include <idna.h>
#endif

#define JID_PORTION_SIZE 1023

namespace gloox
{

  namespace prep
  {

#ifdef HAVE_LIBIDN
    /**
     * Applies a Stringprep profile to a string. This function does the actual
     * work behind nodeprep, nameprep and resourceprep.
     * @param s The string to apply the profile to.
     * @param out Contains the prepped string if prepping was successful, else untouched.
     * @param profile The Stringprep profile to apply.
     * @return Returns @b true if prepping was successful, @b false otherwise.
     */
    static bool prepare( const std::string& s, std::string& out, const Stringprep_profile* profile )
    {
      if( s.empty() || s.length() > JID_PORTION_SIZE )
        return false;

      char* p = static_cast<char*>( calloc( JID_PORTION_SIZE, sizeof( char ) ) );
      strncpy( p, s.c_str(), s.length() );
      int rc = stringprep( p, JID_PORTION_SIZE, (Stringprep_profile_flags)0, profile );
      if( rc == STRINGPREP_OK )
        out = p;
      free( p );
      return rc == STRINGPREP_OK;
    }
#endif

    bool nodeprep( const std::string& node, std::string& out )
    {
#ifdef HAVE_LIBIDN
      return prepare( node, out, stringprep_xmpp_nodeprep );
#else
      out = node;
      return false;
#endif
    }

    bool nameprep( const std::string& domain, std::string& out )
    {
#ifdef HAVE_LIBIDN
      return prepare( domain, out, stringprep_nameprep );
#else
      out = domain;
      return false;
#endif
    }

    bool resourceprep( const std::string& resource, std::string& out )
    {
#ifdef HAVE_LIBIDN
      return prepare( resource, out, stringprep_xmpp_resourceprep );
#else
      out = resource;
      return false;
#endif
    }

    bool idna( const std::string& domain, std::string& out )
    {
#ifdef HAVE_LIBIDN
      if( domain.empty() || domain.length() > JID_PORTION_SIZE )
        return false;

      char* prepped;
      int rc = idna_to_ascii_8z( domain.c_str(), &prepped, (Idna_flags)0 );
      if( rc == IDNA_SUCCESS )
      {
        out = prepped;
        return true;
      }
      if( rc != IDNA_MALLOC_ERROR )
        free( prepped );
      return false;
#else
      out = domain;
      return false;
#endif
    }
  }
}
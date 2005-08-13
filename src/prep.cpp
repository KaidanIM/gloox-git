/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/


#include "config.h"

#include "prep.h"

#ifdef HAVE_LIBIDN
#include <stringprep.h>
#include <idna.h>
#endif

#define JID_PORTION_SIZE 1023

namespace gloox
{

  string Prep::nodeprep( const string& node )
  {
    if( node.empty() )
      return node;

#ifdef HAVE_LIBIDN
  char* p;
  char buf[JID_PORTION_SIZE + 1];
  memset( &buf, '\0', JID_PORTION_SIZE + 1 );
  strncpy( buf, node.c_str(), node.length() );
  p = stringprep_locale_to_utf8( buf );
    if ( p )
    {
      strncpy( buf, p, JID_PORTION_SIZE + 1 );
      free( p );
    }

    int rc = stringprep( (char*)&buf, JID_PORTION_SIZE,
                         (Stringprep_profile_flags)0, stringprep_xmpp_nodeprep );
    if ( rc != STRINGPREP_OK )
    {
      return string();
    }
    return buf;
#else
    return node;
#endif
  }

  string Prep::nameprep( const string& domain )
  {
    if( domain.empty() )
      return domain;

#ifdef HAVE_LIBIDN
    char* p;
    char buf[JID_PORTION_SIZE + 1];
    memset( &buf, '\0', JID_PORTION_SIZE + 1 );
    strncpy( buf, domain.c_str(), domain.length() );
    p = stringprep_locale_to_utf8( buf );
    if ( p )
    {
      strncpy( buf, p, JID_PORTION_SIZE + 1 );
      free( p );
    }

    int rc = stringprep( (char*)&buf, JID_PORTION_SIZE,
                         (Stringprep_profile_flags)0, stringprep_nameprep );
    if ( rc != STRINGPREP_OK )
    {
      return string();
    }
    return buf;
#else
    return domain;
#endif
  }

  string Prep::resourceprep( const string& resource )
  {
    if( resource.empty() )
      return resource;

#ifdef HAVE_LIBIDN
    char* p;
    char buf[JID_PORTION_SIZE + 1];
    memset( &buf, '\0', JID_PORTION_SIZE + 1 );
    strncpy( buf, resource.c_str(), resource.length() );
    p = stringprep_locale_to_utf8( buf );
    if ( p )
    {
      strncpy( buf, p, JID_PORTION_SIZE + 1 );
      free( p );
    }

    int rc = stringprep( (char*)&buf, JID_PORTION_SIZE,
                          (Stringprep_profile_flags)0, stringprep_xmpp_resourceprep );
    if ( rc != STRINGPREP_OK )
    {
      return string();
    }
    return buf;
#else
    return resource;
#endif
  }

  string Prep::idna( const string& domain )
  {
    if( domain.empty() )
      return domain;

#ifdef HAVE_LIBIDN
    char* p;
    char buf[JID_PORTION_SIZE + 1];
    memset( &buf, '\0', JID_PORTION_SIZE + 1 );
    strncpy( buf, domain.c_str(), domain.length() );
    p = stringprep_locale_to_utf8( buf );
    if ( p )
    {
      strncpy( buf, p, JID_PORTION_SIZE + 1 );
      free( p );
    }

    int rc = idna_to_ascii_8z( (char*)&buf, &p, (Idna_flags)0 );
    if ( rc != IDNA_SUCCESS )
    {
      return string();
    }
    return p;
#else
    return domain;
#endif
  }

};

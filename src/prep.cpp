/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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

#include <stringprep.h>
#include <idna.h>

#define JID_PORTION_SIZE 1023

string Prep::nodeprep( const string& node )
{
#ifdef LIBIDN
  char* p;
  char* buf = (char*)calloc( JID_PORTION_SIZE, 1 );
  buf = strndup( node.c_str(), node.length() );
  p = stringprep_locale_to_utf8( buf );
  if ( p )
  {
    strcpy( buf, p );
    free( p );
  }

  int rc = stringprep( buf, JID_PORTION_SIZE, (Stringprep_profile_flags)0, stringprep_xmpp_nodeprep );
  if ( rc != STRINGPREP_OK )
  {
    free( buf );
    return string();
  }
  string *t = new string( buf );
  free( buf );
  return *t;
#else
  return node;
#endif
}

string Prep::nameprep( const string& domain )
{
#ifdef LIBIDN
  char* p;
  char* buf = (char*)calloc( JID_PORTION_SIZE, 1 );
  buf = strndup( domain.c_str(), domain.length() );
  p = stringprep_locale_to_utf8( buf );
  if ( p )
  {
    strcpy( buf, p );
    free( p );
  }

  int rc = stringprep( buf, JID_PORTION_SIZE, (Stringprep_profile_flags)0, stringprep_nameprep);
  if ( rc != STRINGPREP_OK )
  {
    free( buf );
    return string();
  }
  string *t = new string( buf );
  free( buf );
  return *t;
#else
  return domain;
#endif
}

string Prep::resourceprep( const string& resource )
{
#ifdef LIBIDN
  char* p;
  char* buf = (char*)calloc( JID_PORTION_SIZE, 1 );
  buf = strndup( resource.c_str(), resource.length() );
  p = stringprep_locale_to_utf8( buf );
  if ( p )
  {
    strcpy( buf, p );
    free( p );
  }

  int rc = stringprep( buf, JID_PORTION_SIZE, (Stringprep_profile_flags)0, stringprep_xmpp_resourceprep );
  if ( rc != STRINGPREP_OK )
  {
    free( buf );
    return string();
  }
  string *t = new string( buf );
  free( buf );
  return *t;
#else
  return resource;
#endif
}

string Prep::idna( const string& domain )
{
#ifdef LIBIDN
  char* p;
  char* buf = (char*)calloc( JID_PORTION_SIZE, 1 );
  buf = strndup( domain.c_str(), domain.length() );
  p = stringprep_locale_to_utf8( buf );
  if ( p )
  {
    strcpy( buf, p );
    free( p );
  }

  int rc = idna_to_ascii_8z( buf, &p, (Idna_flags)0 );
  if ( rc != IDNA_SUCCESS )
  {
    free( buf );
    return string();
  }
  string *t = new string( p );
  free( buf );
  free( p );
  return *t;
#else
  return domain;
#endif
}

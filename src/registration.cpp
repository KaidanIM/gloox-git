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


#include "registration.h"

#include "jclient.h"

Registration::Registration( JClient* parent )
  : m_parent( parent ), m_registrationHandler( 0 )
{
  if( !m_parent )
  {
    m_parent = new JClient();
  }

  m_parent->registerIqHandler( this, XMLNS_REGISTER );
}

Registration::~Registration()
{
}

void Registration::fetchRegistrationFields()
{
  if( !m_parent )
    return;

  string id = m_parent->getID();

  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_REGISTER );
  iks_insert_attrib( x, "id", id.c_str() );
  m_parent->send( x );
}

void Registration::createAccount( int fields, fieldStruct values )
{
  if( !m_parent )
    return;

  string id = m_parent->getID();
  m_parent->trackID( this, id.c_str() );

  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_REGISTER );
  iks_insert_attrib( x, "id", id.c_str() );
  m_parent->send( x );
}

void Registration::removeAccount()
{
  if( m_parent && ( m_parent->clientState() == JClient::STATE_AUTHENTICATED ) )
    return;
}

void Registration::changePassword()
{
  if( m_parent && ( m_parent->clientState() == JClient::STATE_AUTHENTICATED ) )
    return;
}

void Registration::registerRegistrationHandler( RegistrationHandler* rh )
{
  m_registrationHandler = rh;
}

void Registration::removeRegistrationHandler( RegistrationHandler* rh )
{
  m_registrationHandler = 0;
}

void Registration::handleIq( const char* xmlns, ikspak* pak )
{
  int fields = 0;
  string instructions;

  switch( pak->type )
  {
    case IKS_TYPE_RESULT:
      iks* ft = iks_first_tag( pak->query );
      while( ( ft = iks_next_tag( pak->query ) ) != 0 )
      {
        if( strncmp( iks_name( ft ), "username", 8 ) == 0 )
        {
          fields &= FIELD_USERNAME;
        }
        else if( strncmp( iks_name( ft ), "nick", 4 ) == 0 )
        {
          fields &= FIELD_NICK;
        }
        else if( strncmp( iks_name( ft ), "password", 8 ) == 0 )
        {
          fields &= FIELD_PASSWORD;
        }
        else if( strncmp( iks_name( ft ), "name", 4 ) == 0 )
        {
          fields &= FIELD_NAME;
        }
        else if( strncmp( iks_name( ft ), "first", 5 ) == 0 )
        {
          fields &= FIELD_FIRST;
        }
        else if( strncmp( iks_name( ft ), "last", 4 ) == 0 )
        {
          fields &= FIELD_LAST;
        }
        else if( strncmp( iks_name( ft ), "email", 5 ) == 0 )
        {
          fields &= FIELD_EMAIL;
        }
        else if( strncmp( iks_name( ft ), "address", 7 ) == 0 )
        {
          fields &= FIELD_ADDRESS;
        }
        else if( strncmp( iks_name( ft ), "city", 4 ) == 0 )
        {
          fields &= FIELD_CITY;
        }
        else if( strncmp( iks_name( ft ), "state", 5 ) == 0 )
        {
          fields &= FIELD_STATE;
        }
        else if( strncmp( iks_name( ft ), "zip", 3 ) == 0 )
        {
          fields &= FIELD_ZIP;
        }
        else if( strncmp( iks_name( ft ), "phone", 5 ) == 0 )
        {
          fields &= FIELD_PHONE;
        }
        else if( strncmp( iks_name( ft ), "url", 3 ) == 0 )
        {
          fields &= FIELD_URL;
        }
        else if( strncmp( iks_name( ft ), "date", 4 ) == 0 )
        {
          fields &= FIELD_DATE;
        }
        else if( strncmp( iks_name( ft ), "misc", 4 ) == 0 )
        {
          fields &= FIELD_MISC;
        }
        else if( strncmp( iks_name( ft ), "text", 4 ) == 0 )
        {
          fields &= FIELD_TEXT;
        }
        else if( strncmp( iks_name( ft ), "instructions", 4 ) == 0 )
        {
          instructions = ( iks_cdata( ft ) )?( iks_cdata( ft ) ):( "" );
        }
      }

      m_registrationHandler->handleRegistrationFields( fields, instructions );
      break;
    case IKS_TYPE_ERROR:

      break;
  }
}

void Registration::handleIqID( const char* id, ikspak* pak )
{
}

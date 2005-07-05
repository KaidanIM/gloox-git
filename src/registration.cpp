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
  m_type = FIELDS_PENDING;

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
  m_type = REGISTRATION_PENDING;

  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_REGISTER );
  iks_insert_attrib( x, "id", id.c_str() );

  if( fields & FIELD_USERNAME )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "username" ), values.username.c_str(), values.username.length() );
  if( fields & FIELD_NICK )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "nick" ), values.nick.c_str(), values.nick.length() );
  if( fields & FIELD_PASSWORD )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "password" ), values.password.c_str(), values.password.length() );
  if( fields & FIELD_NAME )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "name" ), values.name.c_str(), values.name.length() );
  if( fields & FIELD_FIRST )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "first" ), values.first.c_str(), values.first.length() );
  if( fields & FIELD_LAST )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "last" ), values.last.c_str(), values.last.length() );
  if( fields & FIELD_EMAIL )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "email" ), values.email.c_str(), values.email.length() );
  if( fields & FIELD_ADDRESS )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "address" ), values.address.c_str(), values.address.length() );
  if( fields & FIELD_CITY )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "city" ), values.city.c_str(), values.city.length() );
  if( fields & FIELD_STATE )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "state" ), values.state.c_str(), values.state.length() );
  if( fields & FIELD_ZIP )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "zip" ), values.zip.c_str(), values.zip.length() );
  if( fields & FIELD_PHONE )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "phone" ), values.phone.c_str(), values.phone.length() );
  if( fields & FIELD_URL )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "url" ), values.url.c_str(), values.url.length() );
  if( fields & FIELD_DATE )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "date" ), values.date.c_str(), values.date.length() );
  if( fields & FIELD_MISC )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "misc" ), values.misc.c_str(), values.misc.length() );
  if( fields & FIELD_TEXT )
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "text" ), values.text.c_str(), values.text.length() );

  m_parent->trackID( this, id.c_str() );
  m_parent->send( x );
}

void Registration::removeAccount()
{
  if( m_parent && ( m_parent->clientState() == JClient::STATE_AUTHENTICATED ) )
    return;

  m_type = UNREGISTER_PENDING;

}

void Registration::changePassword()
{
  if( m_parent && ( m_parent->clientState() == JClient::STATE_AUTHENTICATED ) )
    return;

  m_type = PASSWORD_PENDING;

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

  m_type = 0;

  switch( pak->subtype )
  {
    case IKS_TYPE_RESULT:
      if( iks_find( pak->query, "registered" ) )
      {
        if( m_registrationHandler )
          m_registrationHandler->handleAlreadyRegistered();
        break;
      }

      {
        iks* ft = iks_first_tag( pak->query );
        while( ( ft = iks_next_tag( pak->query ) ) != 0 )
        {
          if( iks_strncmp( iks_name( ft ), "username", 8 ) == 0 )
          {
            fields &= FIELD_USERNAME;
          }
          else if( iks_strncmp( iks_name( ft ), "nick", 4 ) == 0 )
          {
            fields &= FIELD_NICK;
          }
          else if( iks_strncmp( iks_name( ft ), "password", 8 ) == 0 )
          {
            fields &= FIELD_PASSWORD;
          }
          else if( iks_strncmp( iks_name( ft ), "name", 4 ) == 0 )
          {
            fields &= FIELD_NAME;
          }
          else if( iks_strncmp( iks_name( ft ), "first", 5 ) == 0 )
          {
            fields &= FIELD_FIRST;
          }
          else if( iks_strncmp( iks_name( ft ), "last", 4 ) == 0 )
          {
            fields &= FIELD_LAST;
          }
          else if( iks_strncmp( iks_name( ft ), "email", 5 ) == 0 )
          {
            fields &= FIELD_EMAIL;
          }
          else if( iks_strncmp( iks_name( ft ), "address", 7 ) == 0 )
          {
            fields &= FIELD_ADDRESS;
          }
          else if( iks_strncmp( iks_name( ft ), "city", 4 ) == 0 )
          {
            fields &= FIELD_CITY;
          }
          else if( iks_strncmp( iks_name( ft ), "state", 5 ) == 0 )
          {
            fields &= FIELD_STATE;
          }
          else if( iks_strncmp( iks_name( ft ), "zip", 3 ) == 0 )
          {
            fields &= FIELD_ZIP;
          }
          else if( iks_strncmp( iks_name( ft ), "phone", 5 ) == 0 )
          {
            fields &= FIELD_PHONE;
          }
          else if( iks_strncmp( iks_name( ft ), "url", 3 ) == 0 )
          {
            fields &= FIELD_URL;
          }
          else if( iks_strncmp( iks_name( ft ), "date", 4 ) == 0 )
          {
            fields &= FIELD_DATE;
          }
          else if( iks_strncmp( iks_name( ft ), "misc", 4 ) == 0 )
          {
            fields &= FIELD_MISC;
          }
          else if( iks_strncmp( iks_name( ft ), "text", 4 ) == 0 )
          {
            fields &= FIELD_TEXT;
          }
          else if( iks_strncmp( iks_name( ft ), "instructions", 4 ) == 0 )
          {
            instructions = ( iks_cdata( ft ) )?( iks_cdata( ft ) ):( "" );
          }
        }
      }

      if( m_registrationHandler )
        m_registrationHandler->handleRegistrationFields( fields, instructions );

      break;
    case IKS_TYPE_ERROR:

      break;
  }
}

void Registration::handleIqID( const char* id, ikspak* pak )
{
  if( !pak->query )
  {
    m_registrationHandler->handleResult( RegistrationHandler::REGISTRATION_SUCCESS );
    return;
  }
}

void Registration::clear()
{
  m_type = NO_OP_PENDING;
}

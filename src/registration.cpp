/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
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


#include "registration.h"

#include "clientbase.h"

namespace gloox
{

  Registration::Registration( ClientBase *parent )
    : m_parent( parent ), m_registrationHandler( 0 )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_REGISTER );
  }

  Registration::~Registration()
  {
    if( m_parent )
      m_parent->removeIqHandler( XMLNS_REGISTER );
  }

  void Registration::fetchRegistrationFields()
  {
    if( !m_parent || m_parent->state() != STATE_CONNECTED )
      return;

    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_GET, XMLNS_REGISTER );
    iks_insert_attrib( x, "id", id.c_str() );

    m_parent->trackID( this, id.c_str(), FETCH_REGISTRATION_FIELDS );
    m_parent->send( x );
  }

  void Registration::createAccount( int fields, fieldStruct values )
  {
    if( !m_parent )
      return;

    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_REGISTER );
    iks_insert_attrib( x, "id", id.c_str() );

    if( fields & FIELD_USERNAME )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "username" ), values.username.c_str(),
                        values.username.length() );
    if( fields & FIELD_NICK )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "nick" ), values.nick.c_str(), values.nick.length() );
    if( fields & FIELD_PASSWORD )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "password" ), values.password.c_str(),
                        values.password.length() );
    if( fields & FIELD_NAME )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "name" ), values.name.c_str(), values.name.length() );
    if( fields & FIELD_FIRST )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "first" ), values.first.c_str(),
                        values.first.length() );
    if( fields & FIELD_LAST )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "last" ), values.last.c_str(), values.last.length() );
    if( fields & FIELD_EMAIL )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "email" ), values.email.c_str(),
                        values.email.length() );
    if( fields & FIELD_ADDRESS )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "address" ), values.address.c_str(),
                        values.address.length() );
    if( fields & FIELD_CITY )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "city" ), values.city.c_str(), values.city.length() );
    if( fields & FIELD_STATE )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "state" ), values.state.c_str(),
                        values.state.length() );
    if( fields & FIELD_ZIP )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "zip" ), values.zip.c_str(), values.zip.length() );
    if( fields & FIELD_PHONE )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "phone" ), values.phone.c_str(),
                        values.phone.length() );
    if( fields & FIELD_URL )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "url" ), values.url.c_str(), values.url.length() );
    if( fields & FIELD_DATE )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "date" ), values.date.c_str(), values.date.length() );
    if( fields & FIELD_MISC )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "misc" ), values.misc.c_str(), values.misc.length() );
    if( fields & FIELD_TEXT )
      iks_insert_cdata( iks_insert( iks_first_tag( x ), "text" ), values.text.c_str(), values.text.length() );

    m_parent->trackID( this, id.c_str(), CREATE_ACCOUNT );
    m_parent->send( x );
  }

  void Registration::removeAccount()
  {
    if( !m_parent || ( m_parent->state() != STATE_AUTHENTICATED ) )
      return;

    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_REGISTER );
    iks_insert_attrib( x, "id", id.c_str() );
    iks_insert_attrib( x, "from", m_parent->jid().c_str() );
    iks_insert( iks_first_tag( x ), "remove" );

    m_parent->trackID( this, id.c_str(), REMOVE_ACCOUNT );
    m_parent->send( x );
  }

  void Registration::changePassword( const string& password )
  {
    if( !m_parent || ( m_parent->state() != STATE_AUTHENTICATED ) )
      return;

    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_REGISTER );
    iks_insert_attrib( x, "id", id.c_str() );
    iks_insert_attrib( x, "to", m_parent->server().c_str() );
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "username" ), m_parent->username().c_str(),
                      m_parent->username().length() );
    iks_insert_cdata( iks_insert( iks_first_tag( x ), "password" ), password.c_str(), password.length() );

    m_parent->trackID( this, id.c_str(), CHANGE_PASSWORD );
    m_parent->send( x );
  }

  void Registration::registerRegistrationHandler( RegistrationHandler *rh )
  {
    m_registrationHandler = rh;
  }

  void Registration::removeRegistrationHandler( RegistrationHandler *rh )
  {
    m_registrationHandler = 0;
  }

  void Registration::handleIq( const char *tag, const char *xmlns, ikspak *pak )
  {
    if( pak->subtype == IKS_TYPE_ERROR)
    {
      iks *ft = iks_child( iks_find( pak->x, "error" ) );

      if( !ft || !m_registrationHandler )
        return;

      if( iks_strncmp( iks_name( ft ), "conflict", 8 ) == 0 )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_CONFLICT );
      else if( iks_strncmp( iks_name( ft ), "not-acceptable", 14 ) == 0 )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_ACCEPTABLE );
      else if( iks_strncmp( iks_name( ft ), "bad-request", 11 ) == 0 )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_BAD_REQUEST );
      else if( iks_strncmp( iks_name( ft ), "forbidden", 9 ) == 0 )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_FORBIDDEN );
      else if( iks_strncmp( iks_name( ft ), "registration-required", 22) == 0 )
        m_registrationHandler->handleRegistrationResult(
            RegistrationHandler::REGISTRATION_REGISTRATION_REQUIRED );
      else if( iks_strncmp( iks_name( ft ), "unexpected-request", 19 ) == 0 )
        m_registrationHandler->handleRegistrationResult(
            RegistrationHandler::REGISTRATION_UNEXPECTED_REQUEST );
      else if( iks_strncmp( iks_name( ft ), "not-authorized", 14 ) == 0 )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_AUTHORIZED );
      else if( iks_strncmp( iks_name( ft ), "not-allowed", 11 ) == 0 )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_ALLOWED );
    }
  }

  void Registration::handleIqID( const char *id, ikspak *pak, int context )
  {
    if( pak->subtype != IKS_TYPE_RESULT )
      return;

    if( !m_registrationHandler )
      return;

    switch( context )
    {
      case FETCH_REGISTRATION_FIELDS:
        if( iks_find( pak->query, "registered" ) )
        {
          m_registrationHandler->handleAlreadyRegistered();
          break;
        }

        {
          int fields = 0;
          string instructions;

          iks *ft = iks_first_tag( pak->query );
          do
          {
            if( iks_strncmp( iks_name( ft ), "username", 8 ) == 0 )
            {
              fields |= FIELD_USERNAME;
            }
            else if( iks_strncmp( iks_name( ft ), "nick", 4 ) == 0 )
            {
              fields |= FIELD_NICK;
            }
            else if( iks_strncmp( iks_name( ft ), "password", 8 ) == 0 )
            {
              fields |= FIELD_PASSWORD;
            }
            else if( iks_strncmp( iks_name( ft ), "name", 4 ) == 0 )
            {
              fields |= FIELD_NAME;
            }
            else if( iks_strncmp( iks_name( ft ), "first", 5 ) == 0 )
            {
              fields |= FIELD_FIRST;
            }
            else if( iks_strncmp( iks_name( ft ), "last", 4 ) == 0 )
            {
              fields |= FIELD_LAST;
            }
            else if( iks_strncmp( iks_name( ft ), "email", 5 ) == 0 )
            {
              fields |= FIELD_EMAIL;
            }
            else if( iks_strncmp( iks_name( ft ), "address", 7 ) == 0 )
            {
              fields |= FIELD_ADDRESS;
            }
            else if( iks_strncmp( iks_name( ft ), "city", 4 ) == 0 )
            {
              fields |= FIELD_CITY;
            }
            else if( iks_strncmp( iks_name( ft ), "state", 5 ) == 0 )
            {
              fields |= FIELD_STATE;
            }
            else if( iks_strncmp( iks_name( ft ), "zip", 3 ) == 0 )
            {
              fields |= FIELD_ZIP;
            }
            else if( iks_strncmp( iks_name( ft ), "phone", 5 ) == 0 )
            {
              fields |= FIELD_PHONE;
            }
            else if( iks_strncmp( iks_name( ft ), "url", 3 ) == 0 )
            {
              fields |= FIELD_URL;
            }
            else if( iks_strncmp( iks_name( ft ), "date", 4 ) == 0 )
            {
              fields |= FIELD_DATE;
            }
            else if( iks_strncmp( iks_name( ft ), "misc", 4 ) == 0 )
            {
              fields |= FIELD_MISC;
            }
            else if( iks_strncmp( iks_name( ft ), "text", 4 ) == 0 )
            {
              fields |= FIELD_TEXT;
            }
            else if( iks_strncmp( iks_name( ft ), "instructions", 4 ) == 0 )
            {
              instructions = ( iks_cdata( iks_child( ft ) ) )?( iks_cdata( iks_child( ft ) ) ):( "" );
            }
          } while( ( ft = iks_next_tag( ft ) ) != 0 );

          m_registrationHandler->handleRegistrationFields( fields, instructions );
        }
        break;

      case CREATE_ACCOUNT:
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_SUCCESS );
        break;

      case CHANGE_PASSWORD:
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_SUCCESS );
        break;

      case REMOVE_ACCOUNT:
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_SUCCESS );
        break;
    }
  }

};

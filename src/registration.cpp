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
#include "stanza.h"

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

    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "get" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_REGISTER );

    iq.addChild( q );
    m_parent->trackID( this, id, FETCH_REGISTRATION_FIELDS );
    m_parent->send( iq );
  }

  void Registration::createAccount( int fields, fieldStruct values )
  {
    if( !m_parent )
      return;

    const std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "id", id );
    iq.addAttrib( "type", "set" );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_REGISTER );

    if( fields & FIELD_USERNAME )
      q.addChild( Tag( "username", values.username ) );
    if( fields & FIELD_NICK )
      q.addChild( Tag( "nick", values.nick ) );
    if( fields & FIELD_PASSWORD )
      q.addChild( Tag( "password", values.password ) );
    if( fields & FIELD_NAME )
      q.addChild( Tag( "name", values.name ) );
    if( fields & FIELD_FIRST )
      q.addChild( Tag( "first", values.first ) );
    if( fields & FIELD_LAST )
      q.addChild( Tag( "last", values.last ) );
    if( fields & FIELD_EMAIL )
      q.addChild( Tag( "email", values.email ) );
    if( fields & FIELD_ADDRESS )
      q.addChild( Tag( "address", values.address ) );
    if( fields & FIELD_CITY )
      q.addChild( Tag( "city", values.city ) );
    if( fields & FIELD_STATE )
      q.addChild( Tag( "state", values.state ) );
    if( fields & FIELD_ZIP )
      q.addChild( Tag( "zip", values.zip ) );
    if( fields & FIELD_PHONE )
      q.addChild( Tag( "phone", values.phone ) );
    if( fields & FIELD_URL )
      q.addChild( Tag( "url", values.url ) );
    if( fields & FIELD_DATE )
      q.addChild( Tag( "date", values.date ) );
    if( fields & FIELD_MISC )
      q.addChild( Tag( "misc", values.misc ) );
    if( fields & FIELD_TEXT )
      q.addChild( Tag( "text", values.text ) );

    iq.addChild( q );
    m_parent->trackID( this, id, CREATE_ACCOUNT );
    m_parent->send( iq );
  }

  void Registration::removeAccount()
  {
    if( !m_parent || ( m_parent->state() != STATE_AUTHENTICATED ) )
      return;

    const std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    iq.addAttrib( "from", m_parent->jid().full() );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_REGISTER );
    q.addChild( Tag( "remove" ) );
    iq.addChild( q );

    m_parent->trackID( this, id, REMOVE_ACCOUNT );
    m_parent->send( iq );
  }

  void Registration::changePassword( const std::string& password )
  {
    if( !m_parent || ( m_parent->state() != STATE_AUTHENTICATED ) )
      return;

    const std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    iq.addAttrib( "to", m_parent->server() );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_REGISTER );
    q.addChild( Tag( "username", m_parent->username() ) );
    q.addChild( Tag( "password", password ) );
    iq.addChild( q );

    m_parent->trackID( this, id, CHANGE_PASSWORD );
    m_parent->send( iq );
  }

  void Registration::registerRegistrationHandler( RegistrationHandler *rh )
  {
    m_registrationHandler = rh;
  }

  void Registration::removeRegistrationHandler( RegistrationHandler *rh )
  {
    m_registrationHandler = 0;
  }

  bool Registration::handleIq( const Stanza& stanza )
  {
    if( stanza.subtype() == STANZA_IQ_ERROR )
    {
      Tag e = stanza.findChild( "error" );

      if( e.empty() || !m_registrationHandler )
        return false;

      if( e.hasChild( "conflict" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_CONFLICT );
      else if( e.hasChild( "not-acceptable" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_ACCEPTABLE );
      else if( e.hasChild( "bad-request" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_BAD_REQUEST );
      else if( e.hasChild( "forbidden" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_FORBIDDEN );
      else if( e.hasChild( "registration-required" ) )
        m_registrationHandler->handleRegistrationResult(
            RegistrationHandler::REGISTRATION_REGISTRATION_REQUIRED );
      else if( e.hasChild( "unexpected-request" ) )
        m_registrationHandler->handleRegistrationResult(
            RegistrationHandler::REGISTRATION_UNEXPECTED_REQUEST );
      else if( e.hasChild( "not-authorized" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_AUTHORIZED );
      else if( e.hasChild( "not-allowed" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_ALLOWED );
    }
    return false;
  }

  bool Registration::handleIqID( const Stanza& stanza, int context )
  {
    if( stanza.subtype() != STANZA_IQ_RESULT )
      return false;

    if( !m_registrationHandler )
      return false;

    switch( context )
    {
      case FETCH_REGISTRATION_FIELDS:
      {
        Tag q = stanza.findChild( "query" );

        if( q.hasChild( "registered" ) )
        {
          m_registrationHandler->handleAlreadyRegistered();
          break;
        }

        int fields = 0;
        std::string instructions;

        if( q.hasChild( "username" ) )
          fields |= FIELD_USERNAME;
        if( q.hasChild( "nick" ) )
          fields |= FIELD_NICK;
        if( q.hasChild( "password" ) )
          fields |= FIELD_PASSWORD;
        if( q.hasChild( "name" ) )
          fields |= FIELD_NAME;
        if( q.hasChild( "first" ) )
          fields |= FIELD_FIRST;
        if( q.hasChild( "last" ) )
            fields |= FIELD_LAST;
        if( q.hasChild( "email" ) )
          fields |= FIELD_EMAIL;
        if( q.hasChild( "address" ) )
          fields |= FIELD_ADDRESS;
        if( q.hasChild( "city" ) )
          fields |= FIELD_CITY;
        if( q.hasChild( "state" ) )
          fields |= FIELD_STATE;
        if( q.hasChild( "zip" ) )
          fields |= FIELD_ZIP;
        if( q.hasChild( "phone" ) )
          fields |= FIELD_PHONE;
        if( q.hasChild( "url" ) )
          fields |= FIELD_URL;
        if( q.hasChild( "date" ) )
          fields |= FIELD_DATE;
        if( q.hasChild( "misc" ) )
          fields |= FIELD_MISC;
        if( q.hasChild( "text" ) )
          fields |= FIELD_TEXT;
        if( q.hasChild( "instructions" ) )
          instructions = q.findChild( "instructions" ).cdata();

        m_registrationHandler->handleRegistrationFields( fields, instructions );
        break;
      }

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
    return false;
  }

};

/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "registration.h"

#include "clientbase.h"
#include "stanza.h"
#include "prep.h"

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

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "type", "get" );
    iq->addAttribute( "id", id );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );

    m_parent->trackID( this, id, FETCH_REGISTRATION_FIELDS );
    m_parent->send( iq );
  }

  void Registration::createAccount( int fields, fieldStruct values )
  {
    if( !m_parent )
      return;

    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "id", id );
    iq->addAttribute( "type", "set" );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );

    if( fields & FIELD_USERNAME )
      q->addChild( new Tag( "username", Prep::nodeprep( values.username ) ) );
    if( fields & FIELD_NICK )
      q->addChild( new Tag( "nick", values.nick ) );
    if( fields & FIELD_PASSWORD )
      q->addChild( new Tag( "password", values.password ) );
    if( fields & FIELD_NAME )
      q->addChild( new Tag( "name", values.name ) );
    if( fields & FIELD_FIRST )
      q->addChild( new Tag( "first", values.first ) );
    if( fields & FIELD_LAST )
      q->addChild( new Tag( "last", values.last ) );
    if( fields & FIELD_EMAIL )
      q->addChild( new Tag( "email", values.email ) );
    if( fields & FIELD_ADDRESS )
      q->addChild( new Tag( "address", values.address ) );
    if( fields & FIELD_CITY )
      q->addChild( new Tag( "city", values.city ) );
    if( fields & FIELD_STATE )
      q->addChild( new Tag( "state", values.state ) );
    if( fields & FIELD_ZIP )
      q->addChild( new Tag( "zip", values.zip ) );
    if( fields & FIELD_PHONE )
      q->addChild( new Tag( "phone", values.phone ) );
    if( fields & FIELD_URL )
      q->addChild( new Tag( "url", values.url ) );
    if( fields & FIELD_DATE )
      q->addChild( new Tag( "date", values.date ) );
    if( fields & FIELD_MISC )
      q->addChild( new Tag( "misc", values.misc ) );
    if( fields & FIELD_TEXT )
      q->addChild( new Tag( "text", values.text ) );

    m_parent->trackID( this, id, CREATE_ACCOUNT );
    m_parent->send( iq );
  }

  void Registration::removeAccount()
  {
    if( !m_parent || !m_parent->authed() )
      return;

    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "type", "set" );
    iq->addAttribute( "id", id );
    iq->addAttribute( "from", m_parent->jid().full() );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );
    q->addChild( new Tag( "remove" ) );

    m_parent->trackID( this, id, REMOVE_ACCOUNT );
    m_parent->send( iq );
  }

  void Registration::changePassword( const std::string& password )
  {
    if( !m_parent || !m_parent->authed() )
      return;

    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "type", "set" );
    iq->addAttribute( "id", id );
    iq->addAttribute( "to", m_parent->server() );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );
    q->addChild( new Tag( "username", m_parent->username() ) );
    q->addChild( new Tag( "password", password ) );

    m_parent->trackID( this, id, CHANGE_PASSWORD );
    m_parent->send( iq );
  }

  void Registration::registerRegistrationHandler( RegistrationHandler *rh )
  {
    m_registrationHandler = rh;
  }

  void Registration::removeRegistrationHandler()
  {
    m_registrationHandler = 0;
  }

  bool Registration::handleIq( Stanza *stanza )
  {
    if( stanza->subtype() == STANZA_IQ_ERROR )
    {
      Tag *e = stanza->findChild( "error" );

      if( e->empty() || !m_registrationHandler )
        return false;

      if( e->hasChild( "conflict" ) || e->hasAttribute( "code", "409" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_CONFLICT );
      else if( e->hasChild( "not-acceptable" ) || e->hasAttribute( "code", "406" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_ACCEPTABLE );
      else if( e->hasChild( "bad-request" ) || e->hasAttribute( "code", "400" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_BAD_REQUEST );
      else if( e->hasChild( "forbidden" ) || e->hasAttribute( "code", "403" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_FORBIDDEN );
      else if( e->hasChild( "registration-required" ) || e->hasAttribute( "code", "407" ) )
        m_registrationHandler->handleRegistrationResult(
            RegistrationHandler::REGISTRATION_REGISTRATION_REQUIRED );
      else if( e->hasChild( "unexpected-request" ) || e->hasAttribute( "code", "400" ) )
        m_registrationHandler->handleRegistrationResult(
            RegistrationHandler::REGISTRATION_UNEXPECTED_REQUEST );
      else if( e->hasChild( "not-authorized" ) || e->hasAttribute( "code", "401" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_AUTHORIZED );
      else if( e->hasChild( "not-allowed" ) || e->hasAttribute( "code", "405" ) )
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::REGISTRATION_NOT_ALLOWED );
      else
        m_registrationHandler->handleRegistrationResult( RegistrationHandler::UNKNOWN_ERROR );
    }
    return false;
  }

  bool Registration::handleIqID( Stanza *stanza, int context )
  {
    if( stanza->subtype() != STANZA_IQ_RESULT )
      return false;

    if( !m_registrationHandler )
      return false;

    switch( context )
    {
      case FETCH_REGISTRATION_FIELDS:
      {
        Tag *q = stanza->findChild( "query" );

        if( q->hasChild( "registered" ) )
        {
          m_registrationHandler->handleAlreadyRegistered();
          break;
        }

        int fields = 0;
        std::string instructions;

        if( q->hasChild( "username" ) )
          fields |= FIELD_USERNAME;
        if( q->hasChild( "nick" ) )
          fields |= FIELD_NICK;
        if( q->hasChild( "password" ) )
          fields |= FIELD_PASSWORD;
        if( q->hasChild( "name" ) )
          fields |= FIELD_NAME;
        if( q->hasChild( "first" ) )
          fields |= FIELD_FIRST;
        if( q->hasChild( "last" ) )
            fields |= FIELD_LAST;
        if( q->hasChild( "email" ) )
          fields |= FIELD_EMAIL;
        if( q->hasChild( "address" ) )
          fields |= FIELD_ADDRESS;
        if( q->hasChild( "city" ) )
          fields |= FIELD_CITY;
        if( q->hasChild( "state" ) )
          fields |= FIELD_STATE;
        if( q->hasChild( "zip" ) )
          fields |= FIELD_ZIP;
        if( q->hasChild( "phone" ) )
          fields |= FIELD_PHONE;
        if( q->hasChild( "url" ) )
          fields |= FIELD_URL;
        if( q->hasChild( "date" ) )
          fields |= FIELD_DATE;
        if( q->hasChild( "misc" ) )
          fields |= FIELD_MISC;
        if( q->hasChild( "text" ) )
          fields |= FIELD_TEXT;
        if( q->hasChild( "instructions" ) )
          instructions = q->findChild( "instructions" )->cdata();

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

}

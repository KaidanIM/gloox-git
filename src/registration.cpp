/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
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
    if( !m_parent || m_parent->state() != StateConnected )
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

  void Registration::createAccount( int fields, const fieldStruct& values )
  {
    if( !m_parent || m_parent->state() != StateConnected )
      return;

    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "id", id );
    iq->addAttribute( "type", "set" );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );

    if( fields & FIELD_USERNAME )
      new Tag( q, "username", Prep::nodeprep( values.username ) );
    if( fields & FIELD_NICK )
      new Tag( q, "nick", values.nick );
    if( fields & FIELD_PASSWORD )
      new Tag( q, "password", values.password );
    if( fields & FIELD_NAME )
      new Tag( q, "name", values.name );
    if( fields & FIELD_FIRST )
      new Tag( q, "first", values.first );
    if( fields & FIELD_LAST )
      new Tag( q, "last", values.last );
    if( fields & FIELD_EMAIL )
      new Tag( q, "email", values.email );
    if( fields & FIELD_ADDRESS )
      new Tag( q, "address", values.address );
    if( fields & FIELD_CITY )
      new Tag( q, "city", values.city );
    if( fields & FIELD_STATE )
      new Tag( q, "state", values.state );
    if( fields & FIELD_ZIP )
      new Tag( q, "zip", values.zip );
    if( fields & FIELD_PHONE )
      new Tag( q, "phone", values.phone );
    if( fields & FIELD_URL )
      new Tag( q, "url", values.url );
    if( fields & FIELD_DATE )
      new Tag( q, "date", values.date );
    if( fields & FIELD_MISC )
      new Tag( q, "misc", values.misc );
    if( fields & FIELD_TEXT )
      new Tag( q, "text", values.text );

    m_parent->trackID( this, id, CREATE_ACCOUNT );
    m_parent->send( iq );
  }

  void Registration::createAccount( const DataForm& form )
  {
    if( !m_parent || m_parent->state() != StateConnected )
      return;

    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "id", id );
    iq->addAttribute( "type", "set" );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );

    const Tag *tmp = form.tag();
    if( tmp )
    {
      q->addChild( tmp->clone() );
      delete tmp;
      m_parent->trackID( this, id, CREATE_ACCOUNT );
      m_parent->send( iq );
    }
    else
      delete iq;
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
    new Tag( q, "remove" );

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
    iq->addAttribute( "to", m_parent->jid().server() );
    Tag *q = new Tag( iq, "query" );
    q->addAttribute( "xmlns", XMLNS_REGISTER );
    new Tag( q, "username", m_parent->username() );
    new Tag( q, "password", password );

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

  bool Registration::handleIqID( Stanza* stanza, int context )
  {
    if( !m_registrationHandler )
      return false;

    if( stanza->subtype() == StanzaIqResult )
    {
      switch( context )
      {
        case FetchRegistrationFields:
        {
          Tag *q = stanza->findChild( "query" );
          if( !q )
            return false;

          if( q && q->hasChild( "registered" ) )
          {
            m_registrationHandler->handleAlreadyRegistered( stanza->from() );
            break;
          }

          if( q->hasChild( "x", "xmlns", XMLNS_X_DATA ) )
          {
            DataForm form( q->findChild( "x", "xmlns", XMLNS_X_DATA ) );
            m_registrationHandler->handleDataForm( stanza->from(), form );
          }

          if( q->hasChild( "x", "xmlns", XMLNS_X_OOB ) )
          {
            OOB oob( q->findChild( "x", "xmlns", XMLNS_X_OOB ) );
            m_registrationHandler->handleOOB( stanza->from(), oob );
          }

          int fields = 0;
          std::string instructions;

          if( q->hasChild( "username" ) )
            fields |= FieldUsername;
          if( q->hasChild( "nick" ) )
            fields |= FieldNick;
          if( q->hasChild( "password" ) )
            fields |= FieldPassword;
          if( q->hasChild( "name" ) )
            fields |= FieldName;
          if( q->hasChild( "first" ) )
            fields |= FieldFirst;
          if( q->hasChild( "last" ) )
            fields |= FieldLast;
          if( q->hasChild( "email" ) )
            fields |= FieldEmail;
          if( q->hasChild( "address" ) )
            fields |= FieldAddress;
          if( q->hasChild( "city" ) )
            fields |= FieldCity;
          if( q->hasChild( "state" ) )
            fields |= FieldState;
          if( q->hasChild( "zip" ) )
            fields |= FieldZip;
          if( q->hasChild( "phone" ) )
            fields |= FieldPhone;
          if( q->hasChild( "url" ) )
            fields |= FieldUrl;
          if( q->hasChild( "date" ) )
            fields |= FieldDate;
          if( q->hasChild( "misc" ) )
            fields |= FieldMisc;
          if( q->hasChild( "text" ) )
            fields |= FieldText;
          if( q->hasChild( "instructions" ) )
            instructions = q->findChild( "instructions" )->cdata();

          m_registrationHandler->handleRegistrationFields( stanza->from(), fields, instructions );
          break;
        }

        case CreateAccount:
          m_registrationHandler->handleRegistrationResult( stanza->from(), RegistrationSuccess );
          break;

        case ChangePassword:
          m_registrationHandler->handleRegistrationResult( stanza->from(), RegistrationSuccess );
          break;

        case RemoveAccount:
          m_registrationHandler->handleRegistrationResult( stanza->from(), RegistrationSuccess );
          break;
      }
    }
    else if( stanza->subtype() == StanzaIqError )
    {
      Tag *e = stanza->findChild( "error" );

      if( !e )
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

}

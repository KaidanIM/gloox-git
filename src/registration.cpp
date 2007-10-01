/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
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
#include "error.h"
#include "prep.h"
#include "oob.h"

namespace gloox
{

  Registration::Registration( ClientBase* parent, const JID& to )
    : m_parent( parent ), m_to( to ), m_registrationHandler( 0 )
  {
    init();
  }

  Registration::Registration( ClientBase* parent )
  : m_parent( parent ), m_registrationHandler( 0 )
  {
    init();
  }

  void Registration::init()
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_REGISTER );
  }

  Registration::~Registration()
  {
    if( m_parent )
      m_parent->removeIqHandler( this, XMLNS_REGISTER );
  }

  void Registration::fetchRegistrationFields()
  {
    if( !m_parent || m_parent->state() != StateConnected )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Get, m_to, id, XMLNS_REGISTER );

    m_parent->trackID( this, id, FetchRegistrationFields );
    m_parent->send( iq );
  }

  bool Registration::createAccount( int fields, const RegistrationFields& values )
  {
    std::string username;
    if( !m_parent || m_parent->state() != StateConnected
         || !prep::nodeprep( values.username, username ) )
      return false;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, m_to, id, XMLNS_REGISTER );
    Tag* q = iq->query();

    if( fields & FieldUsername )
      new Tag( q, "username", username );
    if( fields & FieldNick )
      new Tag( q, "nick", values.nick );
    if( fields & FieldPassword )
      new Tag( q, "password", values.password );
    if( fields & FieldName )
      new Tag( q, "name", values.name );
    if( fields & FieldFirst )
      new Tag( q, "first", values.first );
    if( fields & FieldLast )
      new Tag( q, "last", values.last );
    if( fields & FieldEmail )
      new Tag( q, "email", values.email );
    if( fields & FieldAddress )
      new Tag( q, "address", values.address );
    if( fields & FieldCity )
      new Tag( q, "city", values.city );
    if( fields & FieldState )
      new Tag( q, "state", values.state );
    if( fields & FieldZip )
      new Tag( q, "zip", values.zip );
    if( fields & FieldPhone )
      new Tag( q, "phone", values.phone );
    if( fields & FieldUrl )
      new Tag( q, "url", values.url );
    if( fields & FieldDate )
      new Tag( q, "date", values.date );
    if( fields & FieldMisc )
      new Tag( q, "misc", values.misc );
    if( fields & FieldText )
      new Tag( q, "text", values.text );

    m_parent->trackID( this, id, CreateAccount );
    m_parent->send( iq );

    return true;
  }

  void Registration::createAccount( const DataForm::Submit& form )
  {
    if( !m_parent || m_parent->state() != StateConnected )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, m_to, id, XMLNS_REGISTER );
    iq->query()->addChild( form.tag() );

    m_parent->trackID( this, id, CreateAccount );
    m_parent->send( iq );
  }

  void Registration::removeAccount()
  {
    if( !m_parent || !m_parent->authed() )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, m_to, id, XMLNS_REGISTER );
    iq->addAttribute( "from", m_parent->jid().full() );
    new Tag( iq->query(), "remove" );

    m_parent->trackID( this, id, RemoveAccount );
    m_parent->send( iq );
  }

  void Registration::changePassword( const std::string& username, const std::string& password )
  {
    if( !m_parent || !m_parent->authed() )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, m_to, id, XMLNS_REGISTER );
    new Tag( iq->query(), "username", username );
    new Tag( iq->query(), "password", password );

    m_parent->trackID( this, id, ChangePassword );
    m_parent->send( iq );
  }

  void Registration::registerRegistrationHandler( RegistrationHandler* rh )
  {
    m_registrationHandler = rh;
  }

  void Registration::removeRegistrationHandler()
  {
    m_registrationHandler = 0;
  }

  void Registration::handleIqID( IQ* iq, int context )
  {
    if( !m_registrationHandler )
      return;

    if( iq->subtype() == IQ::Result )
    {
      switch( context )
      {
        case FetchRegistrationFields:
        {
          const Tag* q = iq->query();
          if( !q )
            return;

          if( q->hasChild( "registered" ) )
          {
            m_registrationHandler->handleAlreadyRegistered( iq->from() );
            break;
          }

          if( q->hasChild( "x", XMLNS, XMLNS_X_DATA ) )
          {
            const DataForm::FormBase form( q->findChild( "x", XMLNS, XMLNS_X_DATA ) );
            m_registrationHandler->handleDataForm( iq->from(), form );
          }

          if( q->hasChild( "x", XMLNS, XMLNS_X_OOB ) )
          {
            const OOB oob( q->findChild( "x", XMLNS, XMLNS_X_OOB ) );
            m_registrationHandler->handleOOB( iq->from(), oob );
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

          m_registrationHandler->handleRegistrationFields( iq->from(), fields, instructions );
          break;
        }

        case CreateAccount:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationSuccess );
          break;

        case ChangePassword:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationSuccess );
          break;

        case RemoveAccount:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationSuccess );
          break;
      }
    }
    else if( iq->subtype() == IQ::Error )
    {
      const Error* e = iq->error();
      if( !e )
        return;

      switch( e->error() )
      {
        case StanzaErrorConflict:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationConflict );
          break;
        case StanzaErrorNotAcceptable:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationNotAcceptable );
          break;
        case StanzaErrorBadRequest:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationBadRequest );
          break;
        case StanzaErrorForbidden:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationForbidden );
          break;
        case StanzaErrorRegistrationRequired:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationRequired );
          break;
        case StanzaErrorUnexpectedRequest:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationUnexpectedRequest );
          break;
        case StanzaErrorNotAuthorized:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationNotAuthorized );
          break;
        case StanzaErrorNotAllowed:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationNotAllowed );
          break;
        default:
          m_registrationHandler->handleRegistrationResult( iq->from(), RegistrationUnknownError );
          break;

      }
    }

  }

}

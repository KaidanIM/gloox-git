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
  : m_parent( parent )
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

void Registration::handleIq( const char* xmlns, ikspak* pak )
{

}

void Registration::handleIqID( const char* id, ikspak* pak )
{
}

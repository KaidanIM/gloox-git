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


#include "jclient.h"
#include "roster.h"
#include "disco.h"
#include "rosterlistener.h"


Roster::Roster( JClient* parent )
  : m_parent( parent ),  m_rosterListener( 0 )
{
  m_parent->registerIqHandler( this, XMLNS_ROSTER );
//   m_parent->disco()->registerDiscoHandler( this );
  m_parent->registerPresenceHandler( this );
  m_parent->registerSubscriptionHandler( this );
}

Roster::~Roster()
{

}

RosterHelper::RosterMap Roster::roster()
{
  return m_roster;
}

void Roster::fill()
{
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_ROSTER );
  iks_insert_attrib( x, "id", m_parent->getID().c_str() );
  m_parent->send( x );
}

void Roster::handleIq( const char* xmlns, ikspak* pak )
{
  if( pak->subtype == IKS_TYPE_RESULT )
  {
    if( iks_strncmp( xmlns, XMLNS_ROSTER, strlen( XMLNS_ROSTER ) ) == 0 )
    {
//       printf( "roster arriving\n");
      iks* y = iks_first_tag( iks_first_tag( pak->x ) );
      while( y )
      {
        if( strcmp( iks_name( y ), "item" ) == 0 )
        {
          char* jid = iks_find_attrib( y, "jid" );
          add( jid, IKS_SHOW_UNAVAILABLE );
//           printf( "received roster item: %s\n", jid );
        }
        y = iks_next_tag( y );
      }
    }
    if( m_rosterListener )
      m_rosterListener->roster( m_roster );

    m_parent->sendPresence();
  }
}

void Roster::handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg )
{
  if( m_rosterListener )
  {
    if( m_roster[from->full] == IKS_SHOW_UNAVAILABLE )
      m_rosterListener->itemAvailable( from->full, msg );
    else if( show == IKS_SHOW_UNAVAILABLE )
      m_rosterListener->itemUnavailable( from->full, msg );
    else
      m_rosterListener->itemChanged( from->full, show, msg );
  }
  m_roster[from->full] = show;
}

void Roster::subscribe( const string& jid, const string& msg )
{
  if( m_roster.find( jid ) != m_roster.end() )
  {
    iks* x = iks_make_s10n( IKS_TYPE_SUBSCRIBE, jid.c_str(), msg.c_str() );
    m_parent->send( x );
  }
}

void Roster::handleSubscription( iksid* from, iksubtype type, const char* msg )
{
  switch( type )
  {
    case IKS_TYPE_SUBSCRIBE:
      if( m_rosterListener )
      {
        if ( m_rosterListener->subscriptionRequest( string(from->full), msg ) )
        {
          iks* x = iks_make_s10n( IKS_TYPE_SUBSCRIBED, from->full, "ok" );
          m_parent->send( x );

          x = iks_make_s10n( IKS_TYPE_SUBSCRIBE, from->full, "" );
          m_parent->send( x );
        }
      }
      break;
    case IKS_TYPE_SUBSCRIBED:
      m_rosterListener->itemAdded( from->full );
      break;
    case IKS_TYPE_UNSUBSCRIBE:
    {
      iks* y = iks_make_s10n( IKS_TYPE_UNSUBSCRIBE, from->full, "ok" );
      m_parent->send( y );

      y = iks_make_iq( IKS_TYPE_SET, XMLNS_ROSTER );
      iks* z = iks_first_tag( y );
      iks* a = iks_insert( z, "item" );
      iks_insert_attrib( a, "jid", from->partial );
      iks_insert_attrib( a, "subscription", "remove" );
      m_parent->send( y );
      break;
    }
    case IKS_TYPE_UNSUBSCRIBED:
      if( m_rosterListener )
        m_rosterListener->itemRemoved( from->full );
      break;
  }
}

// void Roster::handleDiscoInfoResult( const string& id, const ikspak* pak )
// {
//   Identity ident;
//   
// //   printf("received disco result\n");
// }

void Roster::unsubscribe( const string& jid, const string& msg )
{
//   if( m_roster.find( jid ) )
  {
    iks* x = iks_make_s10n( IKS_TYPE_UNSUBSCRIBE, jid.c_str(), msg.c_str() );
    m_parent->send( x );
  }
  if( m_rosterListener )
    m_rosterListener->itemRemoved( jid );
}

void Roster::add( const string& jid, int status )
{
  m_roster[jid] = status;
}

void Roster::registerRosterListener( RosterListener* rl )
{
   m_rosterListener = rl;
}

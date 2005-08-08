/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
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


#include "clientbase.h"
#include "rostermanager.h"
#include "disco.h"
#include "rosteritem.h"
#include "rosterlistener.h"


namespace gloox
{

  RosterManager::RosterManager( ClientBase *parent )
    : m_parent( parent ), m_rosterListener( 0 )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_ROSTER );
      m_parent->registerPresenceHandler( this );
      m_parent->registerSubscriptionHandler( this );
    }
  }

  RosterManager::~RosterManager()
  {
    if( m_parent )
    {
      m_parent->removeIqHandler( XMLNS_ROSTER );
      m_parent->removePresenceHandler( this );
      m_parent->removeSubscriptionHandler( this );
    }
  }

  RosterListener::Roster* RosterManager::roster()
  {
    return &m_roster;
  }

  void RosterManager::fill()
  {
    iks *x = iks_make_iq( IKS_TYPE_GET, XMLNS_ROSTER );
    iks_insert_attrib( x, "id", m_parent->getID().c_str() );
    m_parent->send( x );
  }

  void RosterManager::handleIq( const char *tag, const char *xmlns, ikspak *pak )
  {
    if( pak->subtype == IKS_TYPE_RESULT ) // initial roster
    {
      if( iks_strncmp( xmlns, XMLNS_ROSTER, strlen( XMLNS_ROSTER ) ) == 0 )
      {
        iks *y = iks_first_tag( iks_first_tag( pak->x ) );
        while( y )
        {
          if( strncmp( iks_name( y ), "item", 4 ) == 0 )
          {
            char *jid = iks_find_attrib( y, "jid" );
            add( jid );
          }
          y = iks_next_tag( y );
        }
      }
      if( m_rosterListener )
        m_rosterListener->roster( m_roster );
    }
    else if( pak->subtype == IKS_TYPE_SET ) // roster item push
    {
      // handle roster pushes!
      iks *item = iks_first_tag( pak->query );
      while( item )
      {
        const char *jid = iks_find_attrib( item, "jid" );
        if( !jid )
        {
          item = iks_next( item );
          continue;
        }

        const char *sub = iks_find_attrib( item, "subscription" );
        if( iks_strncmp( sub, "remove", 6 ) == 0 )
        {
          if( m_roster.find( jid ) != m_roster.end() )
          {
            m_roster.erase( jid );
            if( m_rosterListener )
              m_rosterListener->itemRemoved( jid );
          }
          item = iks_next( item );
          continue;
        }

        const char *name = iks_find_attrib( item, "name" );
        bool ask = (bool)iks_find_attrib( item, "ask" );
        iks *group = iks_find( item, "group" );
        RosterItem::GroupList groups;
        while( group )
        {
          groups.push_back( iks_cdata( iks_child( group ) ) );
          group = iks_next( group );
        }

        if( m_roster.find( jid ) == m_roster.end() )
          m_roster[jid] = new RosterItem( jid );

        m_roster[jid]->setGroups( groups );
        if( name )
          m_roster[jid]->setName( name );
        if( sub )
          m_roster[jid]->setSubscription( sub, ask );
        m_roster[jid]->setSynchronized();

        if( m_rosterListener )
          m_rosterListener->itemAdded( jid );

        item = iks_next( item );
      }

      iks *x = iks_new( "iq" );
      iks_insert_attrib( x, "type", "result" );
      iks_insert_attrib( x, "id", pak->id );
      m_parent->send( x );
    }
  }

  void RosterManager::handlePresence( iksid *from, iksubtype type, ikshowtype show, const char *msg )
  {
    m_roster[from->full]->setStatus( show );
    m_roster[from->full]->setStatusMsg( msg );

    if( m_rosterListener )
    {
      if( show == IKS_SHOW_AVAILABLE )
        m_rosterListener->itemAvailable( (*m_roster[from->full]), msg );
      else if( show == IKS_SHOW_UNAVAILABLE )
        m_rosterListener->itemUnavailable( (*m_roster[from->full]), msg );
      else
        m_rosterListener->itemChanged( (*m_roster[from->full]), show, msg );
    }
  }

  void RosterManager::subscribe( const string& jid, const string& name,
                                RosterItem::GroupList& groups, const string& msg )
  {
    if( jid.empty() )
      return;

    add( jid, name, groups );

    iks *x = iks_make_s10n( IKS_TYPE_SUBSCRIBE, jid.c_str(), msg.c_str() );
    m_parent->send( x );
  }


  void RosterManager::add( const string& jid, const string& name, RosterItem::GroupList& groups )
  {
    if( jid.empty() )
      return;

    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_ROSTER );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *y = iks_first_tag( x );
    iks *z = iks_insert( y, "item" );
    iks_insert_attrib( z, "jid", jid.c_str() );
    if( !name.empty() )
      iks_insert_attrib( z, "name", name.c_str() );

    if( groups.size() != 0 )
    {
      iks *g;
      RosterItem::GroupList::const_iterator it = groups.begin();
      for( it; it != groups.end(); it++ )
      {
        g = iks_insert( z, "group" );
        iks_insert_cdata( g, (*it).c_str(), (*it).length() );
      }
    }
    m_parent->send( x );
  }

  void RosterManager::unsubscribe( const string& jid, const string& msg, bool remove )
  {
    iks *x = iks_make_s10n( IKS_TYPE_UNSUBSCRIBE, jid.c_str(), msg.c_str() );
    m_parent->send( x );

    if( remove )
    {
      string id = m_parent->getID();

      iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_ROSTER );
      iks_insert_attrib( x, "id", id.c_str() );
      iks *y = iks_first_tag( x );
      iks *z = iks_insert( y, "item" );
      iks_insert_attrib( z, "jid", jid.c_str() );
      iks_insert_attrib( z, "subscription", "remove" );

      m_parent->send( x );
    }
  }

  void RosterManager::synchronize()
  {
    RosterListener::Roster::const_iterator it = m_roster.begin();
    for( it; it != m_roster.end(); it++ )
    {
      if( (*it).second->changed() )
      {
        string id = m_parent->getID();

        iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_ROSTER );
        iks_insert_attrib( x, "id", id.c_str() );
        iks *y = iks_first_tag( x );
        iks *z = iks_insert( y, "item" );
        iks_insert_attrib( z, "jid", (*it).second->jid().c_str() );
        if( !(*it).second->name().empty() )
          iks_insert_attrib( z, "name", (*it).second->name().c_str() );

        if( (*it).second->groups().size() != 0 )
        {
          iks *g;
          RosterItem::GroupList::const_iterator g_it = (*it).second->groups().begin();
          for( g_it; g_it != (*it).second->groups().end(); g_it++ )
          {
            g = iks_insert( z, "group" );
            iks_insert_cdata( g, (*g_it).c_str(), (*g_it).length() );
          }
        }
        m_parent->send( x );
      }
    }
  }

  void RosterManager::handleSubscription( iksid *from, iksubtype type, const char *msg )
  {
    if( !m_rosterListener )
      return;

    string message;
    if( msg )
      message = msg;

    switch( type )
    {
      case IKS_TYPE_SUBSCRIBE:
        if( m_rosterListener->subscriptionRequest( from->full, message ) )
        {
          iks *x = iks_make_s10n( IKS_TYPE_SUBSCRIBED, from->partial, "ok" );
          m_parent->send( x );
        }
        else
        {
          iks *x = iks_make_s10n( IKS_TYPE_UNSUBSCRIBED, from->partial, "ok" );
          m_parent->send( x );
        }
        break;

      case IKS_TYPE_SUBSCRIBED:
      {
        iks *x = iks_make_s10n( IKS_TYPE_SUBSCRIBE, from->partial, "ok" );
        m_parent->send( x );

        m_rosterListener->itemSubscribed( from->partial );
        break;
      }

      case IKS_TYPE_UNSUBSCRIBE:
      {
        iks *x = iks_make_s10n( IKS_TYPE_UNSUBSCRIBED, from->partial, "ok" );
        m_parent->send( x );

        if( m_rosterListener->unsubscriptionRequest( from->partial, message ) )
          unsubscribe( from->partial, "", true );
        break;
      }

      case IKS_TYPE_UNSUBSCRIBED:
      {
        iks *x = iks_make_s10n( IKS_TYPE_UNSUBSCRIBE, from->partial, "ok" );
        m_parent->send( x );

        m_rosterListener->itemUnsubscribed( from->partial );
        break;
      }
    }
  }

  void RosterManager::registerRosterListener( RosterListener *rl )
  {
    m_rosterListener = rl;
  }

  void RosterManager::removeRosterListener()
  {
    m_rosterListener = 0;
  }

  void RosterManager::add( const string& jid )
  {
    RosterItem *item = new RosterItem( jid );
    item->setStatus( IKS_SHOW_UNAVAILABLE );
    m_roster[jid] = item;
  }

};

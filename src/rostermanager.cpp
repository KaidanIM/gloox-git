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
    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "type", "get" );
    iq->addAttrib( "id", m_parent->getID() );
    Tag *q = new Tag( "query" );
    q->addAttrib( "xmlns", XMLNS_ROSTER );
    iq->addChild( q );
    m_parent->send( iq );
  }

  bool RosterManager::handleIq( Stanza *stanza )
  {
    if( stanza->subtype() == STANZA_IQ_RESULT ) // initial roster
    {
      extractItems( stanza );

      if( m_rosterListener )
        m_rosterListener->roster( m_roster );

      return true;
    }
    else if( stanza->subtype() == STANZA_IQ_SET ) // roster item push
    {
      const std::string jid = extractItems( stanza );

      if( m_rosterListener )
        m_rosterListener->itemAdded( jid );

      Tag *iq = new Tag( "iq" );
      iq->addAttrib( "id", stanza->id() );
      iq->addAttrib( "type", "result" );
      m_parent->send( iq );

      return true;
    }
    return false;
  }

  void RosterManager::handlePresence( Stanza *stanza )
  {
    RosterListener::Roster::iterator it = m_roster.find( stanza->from().bare() );
    if( it != m_roster.end() )
    {
      (*it).second->setStatus( stanza->show() );
      (*it).second->setStatusMsg( stanza->status() );

      if( m_rosterListener )
      {
        if( stanza->show() == PRESENCE_AVAILABLE )
          m_rosterListener->itemAvailable( (*(*it).second), stanza->status() );
        else if( stanza->show() == PRESENCE_UNAVAILABLE )
          m_rosterListener->itemUnavailable( (*(*it).second), stanza->status() );
        else
          m_rosterListener->itemChanged( (*(*it).second), stanza->show(), stanza->status() );
      }
    }
  }

  void RosterManager::subscribe( const std::string& jid, const std::string& name,
                                 RosterItem::GroupList& groups/*, const std::string& msg*/ )
  {
    if( jid.empty() )
      return;

    add( jid, name, groups );

    Tag *s = new Tag( "presence" );
    s->addAttrib( "type", "subscribe" );
    s->addAttrib( "to", jid );
    m_parent->send( s );
  }


  void RosterManager::add( const std::string& jid, const std::string& name, RosterItem::GroupList& groups )
  {
    if( jid.empty() )
      return;

    std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "type", "set" );
    iq->addAttrib( "id", id );
    Tag *q = new Tag( "query" );
    q->addAttrib( "xmlns", XMLNS_ROSTER );
    Tag *i = new Tag( "item" );
    i->addAttrib( "jid", jid );
    if( !name.empty() )
      i->addAttrib( "name", name );

    if( groups.size() != 0 )
    {
      RosterItem::GroupList::const_iterator it = groups.begin();
      for( it; it != groups.end(); it++ )
      {
        Tag *g = new Tag( "group", (*it) );
        i->addChild( g );
      }
    }
    q->addChild( i );
    iq->addChild( q );
    m_parent->send( iq );
  }

  void RosterManager::unsubscribe( const std::string& jid/*, const std::string& msg*/, bool remove )
  {
    Tag *s = new Tag( "presence" );
    s->addAttrib( "type", "unsubscribe" );
    s->addAttrib( "to", jid );
    m_parent->send( s );

    if( remove )
    {
      std::string id = m_parent->getID();

      Tag *iq = new Tag( "iq" );
      iq->addAttrib( "type", "set" );
      iq->addAttrib( "id", id );
      Tag *q = new Tag( "query" );
      q->addAttrib( "xmlns", XMLNS_ROSTER );
      Tag *i = new Tag( "item" );
      i->addAttrib( "jid", jid );
      i->addAttrib( "subscription", "remove" );
      q->addChild( i );
      iq->addChild( q );

      m_parent->send( iq );
    }
  }

  void RosterManager::synchronize()
  {
    RosterListener::Roster::const_iterator it = m_roster.begin();
    for( it; it != m_roster.end(); it++ )
    {
      if( (*it).second->changed() )
      {
        std::string id = m_parent->getID();

        Tag *iq = new Tag( "iq" );
        iq->addAttrib( "type", "set" );
        iq->addAttrib( "id", id );
        Tag *q = new Tag( "query" );
        q->addAttrib( "xmlns", XMLNS_ROSTER );
        Tag *i = new Tag( "item" );
        i->addAttrib( "jid", (*it).second->jid() );
        if( !(*it).second->name().empty() )
          i->addAttrib( "name", (*it).second->name() );

        if( (*it).second->groups().size() != 0 )
        {
          RosterItem::GroupList::const_iterator g_it = (*it).second->groups().begin();
          for( g_it; g_it != (*it).second->groups().end(); g_it++ )
          {
            i->addChild( new Tag( "group", (*g_it) ) );
          }
        }
        q->addChild( i );
        iq->addChild( q );

        m_parent->send( iq );
      }
    }
  }

  void RosterManager::handleSubscription( Stanza *stanza )
  {
    if( !m_rosterListener )
      return;

    switch( stanza->subtype() )
    {
      case STANZA_S10N_SUBSCRIBE:
        if( m_rosterListener->subscriptionRequest( stanza->from().bare() ) )
        {
          Tag *p = new Tag( "presence" );
          p->addAttrib( "type", "subscribed" );
          p->addAttrib( "to", stanza->from().bare() );
          m_parent->send( p );
        }
        else
        {
          Tag *p = new Tag( "presence" );
          p->addAttrib( "type", "unsubscribed" );
          p->addAttrib( "to", stanza->from().bare() );
          m_parent->send( p );
        }
        break;

      case STANZA_S10N_SUBSCRIBED:
      {
        Tag *p = new Tag( "presence" );
        p->addAttrib( "type", "subscribe" );
        p->addAttrib( "to", stanza->from().bare() );
        m_parent->send( p );

        m_rosterListener->itemSubscribed( stanza->from().bare() );
        break;
      }

      case STANZA_S10N_UNSUBSCRIBE:
      {
        Tag *p = new Tag( "presence" );
        p->addAttrib( "type", "unsubscribed" );
        p->addAttrib( "to", stanza->from().bare() );
        m_parent->send( p );

        if( m_rosterListener->unsubscriptionRequest( stanza->from().bare() ) )
          unsubscribe( stanza->from().bare(), true );
        break;
      }

      case STANZA_S10N_UNSUBSCRIBED:
      {
        Tag *p = new Tag( "presence" );
        p->addAttrib( "type", "unsubscribe" );
        p->addAttrib( "to", stanza->from().bare() );
        m_parent->send( p );

        m_rosterListener->itemUnsubscribed( stanza->from().bare() );
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

  const std::string RosterManager::extractItems( Tag *tag )
  {
    std::string jid;
    Tag *t = tag->findChild( "query" );
    Tag::TagList l = t->children();
    Tag::TagList::iterator it = l.begin();
    for( it; it != l.end(); it++ )
    {
      if( (*it)->name() == "item" )
      {
        jid = (*it)->findAttribute( "jid" );
        const std::string name = (*it)->findAttribute( "name" );
        const std::string ask = (*it)->findAttribute( "ask" );
        bool a = false;
        if( !ask.empty() )
          a = true;
        const std::string sub = (*it)->findAttribute( "subscription" );
        RosterItem::SubscriptionEnum subs;
        if( sub == "remove" )
        {
          if( m_roster.find( jid ) != m_roster.end() )
          {
            m_roster.erase( jid );
            if( m_rosterListener )
              m_rosterListener->itemRemoved( jid );
          }
          continue;
        }

        RosterItem::GroupList gl;
        if( (*it)->hasChild( "group" ) )
        {
          Tag::TagList g = (*it)->children();
          Tag::TagList::const_iterator it_g = g.begin();
          for( it_g; it_g != g.end(); it_g++ )
          {
            gl.push_back( (*it_g)->name() );
          }
        }
        add( jid, name, gl, sub, a );
      }
    }
    return jid;
  }

  void RosterManager::add( const std::string& jid, const std::string& name,
                           RosterItem::GroupList& groups, const std::string& sub, bool ask )
  {
    if( m_roster.find( jid ) == m_roster.end() )
      m_roster[jid] = new RosterItem( jid );

    m_roster[jid]->setStatus( IKS_SHOW_UNAVAILABLE );
    m_roster[jid]->setSubscription( sub, ask );
    m_roster[jid]->setGroups( groups );
  }

};

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "clientbase.h"
#include "rostermanager.h"
#include "disco.h"
#include "rosteritem.h"
#include "rosterlistener.h"
#include "privatexml.h"


namespace gloox
{

  RosterManager::RosterManager( ClientBase *parent, bool self )
  : m_rosterListener( 0 ), m_parent( parent ), m_privateXML( 0 ), m_delimiterFetched( false ),
    m_syncSubscribeReq( false )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_ROSTER );
      m_parent->registerPresenceHandler( this );
      m_parent->registerSubscriptionHandler( this );

      if( self )
      {
        RosterItem *i = new RosterItem( m_parent->jid().bare() );
        m_roster[m_parent->jid().bare()] = i;
      }

      m_privateXML = new PrivateXML( m_parent );
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

    RosterListener::Roster::iterator it = m_roster.begin();
    for( ; it != m_roster.end(); ++it )
      delete( (*it).second );
    m_roster.clear();

    if( m_privateXML )
      delete m_privateXML;
  }

  RosterListener::Roster* RosterManager::roster()
  {
    return &m_roster;
  }

  void RosterManager::fill()
  {
    if( !m_delimiterFetched )
    {
      m_privateXML->requestXML( "roster", XMLNS_ROSTER_DELIMITER, this );
      return;
    }

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
      extractItems( stanza, false );

      if( m_rosterListener )
        m_rosterListener->roster( m_roster );

      return true;
    }
    else if( stanza->subtype() == STANZA_IQ_SET ) // roster item push
    {
      extractItems( stanza, true );

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
      PresenceStatus oldStat = (*it).second->status();

      (*it).second->setStatus( stanza->show() );
      (*it).second->setStatusMsg( stanza->status() );

      if( m_rosterListener )
      {
        if( stanza->show() == PRESENCE_AVAILABLE )
        {
          if( oldStat == PRESENCE_UNAVAILABLE )
            m_rosterListener->itemAvailable( (*(*it).second), stanza->status() );
          else
            m_rosterListener->itemChanged( (*(*it).second), stanza->show(), stanza->status() );
        }
        else if( stanza->show() == PRESENCE_UNAVAILABLE )
          m_rosterListener->itemUnavailable( (*(*it).second), stanza->status() );
        else
          m_rosterListener->itemChanged( (*(*it).second), stanza->show(), stanza->status() );
      }
    }
    else
    {
      StringList sl;
      add( stanza->from().bare(), std::string(), sl, "none", false );
      m_roster[stanza->from().bare()]->setStatus( stanza->show() );
      m_roster[stanza->from().bare()]->setStatusMsg( stanza->status() );
    }
  }

  void RosterManager::subscribe( const std::string& jid, const std::string& name,
                                 StringList& groups, const std::string& msg )
  {
    if( jid.empty() )
      return;

    add( jid, name, groups );

    Tag *s = new Tag( "presence" );
    s->addAttrib( "type", "subscribe" );
    s->addAttrib( "to", jid );
    s->addAttrib( "from", m_parent->jid().full() );
    if( !msg.empty() )
      new Tag( s, "status", msg );

    m_parent->send( s );
  }


  void RosterManager::add( const std::string& jid, const std::string& name, StringList& groups )
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
      StringList::const_iterator it = groups.begin();
      for( ; it != groups.end(); ++it )
      {
        Tag *g = new Tag( "group", (*it) );
        i->addChild( g );
      }
    }
    q->addChild( i );
    iq->addChild( q );
    m_parent->send( iq );
  }

  void RosterManager::unsubscribe( const std::string& jid, const std::string& msg, bool remove )
  {
    Tag *s = new Tag( "presence" );
    s->addAttrib( "type", "unsubscribe" );
    s->addAttrib( "from", m_parent->jid().bare() );
    s->addAttrib( "to", jid );
    if( !msg.empty() )
      new Tag( s, "status", msg );

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
    for( ; it != m_roster.end(); ++it )
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
          StringList::const_iterator g_it = (*it).second->groups().begin();
          for( ; g_it != (*it).second->groups().end(); ++g_it )
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

  void RosterManager::ackSubscriptionRequest( const JID& to, bool ack )
  {
    Tag *p = new Tag( "presence" );
    if( ack )
      p->addAttrib( "type", "subscribed" );
    else
      p->addAttrib( "type", "unsubscribed" );

    p->addAttribute( "from", m_parent->jid().bare() );
    p->addAttribute( "to", to.bare() );
    m_parent->send( p );
}

  void RosterManager::handleSubscription( Stanza *stanza )
  {
    if( !m_rosterListener )
      return;

    switch( stanza->subtype() )
    {
      case STANZA_S10N_SUBSCRIBE:
      {
        bool answer = m_rosterListener->subscriptionRequest( stanza->from().bare(), stanza->status() );
        if( m_syncSubscribeReq )
        {
          ackSubscriptionRequest( stanza->from(), answer );
        }
        break;
      }
      case STANZA_S10N_SUBSCRIBED:
      {
//         Tag *p = new Tag( "presence" );
//         p->addAttrib( "type", "subscribe" );
//         p->addAttrib( "from", m_parent->jid().bare() );
//         p->addAttrib( "to", stanza->from().bare() );
//         m_parent->send( p );

        m_rosterListener->itemSubscribed( stanza->from().bare() );
        break;
      }

      case STANZA_S10N_UNSUBSCRIBE:
      {
        Tag *p = new Tag( "presence" );
        p->addAttrib( "type", "unsubscribed" );
        p->addAttrib( "from", m_parent->jid().bare() );
        p->addAttrib( "to", stanza->from().bare() );
        m_parent->send( p );

        bool answer = m_rosterListener->unsubscriptionRequest( stanza->from().bare(), stanza->status() );
        if( m_syncSubscribeReq && answer )
          unsubscribe( stanza->from().bare(), "", true );
        break;
      }

      case STANZA_S10N_UNSUBSCRIBED:
      {
//         Tag *p = new Tag( "presence" );
//         p->addAttrib( "type", "unsubscribe" );
//         p->addAttrib( "from", m_parent->jid().bare() );
//         p->addAttrib( "to", stanza->from().bare() );
//         m_parent->send( p );

        m_rosterListener->itemUnsubscribed( stanza->from().bare() );
        break;
      }

      default:
        break;
    }
  }

  void RosterManager::registerRosterListener( RosterListener *rl, bool syncSubscribeReq )
  {
    m_syncSubscribeReq = syncSubscribeReq;
    m_rosterListener = rl;
  }

  void RosterManager::removeRosterListener()
  {
    m_syncSubscribeReq = false;
    m_rosterListener = 0;
  }

  void RosterManager::extractItems( Tag *tag, bool isPush )
  {
    Tag *t = tag->findChild( "query" );
    Tag::TagList l = t->children();
    Tag::TagList::iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      if( (*it)->name() == "item" )
      {
        StringList gl;
        if( (*it)->hasChild( "group" ) )
        {
          Tag::TagList g = (*it)->children();
          Tag::TagList::const_iterator it_g = g.begin();
          for( ; it_g != g.end(); ++it_g )
          {
            gl.push_back( (*it_g)->cdata() );
          }
        }

        const std::string jid = (*it)->findAttribute( "jid" );
        RosterListener::Roster::iterator it_d = m_roster.find( jid );
        if( it_d != m_roster.end() )
        {
          (*it_d).second->setName( (*it)->findAttribute( "name" ) );
          const std::string sub = (*it)->findAttribute( "subscription" );
          if( sub == "remove" )
          {
            delete( (*it_d).second );
            m_roster.erase( it_d );
            if( m_rosterListener )
              m_rosterListener->itemRemoved( jid );
            continue;
          }
          const std::string ask = (*it)->findAttribute( "ask" );
          bool a = false;
          if( !ask.empty() )
            a = true;
          (*it_d).second->setSubscription( sub, a );
          (*it_d).second->setGroups( gl );

          if( m_rosterListener )
            m_rosterListener->itemUpdated( jid );
        }
        else
        {
          const std::string sub = (*it)->findAttribute( "subscription" );
          if( sub == "remove" )
            continue;
          const std::string name = (*it)->findAttribute( "name" );
          const std::string ask = (*it)->findAttribute( "ask" );
          bool a = false;
          if( !ask.empty() )
            a = true;

          add( jid, name, gl, sub, a );
          if( isPush && m_rosterListener )
            m_rosterListener->itemAdded( jid );
        }
      }
    }
  }

  void RosterManager::add( const std::string& jid, const std::string& name,
                           StringList& groups, const std::string& sub, bool ask )
  {
    if( m_roster.find( jid ) == m_roster.end() )
      m_roster[jid] = new RosterItem( jid, name );

    m_roster[jid]->setStatus( PRESENCE_UNAVAILABLE );
    m_roster[jid]->setSubscription( sub, ask );
    m_roster[jid]->setGroups( groups );
  }

  void RosterManager::setDelimiter( const std::string& delimiter )
  {
    m_delimiter = delimiter;
    Tag *t = new Tag( "roster", m_delimiter );
    t->addAttrib( "xmlns", XMLNS_ROSTER_DELIMITER );
    m_privateXML->storeXML( t, this );
  }

  void RosterManager::handlePrivateXML( const std::string& /*tag*/, Tag *xml )
  {
    m_delimiterFetched = true;
    m_delimiter = xml->cdata();
    fill();
  }

  void RosterManager::handlePrivateXMLResult( const std::string /*uid*/, PrivateXMLResult result )
  {
    m_delimiterFetched = true;
    if( result == PrivateXMLHandler::PXML_REQUEST_ERROR )
      fill();
  }

}

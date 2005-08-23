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



#include "privacymanager.h"
#include "clientbase.h"

namespace gloox
{

  PrivacyManager::PrivacyManager( ClientBase *parent )
    : m_parent( parent ), m_privacyListHandler( 0 )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_PRIVACY );
  }

  PrivacyManager::~PrivacyManager()
  {
    if( m_parent )
      m_parent->removeIqHandler( XMLNS_PRIVACY );
  }

  std::string PrivacyManager::requestListNames()
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "get" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_REQUEST_NAMES );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::requestList( const std::string& name )
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "get" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag l( "list" );
    l.addAttrib( "name", name );
    q.addChild( l );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_REQUEST_LIST );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::removeList( const std::string& name )
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag l( "list" );
    l.addAttrib( "name", name );
    q.addChild( l );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_REMOVE );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::setDefault( const std::string& name )
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag d( "default" );
    d.addAttrib( "name", name );
    q.addChild( d );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_DEFAULT );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::unsetDefault()
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag d( "default" );
    q.addChild( d );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_UNSET_DEFAULT );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::setActive( const std::string& name )
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag a( "active" );
    a.addAttrib( "name", name );
    q.addChild( a );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_ACTIVATE );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::unsetActive()
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag a( "active" );
    q.addChild( a );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_UNSET_ACTIVATE );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::store( const std::string& name, PrivacyListHandler::PrivacyList& list )
  {
    std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "type", "set" );
    iq.addAttrib( "id", id );
    Tag q( "query" );
    q.addAttrib( "xmlns", XMLNS_PRIVACY );
    Tag l( "list" );
    l.addAttrib( "name", name );

    int count = 0;
    PrivacyListHandler::PrivacyList::iterator it = list.begin();
    for( it; it != list.end(); it++ )
    {
      Tag i( "item" );
      switch( (*it).type() )
      {
        case PrivacyItem::TYPE_JID:
          i.addAttrib( "type", "jid" );
          break;
        case PrivacyItem::TYPE_GROUP:
          i.addAttrib( "type", "group" );
          break;
        case PrivacyItem::TYPE_SUBSCRIPTION:
          i.addAttrib( "type", "subscription" );
          break;
      }

      switch( (*it).action() )
      {
        case PrivacyItem::ACTION_ALLOW:
          i.addAttrib( "action", "allow" );
          break;
        case PrivacyItem::ACTION_DENY:
          i.addAttrib( "action", "deny" );
          break;
      }

      int pType = (*it).packetType();
      if( pType != 15 )
      {
        if( pType & PrivacyItem::PACKET_MESSAGE )
          i.addChild( Tag( "message" ) );
        if( pType & PrivacyItem::PACKET_PRESENCE_IN )
          i.addChild( Tag( "presence-in" ) );
        if( pType & PrivacyItem::PACKET_PRESENCE_OUT )
          i.addChild( Tag( "presence-out" ) );
        if( pType & PrivacyItem::PACKET_IQ )
          i.addChild( Tag( "iq" ) );
      }

      i.addAttrib( "value", (*it).value() );

      char order[15];
      sprintf( order, "%d", ++count );
      i.addAttrib( "order", order );
    }

    q.addChild( l );
    iq.addChild( q );

    m_parent->trackID( this, id, PL_STORE );
    m_parent->send( iq );
    return id;
  }

  bool PrivacyManager::handleIq( const Stanza& stanza )
  {
    if( stanza.subtype() != STANZA_IQ_SET || !m_privacyListHandler )
      return false;

    Tag l = stanza.findChild( "query" ).findChild( "list" );
    if( l.hasAttribute( "name" ) )
    {
      std::string name = l.findAttribute( "name" );
      m_privacyListHandler->handlePrivacyListChanged( name );

      Tag iq( "iq" );
      iq.addAttrib( "type", "result" );
      iq.addAttrib( "id", stanza.id() );
      m_parent->send( iq );
      return true;
    }

    return false;
  }

  bool PrivacyManager::handleIqID( const Stanza& stanza, int context )
  {
    if( stanza.subtype() != STANZA_IQ_RESULT || !m_privacyListHandler )
      return false;

    switch( stanza.subtype() )
    {
      case STANZA_IQ_RESULT:
        switch( context )
        {
          case PL_STORE:
            m_privacyListHandler->handlePrivacyListResult( stanza.id(),
                PrivacyListHandler::RESULT_STORE_SUCCESS );
            break;
          case PL_ACTIVATE:
            m_privacyListHandler->handlePrivacyListResult( stanza.id(),
                PrivacyListHandler::RESULT_ACTIVATE_SUCCESS );
            break;
          case PL_DEFAULT:
            m_privacyListHandler->handlePrivacyListResult( stanza.id(),
                PrivacyListHandler::RESULT_DEFAULT_SUCCESS );
            break;
          case PL_REMOVE:
            m_privacyListHandler->handlePrivacyListResult( stanza.id(),
                PrivacyListHandler::RESULT_REMOVE_SUCCESS );
            break;
          case PL_REQUEST_NAMES:
          {
            StringList lists;
            std::string def;
            std::string active;
            Tag q = stanza.findChild( "query" );
            Tag::TagList l = q.children();
            Tag::TagList::const_iterator it = l.begin();
            for( it; it != l.end(); it++ )
            {
              if( (*it).name() == "default" )
                def = (*it).findAttribute( "name" );
              if( (*it).name() == "active" )
                def = (*it).findAttribute( "name" );
              if( (*it).name() == "list" )
              {
                const std::string name = (*it).findAttribute( "name" );
                lists.push_back( name );
              }
            }

            m_privacyListHandler->handlePrivacyListNames( def, active, lists );
            break;
          }
          case PL_REQUEST_LIST:
          {
            PrivacyListHandler::PrivacyList items;

            Tag list = stanza.findChild( "query" ).findChild( "list" );
            const std::string name = list.name();
            Tag::TagList l = list.children();
            Tag::TagList::iterator it = l.begin();
            for( it; it != l.end(); it++ )
            {
              PrivacyItem::ItemType type;
              PrivacyItem::ItemAction action;
              int packetType = 0;

              const std::string t = (*it).findAttribute( "type" );
              if( t == "jid" )
                type = PrivacyItem::TYPE_JID;
              else if( t == "group" )
                type = PrivacyItem::TYPE_GROUP;
              else if( t == "subscription" )
                type = PrivacyItem::TYPE_SUBSCRIPTION;
              else
                type = PrivacyItem::TYPE_UNDEFINED;

              const std::string a = (*it).findAttribute( "action" );
              if( a == "allow" )
                action = PrivacyItem::ACTION_ALLOW;
              else if( a == "deny" )
                action = PrivacyItem::ACTION_DENY;
              else
                action = PrivacyItem::ACTION_ALLOW;

              std::string value = (*it).findAttribute( "value" );

              Tag::TagList c = (*it).children();
              Tag::TagList::const_iterator it_c = c.begin();
              for( it_c; it_c != c.end(); it_c++ )
              {
                if( (*it_c).name() == "iq" )
                  packetType |= PrivacyItem::PACKET_IQ;
                else if( (*it_c).name() == "presence-out" )
                  packetType |= PrivacyItem::PACKET_PRESENCE_OUT;
                else if( (*it_c).name() == "presence-in" )
                  packetType |= PrivacyItem::PACKET_PRESENCE_IN;
                else if( (*it_c).name() == "message" )
                  packetType |= PrivacyItem::PACKET_MESSAGE;
              }

              PrivacyItem item( type, action, packetType, value );
              items.push_back( item );
            }
            m_privacyListHandler->handlePrivacyList( name, items );
            break;
          }
        }
        break;

      case STANZA_IQ_ERROR:
        Tag e = stanza.findChild( "error" );
        if( e.hasChild( "conflict" ) )
          m_privacyListHandler->handlePrivacyListResult( stanza.id(),
            PrivacyListHandler::RESULT_CONFLICT );
        else if( e.hasChild( "item-not-found" ) )
          m_privacyListHandler->handlePrivacyListResult( stanza.id(),
            PrivacyListHandler::RESULT_ITEM_NOT_FOUND );
        else if( e.hasChild( "bad-request" ) )
          m_privacyListHandler->handlePrivacyListResult( stanza.id(),
            PrivacyListHandler::RESULT_BAD_REQUEST );
        break;
    }
    return false;
  }

  void PrivacyManager::registerPrivacyListHandler( PrivacyListHandler *plh )
  {
    m_privacyListHandler = plh;
  }

  void PrivacyManager::removePrivacyListHandler()
  {
    m_privacyListHandler = 0;
  }

};

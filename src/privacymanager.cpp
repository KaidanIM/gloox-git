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

  string PrivacyManager::requestListNames()
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_GET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );

    m_parent->trackID( this, id, PL_REQUEST_NAMES );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::requestList( const string& name )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_GET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *list = iks_insert( query, "list" );
    iks_insert_attrib( list, "name", name.c_str() );

    m_parent->trackID( this, id, PL_REQUEST_LIST );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::removeList( const string& name )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *list = iks_insert( query, "list" );
    iks_insert_attrib( list, "name", name.c_str() );

    m_parent->trackID( this, id, PL_REMOVE );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::setDefault( const string& name )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *def = iks_insert( query, "default" );
    iks_insert_attrib( def, "name", name.c_str() );

    m_parent->trackID( this, id, PL_DEFAULT );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::unsetDefault()
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *def = iks_insert( query, "default" );

    m_parent->trackID( this, id, PL_UNSET_DEFAULT );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::setActive( const string& name )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *active = iks_insert( query, "active" );
    iks_insert_attrib( active, "name", name.c_str() );

    m_parent->trackID( this, id, PL_ACTIVATE );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::unsetActive()
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *def = iks_insert( query, "active" );

    m_parent->trackID( this, id, PL_UNSET_ACTIVATE );
    m_parent->send( x );
    return id;
  }

  string PrivacyManager::store( const string& name, PrivacyListHandler::PrivacyList& list )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVACY );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *query = iks_first_tag( x );
    iks *l = iks_insert( query, "list" );
    iks_insert_attrib( l, "name", name.c_str() );

    int count = 0;
    PrivacyListHandler::PrivacyList::iterator it = list.begin();
    for( it; it != list.end(); it++ )
    {
      iks *item = iks_insert( l, "item" );
      switch( (*it).type() )
      {
        case PrivacyItem::TYPE_JID:
          iks_insert_attrib( item, "type", "jid" );
          break;
        case PrivacyItem::TYPE_GROUP:
          iks_insert_attrib( item, "type", "group" );
          break;
        case PrivacyItem::TYPE_SUBSCRIPTION:
          iks_insert_attrib( item, "type", "subscription" );
          break;
      }

      switch( (*it).action() )
      {
        case PrivacyItem::ACTION_ALLOW:
          iks_insert_attrib( item, "action", "allow" );
          break;
        case PrivacyItem::ACTION_DENY:
          iks_insert_attrib( item, "action", "deny" );
          break;
      }

      int pType = (*it).packetType();
      if( !pType == 15 )
      {
        if( pType & PrivacyItem::PACKET_MESSAGE )
          iks_insert( item, "message" );
        if( pType & PrivacyItem::PACKET_PRESENCE_IN )
          iks_insert( item, "presence-in" );
        if( pType & PrivacyItem::PACKET_PRESENCE_OUT )
          iks_insert( item, "presence-out" );
        if( pType & PrivacyItem::PACKET_IQ )
          iks_insert( item, "iq" );
      }

      iks_insert_attrib( item, "value", (*it).value().c_str() );

      char order[15];
      sprintf( order, "%d", ++count );
      iks_insert_attrib( item, "order", order );
    }

    m_parent->trackID( this, id, PL_STORE );
    m_parent->send( x );
    return id;
  }

  void PrivacyManager::handleIq( const char *tag, const char *xmlns, ikspak *pak )
  {
    if( pak->subtype != IKS_TYPE_SET || !m_privacyListHandler )
      return;

    string name;

    iks *x = iks_first_tag( pak->query );
    if( iks_strncmp( iks_name( x ), "list", 4 ) == 0 )
    {
      char *n = iks_find_attrib( x, "name" );
      if( n )
      {
        name = n;
        m_privacyListHandler->handlePrivacyListChanged( name );
      }
    }
  }

  void PrivacyManager::handleIqID( const char *id, ikspak *pak, int context )
  {
    if( !m_privacyListHandler )
      return;

    switch( pak->subtype )
    {
      case IKS_TYPE_RESULT:
        switch( context )
        {
          case PL_STORE:
            m_privacyListHandler->handlePrivacyListResult( id,
                PrivacyListHandler::RESULT_STORE_SUCCESS );
            break;
          case PL_ACTIVATE:
            m_privacyListHandler->handlePrivacyListResult( id,
                PrivacyListHandler::RESULT_ACTIVATE_SUCCESS );
            break;
          case PL_DEFAULT:
            m_privacyListHandler->handlePrivacyListResult( id,
                PrivacyListHandler::RESULT_DEFAULT_SUCCESS );
            break;
          case PL_REMOVE:
            m_privacyListHandler->handlePrivacyListResult( id,
                PrivacyListHandler::RESULT_REMOVE_SUCCESS );
            break;
          case PL_REQUEST_NAMES:
          {
            PrivacyListHandler::StringList lists;
            string def;
            string active;
            iks *x = iks_first_tag( pak->query );
            while( x )
            {
              if( iks_strncmp( iks_name( x ), "default", 7 ) == 0 )
              {
                char *d = iks_find_attrib( x, "name" );
                if( d )
                  def = d;
              }
              if( iks_strncmp( iks_name( x ), "active", 6 ) == 0 )
              {
                char *a = iks_find_attrib( x, "name" );
                if( a )
                  active = a;
              }
              if( iks_strncmp( iks_name( x ), "list", 4 ) == 0 )
              {
                char *name = iks_find_attrib( x, "name" );
                if( name )
                  lists.push_back( name );
              }
              x = iks_next( x );
            }
            m_privacyListHandler->handlePrivacyListNames( def, active, lists );
            break;
          }
          case PL_REQUEST_LIST:
          {
            PrivacyListHandler::PrivacyList items;
            string name;

            iks *x = iks_first_tag( pak->query );
            char *n = iks_find_attrib( x, "name" );
            if( n )
              name = n;

            x = iks_first_tag( x );
            while( x )
            {
              PrivacyItem::ItemType type;
              PrivacyItem::ItemAction action;
              int packetType;
              string value;

              char *t = iks_find_attrib( x, "type" );
              if( iks_strncmp( t, "jid", 3 ) == 0 )
                type = PrivacyItem::TYPE_JID;
              else if( iks_strncmp( t, "group", 5 ) == 0 )
                type = PrivacyItem::TYPE_GROUP;
              else if( iks_strncmp( t, "subscription", 12 ) == 0 )
                type = PrivacyItem::TYPE_SUBSCRIPTION;
              else
                type = PrivacyItem::TYPE_UNDEFINED;

              char *a = iks_find_attrib( x, "action" );
              if( iks_strncmp( a, "allow", 5 ) == 0 )
                action = PrivacyItem::ACTION_ALLOW;
              else if( iks_strncmp( a, "deny", 4 ) == 0 )
                action = PrivacyItem::ACTION_DENY;
              else
                action = PrivacyItem::ACTION_ALLOW;

              char *v = iks_find_attrib( x, "value" );
              if( v )
                value = v;

              iks *y = iks_first_tag( x );
              while( y )
              {
                if( iks_strncmp( iks_name( y ), "iq", 2 ) == 0 )
                  packetType &= PrivacyItem::PACKET_IQ;
                else if( iks_strncmp( iks_name( y ), "presence-out", 12 ) == 0 )
                  packetType &= PrivacyItem::PACKET_PRESENCE_OUT;
                else if( iks_strncmp( iks_name( y ), "presence-in", 11 ) == 0 )
                  packetType &= PrivacyItem::PACKET_PRESENCE_IN;
                else if( iks_strncmp( iks_name( y ), "message", 7 ) == 0 )
                  packetType &= PrivacyItem::PACKET_MESSAGE;

                y = iks_next( y );
              }

              PrivacyItem item( type, action, packetType, value );
              items.push_back( item );
              x = iks_next( x );
            }
            m_privacyListHandler->handlePrivacyList( name, items );
            break;
          }
        }
        break;

      case IKS_TYPE_ERROR:
        iks *error = iks_find( pak->x, "error" );
        iks *name = iks_first_tag( error );
        if( iks_strncmp( iks_name( name ), "conflict", 8 ) == 0 )
          m_privacyListHandler->handlePrivacyListResult( id, PrivacyListHandler::RESULT_CONFLICT );
        if( iks_strncmp( iks_name( name ), "item-not-found", 14 ) == 0 )
          m_privacyListHandler->handlePrivacyListResult( id, PrivacyListHandler::RESULT_ITEM_NOT_FOUND );
        if( iks_strncmp( iks_name( name ), "bad-request", 11 ) == 0 )
          m_privacyListHandler->handlePrivacyListResult( id, PrivacyListHandler::RESULT_BAD_REQUEST );
        break;
    }
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

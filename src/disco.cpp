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


#include "disco.h"
#include "discohandler.h"
#include "jclient.h"
#include "disconodehandler.h"


Disco::Disco( JClient* parent )
  : m_parent( parent )
{
  addFeature( XMLNS_VERSION );
  addFeature( XMLNS_DISCO_INFO );
  addFeature( XMLNS_DISCO_ITEMS );
  m_parent->registerIqHandler( this, XMLNS_DISCO_INFO );
  m_parent->registerIqHandler( this, XMLNS_DISCO_ITEMS );
  m_parent->registerIqHandler( this, XMLNS_VERSION );
}

Disco::~Disco()
{

}

void Disco::handleIq( const char* xmlns, ikspak* pak )
{
  switch( pak->subtype )
  {
    case IKS_TYPE_GET:
      if( iks_strncmp( XMLNS_VERSION, xmlns, iks_strlen( XMLNS_VERSION ) ) == 0 )
      {
        iks* x = iks_make_iq( IKS_TYPE_RESULT, XMLNS_VERSION );
        iks_insert_attrib( x, "id", pak->id );
        iks_insert_attrib( x, "to", pak->from->full );
        iks_insert_attrib( x, "from", m_parent->jid().c_str() );
        iks* y = iks_find( x, "query" );
        iks* z = iks_insert( y, "name" );
        iks_insert_cdata( z, m_versionName.c_str(), m_versionName.length() );
        z = iks_insert( y, "version" );
        iks_insert_cdata( z, m_versionVersion.c_str(), m_versionVersion.length() );
        m_parent->send( x );
      }
      else if( iks_strncmp( XMLNS_DISCO_INFO, xmlns, iks_strlen( XMLNS_DISCO_INFO ) ) == 0 )
      {
        iks* x = iks_make_iq( IKS_TYPE_RESULT, XMLNS_DISCO_INFO );
        iks_insert_attrib( x, "id", pak->id );
        iks_insert_attrib( x, "to", pak->from->full );
        iks_insert_attrib( x, "from", m_parent->jid().c_str() );
        iks* y = iks_find( x, "query" );

        char* node = iks_find_attrib( pak->query, "node" );
        if( node )
        {
          DiscoNodeHandlerMap::const_iterator it = m_nodeHandlers.find( node );
          if( it != m_nodeHandlers.end() )
          {
            DiscoNodeHandler::IdentityMap identities = (*it).second->handleDiscoNodeIdentities( node );
            DiscoNodeHandler::IdentityMap::const_iterator im = identities.begin();
            for( im; im != identities.end(); im++ )
            {
              iks* i = iks_insert( y, "identity" );
              iks_insert_attrib( i, "category", (*im).first.c_str() );
              iks_insert_attrib( i, "type", (*im).second.c_str() );
  //             iks_insert_attrib( i, "name", m_versionName.c_str() );
            }
            DiscoNodeHandler::FeatureList features = (*it).second->handleDiscoNodeFeatures( node );
            DiscoNodeHandler::FeatureList::const_iterator fi = features.begin();
            for( fi; fi != features.end(); fi++ )
            {
              iks* i = iks_insert( y, "feature" );
              iks_insert_attrib( i, "var", (*fi).c_str() );
            }
          }
        }
        else
        {
          iks* i = iks_insert( y, "identity" );
          iks_insert_attrib( i, "category", m_identityCategory.c_str() );
          iks_insert_attrib( i, "type", m_identityType.c_str() );
          iks_insert_attrib( i, "name", m_versionName.c_str() );

          StringList::const_iterator it = m_features.begin();
          for( it; it != m_features.end(); ++it )
          {
            iks* z = iks_insert( y, "feature" );
            iks_insert_attrib( z, "var", (*it).c_str() );
          }
        }
        m_parent->send( x );
      }
      else if( iks_strncmp( XMLNS_DISCO_ITEMS, xmlns, iks_strlen( XMLNS_DISCO_ITEMS ) ) == 0 )
      {
        iks* x = iks_make_iq( IKS_TYPE_RESULT, XMLNS_DISCO_ITEMS );
        iks_insert_attrib( x, "id", pak->id );
        iks_insert_attrib( x, "to", pak->from->full );
        iks_insert_attrib( x, "from", m_parent->jid().c_str() );
        iks* y = iks_find( x, "query" );

        DiscoNodeHandler::ItemMap items;
        DiscoNodeHandlerMap::const_iterator it;
        char* node = iks_find_attrib( pak->query, "node" );
        if( node )
        {
          it = m_nodeHandlers.find( node );
          if( it != m_nodeHandlers.end() )
          {
            items = (*it).second->handleDiscoNodeItems( node );
          }
        }
        else
        {
          it = m_nodeHandlers.begin();
          for( it; it != m_nodeHandlers.end(); it++ )
          {
            items = (*it).second->handleDiscoNodeItems();
          }
        }

        if( items.size() )
        {
          DiscoNodeHandler::ItemMap::const_iterator it = items.begin();
          for( it; it != items.end(); it++ )
          {
            if( !(*it).first.empty() && !(*it).second.empty() )
            {
              iks* z = iks_insert( y, "item" );
              iks_insert_attrib( z, "jid", m_parent->jid().c_str() );
              iks_insert_attrib( z, "node", (*it).first.c_str() );
              iks_insert_attrib( z, "name", (*it).second.c_str() );
            }
          }
        }

        m_parent->send( x );
      }
      break;

    case IKS_TYPE_SET:
    {
      DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
      for( it; it != m_discoHandlers.end(); it++ )
      {
        (*it)->handleDiscoSet( pak->id, pak );
      }
      break;
    }

    case IKS_TYPE_RESULT:
      if( ( iks_strncmp( XMLNS_DISCO_INFO, xmlns, iks_strlen( XMLNS_DISCO_INFO ) ) == 0 )
            &&  findID( pak->id, pak->from->full ) )
      {
        DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
        for( it; it != m_discoHandlers.end(); it++ )
        {
          (*it)->handleDiscoInfoResult( pak->id, pak );
        }
      }
      else if( ( iks_strncmp( XMLNS_DISCO_ITEMS, xmlns, iks_strlen( XMLNS_DISCO_ITEMS ) ) == 0 )
                 &&  findID( pak->id, pak->from->full ) )
      {
        DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
        for( it; it != m_discoHandlers.end(); it++ )
        {
          (*it)->handleDiscoItemsResult( pak->id, pak );
        }
      }
      break;

    case IKS_TYPE_ERROR:
      iks* x = iks_child( iks_child( pak->x ) );
      DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
      for( it; it != m_discoHandlers.end(); it++ )
      {
        (*it)->handleDiscoError( pak->id, iks_name( x ) );
      }
      iks_delete( x );
      break;
  }
}

void Disco::addFeature( const string& feature )
{
  m_features.push_back( feature );
}

void Disco::getDiscoInfo( const string& to )
{
  std::string id = m_parent->getID();
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_DISCO_INFO );
  iks_insert_attrib( x, "from", m_parent->jid().c_str() );
  iks_insert_attrib( x, "to", to.c_str() );
  iks_insert_attrib( x, "id", id.c_str() );
  m_parent->send( x );
  addQueryID( id, to );
}

void Disco::getDiscoItems( const string& to )
{
  std::string id = m_parent->getID();
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_DISCO_ITEMS );
  iks_insert_attrib( x, "from", m_parent->jid().c_str() );
  iks_insert_attrib( x, "to", to.c_str() );
  iks_insert_attrib( x, "id", id.c_str() );
  m_parent->send( x );
  addQueryID( id, to );
}

void Disco::setVersion( const string& name, const string& version )
{
  m_versionName = name;
  m_versionVersion = version;
}

void Disco::setIdentity( const string& category, const string& type )
{
  m_identityCategory = category;
  m_identityType = type;
}

bool Disco::hasFeature( const string& jid, const string& feature )
{
#warning FIXME implement properly
  return false;
}

void Disco::registerDiscoHandler( DiscoHandler* dh )
{
  m_discoHandlers.push_back( dh );
}

void Disco::registerNodeHandler( DiscoNodeHandler* nh, const string& node )
{
  m_nodeHandlers[node] = nh;
}

void Disco::addQueryID( const string& id, const string& to )
{
  m_queryIDs[id] = to;
}

bool Disco::findID( const string& id, const string& from )
{
  StringMap::const_iterator it = m_queryIDs.find( id );
  if( ( it != m_queryIDs.end() ) && ( (*it).second == from ) )
  {
    m_queryIDs.erase( id );
    return true;
  }
  return false;
}


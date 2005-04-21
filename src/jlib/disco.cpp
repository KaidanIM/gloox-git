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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#include "disco.h"
#include "discohandler.h"
#include "jclient.h"


Disco::Disco( JClient* parent )
  : m_parent( parent ), m_discoHandler( 0 )
{
  addFeature( XMLNS_VERSION );
  addFeature( XMLNS_DISCO_INFO );
  addFeature( XMLNS_DISCO_ITEMS );
  m_parent->registerIqHandler( this, XMLNS_DISCO_INFO );
  m_parent->registerIqHandler( this, XMLNS_DISCO_ITEMS );
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
        iks* x = iks_new( "iq" );
        iks_insert_attrib( x, "type", "result" );
        iks_insert_attrib( x, "to", pak->from->full );
        iks_insert_attrib( x, "from", m_parent->jid().c_str() );
        iks_insert_attrib( x, "id", pak->id );
        iks* y = iks_insert( x, "query" );
        iks_insert_attrib( y, "xmlns", XMLNS_VERSION );
        iks* z = iks_insert( y, "name" );
        iks_insert_cdata( z, m_versionName.c_str(), m_versionName.length() );
        z = iks_insert( y, "version" );
        iks_insert_cdata( z, m_versionVersion.c_str(), m_versionVersion.length() );
        m_parent->send( x );
      }
      else if( iks_strncmp( XMLNS_DISCO_INFO, xmlns, iks_strlen( XMLNS_DISCO_INFO ) ) == 0 )
      {
        iks* x = iks_new( "iq" );
        iks_insert_attrib( x, "type", "result" );
        iks_insert_attrib( x, "id", pak->id );
        iks_insert_attrib( x, "to", pak->from->full );
        iks_insert_attrib( x, "from", m_parent->jid().c_str() );
        iks* y = iks_insert( x, "query" );
        iks_insert_attrib( y, "xmlns", XMLNS_DISCO_INFO );
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
        m_parent->send( x );
      }
      else if( iks_strncmp( XMLNS_DISCO_ITEMS, xmlns, iks_strlen( XMLNS_DISCO_ITEMS ) ) == 0 )
      {
        iks* x = iks_new( "iq" );
        iks_insert_attrib( x, "type", "result" );
        iks_insert_attrib( x, "id", pak->id );
        iks_insert_attrib( x, "to", pak->from->full );
        iks_insert_attrib( x, "from", m_parent->jid().c_str() );
        iks* y = iks_insert( x, "query" );
        iks_insert_attrib( y, "xmlns", XMLNS_DISCO_ITEMS );
        m_parent->send( x );
      }
      break;

    case IKS_TYPE_SET:
      
      break;

    case IKS_TYPE_RESULT:
      if( ( iks_strncmp( XMLNS_DISCO_INFO, xmlns, iks_strlen( XMLNS_DISCO_INFO ) ) == 0 )
            &&  findID( pak->id, pak->from->full ) )
      {
       if( m_discoHandler )
         m_discoHandler->discoInfoResult( pak->id, pak );
      }
      else if( ( iks_strncmp( XMLNS_DISCO_ITEMS, xmlns, iks_strlen( XMLNS_DISCO_ITEMS ) ) == 0 )
                 &&  findID( pak->id, pak->from->full ) )
      {
        if( m_discoHandler )
          m_discoHandler->discoItemsResult( pak->id, pak );
      }
      break;

    case IKS_TYPE_ERROR:
      if( m_discoHandler )
      {
        iks* x = iks_child( iks_child( pak->x ) );
        m_discoHandler->discoError( pak->id, iks_name( x ) );
      }
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
  addQueryID( to, id );
}

void Disco::getDiscoItems( const string& to )
{
  std::string id = m_parent->getID();
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_DISCO_ITEMS );
  iks_insert_attrib( x, "from", m_parent->jid().c_str() );
  iks_insert_attrib( x, "to", to.c_str() );
  iks_insert_attrib( x, "id", id.c_str() );
  m_parent->send( x );
  addQueryID( to, id );
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
  m_discoHandler = dh;
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
    return true;
    m_queryIDs.erase( id );
  }
  return false;
}


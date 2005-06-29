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


#include "privatexml.h"

#include "jclient.h"

#include <iksemel.h>


PrivateXML::PrivateXML( JClient* parent )
  : m_parent( parent )
{
  m_parent->registerIqHandler( this, XMLNS_PRIVATE_XML );
}

PrivateXML::~PrivateXML ()
{

}

void PrivateXML::requestXML( const string& tag, const string& xmlns )
{
  string id = m_parent->getID();
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_PRIVATE_XML );
  iks_insert_attrib( x, "id", id.c_str() );
  iks* y = iks_find( x, "query" );
  iks* z = iks_insert( y, tag.c_str() );
  iks_insert_attrib( z, "xmlns", xmlns.c_str() );
  m_parent->send( x );
  m_requestIds[ id ] = xmlns;
}

void PrivateXML::storeXML( iks* xml, const string& xmlns )
{
  string id = m_parent->getID();
  iks* x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVATE_XML );
  iks_insert_attrib( x, "id", id.c_str() );
  iks_insert_node( x, xml );
  m_parent->send( x );
  m_storeIds[ id ] = xmlns;
}

void PrivateXML::handleIq( const char* xmlns, ikspak* pak )
{
  if( pak->subtype == IKS_TYPE_RESULT )
  {
    IDMap::iterator it;
    if( ( it = m_requestIds.find( pak->id ) ) != m_requestIds.end() )
    {
      char* ns = iks_name( iks_first_tag( iks_first_tag( pak->x ) ) );
      char* tag = iks_find_attrib( iks_first_tag( iks_first_tag( pak->x ) ), xmlns );
      PrivateXMLHandlers::const_iterator pi = m_privateXMLHandlers.begin();
      for( pi; pi != m_privateXMLHandlers.end(); pi++ )
      {
        if( ( iks_strcmp( (*pi)->tag->c_str(), tag ) ) && ( iks_strcmp( (*pi)->xmlns->c_str(), ns ) ) )
          (*pi)->pxh->handlePrivateXML( ns, tag, pak );
      }
      m_requestIds.erase( it );
    }
    else if( ( it = m_storeIds.find( pak->id ) ) != m_storeIds.end() )
    {
      m_storeIds.erase( it );
    }
  }
  else if( pak->subtype == IKS_TYPE_ERROR )
  {
    IDMap::iterator it;
    if( ( it = m_requestIds.find( pak->id ) ) != m_requestIds.end() )
    {
      m_requestIds.erase( it );
    }
    else if( ( it = m_storeIds.find( pak->id ) ) != m_storeIds.end() )
    {
      m_storeIds.erase( it );
    }
  }
}

void PrivateXML::registerPrivateXMLHandler( PrivateXMLHandler* pxh, const string& tag, const string& xmlns )
{
  HandlerStruct *tmp;
  tmp->xmlns = &xmlns;
  tmp->tag = &tag;
  tmp->pxh = pxh;
  m_privateXMLHandlers.push_back( tmp );
}

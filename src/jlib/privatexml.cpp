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
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_PRIVATE_XML );
  iks_insert_attrib( x, "id", m_parent->getID().c_str() );
  iks* y = iks_find( x, "query" );
  iks* z = iks_insert( y, tag.c_str() );
  iks_insert_attrib( z, "xmlns", xmlns.c_str() );
  m_parent->send( x );
}

void PrivateXML::storeXML( iks* xml )
{
  iks* x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVATE_XML );
  iks_insert_node( x, xml );
  m_parent->send( x );
}

void PrivateXML::handleIq( const char* xmlns, ikspak* pak )
{
  
}

void PrivateXML::registerPrivateXMLHandler( PrivateXMLHandler* pxh, const string& tag, const string& xmlns )
{
  HandlerStruct *tmp;
  tmp->xmlns = &xmlns;
  tmp->tag = &tag;
  tmp->pxh = pxh;
  m_privateXMLHandlers.push_back( tmp );
}

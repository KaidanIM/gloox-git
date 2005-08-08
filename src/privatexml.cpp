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


#include "privatexml.h"
#include "clientbase.h"

#include <iksemel.h>


namespace gloox
{

  PrivateXML::PrivateXML( ClientBase *parent )
    : m_parent( parent )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_PRIVATE_XML );
  }

  PrivateXML::~PrivateXML()
  {
    if( m_parent )
      m_parent->removeIqHandler( XMLNS_PRIVATE_XML );
  }

  void PrivateXML::requestXML( const string& tag, const string& xmlns )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_GET, XMLNS_PRIVATE_XML );
    iks_insert_attrib( x, "id", id.c_str() );
    iks *y = iks_find( x, "query" );
    iks *z = iks_insert( y, tag.c_str() );
    iks_insert_attrib( z, "xmlns", xmlns.c_str() );

    m_parent->trackID( this, id, REQUEST_XML );
    m_parent->send( x );
  }

  void PrivateXML::storeXML( iks *xml, const string& xmlns )
  {
    string id = m_parent->getID();

    iks *x = iks_make_iq( IKS_TYPE_SET, XMLNS_PRIVATE_XML );
    iks_insert_attrib( x, "id", id.c_str() );
    iks_insert_node( iks_child( x ), xml );

    m_parent->trackID( this, id, STORE_XML );
    m_parent->send( x );
  }

  void PrivateXML::handleIq( const char *tag, const char *xmlns, ikspak *pak )
  {
  }

  void PrivateXML::handleIqID( const char *id, ikspak *pak, int context )
  {
    if( pak->subtype == IKS_TYPE_RESULT )
    {
      switch( context )
      {
        case REQUEST_XML:
        {
          char *tag = iks_name( iks_child( pak->query ) );
          char *ns = iks_find_attrib( iks_child( pak->query ), "xmlns" );
          PrivateXMLHandlers::const_iterator pi = m_privateXMLHandlers.begin();
          for( pi; pi != m_privateXMLHandlers.end(); pi++ )
          {
            if( ( iks_strncmp( tag, (*pi).second.tag.c_str(), (*pi).second.tag.length() ) == 0 )
                  && ( iks_strncmp( ns, (*pi).first.c_str(), (*pi).first.length() )  == 0 ) )
            {
              (*pi).second.pxh->handlePrivateXML( tag, ns, pak );
            }
          }
          break;
        }

        case STORE_XML:
          break;
      }
    }
    else if( pak->subtype == IKS_TYPE_ERROR )
    {
    }
}

  void PrivateXML::registerPrivateXMLHandler( PrivateXMLHandler *pxh, const string& tag, const string& xmlns )
  {
    XMLHandlerStruct tmp;
    tmp.xmlns = xmlns;
    tmp.tag = tag;
    tmp.pxh = pxh;
    m_privateXMLHandlers[xmlns] = tmp;
  }

  void PrivateXML::removePrivateXMLHandler( const string& xmlns )
  {
    m_privateXMLHandlers.erase( xmlns );
  }

};

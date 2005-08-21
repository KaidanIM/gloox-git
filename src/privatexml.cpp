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
#include "stanza.h"

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

  void PrivateXML::requestXML( const std::string& tag, const std::string& xmlns )
  {
    const std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "id", id );
    iq.addAttrib( "type", "get" );
    Tag query( "query" );
    query.addAttrib( "xmlns", XMLNS_PRIVATE_XML );
    Tag x( tag );
    x.addAttrib( "xmlns", xmlns );
    query.addChild( x );
    iq.addChild( query );

    m_parent->trackID( this, id, REQUEST_XML );
    m_parent->send( iq );
  }

  void PrivateXML::storeXML( const Tag& tag, const std::string& xmlns )
  {
    const std::string id = m_parent->getID();

    Tag iq( "iq" );
    iq.addAttrib( "id", id );
    iq.addAttrib( "type", "set" );
    Tag query( "query" );
    query.addAttrib( "xmlns", XMLNS_PRIVATE_XML );
    query.addChild( tag );
    iq.addChild( query );

    m_parent->trackID( this, id, STORE_XML );
    m_parent->send( iq );
  }

  bool PrivateXML::handleIq( const Stanza& stanza )
  {
    return false;
  }

  bool PrivateXML::handleIqID( const Stanza& stanza, int context )
  {
    if( stanza.subtype() == STANZA_IQ_RESULT )
    {
      switch( context )
      {
        case REQUEST_XML:
        {
          Tag q = stanza.findChild( "query" );
          Tag::TagList l = q.children();
          Tag::TagList::const_iterator it = l.begin();
          if( it != l.end() )
          {
            Tag tag = (*it);
            const std::string xmlns = tag.findAttribute( "xmlns" );
            PrivateXMLHandlers::const_iterator pi = m_privateXMLHandlers.find( xmlns );
            if( ( pi != m_privateXMLHandlers.end() ) && ( tag.name() == (*pi).second.tag ) )
            {
              (*pi).second.pxh->handlePrivateXML( tag.name(), xmlns, tag );
            }
          }
          break;
        }

        case STORE_XML:
          break;
      }

      return true;
    }
    else if( stanza.subtype() == STANZA_IQ_ERROR )
    {
      return false;
    }

    return false;
  }

  void PrivateXML::registerPrivateXMLHandler( PrivateXMLHandler *pxh, const std::string& tag,
                                              const std::string& xmlns )
  {
    XMLHandlerStruct tmp;
    tmp.xmlns = xmlns;
    tmp.tag = tag;
    tmp.pxh = pxh;
    m_privateXMLHandlers[xmlns] = tmp;
  }

  void PrivateXML::removePrivateXMLHandler( const std::string& xmlns )
  {
    m_privateXMLHandlers.erase( xmlns );
  }

};

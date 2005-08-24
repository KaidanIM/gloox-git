/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
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

    m_privateXMLHandlers.clear();
  }

  void PrivateXML::requestXML( const std::string& tag, const std::string& xmlns )
  {
    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "id", id );
    iq->addAttrib( "type", "get" );
    Tag *query = new Tag( "query" );
    query->addAttrib( "xmlns", XMLNS_PRIVATE_XML );
    Tag *x = new Tag( tag );
    x->addAttrib( "xmlns", xmlns );
    query->addChild( x );
    iq->addChild( query );

    m_parent->trackID( this, id, REQUEST_XML );
    m_parent->send( iq );
  }

  void PrivateXML::storeXML( Tag *tag, const std::string& xmlns )
  {
    const std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "id", id );
    iq->addAttrib( "type", "set" );
    Tag *query = new Tag( "query" );
    query->addAttrib( "xmlns", XMLNS_PRIVATE_XML );
    query->addChild( tag );
    iq->addChild( query );

    m_parent->trackID( this, id, STORE_XML );
    m_parent->send( iq );
  }

  bool PrivateXML::handleIq( Stanza *stanza )
  {
    return false;
  }

  bool PrivateXML::handleIqID( Stanza *stanza, int context )
  {
    if( stanza->subtype() == STANZA_IQ_RESULT )
    {
      switch( context )
      {
        case REQUEST_XML:
        {
          Tag *q = stanza->findChild( "query" );
          Tag::TagList l = q->children();
          Tag::TagList::const_iterator it = l.begin();
          if( it != l.end() )
          {
            Tag *tag = (*it);
            const std::string xmlns = tag->findAttribute( "xmlns" );
            PrivateXMLHandlers::const_iterator pi = m_privateXMLHandlers.find( xmlns );
            if( ( pi != m_privateXMLHandlers.end() ) && ( tag->name() == (*pi).second.tag ) )
            {
              (*pi).second.pxh->handlePrivateXML( tag->name(), xmlns, tag );
            }
          }
          break;
        }

        case STORE_XML:
          break;
      }

      return true;
    }
    else if( stanza->subtype() == STANZA_IQ_ERROR )
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

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



#include "annotations.h"
#include "clientbase.h"


namespace gloox
{

  Annotations::Annotations( ClientBase *parent )
    : PrivateXML( parent ),
    m_annotationsHandler( 0 )
  {
    registerPrivateXMLHandler( this, "storage", XMLNS_ANNOTATIONS );
  }

  Annotations::~Annotations()
  {
    removePrivateXMLHandler( XMLNS_ANNOTATIONS );
  }

  void Annotations::storeAnnotations( const AnnotationsHandler::AnnotationsList& aList )
  {
    Tag *s = new Tag( "storage" );
    s->addAttrib( "xmlns", XMLNS_ANNOTATIONS );

    if( aList.size() )
    {
      AnnotationsHandler::AnnotationsList::const_iterator it = aList.begin();
      for( it; it != aList.end(); it++ )
      {
        Tag *n = new Tag( "note", (*it).note );
        n->addAttrib( "jid", (*it).jid );
        n->addAttrib( "cdate", (*it).cdate );
        n->addAttrib( "mdate", (*it).mdate );
        s->addChild( n );
      }
    }

    storeXML( s, XMLNS_ANNOTATIONS );
  }

  void Annotations::requestAnnotations()
  {
    requestXML( "storage", XMLNS_ANNOTATIONS );
  }

  void Annotations::handlePrivateXML( const std::string& tag, const std::string& xmlns, Tag *xml )
  {
    AnnotationsHandler::AnnotationsList aList;
    const Tag::TagList l = xml->children();
    Tag::TagList::const_iterator it = l.begin();
    for( it; it != l.end(); it++ )
    {
      if( (*it)->name() == "note" )
      {
        const std::string jid = (*it)->findAttribute( "jid" );
        const std::string mdate = (*it)->findAttribute( "mdate" );
        const std::string cdate = (*it)->findAttribute( "cdate" );
        const std::string note = (*it)->cdata();

        if( !jid.empty() && !note.empty() )
        {
          AnnotationsHandler::annotationsListItem item;
          item.jid = jid;
          item.note = note;
          item.mdate = mdate;
          item.cdate = cdate;
          aList.push_back( item );
        }
      }
    }

    if( m_annotationsHandler )
      m_annotationsHandler->handleAnnotations( aList );
  }

  void Annotations::registerAnnotationsHandler( AnnotationsHandler *ah )
  {
    m_annotationsHandler = ah;
  }

  void Annotations::removeAnnotationsHandler()
  {
    m_annotationsHandler = 0;
  }

};

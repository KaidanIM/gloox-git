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
    iks *storage = iks_new( "storage" );
    iks_insert_attrib( storage, "xmlns", XMLNS_ANNOTATIONS );

    if( aList.size() )
    {
      AnnotationsHandler::AnnotationsList::const_iterator it = aList.begin();
      for( it; it != aList.end(); it++ )
      {
        iks *item = iks_insert( storage, "note" );
        iks_insert_attrib( item, "jid", (*it).jid.c_str() );
        iks_insert_attrib( item, "cdate", (*it).cdate.c_str() );
        iks_insert_attrib( item, "mdate", (*it).mdate.c_str() );
        iks_insert_cdata( item, (*it).note.c_str(), (*it).note.length() );
      }
    }

    storeXML( storage, XMLNS_ANNOTATIONS );
  }

  void Annotations::requestAnnotations()
  {
    requestXML( "storage", XMLNS_ANNOTATIONS );
  }

  void Annotations::handlePrivateXML( const string& tag, const string& xmlns, ikspak* pak )
  {
    iks *x = iks_first_tag( iks_first_tag( pak->query ) );

    AnnotationsHandler::AnnotationsList aList;

    while( x )
    {
      if( iks_strncmp( iks_name( x ), "note", 4 ) == 0 )
      {
        char *jid = iks_find_attrib( x, "jid" );
        char *mdate = iks_find_attrib( x, "mdate" );
        char *cdate = iks_find_attrib( x, "cdate" );
        char *note = iks_cdata( iks_child( x ) );

        if( jid && note )
        {
          AnnotationsHandler::annotationsListItem item;
          item.jid = jid;
          item.note = note;
          if( mdate )
            item.mdate = mdate;
          if( cdate )
            item.cdate = cdate;
          aList.push_back( item );
        }
      }
      x = iks_next( x );
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

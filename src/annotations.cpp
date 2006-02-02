/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "annotations.h"
#include "clientbase.h"


namespace gloox
{

  Annotations::Annotations( ClientBase *parent )
    : PrivateXML( parent ),
    m_annotationsHandler( 0 )
  {
  }

  Annotations::~Annotations()
  {
  }

  void Annotations::storeAnnotations( const AnnotationsHandler::AnnotationsList& aList )
  {
    Tag *s = new Tag( "storage" );
    s->addAttrib( "xmlns", XMLNS_ANNOTATIONS );

    if( aList.size() )
    {
      AnnotationsHandler::AnnotationsList::const_iterator it = aList.begin();
      for( ; it != aList.end(); ++it )
      {
        Tag *n = new Tag( "note", (*it).note );
        n->addAttrib( "jid", (*it).jid );
        n->addAttrib( "cdate", (*it).cdate );
        n->addAttrib( "mdate", (*it).mdate );
        s->addChild( n );
      }
    }

    storeXML( s, this );
  }

  void Annotations::requestAnnotations()
  {
    requestXML( "storage", XMLNS_ANNOTATIONS, this );
  }

  void Annotations::handlePrivateXML( const std::string& /*tag*/, Tag *xml )
  {
    AnnotationsHandler::AnnotationsList aList;
    const Tag::TagList l = xml->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
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

}

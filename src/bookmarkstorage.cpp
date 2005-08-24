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



#include "bookmarkstorage.h"
#include "clientbase.h"


namespace gloox
{

  BookmarkStorage::BookmarkStorage( ClientBase *parent )
    : PrivateXML( parent ),
    m_bookmarkHandler( 0 )
  {
    registerPrivateXMLHandler( this, "storage", XMLNS_BOOKMARKS );
  }

  BookmarkStorage::~BookmarkStorage()
  {
    removePrivateXMLHandler( XMLNS_BOOKMARKS );
  }

  void BookmarkStorage::storeBookmarks( const BookmarkHandler::BookmarkList& bList,
                                        const BookmarkHandler::ConferenceList& cList )
  {
    Tag *s = new Tag( "storage" );
    s->addAttrib( "xmlns", XMLNS_BOOKMARKS );

    if( bList.size() )
    {
      BookmarkHandler::BookmarkList::const_iterator it = bList.begin();
      for( it; it != bList.end(); it++ )
      {
        Tag *i = new Tag( "url" );
        i->addAttrib( "name", (*it).name );
        i->addAttrib( "url", (*it).url );
        s->addChild( i );
      }
    }

    if( cList.size() )
    {
      BookmarkHandler::ConferenceList::const_iterator it = cList.begin();
      for( it; it != cList.end(); it++ )
      {
        Tag *i = new Tag( "conference" );
        i->addAttrib( "name", (*it).name );
        i->addAttrib( "jid", (*it).jid );
        if( (*it).autojoin )
          i->addAttrib( "autojoin", "true" );
        else
          i->addAttrib( "autojoin", "false" );
        i->addChild( new Tag( "nick", (*it).nick ) );
        i->addChild( new Tag( "password", (*it).password ) );
        s->addChild( i );
      }
    }

    storeXML( s, XMLNS_BOOKMARKS );
  }

  void BookmarkStorage::requestBookmarks()
  {
    requestXML( "storage", XMLNS_BOOKMARKS );
  }

  void BookmarkStorage::handlePrivateXML( const std::string& tag, const std::string& xmlns, Tag *xml )
  {
    BookmarkHandler::BookmarkList bList;
    BookmarkHandler::ConferenceList cList;
    const Tag::TagList l = xml->children();
    Tag::TagList::const_iterator it = l.begin();
    for( it; it != l.end(); it++ )
    {
      if( (*it)->name() == "url" )
      {
        const std::string url = (*it)->findAttribute( "url" );
        const std::string name = (*it)->findAttribute( "name" );

        if( !url.empty() && !name.empty() )
        {
          BookmarkHandler::bookmarkListItem item;
          item.url = url;
          item.name = name;
          bList.push_back( item );
        }
      }
      else if( (*it)->name() == "conference" )
      {
        bool autojoin = false;
        const std::string jid = (*it)->findAttribute( "jid" );
        const std::string name = (*it)->findAttribute( "name" );
        const std::string join = (*it)->findAttribute( "autojoin" );
        if( ( join == "true" ) || ( join == "1" ) )
          autojoin = true;
        const std::string nick = (*it)->findChild( "nick" )->cdata();
        const std::string pwd = (*it)->findChild( "password" )->cdata();

        if( !jid.empty() && !name.empty() )
        {
          BookmarkHandler::conferenceListItem item;
          item.jid = jid;
          item.name = name;
          item.nick = nick;
          item.password = pwd;
          item.autojoin = autojoin;
          cList.push_back( item );
        }
      }
    }

    if( m_bookmarkHandler )
      m_bookmarkHandler->handleBookmarks( bList, cList );
  }

  void BookmarkStorage::registerBookmarkHandler( BookmarkHandler *bmh )
  {
    m_bookmarkHandler = bmh;
  }

  void BookmarkStorage::removeBookmarkHandler()
  {
    m_bookmarkHandler = 0;
  }

};

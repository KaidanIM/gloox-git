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
    iks *storage = iks_new( "storage" );
    iks_insert_attrib( storage, "xmlns", XMLNS_BOOKMARKS );

    if( bList.size() )
    {
      BookmarkHandler::BookmarkList::const_iterator it = bList.begin();
      for( it; it != bList.end(); it++ )
      {
        iks *item = iks_insert( storage, "url" );
        iks_insert_attrib( item, "name", (*it).name.c_str() );
        iks_insert_attrib( item, "url", (*it).url.c_str() );
      }
    }

    if( cList.size() )
    {
      BookmarkHandler::ConferenceList::const_iterator it = cList.begin();
      for( it; it != cList.end(); it++ )
      {
        iks *item = iks_insert( storage, "conference" );
        iks_insert_attrib( item, "name", (*it).name.c_str() );
        iks_insert_attrib( item, "jid", (*it).jid.c_str() );
        iks_insert_cdata( iks_insert( item, "nick" ), (*it).nick.c_str(), (*it).nick.length() );
        iks_insert_cdata( iks_insert( item, "password" ), (*it).password.c_str(), (*it).password.length() );
      }
    }

    storeXML( storage, XMLNS_BOOKMARKS );
  }

  void BookmarkStorage::requestBookmarks()
  {
    requestXML( "storage", XMLNS_BOOKMARKS );
  }

  void BookmarkStorage::handlePrivateXML( const string& tag, const string& xmlns, ikspak* pak )
  {
    iks *x = iks_first_tag( iks_first_tag( iks_first_tag( pak->x ) ) );

    BookmarkHandler::BookmarkList bList;
    BookmarkHandler::ConferenceList cList;

    while( x )
    {
      if( iks_strncmp( iks_name( x ), "url", 3 ) == 0 )
      {
        char *url = iks_find_attrib( x, "url" );
        char *name = iks_find_attrib( x, "name" );

        if( url && name )
        {
          BookmarkHandler::bookmarkListItem item;
          item.url = url;
          item.name = name;
          bList.push_back( item );
        }
      }
      else if( iks_strncmp( iks_name( x ), "conference", 10 ) == 0 )
      {
        char *jid = iks_find_attrib( x, "jid" );
        char *name = iks_find_attrib( x, "name" );
        char *nick = iks_find_cdata( x, "nick" );
        char *password = iks_find_cdata( x, "password" );

        if( jid && name )
        {
          BookmarkHandler::conferenceListItem item;
          item.jid = jid;
          item.name = name;
          if( nick )
            item.nick = nick;
          if( password )
            item.password = password;
          cList.push_back( item );
        }
      }
      x = iks_next( x );
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

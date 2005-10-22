/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#include "bookmarkstorage.h"
#include "clientbase.h"


namespace gloox
{

  BookmarkStorage::BookmarkStorage( ClientBase *parent )
    : PrivateXML( parent ),
    m_bookmarkHandler( 0 )
  {
  }

  BookmarkStorage::~BookmarkStorage()
  {
  }

  void BookmarkStorage::storeBookmarks( const BookmarkHandler::BookmarkList& bList,
                                        const BookmarkHandler::ConferenceList& cList )
  {
    Tag *s = new Tag( "storage" );
    s->addAttrib( "xmlns", XMLNS_BOOKMARKS );

    if( bList.size() )
    {
      BookmarkHandler::BookmarkList::const_iterator it = bList.begin();
      for( ; it != bList.end(); it++ )
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
      for( ; it != cList.end(); it++ )
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

    storeXML( s, this );
  }

  void BookmarkStorage::requestBookmarks()
  {
    requestXML( "storage", XMLNS_BOOKMARKS, this );
  }

  void BookmarkStorage::handlePrivateXML( const std::string& /*tag*/, Tag *xml )
  {
    BookmarkHandler::BookmarkList bList;
    BookmarkHandler::ConferenceList cList;
    const Tag::TagList l = xml->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); it++ )
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

}

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



#ifndef BOOKMARKSTORAGE_H__
#define BOOKMARKSTORAGE_H__

#include "bookmarkhandler.h"
#include "privatexml.h"
#include "privatexmlhandler.h"

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  /**
   * This is an implementation of JEP-0048 (Bookmark Storage).
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class BookmarkStorage : public PrivateXML, PrivateXMLHandler
  {
    public:
      /**
       * Constructs a new BookmarkStorage object.
       * @param parent The ClientBase to use for communication.
       */
      BookmarkStorage( ClientBase *parent );

      /**
       * Virtual destructor.
       */
      virtual ~BookmarkStorage();

      /**
       * Use this function to store number of URLs and conference bookmarks on the server.
       * Make sure you store the whole set of bookmarks, not a 'delta'.
       * @param bList A list of URLs to store.
       * @param cList A list of conferences to store.
       */
      void storeBookmarks( const BookmarkHandler::BookmarkList& bList,
                           const BookmarkHandler::ConferenceList& cList );

      /**
       * Use this function to initiate retrieval of bookmarks. Use registerBookmarkHandler()
       * to register an object which will receive the lists of bookmarks.
       */
      void requestBookmarks();

      /**
       * Use this function to register a BookmarkHandler.
       * @param bmh The BookmarkHandler which shall receive retrieved bookmarks.
       */
      void registerBookmarkHandler( BookmarkHandler *bmh );

      /**
       * Use this function to un-register the BookmarkHandler.
       */
      void removeBookmarkHandler();

      // reimplemented from PrivateXMLHandler
      virtual void handlePrivateXML( const std::string& tag, const std::string& xmlns, const Tag& xml );

    private:
      BookmarkHandler *m_bookmarkHandler;
  };

};

#endif // BOOKMARKSTORAGE_H__

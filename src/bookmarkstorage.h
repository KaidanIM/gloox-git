/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
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
       * Use this function to store a number of URL and conference bookmarks on the server.
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
      virtual void handlePrivateXML( const std::string& tag, const std::string& xmlns, Tag *xml );

    private:
      BookmarkHandler *m_bookmarkHandler;
  };

};

#endif // BOOKMARKSTORAGE_H__

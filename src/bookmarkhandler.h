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



#ifndef BOOKMARKHANDLER_H__
#define BOOKMARKHANDLER_H__

#include <string>
#include <list>
using namespace std;


namespace gloox
{

  /**
   * A virtual interface which can be reimplemented to receive bookmarks with help of a
   * BookmarkStorage object.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class BookmarkHandler
  {
    public:
      /**
       * This describes a single bookmarked URL item.
       */
      struct bookmarkListItem
      {
        string name;
        string url;
      };

      /**
       * This describes a single bookmarked conference item.
       */
      struct conferenceListItem
      {
        string name;
        string jid;
        string nick;
        string password;
      };

      /**
       * A list of URL items.
       */
      typedef list<bookmarkListItem> BookmarkList;

      /**
       * A list of conference items.
       */
      typedef list<conferenceListItem> ConferenceList;

      /**
       * This function is called when bookmarks arrive from the server.
       * @param bList A list of URL bookmarks.
       * @param cList A list of conference bookmarks.
       */
      virtual void handleBookmarks( BookmarkList bList, ConferenceList cList ) {};
  };

};

#endif // BOOKMARKHANDLER_H__

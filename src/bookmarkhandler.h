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
        std::string name;             /**< A human readable name of the bookmark. */
        std::string url;              /**< The URL of the bookmark. */
      };

      /**
       * This describes a single bookmarked conference item.
       */
      struct conferenceListItem
      {
        std::string name;             /**< A human readable name of the conference room. */
        std::string jid;              /**< The address of the room. */
        std::string nick;             /**< The nick name to use in this room. */
        std::string password;         /**< The password to use for a protected room. */
        bool autojoin;                /**< The conference shall be joined automatically on login. */
      };

      /**
       * A list of URL items.
       */
      typedef std::list<bookmarkListItem> BookmarkList;

      /**
       * A list of conference items.
       */
      typedef std::list<conferenceListItem> ConferenceList;

      /**
       * This function is called when bookmarks arrive from the server.
       * @param bList A list of URL bookmarks.
       * @param cList A list of conference bookmarks.
       */
      virtual void handleBookmarks( BookmarkList bList, ConferenceList cList ) {};
  };

};

#endif // BOOKMARKHANDLER_H__

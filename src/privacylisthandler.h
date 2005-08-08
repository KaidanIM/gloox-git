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


#ifndef PRIVACYLISTHANDLER_H__
#define PRIVACYLISTHANDLER_H__

#include "privacyitem.h"

#include <string>
#include <list>
using namespace std;

namespace gloox
{

  /**
   * A virtual interface that allows to retrieve Privacy Lists.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class PrivacyListHandler
  {
    public:

      /**
       * The possible results of an operation on a privacy list.
       */
      enum resultEnum
      {
        RESULT_STORE_SUCCESS,
        RESULT_ACTIVATE_SUCCESS,
        RESULT_DEFAULT_SUCCESS,
        RESULT_REMOVE_SUCCESS,
        RESULT_REQUEST_NAMES_SUCCESS,
        RESULT_REQUEST_LIST_SUCCESS,
        RESULT_CONFLICT,
        RESULT_ITEM_NOT_FOUND,
        RESULT_BAD_REQUEST
      };

      /**
       * A list of PrivacyItems.
       */
      typedef list<PrivacyItem> PrivacyList;

      /**
       * A list of list names.
       */
      typedef list<string> StringList;

      /**
       * Reimplement this function to retrieve the list of privacy list names after requesting it using
       * PrivacyManager::requestListNames().
       * @param active The name of the active list.
       * @param def The name of the default list.
       * @param lists All the lists.
       */
      virtual void handlePrivacyListNames( const string& active, const string& def,
                                           const StringList& lists ) {};

      /**
       * Reimplement this function to retrieve the content of a privacy list after requesting it using
       * PrivacyManager::requestList().
       * @param name The name of the list.
       * @param items A list of PrivacyItem's.
       */
      virtual void handlePrivacyList( const string& name, PrivacyList& items ) {};

      /**
       * Reimplement this function to be notified about new or changed lists.
       * @param name The name of the new or changed list.
       */
      virtual void handlePrivacyListChanged( const string& name ) {};

      /**
       * Reimplement this function to receive results of stores etc.
       * @param id The ID of the request, as returned by the initiating function.
       * @param result The result of an operation.
       */
      virtual void handlePrivacyListResult( const string& id, resultEnum result ) {};

  };

};

#endif // PRIVACYLISTHANDLER_H__

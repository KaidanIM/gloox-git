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


#ifndef PRIVACYMANAGER_H__
#define PRIVACYMANAGER_H__

#include "iqhandler.h"
#include "privacylisthandler.h"

#include <string>
using namespace std;

namespace gloox
{

  class ClientBase;

  /**
   * This class implements a manager for privacy lists as defined in section 10 of RFC 3921.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class PrivacyManager : public IqHandler
  {
    public:
      /**
       * Constructs a new PrivacyManager.
       * @param parent The ClientBase to use for communication.
       */
      PrivacyManager( ClientBase *parent );

      /**
       * Virtual destructor.
       */
      virtual ~PrivacyManager();

      /**
       * Triggers the request of the privacy lists currently stored on the server.
       */
      string requestListNames();

      /**
       * Triggers the retrieval of the named privacy lists.
       * @param name The name of the list to retrieve.
       */
      string requestList( const string& name );

      /**
       * Stores the given list on the server. If a list with the given name exists it is overwritten.
       * @param name The list's name.
       * @param list A list of privacy items which describe the list.
       * @note If @c list is empty the privacy list with the given name will be removed
       * if it exists on the server. (Same as removeList().)
       */
      string store( const string& name, PrivacyListHandler::PrivacyList& list );

      /**
       * Removes a list by its name.
       * @param name The name of the list to remove.
       */
      string removeList( const string& name );

      /**
       * Sets the named list as the default list, i.e. active by default after login.
       * @param name The name of the list to set as default.
       */
      string setDefault( const string& name );

      /**
       * This function declines the use of any default list.
       */
      string unsetDefault();

      /**
       * Sets the named list as active, i.e. active for this session
       * @param name The name of the list to set active.
       */
      string setActive( const string& name );

      /**
       * This function declines the use of any active list.
       */
      string unsetActive();

      /**
       * Use this function to register an object as PrivacyListHandler.
       * Only one PrivacyListHandler at a time is possible.
       * @param plh The object to register as handler for privacy list related events.
       */
      void registerPrivacyListHandler( PrivacyListHandler *plh );

      /**
       * Use this function to clear the registered PrivacyListHandler.
       */
      void removePrivacyListHandler();

      // reimplemented from IqHandler
      virtual void handleIq( const char *tag, const char *xmlns, ikspak *pak );

      // reimplemented from IqHandler
      virtual void handleIqID( const char *id, ikspak *pak, int context );

    private:
      enum IdType
      {
        PL_REQUEST_NAMES,
        PL_REQUEST_LIST,
        PL_ACTIVATE,
        PL_DEFAULT,
        PL_UNSET_ACTIVATE,
        PL_UNSET_DEFAULT,
        PL_REMOVE,
        PL_STORE
      };

      ClientBase *m_parent;
      PrivacyListHandler *m_privacyListHandler;
  };

};

#endif // PRIVACYMANAGER_H__

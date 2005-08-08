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



#ifndef REGISTRATIONHANDLER_H__
#define REGISTRATIONHANDLER_H__

#include <iksemel.h>

#include <string>
using namespace std;


namespace gloox
{

  /**
   * A virtual interface that receives events from an @ref Registration object.
   * Derived classes can be registered as RegistrationHandlers with an
   * Registration object. Incoming result for operations initiated through
   * the Registration object are forwarded to this handler.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.2
   */
  class RegistrationHandler
  {
    public:
      /**
       * Possible results of a JEP-0077 operation.
       */
      enum resultEnum
      {
        REGISTRATION_SUCCESS = 0,             /*!< The last operation (account registration, account
                                               * deletion or password change) was successful. */
        REGISTRATION_NOT_ACCEPTABLE,          /*!< 406: Not all necessary information provided */
        REGISTRATION_CONFLICT,                /*!< 409: Username alreday exists. */
        REGISTRATION_NOT_AUTHORIZED,          /*!< Account removal: Unregistered entity waits too long
                                               * before authentication or performs tasks other than
                                               * authentication after registration.<br>
                                               * Password change: The server or service does not consider
                                               * the channel safe enough to enable a password change. */
        REGISTRATION_BAD_REQUEST,             /*!< Account removal: The &lt;remove/&gt; element was not the
                                               * only child element of the &lt;query/&gt; element. Should not
                                               * happen when only gloox functions are being used.<br>
                                               * Password change: The password change request does not
                                               * contain complete information (both &lt;username/&gt; and
                                               * &lt;password/&gt; are required). */
        REGISTRATION_FORBIDDEN,               /*!< Account removal: The sender does not have sufficient
                                               * permissions to cancel the registration. */
        REGISTRATION_REGISTRATION_REQUIRED,   /*!< Account removal: The entity sending the remove request was
                                               * not previously registered. */
        REGISTRATION_UNEXPECTED_REQUEST,      /*!< Account removal: The host is an instant messaging server
                                               * and the IQ get does not contain a 'from' address because the
                                               * entity is not registered with the server.<br>
                                               * Password change: The host is an instant messaging server and
                                               * the IQ set does not contain a 'from' address because the
                                               * entity is not registered with the server. */
        REGISTRATION_NOT_ALLOWED              /*!< Password change: The server or service does not allow
                                               * password changes. */
      };

      /**
       * Reimplement this function to receive results of the @ref Registration::fetchRegistrationFields()
       * function.
       * @param fields The OR'ed fields the server requires. From @ref Registration::fieldEnum.
       * @param instructions Any additional information the server sends along.
       */
      virtual void handleRegistrationFields( int fields, string instructions ) {};

      /**
       * This function is called if @ref Registration::createAccount() was called on an authenticated
       * stream and the server lets us know about this.
       */
      virtual void handleAlreadyRegistered() {};

      /**
       * This funtion is called to notify about the result of an operation.
       * @param result The result of the last operation.
       */
      virtual void handleRegistrationResult( resultEnum result ) {};
  };

};

#endif // REGISTRATIONHANDLER_H__

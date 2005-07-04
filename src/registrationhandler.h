/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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
     * Reimplement this function to receive results of the @ref Registration::fetchRegistrationFields()
     * function.
     * @param fields The OR'ed fields the server requires. From @ref Registration::fieldEnum.
     * @param instructions Any additional information the server sends along.
     */
    virtual void handleRegistrationFields( int fields, string instructions ) {};
};

#endif // REGISTRATIONHANDLER_H__

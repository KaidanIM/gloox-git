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


#ifndef REGISTRATION_H__
#define REGISTRATION_H__

#include "iqhandler.h"
#include "registrationhandler.h"

class JClient;

/**
 * This class is an implementation of JEP-0077 In-Band Registration.
 * Usage is very straight-forward:<br>
 * Derive your object from @ref RegistrationHandler and implement the
 * virtual functions offered by that interface.
 * @code
 * Registration* r = new Registration( jclient );
 * r->registerRegistrationHandler( this );
 * r->createAccount( ... );
 * @endcode
 * @author Jakob Schroeter <js@camaya.net>
 * @since 0.2
 */
class Registration : public IqHandler
{

  public:
    /**
     * Constructor.
     * If no @c JClient is supplied, one is created. However, this can only be used
     * for creating a new accopunt.
     * @param parent The JClient which is used to establish the connection.
     */
    Registration( JClient* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~Registration();

    /**
     * Use this function to request the required registration fields from the server.
     * The result of the query is returned to the object registered as RegistrationHandler.
     */
    void fetchRegistrationFields();

    /**
     * Starts the interaction with the server to accomplish an in-band
     * registration.
     */
    void createAccount();

    /**
     * Tells the server to remove the currently authenticated account from the server.
     */
    void removeAccount();

    /**
     * Tells the server to change the password for the current account.
     */
    void changePassword();

    /**
     * Registers the given @c rh as RegistrationHandler. Only one handler is possibel at a time.
     * @param rh The RegistrationHandler to register.
     */
    void registerRegistrationHandler( RegistrationHandler* rh );

    /* reimplemented from IqHandler */
    virtual void handleIq( const char* xmlns, ikspak* pak );

    /* reimplemented from IqHandler */
    virtual void handleIqID( const char* id, ikspak* pak );

  private:
    JClient* m_parent;
    RegistrationHandler* m_registrationHandler;
};

#endif // REGISTRATION_H__

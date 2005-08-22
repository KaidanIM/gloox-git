/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
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


#ifndef CONNECTIONLISTENER_H__
#define CONNECTIONLISTENER_H__

#include <iksemel.h>

namespace gloox
{

  /**
   * A virtual interface.
   * Derived classes can be registered as ConnectionListeners with the Client
   * @author Jakob Schroeter <js@camaya.net>
   */
  class ConnectionListener
  {
    public:

      /**
       * Describes the possible error conditions for resource binding.
       */
      enum ResourceBindError
      {
        RB_UNKNOWN_ERROR,                  /**< An unknown error occured. */
        RB_BAD_REQUEST,                    /**< Resource identifier cannot be processed. */
        RB_NOT_ALLOWED,                    /**< Client is not allowed to bind a resource. */
        RB_CONFLICT                        /**< Resource identifier is in use. */
      };

      /**
       * Describes the possible error conditions for session establishemnt.
       */
      enum SessionCreateError
      {
        SC_UNKNOWN_ERROR,                  /**< An unknown error occured. */
        SC_INTERNAL_SERVER_ERROR,          /**< Internal server error. */
        SC_FORBIDDEN,                      /**< Username or resource not allowed to create session. */
        SC_CONFLICT                        /**< Server informs newly-requested session of resource
                                            * conflict. */
      };

      /**
       * This function notifies about successful connections. It will be called either after all
       * authentication is finished if username/password were supplied, or after a connection has
       * been established if no credentials were supplied. Depending on the setting of AutoPresence,
       * a presence stanza is sent or not.
       */
      virtual void onConnect() = 0;

      /**
       * This function notifies about disconnection.
       */
      virtual void onDisconnect() = 0;

      /**
       * This function is called (by a Client object) if an error occurs while trying to bind a resource.
       * @param error Describes the error condition.
       */
      virtual void onResourceBindError( ResourceBindError error ) {};

      /**
       * This function is called (by a Client object) if an error occurs while trying to establish
       * a session.
       * @param error Describes the error condition.
       */
      virtual void onSessionCreateError( SessionCreateError error ) {};

      /**
       * This function is called when the connection was TLS/SSL secured.
       * @param info Comprehensive info on the certificate.
       * @return @b True if cert credentials are accepted, @b false otherwise. If @b false is returned
       * the connection is terminated.
       */
      virtual bool onTLSConnect( const CertInfo& info ) = 0;

  };

};

#endif // CONNECTIONLISTENER_H__

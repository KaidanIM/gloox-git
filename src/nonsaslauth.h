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


#ifndef NONSASLAUTH_H__
#define NONSASLAUTH_H__

#include "iqhandler.h"

#include <string>

namespace gloox
{

  class JClient;
  class Stanza;
  class Tag;

  /**
   * This class is an implementation of JEP-0078 (Non-SASL Authentication).
   * It is invoked by @ref JClient automatically if supported by the server and if SASL authentication
   * is not supported.
   * You should not need to use this class manually.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class NonSaslAuth : public IqHandler
  {
    public:
      /**
       * Constructor.
       * @param parent The @ref ClientBase which is used to authenticate.
       * @param sid The session ID given by the server with the stream opening tag.
       */
      NonSaslAuth( JClient *parent, const std::string& sid );

      /**
       * Virtual Destructor.
       */
      virtual ~NonSaslAuth();

      /**
       * Starts authentication by querying the server for the required authentication fields.
       * Digest authentication is preferred over plain text passwords.
       */
      void doAuth();

      // reimplemented from IqHandler
      virtual void handleIq( const Stanza& stanza );

      // reimplemented from IqHandler
      virtual void handleIqID( const Stanza& stanza, int context );

    private:
      enum NonSaslAuthType
      {
        TYPE_DIGEST,
        TYPE_PLAIN
      };

      const Tag createAuthTag( NonSaslAuthType type, const JID& jid,
                               const std::string& password, const std::string& sid  );

      JClient *m_parent;
      std::string m_sid;

  };

};

#endif // NONSASLAUTH_H__

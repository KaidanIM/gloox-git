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



#ifndef JID_H__
#define JID_H__

#include <string>

namespace gloox
{
  /**
   * An implementation of a JID.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class JID
  {
    public:

      /**
       * Constructs an empty JID.
       */
      JID();

      /**
       * Constructs a new JID from a string.
       * @param jid The string containing the JID.
       */
      JID( const std::string& jid );

      /**
       * Virtual destructor.
       */
      virtual ~JID();

      /**
       * Sets the JID from a string.
       * @param jid The string containing the JID.
       */
      void setJID( const std::string& jid );

      /**
       * Returns the full JID (user@host/resource).
       * @return The full JID.
       */
      std::string full() const;

      /**
       * Returns the bare JID (user@host).
       * @return The bare JID.
       */
      std::string bare() const;

      /**
       * Sets the username.
       * @param username The new username.
       */
      void setUsername( const std::string& username );

      /**
       * Sets the server.
       * @param server The new server.
       */
      void setServer( const std::string& server );

      /**
       * Sets the resource.
       * @param resource The new resource.
       */
      void setResource( const std::string& resource );

      /**
       * Returns the username.
       * @return The current username.
       */
      std::string username() const { return m_username; };

      /**
       * Returns the prepped server name.
       * @return The current server.
       */
      std::string server() const { return m_server; };

      /**
       * Returns the raw (unprepped) server name.
       * @return The raw server name.
       */
      std::string serverRaw() const { return m_serverRaw; };

      /**
       * Returns the resource.
       * @return The current resource.
       */
      std::string resource() const { return m_resource; };

      /**
       * A JID is empty as long as no server is set.
       * @return @b True if the JID is empty, @b false otherwise.
       */
      bool empty() const { return m_server.empty(); };

      /**
       * Compares two JIDs.
       * @param right The second JID.
       */
      int operator==( const JID& right );

    private:
      std::string m_resource;
      std::string m_username;
      std::string m_server;
      std::string m_serverRaw;
  };

};

#endif // JID_H__

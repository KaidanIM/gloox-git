/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#ifndef COMPONENT_H__
#define COMPONENT_H__

#include "clientbase.h"

#include <string>

namespace gloox
{

  class Disco;

  /**
   * This is an implementation of a Component, using JEP-0114 (Jabber Component Protocol) to
   * authenticate with a server.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class Component : public ClientBase
  {
    public:
      /**
       * Constructs a new Component.
       * @param ns The namespace that qualifies the stream. Either @b jabber:component:accept or
       * @b jabber:component:connect.
       * @param server The server to connect to.
       * @param component The component's hostname. FQDN.
       * @param password The component's password.
       * @param port The port to connect to. The default of -1 means to look up the port via DNS SRV.
       */
      Component( const std::string& ns, const std::string& server,
                  const std::string& component, const std::string& password, int port = -1 );

      /**
       * Virtual Destructor.
       */
      virtual ~Component();

      /**
       * Sets the components host name.
       * @param to The components host name.
       */
      void setStreamTo( const std::string& to ) { m_to = to; };

      // reimplemented from ClientBase
      virtual const std::string username() const { return m_jid.username(); };

      // reimplemented from ClientBase
      virtual const std::string password() const { return ""; };

      /**
       * Returns the JID of the component.
       * @return The components JID.
       */
      virtual const std::string jid() const { return Prep::nameprep( m_to ); };

      /**
       * Gives access to the component's Disco object.
       * @return A pointer to the Disco object.
       */
      Disco* disco() const { return m_disco; };

    protected:
      /**
       * Returns the hostname which should be put into the stream's 'to' attribute.
       * @return The host to name in the stream's 'to' attribute.
       */
      virtual const std::string streamTo() const { return Prep::nameprep( m_to ); };

      virtual void handleStartNode();
      virtual bool handleNormalNode( Stanza *stanza );

    private:

      Disco *m_disco;
      std::string m_to;
  };

};

#endif // COMPONENT_H__

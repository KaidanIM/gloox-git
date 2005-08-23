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



#ifndef DISCO_H__
#define DISCO_H__

#include "iqhandler.h"
#include "disconodehandler.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  class ClientBase;
  class DiscoHandler;
  class DiscoItem;
  class Stanza;

  /**
   * This class implements JEP-0030 (Service Discovery) and JEP-0092 (Software Version).
   * Please see @ref Adhoc for an implementation that uses this class.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class Disco : public IqHandler
  {
    public:
      /**
       * Constructor.
       * Creates a new Disco client that registers as IqHandler with @c Client.
       * You should access the Disco object through the @c Client object.
       * @param parent The ClientBase used for XMPP communication
       */
      Disco( ClientBase *parent );

      /**
       * Virtual destructor.
       */
      virtual ~Disco();

      /**
       * Adds a feature to the list of supported Jabber features.
       * The list will be posted as an answer to IQ queries in the
       * "http://jabber.org/protocol/disco#info" namespace.
       * These IQ packets will also be forwarded to the
       * application's IqHandler, if it listens to the disco\#info namespace.
       * You can call Client::disableDisco() to disable automatic disco
       * entirely. By default, disco(very) queries are handled by the library.
       * By default, all supported, not disabled features are announced.
       * @param feature A feature (namespace) the host app supports.
       * @note Use this function for non-queryable features. For nodes that shall
       * answer to disco\#info queries, use addNode().
       */
      void addFeature( const std::string& feature );

      /**
       * Queries the given JID for general infomation according to
       * JEP-0030 (Service Discovery).
       * To receive the results inherit from DiscoHandler and register with the Disco object.
       * @param to The destination-JID of the query.
       */
      void getDiscoInfo( const std::string& to );

      /**
       * Queries the given JID for its items according to
       * JEP-0030 (Service Discovery).
       * To receive the results inherit from DiscoHandler and register with the Disco object.
       * @param to The destination-JID of the query.
       */
      void getDiscoItems( const std::string& to );

      /**
       * Sets the version of the host application using this library.
       * The library takes care of jabber:iq:version requests. These
       * IQ packets will not be forwarded to the IqHandlers.
       * @param name The name to be returned to inquireing clients.
       * @param version The version to be returned to inquireing clients.
       */
      void setVersion( const std::string& name, const std::string& version );

      /**
       * Sets the identity of this entity.
       * The library uses this information to answer disco#info requests
       * with a correct identity.
       * JEP-0030 requires an entity to have at least one identity. See JEP-0030
       * for more information on categories and types.
       * @param category The entity category of this client. Default: client
       * @param type The type of this entity. Default: bot
       */
      void setIdentity( const std::string& category, const std::string& type );

      /**
       * Use this function to register an @ref DiscoHandler with the Disco
       * object. The DiscoHandler will receive the results of disco queries.
       * @param dh The DiscoHandler-derived object to register.
       */
      void registerDiscoHandler( DiscoHandler *dh );

      /**
       * Unregisters the given DiscoHandler.
       * @param dh The DiscoHandler to unregister.
       */
      void removeDiscoHandler( DiscoHandler *dh );

      /**
       * Use this function to register an @ref DiscoNodeHandler with the Disco
       * object. The DiscoNodeHandler will receive disco#items queries which are
       * directed to the corresponding node registered for the handler.
       * Only one handler per node is possible.
       * @param nh The NodeHandler-derived object to register.
       * @param node The node name to associate with this handler.
       */
      void registerNodeHandler( DiscoNodeHandler *nh, const std::string& node );

      /**
       * Removes the node handler.
       * @param node The node for which the handler shall be removed.
       */
      void removeNodeHandler( const std::string& node );

      // reimplemented from IqHandler.
      virtual bool handleIq( const Stanza& stanza );

      // reimplemented from IqHandler.
      virtual bool handleIqID( const Stanza& stanza, int context );

    private:
      enum IdType
      {
        GET_DISCO_INFO,
        GET_DISCO_ITEMS
      };

      ClientBase *m_parent;

      typedef std::list<DiscoHandler*>                  DiscoHandlerList;
      typedef std::map<std::string, DiscoNodeHandler*>  DiscoNodeHandlerMap;
      typedef std::list<DiscoItem*>                     ItemList;
      typedef std::map<std::string, std::string>        StringMap;

      DiscoHandlerList m_discoHandlers;
      DiscoNodeHandlerMap m_nodeHandlers;
      ItemList m_items;
      StringList m_features;
      StringMap  m_queryIDs;

      std::string m_versionName;
      std::string m_versionVersion;
      std::string m_identityCategory;
      std::string m_identityType;

  };

};

#endif // DISCO_H__

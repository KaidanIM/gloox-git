/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef DISCO_H__
#define DISCO_H__

#include "gloox.h"

#include "iqhandler.h"
#include "disconodehandler.h"
#include "jid.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  class ClientBase;
  class DiscoHandler;
  class IQ;

  /**
   * @brief This class implements XEP-0030 (Service Discovery) and XEP-0092 (Software Version).
   *
   * ClientBase will automatically instantiate a Disco object. It can be used to
   * announce special features of your client, or its version, or...
   *
   * XEP version: 2.2
   * @author Jakob Schroeter <js@camaya.net>
   */
  class GLOOX_API Disco : public IqHandler
  {
    friend class ClientBase;

    public:

      /**
       * @brief An abstraction of a Disco identity (Service Discovery, XEP-0030).
       *
       * @author Jakob Schroeter <js@camaya.net>
       * @since 1.0
       */
      class Identity
      {
        public:
          /**
           * Constructs a Disco Identity from a category, type and name.
           * @param category The identity's category.
           * @param type The identity's type.
           * @param name The identity's name.
           */
          Identity( const std::string& category,
                    const std::string& type,
                    const std::string& name )
            : m_category( category ), m_type( type ), m_name( name ) {}

          /**
           * Creates a Disco Identity from the given Tag.
           * @param tag A Tag representation of a disco identity.
           */
          Identity( const Tag* tag );

          /**
           * Destructor.
           */
          ~Identity() {}

          /**
           * Returns the identity's category.
           * @return The identity's category.
           */
          const std::string& category() const { return m_category; }

          /**
           * Returns the identity's type.
           * @return The identity's type.
           */
          const std::string& type() const { return m_type; }

          /**
           * Returns the identity's name.
           * @return The identity's name.
           */
          const std::string& name() const { return m_name; }

          /**
           * Creates and returns a Tag representation of this identity.
           * @return A Tag, or 0.
           */
          Tag* tag() const;

        private:
          std::string m_category;   /**< The identity's category. */
          std::string m_type;       /**< The identity's type. */
          std::string m_name;       /**< The identity's name. */

      };

      /**
       * A list of pointers to Identity objects. Used with Disco::Info.
       */
      typedef std::list<Identity*> IdentityList;

      /**
       * @brief An abstraction of a Disco Info element (from Service Discovery, XEP-0030)
       * as a StanzaExtension.
       *
       * @author Jakob Schroeter <js@camaya.net>
       * @since 1.0
       */
      class Info : public StanzaExtension
      {
        public:
          /**
           * Creates a empty Info object, suitable for making disco#info requests.
           * @param node The node identifier to query (optional).
           */
          Info( const std::string& node = EmptyString );

          /**
           * Creates an Info object from the given Tag.
           * @param tag A &lt;query&gt; tag in the disco#info namespace, (possibly) containing
           * a disco#info reply.
           */
          Info( const Tag* tag );

          /**
           * Virtual destructor.
           */
          virtual ~Info();

          /**
           * Returns the queried node identifier, if any.
           * @return The node identifier. May be empty.
           */
          const std::string& node() const { return m_node; }

          /**
           * This function can be used to set the entity's features.
           * @param features A list of supported features/namespaces.
           */
          void setFeatures( const StringList& features ) { m_features = features; }

          /**
           * Returns the entity's supported features.
           * @return A list of supported features/namespaces.
           */
          const StringList& features() const { return m_features; }

          /**
           * This function can be used to set the entity's identities.
           * @param identities A list of pointers to the entity's identities.
           * @note The Identity objects pointed to will be owned by the Info object. The
           * list should neither be used again nor should the Identity objects be deleted.
           */
          void setIdentities( const IdentityList& identities ) { m_identities = identities; }

          /**
           * Returns the entity's identities.
           * @return A list of pointers to Identity objects.
           */
          const IdentityList& identities() const { return m_identities; }

          // reimplemented from StanzaExtension
          virtual const std::string& filterString() const;

          // reimplemented from StanzaExtension
          virtual StanzaExtension* newInstance( const Tag* tag ) const
          {
            return new Info( tag );
          }

          // reimplemented from StanzaExtension
          virtual Tag* tag() const;

        private:
          std::string m_node;
          StringList m_features;
          IdentityList m_identities;
      };

      /**
       * @brief An abstraction of a Disco item (Service Discovery, XEP-0030).
       *
       * @author Jakob Schroeter <js@camaya.net>
       * @since 1.0
       */
      class Item
      {
        public:
          /**
           * Constructs a Disco Item from a JID, node and name.
           * @param jid The item's JID.
           * @param node The item's type.
           * @param name The item's name.
           */
          Item( const JID& jid,
                const std::string& node,
                const std::string& name )
          : m_jid( jid ), m_node( node ), m_name( name ) {}

          /**
           * Creates a Disco Item from the given Tag.
           * @param tag A Tag representation of a Disco item.
           */
          Item( const Tag* tag );

          /**
           * Destructor.
           */
          ~Item() {}

          /**
           * Returns the item's category.
           * @return The item's category.
           */
          const JID& jid() const { return m_jid; }

          /**
           * Returns the item's node.
           * @return The item's node.
           */
          const std::string& node() const { return m_node; }

          /**
           * Returns the identity's name.
           * @return The identity's name.
           */
          const std::string& name() const { return m_name; }

          /**
           * Creates and returns a Tag representation of this item.
           * @return A Tag, or 0.
           */
          Tag* tag() const;

        private:
          JID m_jid;                /**< The item's jid. */
          std::string m_node;       /**< The item's type. */
          std::string m_name;       /**< The item's name. */

      };

      /**
       * A list of pointers to Item objects. Used with Disco::Items.
       */
      typedef std::list<Item*> ItemList;

      /**
       * @brief An abstraction of a Disco query element (from Service Discovery, XEP-0030)
       * in the disco#items namespace, implemented as a StanzaExtension.
       *
       * @author Jakob Schroeter <js@camaya.net>
       * @since 1.0
       */
      class Items : public StanzaExtension
      {
        public:
          /**
           * Creates a empty Items object, suitable for making disco#info requests.
           * @param node The node identifier to query (optional).
           */
          Items( const std::string& node = EmptyString );

          /**
           * Creates an Items object from the given Tag.
           * @param tag A &lt;query&gt; tag in the disco#items namespace, (possibly) containing
           * a disco#items reply.
           */
          Items( const Tag* tag );

          /**
           * Virtual destructor.
           */
          virtual ~Items();

          /**
           * Returns the queried node identifier, if any.
           * @return The node identifier. May be empty.
           */
          const std::string& node() const { return m_node; }

          /**
           * This function can be used to set the entity's/node's items.
           * @param items A list of pointers to the entity's/node's items.
           * @note The Item objects pointed to will be owned by the Items object. The
           * list should neither be used again nor should the Item objects be deleted.
           */
          void setItems( const ItemList& items ) { m_items = items; }

          /**
           * Returns the entity's/node's items.
           * @return A list of pointers to Item objects.
           */
          const ItemList& items() const { return m_items; }

          // reimplemented from StanzaExtension
          virtual const std::string& filterString() const;

          // reimplemented from StanzaExtension
          virtual StanzaExtension* newInstance( const Tag* tag ) const
          {
            return new Items( tag );
          }

          // reimplemented from StanzaExtension
          virtual Tag* tag() const;

        private:
          std::string m_node;
          ItemList m_items;
      };

      /**
       * Adds a feature to the list of supported Jabber features.
       * The list will be posted as an answer to IQ queries in the
       * "http://jabber.org/protocol/disco#info" namespace.
       * These IQ packets will also be forwarded to the
       * application's IqHandler, if it listens to the @c disco\#info namespace.
       * By default, disco(very) queries are handled by the library.
       * By default, all supported, not disabled features are announced.
       * @param feature A feature (namespace) the host app supports.
       * @note Use this function for non-queryable features. For nodes that shall
       * answer to @c disco\#info queries, use registerNodeHandler().
       */
      void addFeature( const std::string& feature )
        { m_features.push_back( feature ); }

      /**
       * Removes the given feature from the list of advertised client features.
       * @param feature The feature to remove.
       * @since 0.9
       */
      void removeFeature( const std::string& feature )
        { m_features.remove( feature ); }

      /**
       * Lets you retrieve the features this Disco instance supports.
       * @return A list of disco items.
       */
      const StringList& features() const { return m_features; }

      /**
       * Queries the given JID for general infomation according to
       * XEP-0030 (Service Discovery).
       * To receive the results inherit from DiscoHandler and register with the Disco object.
       * @param to The destination-JID of the query.
       * @param node An optional node to query. Not inserted if empty.
       * @param dh The DiscoHandler to notify about results.
       * @param context A context identifier.
       * @param tid An optional id that is going to be used as the IQ request's id. Only
       * necessary if you need to know the request's id.
       */
      void getDiscoInfo( const JID& to, const std::string& node, DiscoHandler* dh, int context,
                         const std::string& tid = EmptyString )
        { getDisco( to, node, dh, context, GET_DISCO_INFO, tid ); }

      /**
       * Queries the given JID for its items according to
       * XEP-0030 (Service Discovery).
       * To receive the results inherit from DiscoHandler and register with the Disco object.
       * @param to The destination-JID of the query.
       * @param node An optional node to query. Not inserted if empty.
       * @param dh The DiscoHandler to notify about results.
       * @param context A context identifier.
       * @param tid An optional id that is going to be used as the IQ request's id. Only
       * necessary if you need to know the request's id.
       */
      void getDiscoItems( const JID& to, const std::string& node, DiscoHandler* dh, int context,
                          const std::string& tid = EmptyString )
        { getDisco( to, node, dh, context, GET_DISCO_ITEMS, tid ); }

      /**
       * Sets the version of the host application using this library.
       * The library takes care of jabber:iq:version requests. These
       * IQ packets will not be forwarded to the IqHandlers.
       * @param name The name to be returned to inquireing clients.
       * @param version The version to be returned to inquireing clients.
       * @param os The operating system to announce. Default: don't include.
       */
      void setVersion( const std::string& name, const std::string& version,
                       const std::string& os = EmptyString );

      /**
       * Sets the identity of this entity.
       * The library uses this information to answer disco#info requests
       * with a correct identity.
       * XEP-0030 requires an entity to have at least one identity. See XEP-0030
       * for more information on categories and types.
       * @param category The entity category of this client. Default: client
       * @param type The type of this entity. Default: bot
       */
      void setIdentity( const std::string& category, const std::string& type );

      /**
       * Returns the set identity category, e.g. 'client'.
       * @return The identity category.
       */
      const std::string& category() const { return m_identityCategory; }

      /**
       * Returns the set identity type, e.g. 'bot'.
       * @return The identity type.
       */
      const std::string& type() const { return m_identityType; }

      /**
       * Use this function to register an @ref DiscoHandler with the Disco
       * object. This is only necessary if you want to receive Disco-set requests. Else
       * a one-time registration happens when calling getDiscoInfo() and getDiscoItems(), respectively.
       * @param dh The DiscoHandler-derived object to register.
       */
      void registerDiscoHandler( DiscoHandler* dh )
        { m_discoHandlers.push_back( dh ); }

      /**
       * Unregisters the given DiscoHandler.
       * @param dh The DiscoHandler to unregister.
       */
      void removeDiscoHandler( DiscoHandler* dh )
        { m_discoHandlers.remove( dh ); }

      /**
       * Use this function to register a @ref DiscoNodeHandler with the Disco
       * object. The DiscoNodeHandler will receive disco#items queries which are
       * directed to the corresponding node registered for the handler.
       * @param nh The NodeHandler-derived object to register.
       * @param node The node name to associate with this handler. Use an empty string to
       * register for the root node.
       */
      void registerNodeHandler( DiscoNodeHandler* nh, const std::string& node );

      /**
       * Removes the node handler for the given node.
       * @param nh The NodeHandler to unregister.
       * @param node The node for which the handler shall be removed. Use an empty string to
       * remove the root node's handler.
       */
      void removeNodeHandler( DiscoNodeHandler* nh, const std::string& node );

      // reimplemented from IqHandler.
      virtual bool handleIq( IQ* iq );

      // reimplemented from IqHandler.
      virtual void handleIqID( IQ* iq, int context );

    private:
      Disco( ClientBase* parent );
      virtual ~Disco();

      enum IdType
      {
        GET_DISCO_INFO,
        GET_DISCO_ITEMS
      };

      void getDisco( const JID& to, const std::string& node, DiscoHandler* dh,
                     int context, IdType idType, const std::string& tid );

      struct DiscoHandlerContext
      {
        DiscoHandler* dh;
        int context;
      };

      ClientBase* m_parent;

      typedef std::list<DiscoHandler*> DiscoHandlerList;
      typedef std::list<DiscoNodeHandler*> DiscoNodeHandlerList;
      typedef std::map<std::string, DiscoNodeHandlerList> DiscoNodeHandlerMap;
      typedef std::map<std::string, DiscoHandlerContext> DiscoHandlerMap;
//       typedef std::list<DiscoItem*> ItemList;

      DiscoHandlerList m_discoHandlers;
      DiscoNodeHandlerMap m_nodeHandlers;
      DiscoHandlerMap m_track;
//       ItemList m_items;
      StringList m_features;
      StringMap  m_queryIDs;

      std::string m_versionName;
      std::string m_versionVersion;
      std::string m_versionOs;
      std::string m_identityCategory;
      std::string m_identityType;

  };

}

#endif // DISCO_H__

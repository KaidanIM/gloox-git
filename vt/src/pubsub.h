#ifndef PUBSUB_H__
#define PUBSUB_H__

#include <map>
#include <string>
#include "iqhandler.h"

namespace gloox
{
  /**
   * Describes the different subscription types.
   */
  enum SubscriptionType {
    SubscriptionInvalid,      /**< Invalid subscription type. */
    SubscriptionNone,         /**< The node MUST NOT send event notifications or payloads to the Entity. */
    SubscriptionPending,      /**< An entity has requested to subscribe to a node and the request has not yet
                               *   been approved by a node owner. The node MUST NOT send event notifications
                               *   or payloads to the entity while it is in this state. */
    SubscriptionUnconfigured, /**< An entity has subscribed but its subscription options have not yet been
                               *   configured. The node MAY send event notifications or payloads to the entity
                               *   while it is in this state. The service MAY timeout unconfigured subscriptions. */
    SubscriptionSubscribed    /**< An entity is subscribed to a node. The node MUST send all event notifications
                               *   (and, if configured, payloads) to the entity while it is in this state. */
  };

  /**
   * Describes the different access types.
   */
  enum AccessType {
    AccessOpen,      /**< Any entity may subscribe to the node (i.e., without the necessity for subscription
                      *   approval) and any entity may retrieve items from the node (i.e., without being
                      *   subscribed); this SHOULD be the default access model for generic pubsub services. */
    AccessPresence,  /**< Any entity with a subscription of type "from" or "both" may subscribe to the node
                      *   and retrieve items from the node; this access model applies mainly to instant
                      *   messaging systems (see RFC 3921). */
    AccessRoster,    /**< Any entity in the specified roster group(s) may subscribe to the node and retrieve
                      *   items from the node; this access model applies mainly to instant messaging systems
                      *   (see RFC 3921). */
    AccessAuthorize, /**< The node owner must approve all subscription requests, and only subscribers may
                      *   retrieve items from the node. */
    AccessWhitelist  /**< An entity may be subscribed only through being added to a whitelist by the node
                      *   owner (unsolicited subscription requests are rejected), and only subscribers may
                      *   retrieve items from the node. In effect, the default affiliation is outcast. The
                      *   node owner MUST automatically be on the whitelist. In order to add entities to the
                      *   whitelist, the node owner SHOULD use the protocol specified in the Manage Affiliated
                      *   Entities section of this document. */
  };

  /**
   * Describes the different possible errors when subscribing to a node.
   */
  enum SubscriptionError {
    SubscriptionErrorNone,            /**< No error */
    SubscriptionErrorJIDMismatch,     /**< The bare JID portions of the JIDs do not match. */
    SubscriptionErrorAccessPresence,  /**< The node has an access model of "presence" and the requesting
                                       *   entity is not subscribed to the owner's presence. */
    SubscriptionErrorAccessRoster,    /**< The node has an access model of "roster" and the requesting
                                       *   entity is not in one of the authorized roster groups. */
    SubscriptionErrorAccessWhiteList, /**< The node has an access model of "whitelist" and the requesting
                                       *   entity is not on the whitelist. */
    SubscriptionErrorPayment,         /**< The service requires payment for subscriptions to the node. */
    SubscriptionErrorAnonymous,       /**< The requesting entity is anonymous and the service does not
                                       *   allow anonymous entities to subscribe. */
    SubscriptionErrorPending,         /**< The requesting entity has a pending subscription. */
    SubscriptionErrorBlocked,         /**< The requesting entity is blocked from subscribing
                                       *   (e.g., because having an affiliation of outcast). */
    SubscriptionErrorUnsupported,     /**< The node does not support subscriptions. */
    SubscriptionErrorItemNotFound     /**< The node does not exist. */
  };

  /**
   * Describes the different possible errors when unsubscribing from a node.
   */
  enum UnsubscriptionError {
    UnsubscriptionErrorNone,          /**< No error */
    UnsubscriptionErrorMissingSID,    /**< The requesting entity has multiple subscriptions to the node
                                       *   but does not specify a subscription ID. */
    UnsubscriptionErrorNotSubscriber, /**< The request does not specify an existing subscriber. */
    UnsubscriptionErrorUnprivileged,  /**< The requesting entity does not have sufficient privileges to
                                       *   unsubscribe the specified JID. */
    UnsubscriptionErrorItemNotFound,    /**< The node does not exist. */
    UnsubscriptionErrorInvalidSID     /**< The request specifies a subscription ID that is not valid or current. */
  };

  /**
   * Describes the different possible errors related to subscription options.
   */
  enum OptionRequestError {
    OptionRequestErrorNone,       /**< No error */
    OptionRequestUnprivileged,    /**< The requesting entity does not have sufficient privileges to
                                   *   modify subscription options for the specified JID. */
    OptionRequestUnsubscribed,    /**< The requesting entity (or specified subscriber) is not subscribed. */
    OptionRequestNodeAndJID,      /**< The request does not specify both the NodeID and the subscriber's JID. */
    OptionRequestMissingSID,      /**< The request does not specify a subscription ID but one is required. */
    OptionRequestInvalidSID,      /**< The request specifies a subscription ID that is not valid or current. */
    OptionRequestUnsupported,     /**< Subscription options are not supported. */
    OptionRequestItemNotFound     /**< The node does not exist. */
  };

// [Persistent - Notification]
/**< Publisher MUST include an <item/> element, which MAY be empty or contain a payload; if item ID is not provided by publisher, it MUST be generated by pubsub service */

// [Persistent - Payload]
/**< Publisher MUST include an <item/> element that contains the payload; if item ID is not provided by publisher, it MUST be generated by pubsub service */

// [Transient - Notification]
/**< Publisher MUST NOT include an <item/> element (therefore item ID is neither provided nor generated) but the notification will include an empty <items/> element */

// [Transient - Payload]
/**< Publisher MUST include an <item/> element that contains the payload, but the item ID is OPTIONAL */


  class ClientBase;
  class DataForm;

  /**
   * Describes the different node types.
   */
  enum NodeType {
    NodeInvalid,     /**< Invalid node type */
    NodeLeaf,        /**< A node that contains published items only. It is NOT a container for other nodes. */
    NodeCollection   /**< A node that contains nodes and/or other collections but no published items.
                      *   Collections make it possible to represent hierarchial node structures. */
  };


  /**
   * \bug No 'from' field to iq (use m_parent->BareJID() ?),
   *      same for the jid field of the subscription tag in subscribe.
   * \bug Better use/configurability of JID's
   * \bug Tracking...
   * \bug HandleOptions is incomplete
   * \bug conflicting AffiliationType w/ MUCXXX
   */
  class PubSubManager : public IqHandler
  {
    public:

      /**
       * Describes the different affiliation types.
       */
      enum AffiliationType {
        AffiliationInvalid,   /**< Invalid Affiliation type. */
        AffiliationOutcast,   /**< Entity is disallowed from subscribing or publishing. */
        AffiliationNone,      /**<  */
        AffiliationPublisher, /**<  */
        AffiliationOwner      /**<  */
      };

      typedef std::map< std::string, SubscriptionType > SubscriptionMap;
      typedef std::map< std::string, AffiliationType  > AffiliationMap;

      class Node;
      class Item;

      typedef std::list< Node * > NodeList;
      typedef std::list< Item * > ItemList;

      typedef std::map< JID, NodeList > ServiceMap;
      typedef std::map< std::string, AffiliationType > NodeMap;

      struct Item
      {
        public:
          Item( const std::string& _id )
            : id( _id )
          {}
          std::string id;
          
          
      };

      struct Node
      {
        public:
          /**
           * Constructs a Node from a type, a JID (XEP-0060 Sect 4.6.1) and a name.
           */
          Node( NodeType _type, const std::string& _jid, const std::string& _name )
            : type( _type ), jid( _jid ), name( _name) {}
          /**
           * Constructs a Node from a type, a JID+NodeID (XEP-0060 Sect 4.6.2) and a name.
           */
          Node( NodeType _type, const std::string& _jid,
                                const std::string& _node,
                                const std::string& _name )
            : type( _type ), jid( _jid ), name( _name) { jid.setResource( _node ); }
          NodeType type;
          JID jid;
          std::string name;
          /*
          union {
            ItemList items;
            NodeList nodes;
          };
          */
      };

      struct LeafNode : public Node
      {
        LeafNode( const std::string& _service, const std::string& _name )
          : Node( NodeLeaf, _service, _name )
        {}
        ItemList m_itemList;
      };

      struct CollectionNode : public Node
      {
        CollectionNode( const std::string& _service, const std::string& _name )
          : Node( NodeCollection, _service, _name )
        {}
        NodeList m_nodeList;
      };

      /**
       * Initialize the manager.
       */
      PubSubManager( ClientBase* parent ) : m_parent(parent) {}
      /**
       * Virtual Destructor.
       */
      virtual ~PubSubManager() {}

      /**
       * Requests the subscription list from a service.
       * @param jid Service to query.
       */
      void requestSubscriptionList( const std::string& jid );
      /**
       * Receives the Subscription map for a specific service.
       * @param jid The service 
       * @param subMap The map of subscriptions.
       */
      virtual void handleSubscriptionListResult( const JID& jid, const SubscriptionMap& subMap ) = 0;
      virtual void handleSubscriptionListError( const std::string& jid, const std::string& node ) = 0;

      /**
       * Requests the affiliation list from a service.
       * @param jid Service to query.
       */
      void requestAffiliationList( const std::string& jid );
      /**
       * Receives the Subscription map for a specific service.
       * @param jid The service 
       * @param subMap The map of subscriptions.
       */
      virtual void handleAffiliationListResult( const JID& jid, const AffiliationMap& subList ) = 0;
      virtual void handleAffiliationListError( const std::string& jid, const std::string& node ) = 0;

      /**
       * 
       */
      void subscribe( const std::string& jid, const std::string& node );
      /**
       * 
       */
      void unsubscribe( const std::string& jid, const std::string& node );

      /**
       * 
       */
      virtual void handleSubscriptionResult( const JID& jid,
                                             const std::string& node,
                                             const std::string& sid,
                                             const SubscriptionType subType,
                                             const SubscriptionError se ) = 0;
      /**
       * 
       */
      virtual void handleUnsubscriptionResult( const JID& jid,
                                               const JID& service,
                                               const UnsubscriptionError se ) = 0;

      /**
       * 
       */
      void requestOptions( const std::string& jid, const std::string& node );
      /**
       * 
       */
      virtual void handleOptions( const JID& jid,
                                  const std::string& node,
                                  const DataForm& dataForm,
                                  const OptionRequestError e ) = 0;

      bool handleIq (Stanza *stanza);

      bool handleIqID (Stanza *stanza, int context);

      

      NodeMap m_nodeMap;
      ServiceMap m_service_list;

    private:
      ClientBase* m_parent;
  };

}

#endif /* PUBSUB_H__ */

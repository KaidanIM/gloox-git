/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUB_H__
#define PUBSUB_H__

#include <map>
#include <string>

namespace gloox
{
  /**
   * @brief to avoid clashing with MUCAffiliationType's
   *
   * \todo Be consistent with naming, either always use PubSubXXX, or PSXXX,
   *       or PubSubXXX for classes and PSXXX for enums.
   */
  namespace PubSub
  {

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
     * Describes the different affiliation types.
     */
    enum AffiliationType
    {
      AffiliationOutcast = 0,       /**< Entity is disallowed from subscribing or publishing. */
      AffiliationNone,          /**<  */
      AffiliationPublisher,     /**<  */
      AffiliationOwner,          /**<  */
      AffiliationInvalid        /**< Invalid Affiliation type. */
    };

    /**
     * Describes the different subscription types.
     */
    enum SubscriptionType
    {
      SubscriptionNone = 0,     /**< The node MUST NOT send event notifications or payloads to the Entity. */
      SubscriptionPending,      /**< An entity has requested to subscribe to a node and the request has not yet
                                 *   been approved by a node owner. The node MUST NOT send event notifications
                                 *   or payloads to the entity while it is in this state. */
      SubscriptionUnconfigured, /**< An entity has subscribed but its subscription options have not yet been
                                 *   configured. The node MAY send event notifications or payloads to the entity
                                 *   while it is in this state. The service MAY timeout unconfigured subscriptions. */
      SubscriptionSubscribed,   /**< An entity is subscribed to a node. The node MUST send all event notifications
                                 *   (and, if configured, payloads) to the entity while it is in this state. */
      SubscriptionInvalid       /**< Invalid subscription type. */
    };

    /**
     * Describes the different subscription types.
     */
    enum SubscriptionObject
    {
      SubscriptionNodes,      /**< Invalid subscription type. */
      SubscriptionItems       /**< The node MUST NOT send event notifications or payloads to the Entity. */
    };

    /**
     * Describes the possible errors when subscribing to a node.
     */
    enum SubscriptionError
    {
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
     * Describes the possible errors when unsubscribing from a node.
     */
    enum UnsubscriptionError
    {
      UnsubscriptionErrorNone,          /**< No error */
      UnsubscriptionErrorMissingSID,    /**< The requesting entity has multiple subscriptions to the node
                                         *   but does not specify a subscription ID. */
      UnsubscriptionErrorNotSubscriber, /**< The request does not specify an existing subscriber. */
      UnsubscriptionErrorUnprivileged,  /**< The requesting entity does not have sufficient privileges to
                                         *   unsubscribe the specified JID. */
      UnsubscriptionErrorItemNotFound,  /**< The node does not exist. */
      UnsubscriptionErrorInvalidSID     /**< The request specifies a subscription ID that is not valid or current. */
    };

    /**
     * Describes the access types.
     */
    enum AccessModel
    {
      AccessOpen = 0,  /**< Any entity may subscribe to the node (i.e., without the necessity for subscription
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
      AccessWhitelist, /**< An entity may be subscribed only through being added to a whitelist by the node
                        *   owner (unsolicited subscription requests are rejected), and only subscribers may
                        *   retrieve items from the node. In effect, the default affiliation is outcast. The
                        *   node owner MUST automatically be on the whitelist. In order to add entities to the
                        *   whitelist, the node owner SHOULD use the protocol specified in the Manage Affiliated
                        *   Entities section of this document. */
      AccessDefault    /**< Unspecified (default) Access Model (does not represent a real access type by itself). */
    };

    /**
     * Describes the possible errors related to subscription options.
     */
    enum OptionRequestError
    {
      OptionRequestErrorNone,       /**< No error */
      OptionRequestUnprivileged,    /**< The requesting entity does not have sufficient privileges to
                                     *   modify subscription options for the specified JID. */
      OptionRequestUnsubscribed,    /**< The requesting entity (or specified subscriber) is not subscribed. */
      OptionRequestMissingJID,      /**< The request does not specify both the NodeID and the subscriber's JID. */
      OptionRequestMissingSID,      /**< The request does not specify a subscription ID but one is required. */
      OptionRequestInvalidSID,      /**< The request specifies a subscription ID that is not valid or current. */
      OptionRequestUnsupported,     /**< Subscription options are not supported. */
      OptionRequestItemNotFound     /**< The node does not exist. */
    };

    /**
     * Describes the possible errors related to item retrieval.
     */
    enum ItemPublicationError
    {
      ItemPublicationErrorNone,     /**< No error */
      ItemPublicationUnprivileged,  /**< The requesting entity does not have sufficient
                                     *   privileges to publish. */
      ItemPublicationUnsupported,   /**< The node does not support item publication. */
      ItemPublicationNodeNotFound,  /**< The node does not exist. */
      ItemPublicationPayloadSize,   /**< The payload size exceeds a service-defined limit. */
      ItemPublicationPayload,       /**< The item contains more than one payload element or the
                                     *   namespace of the root payload element does not match
                                     *   the configured namespace for the node. */
      ItemPublicationConfiguration  /**< The request does not match the node configuration. */
    };

    /**
     * Describes the possible errors related to item deletation.
     */
    enum ItemDeletationError
    {
      ItemDeletationErrorNone,     /**< No error */
      ItemDeletationUnpriviledged, /**< The publisher does not have sufficient privileges to delete the
                                    *   requested item. */
      ItemDeletationItemNotFound,  /**< The node or item does not exist. */
      ItemDeletationMissingNode,   /**< The request does not specify a node. */
      ItemDeletationMissingItem,   /**< The request does not include an <item/> element or the <item/> element
                                    *   does not specify an ItemID. */
      ItemDeletationNoPersistent,  /**< The node does not support persistent items. */
      ItemDeletationUnsupported    /**< The service does not support the deletion of items. */
    };

    /**
     * Describes the possible errors related to item retrieval.
     */
    enum ItemRetrivalError
    {
      ItemRequestErrorNone,       /**< No error */
      ItemRequestMissingSID,      /**< The requesting entity has multiple subscriptions to the node but does not
                                   *   specify a subscription ID. */
      ItemRequestInvalidSID,      /**< The requesting entity is subscribed but specifies an invalid subscription ID. */
      ItemRequestNotSubscribed,   /**< The node does not return items to unsubscribed entities and the requesting
                                   *   entity is not subscribed. */
      ItemRequestNoPersistent,    /**< The service or node does not support persistent items and does not return
                                   *   the last published item. */
      ItemRequestUnsupported,     /**< The service or node does not support item retrieval. */
      ItemRequestAccessPresence,  /**< The node has an access model of "presence" and the requesting entity
                                   *   is not subscribed to the owner's presence. */
      ItemRequestAccessRoster,    /**< The node has an access model of "roster" and the requesting entity
                                   *   is not in one of the authorized roster groups. */
      ItemRequestAccessWhiteList, /**< The node has an access model of "whitelist" and the requesting entity
                                   *   is not on the whitelist. */
      ItemRequestPayment,         /**< The service or node requires payment for item retrieval. */
      ItemRequestBlocked,         /**< The requesting entity is blocked from retrieving items from the node
                                   *   (e.g., because having an affiliation of outcast). */
      ItemRetrievalItemNotFound   /**< The node does not exist. */
    };

    /**
     * Describes the possible errors when purging a node.
     * @note The last 3 errors are specific to collection nodes.
     */
    enum NodeCreationError
    {
      NodeCreationErrorNone,      /**< No error. */
      NodeCreationUnsupported,    /**< The service does not support node creation. */
      NodeCreationUnregistered,   /**< Only entities that are registered with the service are
                                   *   allowed to create nodes but the requesting entity is
                                   *   not registered. */
      NodeCreationUnpriviledged,  /**< The requesting entity does not have sufficient privileges
                                   *   to create nodes. */
      NodeCreationExistingNodeID, /**< The requested NodeID already exists. */
      NodeCreationMissingNodeID,  /**< The request did not include a NodeID and "instant nodes"
                                   *   are not supported. */
      NodeCreationColUnsupported,         /**< The service does not support collection nodes. */
      NodeCreationColCreationUnsupported, /**< The service does not support creation of
                                           *   collection nodes. */
      NodeCreationColUnpriviledged        /**< The requesting entity does not have sufficient
                                           *   privileges to create collection nodes. */
    };

    /**
     * Describes the possible errors when purging a node.
     */
    enum NodeRemovalError
    {
      NodeRemovalErrorNone,     /**< No error. */
      NodeRemovalUnpriviledged, /**< The requesting entity does not have sufficient privileges
                                 *   to delete the node. */
      NodeRemovalRoot,          /**< The node is the root collection node, which cannot be
                                 *   deleted. */
      NodeRemovalItemNotFound   /**< The specified node does not exist. */
    };

    /**
     * Describes the possible errors when purging a node.
     */
    enum NodeConfigurationError
    {
    };

    /**
     * Describes the possible errors when purging a node.
     */
    enum NodeConfigurationRequestError
    {
    };

    /**
     * Describes the possible errors when associating a node with a Collection node.
     */
    enum NodeAssociationError
    {
    };

    /**
     * Describes the possible errors when disassociating a node with a Collection node.
     */
    enum NodeDisassociationError
    {
    };



    enum SubscriptionModificationError
    {
      SubscriptionModificationErrorNone,   /**< No error. */
      SubscriptionModificationUnsupported,
      SubscriptionModification,
      SubscriptionModificationItemNotFound
    };

    enum AffiliationModificationError
    {
      AffiliationModificationErrorNone,     /**< No error. */
      AffiliationModificationUnsupported,   /**< The service does not support modification of affiliations. */
      AffiliationModificationUnpriviledged, /**< The requesting entity does not have sufficient privileges
                                             *   to modify affiliations. */
      AffiliationModificationItemNotFound   /**< The specified node does not exist. */
    };

    /**
     * Describes the different possible errors when purging a node.
     */
    enum ItemPurgeError
    {
      ItemPurgeUnsupported,   /**< The node or service does not support node purging. */
      ItemPurgeUnpriviledged, /**< The requesting entity does not have sufficient privileges to purge the node. */
      ItemPurgePersist,       /**< The node is not configured to persist items. */
      ItemPurgeItemNotFound   /**< The specified node does not exist. */
    };

    /**
     * Describes the different PubSub features (XEP-0060 Sect. 10).
     */
    enum PubSubFeature
    {
      FeatureUnknown               = 0,     /**< Unrecognized feature */
      FeatureCollections           = 1,     /**< Collection nodes are supported. RECOMMENDED */
      FeatureConfigNode            = 1<<1,  /**< Configuration of node options is supported. RECOMMENDED */
      FeatureCreateAndConfig       = 1<<2,  /**< Simultaneous creation and configuration of nodes is supported. RECOMMENDED */
      FeatureCreateNodes           = 1<<3,  /**< Creation of nodes is supported. RECOMMENDED */
      FeatureDeleteAny             = 1<<4,  /**< Any publisher may delete an item (not only the originating publisher). OPTIONAL */
      FeatureDeleteNodes           = 1<<5,  /**< Deletion of nodes is supported. RECOMMENDED */
      FeatureGetPending            = 1<<6,  /**< Retrieval of pending subscription approvals is supported. OPTIONAL */
      FeatureInstantNodes          = 1<<7,  /**< Creation of instant nodes is supported. RECOMMENDED */
      FeatureItemIDs               = 1<<8,  /**< Publishers may specify item identifiers. RECOMMENDED */
      FeatureLeasedSubscription    = 1<<9,  /**< Time-based subscriptions are supported. OPTIONAL */
      FeatureManageSubscriptions   = 1<<10, /**< Node owners may manage subscriptions. OPTIONAL */
      FeatureMetaData              = 1<<11, /**< Node meta-data is supported. RECOMMENDED */
      FeatureModifyAffiliations    = 1<<12, /**< Node owners may modify affiliations. OPTIONAL */
      FeatureMultiCollection       = 1<<13, /**< A single leaf node may be associated with multiple collections. OPTIONAL */
      FeatureMultiSubscribe        = 1<<14, /**< A single entity may subscribe to a node multiple times. OPTIONAL */
      FeaturePutcastAffiliation    = 1<<15, /**< The outcast affiliation is supported. RECOMMENDED */
      FeaturePersistentItems       = 1<<16, /**< Persistent items are supported. RECOMMENDED */
      FeaturePresenceNotifications = 1<<17, /**< Presence-based delivery of event notifications is supported. OPTIONAL */
      FeaturePublish               = 1<<18, /**< Publishing items is supported (note: not valid for collection nodes). REQUIRED */
      FeaturePublisherAffiliation  = 1<<19, /**< The publisher affiliation is supported. OPTIONAL */
      FeaturePurgeNodes            = 1<<20, /**< Purging of nodes is supported. OPTIONAL */
      FeatureRetractItems          = 1<<21, /**< Item retraction is supported. OPTIONAL */
      FeatureRetrieveAffiliations  = 1<<22, /**< Retrieval of current affiliations is supported. RECOMMENDED */
      FeatureRetrieveDefault       = 1<<23, /**< Retrieval of default node configuration is supported. RECOMMENDED */
      FeatureRetrieveItems         = 1<<24, /**< Item retrieval is supported. RECOMMENDED */
      FeatureRetrieveSubscriptions = 1<<25, /**< Retrieval of current subscriptions is supported. RECOMMENDED */
      FeatureSubscribe             = 1<<26, /**< Subscribing and unsubscribing are supported. REQUIRED */
      FeatureSubscriptionOptions   = 1<<27, /**< Configuration of subscription options is supported. OPTIONAL */
      FeatureSubscriptionNotifs    = 1<<28, /**< Notification of subscription state changes is supported. */
      FeatureMetaOwner             = 1<<29,
      FeatureMetaEvent             = 1<<30
    };

// [Persistent - Notification]
/**< Publisher MUST include an <item/> element, which MAY be empty or contain a payload; if item ID is not provided by publisher, it MUST be generated by pubsub service */

// [Persistent - Payload]
/**< Publisher MUST include an <item/> element that contains the payload; if item ID is not provided by publisher, it MUST be generated by pubsub service */

// [Transient - Notification]
/**< Publisher MUST NOT include an <item/> element (therefore item ID is neither provided nor generated) but the notification will include an empty <items/> element */

// [Transient - Payload]
/**< Publisher MUST include an <item/> element that contains the payload, but the item ID is OPTIONAL */

    typedef std::map< std::string, SubscriptionType > SubscriptionMap;
    typedef std::map< std::string, AffiliationType  > AffiliationMap;

  }

}

#endif /* PUBSUB_H__ */

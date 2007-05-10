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
     * Describes the different affiliation types.
     */
    enum AffiliationType {
      AffiliationInvalid,   /**< Invalid Affiliation type. */
      AffiliationOutcast,   /**< Entity is disallowed from subscribing or publishing. */
      AffiliationNone,      /**<  */
      AffiliationPublisher, /**<  */
      AffiliationOwner      /**<  */
    };

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
      UnsubscriptionErrorItemNotFound,  /**< The node does not exist. */
      UnsubscriptionErrorInvalidSID     /**< The request specifies a subscription ID that is not valid or current. */
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
     * Describes the different possible errors related to subscription options.
     */
    enum OptionRequestError {
      OptionRequestErrorNone,       /**< No error */
      OptionRequestUnprivileged,    /**< The requesting entity does not have sufficient privileges to
                                     *   modify subscription options for the specified JID. */
      OptionRequestUnsubscribed,    /**< The requesting entity (or specified subscriber) is not subscribed. */
      OptionRequestMissingJID,     /**< The request does not specify both the NodeID and the subscriber's JID. */
      OptionRequestMissingSID,      /**< The request does not specify a subscription ID but one is required. */
      OptionRequestInvalidSID,      /**< The request specifies a subscription ID that is not valid or current. */
      OptionRequestUnsupported,     /**< Subscription options are not supported. */
      OptionRequestItemNotFound     /**< The node does not exist. */
    };

    /**
     * Describes the different possible errors related to item retrieval.
     */
    enum ItemRequestError
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
     * Describes the different PubSub features (XEP-0060 Sect. 10).
     */
    enum PubSubFeatures
    {
      FeatureCollections,           /**< Collection nodes are supported. RECOMMENDED */
      FeatureConfigNode,            /**< Configuration of node options is supported. RECOMMENDED */
      FeatureCreateAndConfig,       /**< Simultaneous creation and configuration of nodes is supported. RECOMMENDED */
      FeatureCreateNodes,           /**< Creation of nodes is supported. RECOMMENDED */
      FeatureDeleteAny,             /**< Any publisher may delete an item (not only the originating publisher). OPTIONAL */
      FeatureDeleteNodes,           /**< Deletion of nodes is supported. RECOMMENDED */
      FeatureGetPending,            /**< Retrieval of pending subscription approvals is supported. OPTIONAL */
      FeatureInstantNodes,          /**< Creation of instant nodes is supported. RECOMMENDED */
      FeatureItemIDs,               /**< Publishers may specify item identifiers. RECOMMENDED */
      FeatureLeasedSubscription,    /**< Time-based subscriptions are supported. OPTIONAL */
      FeatureManageSubscriptions,   /**< Node owners may manage subscriptions. OPTIONAL */
      FeatureMetaData,              /**< Node meta-data is supported. RECOMMENDED */
      FeatureModifyAffiliations,    /**< Node owners may modify affiliations. OPTIONAL */
      FeatureMultiCollection,       /**< A single leaf node may be associated with multiple collections. OPTIONAL */
      FeatureMultiSubscribe,        /**< A single entity may subscribe to a node multiple times. OPTIONAL */
      FeaturePutcastAffiliation,    /**< The outcast affiliation is supported. RECOMMENDED */
      FeaturePersistentItems,       /**< Persistent items are supported. RECOMMENDED */
      FeaturePresenceNotifications, /**< Presence-based delivery of event notifications is supported. OPTIONAL */
      FeaturePublish,               /**< Publishing items is supported (note: not valid for collection nodes). REQUIRED */
      FeaturePublisherAffiliation,  /**< The publisher affiliation is supported. OPTIONAL */
      FeaturePurgeNodes,            /**< Purging of nodes is supported. OPTIONAL */
      FeatureRetractItems,          /**< Item retraction is supported. OPTIONAL */
      FeatureRetrieveAffiliations,  /**< Retrieval of current affiliations is supported. RECOMMENDED */
      FeatureRetrieveDefault,       /**< Retrieval of default node configuration is supported. RECOMMENDED */
      FeatureRetrieveItems,         /**< Item retrieval is supported. RECOMMENDED */
      FeatureRetrieveSubscriptions, /**< Retrieval of current subscriptions is supported. RECOMMENDED */
      FeatureSubscribe,             /**< Subscribing and unsubscribing are supported. REQUIRED */
      FeatureSubscriptionOptions,   /**< Configuration of subscription options is supported. OPTIONAL */
      FeatureSubscriptionNotifs     /**< Notification of subscription state changes is supported. */
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

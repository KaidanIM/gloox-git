/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "pubsubmanager.h"
#include "clientbase.h"
#include "dataform.h"
#include "iq.h"
#include "pubsub.h"
#include "pubsubeventhandler.h"
#include "pubsubresulthandler.h"
#include "util.h"
#include "error.h"

#include <iostream>

namespace gloox
{

  namespace PubSub
  {

    static const std::string
      XMLNS_PUBSUB_NODE_CONFIG = "http://jabber.org/protocol/pubsub#node_config",
      XMLNS_PUBSUB_SUBSCRIBE_OPTIONS = "http://jabber.org/protocol/pubsub#subscribe_options";

    enum Context
    {
      Subscription,
      Unsubscription,
      GetSubscriptionOptions,
      SetSubscriptionOptions,
      GetSubscriptionList,
      GetSubscriberList,
      SetSubscriberList,
      GetAffiliationList,
      GetAffiliateList,
      SetAffiliateList,
      GetNodeConfig,
      SetNodeConfig,
      DefaultNodeConfig,
      GetItemList,
      PublishItem,
      DeleteItem,
      CreateNode,
      DeleteNode,
      PurgeNodeItems,
      NodeAssociation,
      NodeDisassociation,
      GetFeatureList,
      DiscoServiceInfos,
      DiscoNodeInfos,
      DiscoNodeItems
    };

    /**
     * Finds the associated PubSubFeature for a feature tag 'type' attribute,
     * as received from a disco info query on a pubsub service (XEP-0060 sect. 10).
     * @param feat Feature string to search for.
     * @return the associated PubSubFeature.
     */
/*    static PubSubFeature featureType( const std::string& str )
    {
      static const char* values [] = {
        "collections",
        "config-node",
        "create-and-configure",
        "create-nodes",
        "delete-any",
        "delete-nodes",
        "get-pending",
        "instant-nodes",
        "item-ids",
        "leased-subscription",
        "manage-subscriptions",
        "meta-data",
        "modify-affiliations",
        "multi-collection",
        "multi-subscribe",
        "outcast-affiliation",
        "persistent-items",
        "presence-notifications",
        "publish",
        "publisher-affiliation",
        "purge-nodes",
        "retract-items",
        "retrieve-affiliations",
        "retrieve-default",
        "retrieve-items",
        "retrieve-subscriptions",
        "subscribe",
        "subscription-options",
        "subscription-notifications",
        "owner",
        "event",
      };
      return static_cast< PubSubFeature >( util::lookup2( str, values ) );
    }
*/
    Manager::Manager( ClientBase* parent )
      : m_parent(parent)
    {}

    static const char* subscriptionValues[] = {
      "none", "subscribed", "pending", "unconfigured"
    };

    static inline SubscriptionType subscriptionType( const std::string& subscription )
    {
      return (SubscriptionType)util::lookup( subscription, subscriptionValues );
    }

    static const char* affiliationValues[] = {
      "none", "publisher", "owner", "outcast"
    };

    static inline AffiliationType affiliationType( const std::string& affiliation )
    {
      return (AffiliationType)util::lookup( affiliation, affiliationValues );
    }
/*
    static EventType eventType( const std::string& event )
    {
      static const char* values[] = {
        "collection",
        "configuration",
        "delete",
        "items",
        "purge",
        "subscription"
      };
      return (EventType)util::lookup( event, values );
    }
*/

/*
    void Manager::handleMessage( Message* msg, MessageSession* )
    {
      const Tag* event = msg->findChild( "event", XMLNS, XMLNS_PUBSUB_EVENT );
      if( !event || m_eventHandlerList.empty() )
        return;

      const JID& service = msg->from();
      const TagList& events = event->children();
      EventType type;
      EventHandlerList::iterator ith = m_eventHandlerList.begin();

      // in case an event may contain several different notifications
      TagList::const_iterator it = events.begin();
      for( ; it != events.end(); ++it )
      {
        type = eventType( (*it)->name() );
        for( ; ith != m_eventHandlerList.end(); ++it )
        {
          const std::string& node = (*it)->findAttribute( "node" );
          switch( type )
          {
            case EventCollection:
            {
              const Tag* x = (*it)->findChild( "x" );
              const DataForm* df = x ? new DataForm( x ) : 0;
              (*ith)->handleNodeCreation( service, node, df );
              delete df;
              break;
            }
            case EventConfigure:
            {
              const Tag* x = (*it)->findChild( "x" );
              const DataForm* df = x ? new DataForm( x ) : 0;
              (*ith)->handleConfigurationChange( service, node, df );
              delete df;
              break;
            }
            case EventDelete:
            {
              (*ith)->handleNodeRemoval( service, node );
              break;
            }
            case EventItems:
            {
              const Tag* items = (*it)->findChild( "items" );
              const Tag* item = items->findChild( "item" );
              // This is the collection node responsible for the notification,
              // in case of a subscription type of 'items'. Currently unused.
              const Tag* headers = item->findChild( "headers", "xmlns", "shim" );
              const std::string& id= item->findAttribute( "id" );
              (*ith)->handleItemPublication( service, node, id, item );
              break;
            }
            case EventPurge:
            {
              (*ith)->handleNodePurge( service, node );
              break;
            }
            case EventSubscription:
            {
              const std::string& jid  = (*it)->findAttribute( "jid" );
              const std::string& sub  = (*it)->findAttribute( "subscription" );
              const Tag* body = event->findChild( "body" );
              (*ith)->handleSubscriptionChange( service, jid, node,
                                                  body ? body->cdata() : EmptyString,
                                                  subscriptionType( sub ) );
              break;
            }
          }
        }
      }
    }
*/
    void Manager::subscriptionOptions( const JID& service,
                                       const JID& jid,
                                       const std::string& node,
                                       ResultHandler* handler,
                                       const DataForm* df )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* options = new Tag( iq.query(), "options", "node", node );
      options->addAttribute( "jid", jid ? jid.bare() : m_parent->jid().bare() );
      if( df )
      {
        options->addChild( df->tag() );
        delete df;
      }

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, df ? SetSubscriptionOptions : GetSubscriptionOptions );
    }

    void Manager::getSubscriptions( const JID& service, ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      new Tag( iq.query(), "subscriptions" );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, GetSubscriptionList );
    }

    void Manager::getAffiliations( const JID& service, ResultHandler* handler )
    {
      if( !m_parent || !handler  )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      new Tag( iq.query(), "affiliations" );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, GetAffiliationList );
    }

    void Manager::subscribe( const JID& service,
                             const std::string& node,
                             ResultHandler* handler,
                             const JID& jid,
                             SubscriptionObject type,
                             int depth )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* ps = iq.query();
      Tag* sub = new Tag( ps, "subscribe", "node", node );
      sub->addAttribute( "jid", jid ? jid.full() : m_parent->jid().full() );

      if( type != SubscriptionNodes || depth != 1 )
      {
        Tag* options = new Tag( ps, "options" );
        DataForm df( TypeSubmit );
        df.addField( DataFormField::TypeHidden, "FORM_TYPE", XMLNS_PUBSUB_SUBSCRIBE_OPTIONS );

        if( type == SubscriptionItems )
          df.addField( DataFormField::TypeNone, "pubsub#subscription_type", "items" );

        if( depth != 1 )
        {
          DataFormField* field = df.addField( DataFormField::TypeNone, "pubsub#subscription_depth" );
          if( depth == 0 )
            field->setValue( "all" );
          //else
          //  field->setValue( depth );
        }
        options->addChild( df.tag() );
      }

      m_resultHandlerTrackMap[id] = handler;
      m_nopTrackMap[id] = node;
      m_parent->send( iq, this, Subscription);
    }

    void Manager::unsubscribe( const JID& service,
                               const std::string& node,
                               ResultHandler* handler,
                               const JID& jid )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      const std::string& ujid = jid ? jid.full() : m_parent->jid().full();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* sub = new Tag( iq.query(), "unsubscribe", "node", node );
      sub->addAttribute( "jid", ujid );

      m_resultHandlerTrackMap[id] = handler;
      // need to track info for handler
      m_parent->send( iq, this, Unsubscription );
    }

    void Manager::publishItem( const JID& service,
                               const std::string& node,
                               Tag* item,
                               ResultHandler* handler )
    {
      if( !m_parent || !handler )
      {
        delete item;
        return;
      }

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* publish = new Tag( iq.query(), "publish", "node", node );
      publish->addChild( item );

      m_iopTrackMap[id] = std::make_pair( node, item->findAttribute( "id" ) );
      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, PublishItem );
    }

    void Manager::deleteItem( const JID& service,
                              const std::string& node,
                              const std::string& item,
                              ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* retract = new Tag( iq.query(), "retract", "node", node );
      new Tag( retract, "item", "id", item );

      m_iopTrackMap[id] = TrackedItem( node, item );
      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, DeleteItem );
    }

    void Manager::createNode( NodeType type,
                              const JID& service,
                              const std::string& node,
                              ResultHandler* handler,
                              const std::string& name,
                              const std::string& parent,
                              AccessModel access,
                              const StringMap* config )
    {
      static const char* accessValues[] = {
        "open",
        "presence",
        "roster",
        "authorize",
        "whitelist"
      };

      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* pubsub = iq.query();
      new Tag( pubsub, "create", "node", node );
      Tag* configure = new Tag( pubsub, "configure" );

      if( !parent.empty() || config || type == NodeCollection || access != AccessDefault )
      {
        DataForm df( TypeSubmit );
        df.addField( DataFormField::TypeHidden, "FORM_TYPE", XMLNS_PUBSUB_NODE_CONFIG );

        if( !parent.empty() )
          df.addField( DataFormField::TypeNone, "pubsub#collection", parent );

        if( !name.empty() )
          df.addField( DataFormField::TypeNone, "pubsub#title", name );

        if( type == NodeCollection )
          df.addField( DataFormField::TypeNone, "pubsub#node_type", "collection" );

        if( access != AccessDefault )
          df.addField( DataFormField::TypeNone, "pubsub#access_model",
                            util::lookup( access, accessValues ) );
        if( config )
        {
          StringMap::const_iterator it = config->begin();
          for( ; it != config->end(); ++it )
            df.addField( DataFormField::TypeNone, (*it).first, (*it).first );
          delete config;
        }
        configure->addChild( df.tag() );
      }

      m_nopTrackMap[id] = node;
      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, CreateNode );
    }

    void Manager::deleteNode( const JID& service,
                              const std::string& node,
                              ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      new Tag( iq.query(), "delete", "node", node );

      m_nopTrackMap[id] = node;
      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, DeleteNode );
    }

    void Manager::getDefaultNodeConfig( const JID& service,
                                        NodeType type,
                                        ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag* def = new Tag( iq.query(), "default" );
      if( type == NodeCollection )
      {
        DataForm df( TypeSubmit );
        df.addField( DataFormField::TypeHidden, "FORM_TYPE", XMLNS_PUBSUB_NODE_CONFIG );
        df.addField( DataFormField::TypeNone, "pubsub#node_type", "collection" );
        def->addChild( df.tag() );
      }

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, DefaultNodeConfig );
    }

    void Manager::nodeConfig( const JID& service,
                              const std::string& node,
                              const DataForm* config,
                              ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( config ? IQ::Set : IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag* sub = new Tag( iq.query(), "configure", "node", node );
      if( config )
      {
        sub->addChild( config->tag() );
        delete config;
      }

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, config ? SetNodeConfig : GetNodeConfig );
    }

    void Manager::subscriberList( const JID& service,
                                  const std::string& node,
                                  const SubscriberList* subList,
                                  ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( subList ? IQ::Set : IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag* sub = new Tag( iq.query(), "subscriptions", "node", node );
      if( subList )
      {
        Tag* s;
        SubscriberList::const_iterator it = subList->begin();
        for( ; it != subList->end(); ++it )
        {
          s = new Tag( sub, "subscription", "jid", (*it).jid.full() );
          s->addAttribute( "subscription", util::lookup( (*it).type, subscriptionValues ) );
          if( !(*it).subid.empty() )
            s->addAttribute( "subid", (*it).subid );
        }
        m_nopTrackMap[id] = node;
        delete subList;
      }

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, subList ? SetSubscriberList : GetSubscriberList );
    }

    void Manager::affiliateList( const JID& service,
                                 const std::string& node,
                                 const AffiliateList* affList,
                                 ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( affList ? IQ::Set : IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag* aff = new Tag( iq.query(), "affiliations", "node", node );
      if( affList )
      {
        Tag* a;
        AffiliateList::const_iterator it = affList->begin();
        for( ; it != affList->end(); ++it )
        {
          a = new Tag( aff, "affiliation", "jid", (*it).jid.full() );
          a->addAttribute( "affiliation", util::lookup( (*it).type, affiliationValues ) );
        }
        m_nopTrackMap[id] = node;
        delete affList;
      }

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, affList ? SetAffiliateList : GetAffiliateList );
    }

    void Manager::getItems( const JID& service,
                                const std::string& node,
                                ResultHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      new Tag( iq.query(), "items", "node", node );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, GetItemList );
    }

    void Manager::purgeNode( const JID& service,
                             const std::string& node,
                             ResultHandler* handler  )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      new Tag( iq.query(), "purge", "node", node );

      m_resultHandlerTrackMap[id] = handler;
      m_nopTrackMap[id] = node;
      m_parent->send( iq, this, PurgeNodeItems );
    }

    /**
     * @todo Track context info for Unsubscription result.
     * @todo
     */
    void Manager::handleIqID( IQ* iq, int context )
    {
      const JID& service = iq->from();
      const Tag* query = iq->query();
      const std::string& id = iq->id();

      ResultHandlerTrackMap::iterator ith = m_resultHandlerTrackMap.find( id );
      if( ith == m_resultHandlerTrackMap.end() )
        return;

      ResultHandler* rh = (*ith).second;

      switch( iq->subtype() )
      {
        case IQ::Result:
        {
          switch( context )
          {
            case Subscription:
            {
              const Tag* s = query->findChild( "subscription" );
              if( s )
              {
                const std::string& node = s->findAttribute( "node" ),
                                   sid  = s->findAttribute( "subid" ),
                                   jid  = s->findAttribute( "jid" ),
                                   sub  = s->findAttribute( "subscription" );
                SubscriptionType type = subscriptionType( sub );
                rh->handleSubscriptionResult( service, node, sid, jid, type );
              }
              break;
            }
            case Unsubscription:
            {
/*
              FIXME: info tracking for subscription ID...

              SubscriptionOperationTrackMap::iterator it = m_sopTrackMap.find( id );
              if( it != m_sopTrackMap.end() )
              {
                rh->handleUnsubscriptionResult( service, (*it).second.node,
                                                         (*it).second.sid,
                                                         (*it).second.jid );
                m_sopTrackMap.erase( it );
              }
              break;
*/
            }
            case GetSubscriptionList:
            {
              const Tag* subscription = query->findChild( "subscriptions" );
              if( subscription )
              {
                SubscriptionMap subMap;
                TagList::const_iterator it = subscription->children().begin();
                for( ; it != subscription->children().end(); ++it )
                {
                  const std::string& node = (*it)->findAttribute( "node" ),
                                     sub  = (*it)->findAttribute( "subscription" );
                  subMap[node] = subscriptionType( sub );
                }
                rh->handleSubscriptions( service, &subMap );
              }
              break;
            }
            case GetAffiliationList:
            {
              const Tag* affiliations = query->findChild( "affiliations" );
              if( affiliations )
              {
                AffiliationMap affMap;
                TagList::const_iterator it = affiliations->children().begin();
                for( ; it != affiliations->children().end(); ++it )
                {
                  const std::string& node = affiliations->findAttribute( "node" ),
                                     aff  = affiliations->findAttribute( "affiliation" );
                  affMap[node] = affiliationType( aff );
                }
                rh->handleAffiliations( service, &affMap );
              }
              break;
            }
            case GetSubscriptionOptions:
            case GetSubscriberList:
            case SetSubscriberList:
            case GetAffiliateList:
            case SetAffiliateList:
            case GetNodeConfig:
            case SetNodeConfig:
            {
              switch( context )
              {
                case GetSubscriptionOptions:
                {
                  const Tag* options = query->findChild( "options" );
                  const DataForm df( options->findChild( "x" ) );
                  rh->handleSubscriptionOptions( service,
                                         JID( options->findAttribute( "jid" ) ),
                                         options->findAttribute( "node" ), &df );
                  break;
                }
                case GetSubscriberList:
                {
                  const Tag* subt = query->findChild( "subscriptions" );
                  SubscriberList list;
                  const TagList& subs = subt->children();
                  TagList::const_iterator it = subs.begin();
                  for( ; it != subs.end(); ++it )
                  {
                    const std::string& jid = (*it)->findAttribute( "jid" );
                    const std::string& sub = (*it)->findAttribute( "subscription" );
                    const std::string& subid = (*it)->findAttribute( "subid" );
                    list.push_back( Subscriber( jid, subscriptionType( sub ), subid ) );
                  }
                  rh->handleSubscribers( service, subt->findAttribute( "node" ), &list );
                  break;
                }
                case SetSubscriptionOptions:
                case SetSubscriberList:
                case SetAffiliateList:
                case SetNodeConfig:
                case CreateNode:
                case DeleteNode:
                case PurgeNodeItems:
                {
                  NodeOperationTrackMap::iterator it = m_nopTrackMap.find( id );
                  if( it != m_nopTrackMap.end() )
                  {
                    const std::string& node = (*it).second;
                    switch( context )
                    {
                      case SetSubscriptionOptions:
                        rh->handleSubscriptionOptionsResult( service, JID( /* FIXME */ ), node );
                        break;
                      case SetSubscriberList:
                        rh->handleSubscribersResult( service, node );
                        break;
                      case SetAffiliateList:
                        rh->handleAffiliatesResult( service, node );
                        break;
                      case SetNodeConfig:
                        rh->handleNodeConfigResult( service, node );
                        break;
                      case CreateNode:
                        rh->handleNodeCreation( service, node );
                        break;
                      case DeleteNode:
                        rh->handleNodeDeletation( service, node );
                        break;
                      case PurgeNodeItems:
                        rh->handleNodePurge( service, node );
                        break;
                    }
                    m_nopTrackMap.erase( it );
                  }
                  break;
                }
                case GetAffiliateList:
                {
                  const TagList& affiliates = query->children();
                  AffiliateList affList;
                  TagList::const_iterator it = affiliates.begin();
                  for( ; it != affiliates.end(); ++it )
                  {
                    Affiliate aff( (*it)->findAttribute( "jid" ),
                                   affiliationType( (*it)->findAttribute( "affiliation" ) ) );
                    affList.push_back( aff );
                  }
                  rh->handleAffiliates( service, query->findAttribute( "node" ), &affList );
                  break;
                }
                case GetNodeConfig:
                {
                  Tag* ps = iq->query();
                  if( ps )
                  {
                    const Tag* const options = ps->findTag( "pubsub/configure" );
                    const Tag* const x = options->findChild( "x" );
                    const DataForm* const df = x ? new DataForm( x ) : 0;
                    rh->handleNodeConfig( service, options->findAttribute("node"), df );
                    delete df;
                  }
                  break;
                }
                default:
                  break;
              }

              break;
            }
            case GetItemList:
            {
              const Tag* items = query->findChild( "items" );
              if( items )
              {
                const std::string& node = items->findAttribute( "node" );
                rh->handleItems( service, node, &items->children() );
              }
              break;
            }
            case PublishItem:
            {
              ItemOperationTrackMap::iterator it = m_iopTrackMap.find( id );
              if( it != m_iopTrackMap.end() )
              {
                rh->handleItemPublication( service, (*it).second.first,
                                                    (*it).second.second );
                m_iopTrackMap.erase( it );
              }
              break;
            }
            case DeleteItem:
            {
              ItemOperationTrackMap::iterator it = m_iopTrackMap.find( id );
              if( it != m_iopTrackMap.end() )
              {
                rh->handleItemDeletation( service, (*it).second.first,
                                                   (*it).second.second );
                m_iopTrackMap.erase( it );
              }
              break;
            }
            case DefaultNodeConfig:
            {
              const Tag* deflt = query->findChild( "default" );
              if( deflt )
              {
                const DataForm df( deflt->findChild( "x" ) );
                rh->handleDefaultNodeConfig( service, &df );
              }
              break;
            }
          }
        }
        break;
        case IQ::Error:
        {
          m_nopTrackMap.erase( id );
          m_iopTrackMap.erase( id );
          const Error* error = iq->error();

          switch( context )
          {
            case Subscription:
            {
              const Tag* sub = query->findChild( "subscribe" );
              if( sub )
              {
                const std::string& node = sub->findAttribute( "node" ),
                                   jid  = sub->findAttribute( "jid" );

                const SubscriptionType type = SubscriptionNone;
                rh->handleSubscriptionResult( service, node, EmptyString, jid, type, error );
              }
              break;
            }
            case Unsubscription:
            {
              const Tag* unsub = query->findChild( "unsubscribe" );
              if( unsub )
              {
                const std::string& node = unsub->findAttribute( "node" ),
                                   sid  = unsub->findAttribute( "sid" ),
                                   jid  = unsub->findAttribute( "jid" );
                rh->handleUnsubscriptionResult( service, node, sid, jid, error );
              }
              break;
            }
            case GetSubscriptionList:
            {
              rh->handleSubscriptions( service, 0, error );
              break;
            }
            case GetAffiliationList:
            {
              rh->handleAffiliations( service, 0, error );
              break;
            }
            case GetSubscriptionOptions:
            {
              const Tag* options = query->findChild( "options" );
              if( options )
              {
                const std::string& jid  = options->findAttribute( "jid" ),
                                   node = options->findAttribute( "node" );
                rh->handleSubscriptionOptions( service, jid, node, 0, error );
              }
              break;
            }
            case SetSubscriptionOptions:
            {
              const Tag* options = query->findChild( "options" );
              if( options )
              {
                const std::string& jid  = options->findAttribute( "jid" ),
                                   node = options->findAttribute( "node" );
                rh->handleSubscriptionOptionsResult( service, JID( jid ), node, error );
              }
              break;
            }
            case GetNodeConfig:
            {
              const Tag* configure = query->findChild( "configure" );
              if( configure )
              {
                const std::string& node = configure->findAttribute( "node" );
                rh->handleNodeConfig( service, node, 0, error );
              }
              break;
            }
            case SetNodeConfig:
            {
              const Tag* configure = query->findChild( "configure" );
              if( configure )
              {
                const std::string& node = configure->findAttribute( "node" );
                rh->handleNodeConfigResult( service, node, error );
              }
              break;
            }
            case GetItemList:
            {
              const Tag* items = query->findChild( "items" );
              if( items )
              {
                const std::string& node = items->findAttribute( "node" );
                rh->handleItems( service, node, 0, error );
              }
              break;
            }
            case PurgeNodeItems:
            {
              const Tag* purge = query->findChild( "purge" );
              if( purge )
              {
                const std::string& node = purge->findAttribute( "node" );
                rh->handleNodePurge( service, node, error );
              }
              break;
            }
            case CreateNode:
            {
              const Tag* create = query->findChild( "create" );
              if( create )
              {
                const std::string& node = create->findAttribute( "node" );
                rh->handleNodeCreation( service, node, error );
              }
              break;
            }
            case DeleteNode:
            {
              const Tag* del = query->findChild( "delete" );
              if( del )
              {
                const std::string& node = del->findAttribute( "node" );
                rh->handleNodeDeletation( service, node, error );
              }
              break;
            }
            case PublishItem:
            {
              m_iopTrackMap.erase( iq->id() );

              const Tag* publish = query->findChild( "publish" );
              if( publish )
              {
                const Tag* item = publish->findChild( "item" );
                if( item )
                {
                  const std::string& node = publish->findAttribute( "node" );
                  const std::string& id = item->findAttribute( "id" );
                  rh->handleItemPublication( service, node, id, error );
                }
              }
              break;
            }
            case DeleteItem:
            {
              const Tag* retract = query->findChild( "retract" );
              if( retract )
              {
                const Tag* item = retract->findChild( "item" );
                if( item )
                {
                  const std::string& node = retract->findAttribute( "node" );
                  const std::string& id = item->findAttribute( "id" );
                  rh->handleItemDeletation( service, node, id, error );
                }
              }
              break;
            }
            case DefaultNodeConfig:
            {
              const Tag* deflt = query->findChild( "default" );
              if( deflt )
              {
                rh->handleDefaultNodeConfig( service, 0, error );
              }
              break;
            }
            default:
              break;
          }
          break;
        }
        default:
          break;
      }

      m_resultHandlerTrackMap.erase( ith );
    }

  }

}


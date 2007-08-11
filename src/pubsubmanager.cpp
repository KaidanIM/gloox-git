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
#include "message.h"
#include "iq.h"
#include "pubsub.h"
#include "pubsubeventhandler.h"
#include "pubsubservicehandler.h"
#include "pubsubnodehandler.h"
#include "pubsubitemhandler.h"
#include "pubsubdiscohandler.h"
#include "disco.h"
#include "util.h"
#include "error.h"

#include <iostream>

namespace gloox
{

  namespace PubSub
  {

    static const std::string
    XMLNS_PUBSUB                   = "http://jabber.org/protocol/pubsub",
    XMLNS_PUBSUB_ERRORS            = "http://jabber.org/protocol/pubsub#errors",
    XMLNS_PUBSUB_EVENT             = "http://jabber.org/protocol/pubsub#event",
    XMLNS_PUBSUB_OWNER             = "http://jabber.org/protocol/pubsub#owner",
    XMLNS_PUBSUB_NODE_CONFIG       = "http://jabber.org/protocol/pubsub#node_config",
    XMLNS_PUBSUB_SUBSCRIBE_OPTIONS = "http://jabber.org/protocol/pubsub#subscribe_options";

    enum Context
    {
      Subscription,
      Unsubscription,
      RequestSubscriptionOptions,
          SetSubscriptionOptions,
      RequestSubscriptionList,
      RequestSubscriberList,
          SetSubscriberList,
      RequestAffiliationList,
      RequestAffiliateList,
          SetAffiliateList,
      RequestNodeConfig,
          SetNodeConfig,
      RequestDefaultConfig,
      RequestItemList,
      PublishItem,
      DeleteItem,
      CreateNode,
      DeleteNode,
      PurgeNodeItems,
      NodeAssociation,
      NodeDisassociation,
      RequestFeatureList,
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
    static PubSubFeature featureType( const std::string& str )
    {
      static const char * values [] = {
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

    Manager::Manager( ClientBase *parent )
      : m_parent(parent)
    {
      m_parent->disco()->registerDiscoHandler( this );
      m_parent->registerMessageHandler( this );
    }

    void Manager::discoverInfos( const JID& service, const std::string& node,
                                 PubSub::DiscoHandler *handler )
    {
      if( !m_parent || !handler )
        return;
      const std::string& id = m_parent->getID();
      m_discoHandlerTrackMap[id] = handler;
      int context = node.empty() ? DiscoServiceInfos : DiscoNodeInfos;
      m_parent->disco()->getDiscoInfo( service, node, this, context, id );
    }

    void Manager::discoverNodeItems( const JID& service, const std::string& nodeid,
                                     PubSub::DiscoHandler *handler )
    {
      if( !m_parent || !handler )
        return;
      const std::string& id = m_parent->getID();
      m_discoHandlerTrackMap[id] = handler;
      m_parent->disco()->getDiscoItems( service, nodeid, this, DiscoNodeItems, id );
    }

    void Manager::handleDiscoInfoResult( IQ * iq, int context )
    {
      const Tag * query = iq->query();
      const Tag * identity = query->findChild( "identity" );
      if( !identity )
        return; // ejabberd...
      const JID& service = iq->from();
      const std::string& id = iq->id();
      const std::string& type = identity->findAttribute( TYPE );

      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( id );
      if( ith == m_discoHandlerTrackMap.end() )
        return;

      switch( context )
      {
        case DiscoServiceInfos:
        {
          int features=0;
          size_t pos;
          const Tag::TagList& qchildren = query->children();

          Tag::TagList::const_iterator it = qchildren.begin();
          for( ; it != qchildren.end(); ++it )
          {
            if( (*it)->name() == "feature" )
            {
              const std::string& var = (*it)->findAttribute( "var" );

              if( var.size() > XMLNS_PUBSUB.size()
                  && !var.compare( 0, XMLNS_PUBSUB.size(), XMLNS_PUBSUB ) )
              {
                pos = var.find_last_of( '#' );
                if( pos != std::string::npos )
                  features |= featureType( var.substr( pos+1 ) );
              }
            }
          }

          (*ith).second->handleServiceInfos( service, features );
          break;
        }
        case DiscoNodeInfos:
        {
          Tag * df = query->findChild( "x" );
          const std::string& node = query->findAttribute( "node" );

          NodeType nodeType = NodeInvalid;
          if( type == "collection" )
            nodeType = NodeCollection;
          else if( type == "leaf" )
            nodeType = NodeLeaf;

          const DataForm dataform( df );
          (*ith).second->handleNodeInfos( service, node, nodeType, &dataform );
          break;
        }
      }
      m_discoHandlerTrackMap.erase( ith );
    }

    void Manager::handleDiscoItemsResult( IQ *iq, int )
    {
      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( iq->id() );
      if( ith == m_discoHandlerTrackMap.end() )
        return;

      const Tag::TagList& content = iq->query()->children();

      DiscoNodeItemList contentList;
      Tag::TagList::const_iterator it = content.begin();
      for( ; it != content.end(); ++it )
      {
        contentList.push_back( DiscoNodeItem ( (*it)->findAttribute( "node" ),
                                               (*it)->findAttribute( "jid" ),
                                               (*it)->findAttribute( "name" ) ) );
      }

      const JID& service = iq->from();
      const std::string& parentid = iq->query()->findAttribute( "node" );

      (*ith).second->handleNodeItems( service, parentid, &contentList );
      m_discoHandlerTrackMap.erase( ith );
    }

    void Manager::handleDiscoError( IQ *iq, int context )
    {
      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( iq->id() );
      if( ith == m_discoHandlerTrackMap.end() )
        return;

      const JID& service = iq->from();
      const Tag *query = iq->query();
      const Error * error ;//= stanza->getExtension( ExtError );

      switch( context )
      {
        case DiscoServiceInfos:
          (*ith).second->handleServiceInfos( service, 0, error );
          break;
        case DiscoNodeInfos:
        {
          const std::string& node = query->findAttribute( "node" ); 
          (*ith).second->handleNodeInfos( service, node, NodeInvalid, 0, error );
          break;
        }
        case DiscoNodeItems:
        {
          const std::string& node = query->findAttribute( "node" ); 
          (*ith).second->handleNodeItems( service, node, 0, error );
          break;
        }
        default:
          break;
      }

      m_discoHandlerTrackMap.erase( ith );
    }

    static const char * subscriptionValues[] = {
      "none", "subscribed", "pending", "unconfigured"
    };

    static inline SubscriptionType subscriptionType( const std::string& subscription )
    {
      return (SubscriptionType)util::lookup( subscription, subscriptionValues );
    }

    static const char * affiliationValues[] = {
      "none", "publisher", "owner", "outcast"
    };

    static inline AffiliationType affiliationType( const std::string& affiliation )
    {
      return (AffiliationType)util::lookup( affiliation, affiliationValues );
    }

    enum EventType
    {
      EventCollection,
      EventConfigure,
      EventDelete,
      EventItems,
      EventPurge,
      EventSubscription,
      EventUnkown
    };

    static EventType eventType( const std::string& event )
    {
      static const char * values[] = {
        "collection",
        "configuration",
        "delete",
        "items",
        "purge",
        "subscription",
      };
      return (EventType)util::lookup( event, values );
    }

    void Manager::handleMessage( Message* msg, MessageSession * )
    {
      Tag * event = msg->findChild( "event", XMLNS, XMLNS_PUBSUB_EVENT );
      if( !event || m_eventHandlerList.empty() )
        return;

      const JID& service = msg->from();
      const Tag::TagList& events = event->children();
      EventType type;
      EventHandlerList::iterator ith = m_eventHandlerList.begin();

      // in case an event may contain several different notifications
      Tag::TagList::const_iterator it = events.begin();
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
              Tag * nodet = (*it)->findChild( "node" );
              Tag * df = (*it)->findChild( "x" );
              DataForm form( df );
              (*ith)->handleNodeCreation( service, node, form );
              break;
            }
            case EventConfigure:
            {
              Tag * df = (*it)->findChild( "x" );
              DataForm form( df );
              (*ith)->handleConfigurationChange( service, node, form );
              break;
            }
            case EventDelete:
            {
              (*ith)->handleNodeRemoval( service, node );
              break;
            }
            case EventItems:
            {
              // TODO
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
              const Tag * body = event->findChild( "body" );
              (*ith)->handleSubscriptionChange( service, jid, node,
                                                  body ? body->cdata() : std::string(),
                                                  subscriptionType( sub ) );
              break;
            }
          }
        }
      }
    }

    void Manager::subscriptionOptions( const JID& service,
                                       const JID& jid,
                                       const std::string& node,
                                       NodeHandler * handler,
                                       const DataForm *df )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag *options = new Tag( iq->query(), "options", "node", node );
      options->addAttribute( "jid", jid.empty() ? m_parent->jid().bare() : jid.bare() );
      if( df )
        options->addChild( df->tag() );

      m_parent->trackID( this, id, df ? SetSubscriptionOptions : RequestSubscriptionOptions );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::requestSubscriptionList( const JID& service, ServiceHandler * handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      new Tag( iq->query(), "subscriptions" );

      m_parent->trackID( this, id, RequestSubscriptionList );
      m_serviceHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::requestAffiliationList( const JID& service, ServiceHandler * handler )
    {
      if( !m_parent || !handler  )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      new Tag( iq->query(), "affiliations" );

      m_parent->trackID( this, id, RequestAffiliationList );
      m_serviceHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::subscribe( const JID& service, const std::string& node,
                             NodeHandler * handler, const JID& jid,
                             SubscriptionObject type, int depth )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* ps = iq->query();
      Tag* sub = new Tag( ps, "subscribe", "node", node );
      sub->addAttribute( "jid", jid.empty() ? m_parent->jid().full() : jid.full() );

      if( type != SubscriptionNodes || depth != 1 )
      {
        Tag * options = new Tag( ps, "options" );
        DataForm df( DataForm::FormTypeSubmit );
        df.addField( DataFormField::FieldTypeHidden, "FORM_TYPE", XMLNS_PUBSUB_SUBSCRIBE_OPTIONS );

        if( type == SubscriptionItems )
          df.addField( DataFormField::FieldTypeNone, "pubsub#subscription_type", "items" );

        if( depth != 1 )
        {
          DataFormField * field = df.addField( DataFormField::FieldTypeNone, "pubsub#subscription_depth" );
          if( depth == 0 )
            field->setValue( "all" );
          //else
          //  field->setValue( depth );
        }
        options->addChild( df.tag() );
      }

      m_parent->trackID( this, id, Subscription );
      m_nodeHandlerTrackMap[id] = handler;
      m_nopTrackMap[id] = node;
      m_parent->send( iq );
    }

    void Manager::unsubscribe( const JID& service, const std::string& node,
                                                   NodeHandler * handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag *sub = new Tag( iq->query(), "unsubscribe", "node", node );
      sub->addAttribute( "jid", m_parent->jid().bare() );

      m_parent->trackID( this, id, Unsubscription );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::publishItem( const JID& service, const std::string& node,
                               Tag * item, ItemHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* publish = new Tag( iq->query(), "publish", "node", node );
      publish->addChild( item );

      m_parent->trackID( this, id, PublishItem );
//      m_iopTrackMap[id] = TrackedItem(node, item);
      m_itemHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::deleteItem( const JID& service, const std::string& node,
                              const std::string& item, ItemHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag * retract = new Tag( iq->query(), "retract", "node", node );
      new Tag( retract, "item", "id", item );

      m_parent->trackID( this, id, DeleteItem );
      m_iopTrackMap[id] = TrackedItem( node, item );
      m_itemHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::createNode( NodeType type,
                              const JID& service,
                              const std::string& node,
                              NodeHandler* handler,
                              const std::string& name,
                              const std::string& parent,
                              AccessModel access,
                              const StringMap * config )
    {
      static const char * accessValues[] = {
        "open",
        "presence",
        "roster",
        "authorize",
        "whitelist"
      };

      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB, "pubsub" );
      Tag* pubsub = iq->query();
      new Tag( pubsub, "create", "node", node );
      Tag* configure = new Tag( pubsub, "configure" );

      if( !parent.empty() || config || type == NodeCollection || access != AccessDefault )
      {
        DataForm df( DataForm::FormTypeSubmit );
        df.addField( DataFormField::FieldTypeHidden, "FORM_TYPE", XMLNS_PUBSUB_NODE_CONFIG );

        if( !parent.empty() )
          df.addField( DataFormField::FieldTypeNone, "pubsub#collection", parent );

        if( !name.empty() )
          df.addField( DataFormField::FieldTypeNone, "pubsub#title", name );

        if( type == NodeCollection )
          df.addField( DataFormField::FieldTypeNone, "pubsub#node_type", "collection" );

        if( access != AccessDefault )
          df.addField( DataFormField::FieldTypeNone, "pubsub#access_model",
                            util::lookup( access, accessValues ) );
        if( config )
        {
          StringMap::const_iterator it = config->begin();
          for( ; it != config->end(); ++it )
            df.addField( DataFormField::FieldTypeNone, (*it).first, (*it).first );
        }
        configure->addChild( df.tag() );
      }

      m_parent->trackID( this, id, CreateNode );
      m_nopTrackMap[id] = node;
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::deleteNode( const JID& service, const std::string& node, NodeHandler* handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      new Tag( iq->query(), "delete", "node", node );

      m_parent->trackID( this, id, DeleteNode );
      m_nopTrackMap[id] = node;
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::getDefaultNodeConfig( const JID& service, NodeType type, ServiceHandler * handler )
    {
      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag* def = new Tag( iq->query(), "default" );
      if( type == NodeCollection )
      {
        DataForm df( DataForm::FormTypeSubmit );
        df.addField( DataFormField::FieldTypeHidden, "FORM_TYPE", XMLNS_PUBSUB_NODE_CONFIG );
        df.addField( DataFormField::FieldTypeNone, "pubsub#node_type", "collection" );
        def->addChild( df.tag() );
      }

      m_parent->trackID( this, id, RequestDefaultConfig );
      m_serviceHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::nodeConfig( const JID& service, const std::string& node,
                              const DataForm * config, NodeHandler * handler )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( config ? IQ::Set : IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag *sub = new Tag( iq->query(), "configure", "node", node );
      if( config )
        sub->addChild( config->tag() );

      m_parent->trackID( this, id, config ? SetNodeConfig : RequestNodeConfig );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::subscriberList( const JID& service,
                                  const std::string& node,
                                  const SubscriberList * list,
                                  NodeHandler * handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( list ? IQ::Set : IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag *sub = new Tag( iq->query(), "subscriptions", "node", node );
      if( list )
      {
        Tag * s;
        SubscriberList::const_iterator it = list->begin();
        for( ; it != list->end(); ++it )
        {
          s = new Tag( sub, "subscription", "jid", (*it).jid.full() );
          s->addAttribute( "subscription", util::lookup( (*it).type, subscriptionValues ) );
          if( !(*it).subid.empty() )
            s->addAttribute( "subid", (*it).subid );
        }
        m_nopTrackMap[id] = node;
      }

      m_parent->trackID( this, id, list ? SetSubscriberList : RequestSubscriberList );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::affiliateList( const JID& service,
                                 const std::string& node,
                                 const AffiliateList * list,
                                 NodeHandler * handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( list ? IQ::Set : IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      Tag *aff = new Tag( iq->query(), "affiliations", "node", node );
      if( list )
      {
        Tag * a;
        AffiliateList::const_iterator it = list->begin();
        for( ; it != list->end(); ++it )
        {
          a = new Tag( aff, "affiliation", "jid", (*it).jid.full() );
          a->addAttribute( "affiliation", util::lookup( (*it).type, affiliationValues ) );
        }
        m_nopTrackMap[id] = node;
      }

      m_parent->trackID( this, id, list ? SetAffiliateList : RequestAffiliateList );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::requestItems( const JID& service,
                                const std::string& nodeid,
                                ItemHandler * handler )
    {
      if( !m_parent || !handler )
        return;

      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Get, service, id, XMLNS_PUBSUB, "pubsub" );
      new Tag( iq->query(), "items", "node", nodeid );

      m_parent->trackID( this, id, RequestItemList );
      m_itemHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::purgeNodeItems( const JID& service, const std::string& node,
                                                      NodeHandler * handler  )
    {
      const std::string& id = m_parent->getID();
      IQ* iq = new IQ( IQ::Set, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
      new Tag( iq->query(), "purge", "node", node );

      m_parent->trackID( this, id, PurgeNodeItems );
      m_nodeHandlerTrackMap[id] = handler;
      m_nopTrackMap[id] = node;
      m_parent->send( iq );
    }

    /**
     * @todo Track context info for Unsubscription result.
     * @todo
     */
    void Manager::handleIqID( IQ* iq, int context )
    {
      const JID& service = iq->from();
      const Tag * query = iq->query();
      const std::string& id = iq->id();

      switch( iq->subtype() )
      {
        case IQ::Result:
        {
          switch( context )
          {
            case Subscription:
            {
              Tag *sub = query->findChild( "subscription" );
              if( sub )
              {
                const std::string& node = sub->findAttribute( "node" ),
                                   sid  = sub->findAttribute( "subid" ),
                                   jid  = sub->findAttribute( "jid" );
                SubscriptionType subType = subscriptionType( sub->findAttribute( "subsciption" ) );
                NodeHandlerTrackMap::iterator it = m_nodeHandlerTrackMap.find( id );
                if( it != m_nodeHandlerTrackMap.end() )
                {
                  (*it).second->handleSubscriptionResult( service, node, sid, subType );
                  m_nodeHandlerTrackMap.erase( it );
                }
              }
              break;
            }
            case Unsubscription:
            {
              /*
              NodeHandlerTrackMap::iterator it = m_nodeHandlerTrackMap.find( id );
              if( it != m_nodeHandlerTrackMap.end() )
              {
                (*ith).second->handleUnsubscriptionResult( service, node, jid );
                m_nodeHandlerTrackMap.remove( it );
              }
              */
              break;
            }
            case RequestSubscriptionList:
            {
              ServiceHandlerTrackMap::iterator ith = m_serviceHandlerTrackMap.find( iq->id() );
              Tag *subscription = query->findChild( "subscriptions" );
              if( subscription )
              {
                SubscriptionMap subMap;
                Tag::TagList::const_iterator it = subscription->children().begin();
                for( ; it != subscription->children().end(); ++it )
                {
                  const std::string& node = (*it)->findAttribute( "node" ),
                                     sub  = (*it)->findAttribute( "subscription" );
                  subMap[node] = subscriptionType( sub );
                }
                (*ith).second->handleSubscriptionList( service, &subMap );
                m_serviceHandlerTrackMap.erase( ith );
              }
              break;
            }
            case RequestAffiliationList:
            {
              ServiceHandlerTrackMap::iterator ith = m_serviceHandlerTrackMap.find( iq->id() );
              Tag *affiliations = query->findChild( "affiliations" );
              if( affiliations )
              {
                AffiliationMap affMap;
                Tag::TagList::const_iterator it = affiliations->children().begin();
                for( ; it != affiliations->children().end(); ++it )
                {
                  const std::string& node = affiliations->findAttribute( "node" ),
                                     aff  = affiliations->findAttribute( "affiliation" );
                  affMap[node] = affiliationType( aff );
                }
                (*ith).second->handleAffiliationList( iq->from(), &affMap );
                m_serviceHandlerTrackMap.erase( ith );
              }
              break;
            }
            case RequestSubscriptionOptions:
            case RequestSubscriberList:
            case SetSubscriberList:
            case RequestAffiliateList:
            case SetAffiliateList:
            case RequestNodeConfig:
            case SetNodeConfig:
            {
              NodeHandlerTrackMap::iterator ith = m_nodeHandlerTrackMap.find( id );
              if( ith == m_nodeHandlerTrackMap.end() )
                break;

              switch( context )
              {
                case RequestSubscriptionOptions:
                {
                  Tag *options = query->findChild( "options" );
                  Tag * x = options->findChild( "x" );
                  const DataForm df( x );
                  (*ith).second->handleSubscriptionOptions( iq->from(),
                                         JID( options->findAttribute( "jid" ) ),
                                         options->findAttribute( "node" ), &df );
                  break;
                }
                case RequestSubscriberList:
                {
                  NodeHandlerTrackMap::iterator ith = m_nodeHandlerTrackMap.begin();
                  if( ith == m_nodeHandlerTrackMap.end() )
                    return;

                  Tag * subt = query->findChild( "subscriptions" );                  
                  SubscriberList list;
                  const Tag::TagList& subs = subt->children();
                  Tag::TagList::const_iterator it = subs.begin();
                  for( ; it != subs.end(); ++it )
                  {
                    const std::string& jid = (*it)->findAttribute( "jid" );
                    const std::string& sub = (*it)->findAttribute( "subscription" );
                    const std::string& subid = (*it)->findAttribute( "subid" );
                    list.push_back( Subscriber( jid, subscriptionType( sub ), subid ) );
                  }
                  (*ith).second->handleSubscriberList( service, subt->findAttribute( "node" ), &list );
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
                        (*ith).second->handleSubscriptionOptionsResult( service, JID( /* FIXME */ ), node );
                        break;
                      case SetSubscriberList:
                        (*ith).second->handleSubscriberListResult( service, node );
                        break;
                      case SetAffiliateList:
                        (*ith).second->handleAffiliateListResult( service, node );
                        break;
                      case SetNodeConfig:
                        (*ith).second->handleNodeConfigResult( service, node );
                        break;
                      case CreateNode:
                        (*ith).second->handleNodeCreationResult( service, node );
                        break;
                      case DeleteNode:
                        (*ith).second->handleNodeDeletationResult( service, node );
                        break;
                      case PurgeNodeItems:
                        (*ith).second->handleNodePurgeResult( service, node );
                        break;
                    }
                    m_nopTrackMap.erase( it );
                  }
                  break;
                }
                case RequestAffiliateList:
                {
                  const Tag::TagList& affiliates = query->children();
                  AffiliateList affList;
                  Tag::TagList::const_iterator it = affiliates.begin();
                  for( ; it != affiliates.end(); ++it )
                  {
                    Affiliate aff( (*it)->findAttribute( "jid" ),
                                   affiliationType( (*it)->findAttribute( "affiliation" ) ) );
                    affList.push_back( aff );
                  }
                  (*ith).second->handleAffiliateList( service, query->findAttribute( "node" ), &affList );
                  break;
                }
                case RequestNodeConfig:
                {
                  Tag *ps = iq->findChild( "pubsub", XMLNS, XMLNS_PUBSUB );
                  Tag *options = ps->findChild( "configure" );
                  Tag * x = options->findChild( "x" );
                  const DataForm * df = x ? new DataForm( x ) : 0;
                  const std::string& node = options->findAttribute("node");
                  (*ith).second->handleNodeConfig( service, node, df );
                  if( df )
                    delete df;
                  break;
                }
                default:
                  break;
              }

              m_nodeHandlerTrackMap.erase( ith );
              break;
            }
            case RequestItemList:
            {
              Tag *items = query->findChild( "items" );
              if( !items )
                break;

              const std::string& node = items->findAttribute( "node" );
              ItemHandlerTrackMap::iterator ith = m_itemHandlerTrackMap.find( id );
              if( ith != m_itemHandlerTrackMap.end() )
              {
                (*ith).second->handleItemList( iq->from(), node, &items->children() );
                m_itemHandlerTrackMap.erase( ith );
              }
              break;
            }
            case PublishItem:
            {
              ItemHandlerTrackMap::iterator ith = m_itemHandlerTrackMap.find( id );
              if( ith == m_itemHandlerTrackMap.end() )
                break;

              ItemOperationTrackMap::iterator it = m_iopTrackMap.find( id );
              if( it != m_iopTrackMap.end() )
              {
                (*ith).second->handleItemPublicationResult( service,
                                                            (*it).second.first,
                                                            (*it).second.second );
                m_iopTrackMap.erase( it );
              }
              m_itemHandlerTrackMap.erase( ith );
              break;
            }
            case DeleteItem:
            {
              ItemHandlerTrackMap::iterator ith = m_itemHandlerTrackMap.find( id );
              if( ith == m_itemHandlerTrackMap.end() )
                break;

              ItemOperationTrackMap::iterator it = m_iopTrackMap.find( id );
              if( it != m_iopTrackMap.end() )
              {
                (*ith).second->handleItemDeletationResult( service,
                                                           (*it).second.first,
                                                           (*it).second.second );
                m_iopTrackMap.erase( it );
              }
              m_itemHandlerTrackMap.erase( ith );
              break;
            }
          }
        }
        break;
        case IQ::Error:
        {
          const Error error( iq->findChild( "error" ) );
          switch( context )
          {
            case Subscription:
            {
              NodeHandlerTrackMap::iterator ith = m_nodeHandlerTrackMap.find( id );
              if( ith == m_nodeHandlerTrackMap.end() )
                return;

              Tag *sub = query->findChild( "subscribe" );
              if( sub )
              {
                const std::string& node = sub->findAttribute( "node" ),
                                   jid  = sub->findAttribute( "jid" );

                (*ith).second->handleSubscriptionResult( service, node, jid, SubscriptionNone, &error );
              }

              m_nodeHandlerTrackMap.erase( ith );
              break;
            }
            case Unsubscription:
            {
              NodeHandlerTrackMap::iterator ith = m_nodeHandlerTrackMap.find( id );
              if( ith == m_nodeHandlerTrackMap.end() )
                return;

              Tag *unsub = query->findChild( "unsubscribe" );
              if( unsub )
              {
                const std::string& node = unsub->findAttribute( "node" ),
                                   jid  = unsub->findAttribute( "jid" );

                (*ith).second->handleUnsubscriptionResult( service, node, jid, &error );
              }

              m_nodeHandlerTrackMap.erase( ith );
              break;
            }
            case RequestSubscriptionList:
            {
              ServiceHandlerTrackMap::iterator ith = m_serviceHandlerTrackMap.find( iq->id() );
              if( ith != m_serviceHandlerTrackMap.end() )
                (*ith).second->handleSubscriptionList( service, 0, &error );
              break;
            }
            case RequestAffiliationList:
            {
              ServiceHandlerTrackMap::iterator ith = m_serviceHandlerTrackMap.find( iq->id() );
              if( ith != m_serviceHandlerTrackMap.end() )
                (*ith).second->handleAffiliationList( service, 0, &error );
              break;
            }
            case RequestSubscriptionOptions:
            {
              NodeHandlerTrackMap::iterator ith = m_nodeHandlerTrackMap.find( iq->id() );
              if( ith == m_nodeHandlerTrackMap.end() )
                return;

              Tag * options = query->findChild( "options" );
              if( options )
              {
                const std::string& node = options->findAttribute( "node" );
                (*ith).second->handleSubscriptionOptions( iq->from(),
                                         JID( options->findAttribute( "jid" ) ),
                                         options->findAttribute( "node" ), 0, &error );
              }

              m_nodeHandlerTrackMap.erase( ith );
              break;
            }
            case SetSubscriptionOptions:
            {
              NodeHandlerTrackMap::iterator ith = m_nodeHandlerTrackMap.find( iq->id() );
              if( ith == m_nodeHandlerTrackMap.end() )
                return;

              Tag * options = query->findChild( "options" );
              if( options )
              {
                const std::string& node = options->findAttribute( "node" );
                (*ith).second->handleSubscriptionOptionsResult( iq->from(),
                                         JID( options->findAttribute( "jid" ) ),
                                         options->findAttribute( "node" ), &error );
              }

              m_nodeHandlerTrackMap.erase( ith );
              break;
            }
            case RequestNodeConfig:
            {

            }
            case RequestItemList:
            {
              Tag * items = query->findChild( "items" );
              if( !items )
                return;
              const std::string& node = items->findAttribute( "node" );
              //handleRequestItemListError( service, node, errorType );
              break;
            }
            case PublishItem:
            {
              //handle
              break;
            }
            case DeleteItem:
            {
              //handle
              break;
            }
            case PurgeNodeItems:
            {
              break;
            }
            default:
              return;
          }
          break;
        }
        default:
          break;
      }
    }
  }
}


/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
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
#include "tag.h"
#include "psaffiliationlisthandler.h"
#include "pssubscriptionhandler.h"
#include "pssubscriptionlisthandler.h"
#include "pubsubeventhandler.h"
#include "pubsubitemhandler.h"
#include "pubsubnodehandler.h"
#include "pubsubitem.h"
#include "pubsub.h"
#include "pubsubdiscohandler.h"
#include "disco.h"
#include <iostream>
#include "parserutils.h"

namespace gloox
{

  namespace PubSub
  {

    static const std::string XMLNS_PUBSUB = "http://jabber.org/protocol/pubsub";
    static const std::string XMLNS_PUBSUB_ERRORS = "http://jabber.org/protocol/pubsub#errors";
    static const std::string XMLNS_PUBSUB_EVENT = "http://jabber.org/protocol/pubsub#event";
    static const std::string XMLNS_PUBSUB_OWNER = "http://jabber.org/protocol/pubsub#owner";
    static const std::string XMLNS_PUBSUB_NODE_CONFIG = "http://jabber.org/protocol/pubsub#node_config";
    static const std::string XMLNS_PUBSUB_SUBSCRIBE_OPTIONS = "http://jabber.org/protocol/pubsub#subscribe_opetions";

    enum Context {
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
      DiscoverNode
    };

    Manager::Manager( ClientBase * parent )
      : m_parent(parent)
    {
      m_parent->disco()->registerDiscoHandler( this );
      m_parent->registerMessageHandler( this );
    }

    enum DiscoInfoContext
    {
      ServiceInfo,
      NodeInfo
    };

    void Manager::discoverInfos( const JID& service, const std::string& node, PubSub::DiscoHandler * handler )
    {
      if( !handler )
        return;
      const std::string& id = m_parent->getID();
      m_discoHandlerTrackMap[id] = handler;
      int context = node.empty() ? ServiceInfo : NodeInfo;
      m_parent->disco()->getDiscoInfo( service, node, this, context, id );
    }

    void Manager::discoverNodeItems( const JID& service, const std::string& nodeid, PubSub::DiscoHandler * handler )
    {
      if( !handler )
        return;
      const std::string& id = m_parent->getID();
      m_discoHandlerTrackMap[id] = handler;
      m_parent->disco()->getDiscoItems( service, nodeid, this, 0, id );
    }

    static LookupPair subscriptionValues[] = {
      LookupPair( "pending",      SubscriptionPending ),
      LookupPair( "subscribed",   SubscriptionSubscribed ),
      LookupPair( "none",         SubscriptionNone ),
      LookupPair( "unconfigured", SubscriptionUnconfigured )
    };

    static SubscriptionType subscriptionType( const std::string& subscription )
    {  
      return (SubscriptionType)lookup( subscription, subscriptionValues );
    }

    static LookupPair affiliationValues[] = {
      LookupPair( "none",      AffiliationNone ),
      LookupPair( "publisher", AffiliationPublisher ),
      LookupPair( "owner",     AffiliationOwner ),
      LookupPair( "outcast",   AffiliationOutcast )
    };

    static AffiliationType affiliationType( const std::string& affiliation )
    {
      return (AffiliationType)lookup( affiliation, affiliationValues );
    }

    /**
     * Finds the associated PubSubFeature for a feature tag 'type' attribute,
     * as received from a disco info query on a pubsub service (XEP-0060 sect. 10).
     * @param feat Feature string to search for.
     * @return the associated PubSubFeature.
     */
    static PubSubFeature featureType( const std::string& str )
    {
      static const LookupPair values [] = {
        LookupPair( "collections",                FeatureCollections ),
        LookupPair( "config-node",                FeatureConfigNode ),
        LookupPair( "create-and-configure",       FeatureCreateAndConfig ),
        LookupPair( "create-nodes",               FeatureCreateNodes ),
        LookupPair( "delete-any",                 FeatureDeleteAny ),
        LookupPair( "delete-nodes",               FeatureDeleteNodes ),
        LookupPair( "get-pending",                FeatureGetPending ),
        LookupPair( "instant-nodes",              FeatureInstantNodes ),
        LookupPair( "item-ids",                   FeatureItemIDs ),
        LookupPair( "leased-subscription",        FeatureLeasedSubscription ),
        LookupPair( "manage-subscriptions",       FeatureManageSubscriptions ),
        LookupPair( "meta-data",                  FeatureMetaData ),
        LookupPair( "modify-affiliations",        FeatureModifyAffiliations ),
        LookupPair( "multi-collection",           FeatureMultiCollection ),
        LookupPair( "multi-subscribe",            FeatureMultiSubscribe ),
        LookupPair( "outcast-affiliation",        FeaturePutcastAffiliation ),
        LookupPair( "persistent-items",           FeaturePersistentItems ),
        LookupPair( "presence-notifications",     FeaturePresenceNotifications ),
        LookupPair( "publish",                    FeaturePublish ),
        LookupPair( "publisher-affiliation",      FeaturePublisherAffiliation ),
        LookupPair( "purge-nodes",                FeaturePurgeNodes ),
        LookupPair( "retract-items",              FeatureRetractItems ),
        LookupPair( "retrieve-affiliations",      FeatureRetrieveAffiliations ),
        LookupPair( "retrieve-default",           FeatureRetrieveDefault ),
        LookupPair( "retrieve-items",             FeatureRetrieveItems ),
        LookupPair( "retrieve-subscriptions",     FeatureRetrieveSubscriptions ),
        LookupPair( "subscribe",                  FeatureSubscribe ),
        LookupPair( "subscription-options",       FeatureSubscriptionOptions ),
        LookupPair( "subscription-notifications", FeatureSubscriptionNotifs ),
        LookupPair( "owner",                      FeatureMetaOwner ),
        LookupPair( "event",                      FeatureMetaEvent ),
      };
      return static_cast< PubSubFeature >( lookup( str, values ) );
    }

    enum EventType {
      EventCollection,
      EventConfigure,
      EventDelete,
      EventItems,
      EventPurge,
      EventSubscription
    };

    static EventType eventType( const std::string& event )
    {
      static const LookupPair values[] = {
        LookupPair( "collection",    EventCollection ),
        LookupPair( "configuration", EventConfigure ),
        LookupPair( "delete",        EventDelete ),
        LookupPair( "items",         EventItems ),
        LookupPair( "purge",         EventPurge ),
        LookupPair( "subscription",  EventSubscription )
      };
      return (EventType)lookup( event, values );
    }

    void Manager::handleMessage( Stanza * stanza, MessageSession * )
    {
      Tag * event = stanza->findChild( "event", "xmlns", XMLNS_PUBSUB_EVENT );
      if( !event || m_eventHandlerList.empty() )
        return;

      const JID& service = stanza->from();
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
          switch( type )
          {
            case EventCollection:
            {
              Tag * node = (*it)->findChild( "node" );
              Tag * df = (*it)->findChild( "x" );
              DataForm * form = df ? new DataForm( df ) : 0;
              (*ith)->handleNodeCreation( service, node->findAttribute( "id" ), form );
              break;
            }
            case EventConfigure:
            {
              Tag * df = (*it)->findChild( "x" );
              DataForm * form = df ? new DataForm( df ) : 0;
              const std::string& node = (*it)->findAttribute( "node" );
              (*ith)->handleConfigurationChange( service, node, form );
              break;
            }
            case EventDelete:
            {
              const std::string& node = (*it)->findAttribute( "node" );
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
              const std::string& node = (*it)->findAttribute( "node" );
              (*ith)->handleNodePurge( service, node );
              break;
            }
            case EventSubscription:
            {
              const std::string& node = (*it)->findAttribute( "node" );
              const std::string& jid  = (*it)->findAttribute( "jid" );
              const std::string& sub  = (*it)->findAttribute( "subscription" );
              SubscriptionType type   = subscriptionType( sub );
              const Tag * body = event->findChild( "body" );
              (*ith)->handleSubscriptionChange( service, node, jid,
                                                  body ? body->cdata()
                                                       : std::string(), type );
              break;
            }
          }
        }
      }
    }

    void Manager::handleDiscoInfoResult( Stanza * stanza, int context )
    {
      const Tag * query = stanza->findChild( "query" );
      const Tag * identity = query->findChild( "identity" );
      if( !identity )
        return; // ejabberd...
      const JID& service = stanza->from();
      const std::string& id = stanza->id();
      const std::string& type = identity->findAttribute( "type" );

      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( id );
      if( ith == m_discoHandlerTrackMap.end() )
        return;

      switch( context )
      {
        case ServiceInfo:
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

          (*ith).second->handleServiceInfoResult( service, features );
          break;
        }
        case NodeInfo:
        {
          Tag * df = query->findChild( "x" );
          const std::string& node = query->findAttribute( "node" );

          NodeType nodeType = NodeInvalid;
          if( type == "collection" )
            nodeType = NodeCollection;
          else if( type == "leaf" )
            nodeType = NodeLeaf;

          (*ith).second->handleNodeInfoResult( service, node, nodeType, DataForm( df ) );
          break;
        }
      }
      m_discoHandlerTrackMap.erase( ith );
    }

    void Manager::handleDiscoItemsResult( Stanza *stanza, int )
    {
      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( stanza->id() );
      if( ith == m_discoHandlerTrackMap.end() )
        return;

      const Tag * query = stanza->findChild( "query" );
      const Tag::TagList& content = query->children();


      DiscoNodeItemList contentList;
      Tag::TagList::const_iterator it = content.begin();
      for( ; it != content.end(); ++it )
      {
        contentList.push_back( DiscoNodeItem ( (*it)->findAttribute( "node" ),
                                               (*it)->findAttribute( "jid" ),
                                               (*it)->findAttribute( "name" ) ) );
      }

      const JID& service = stanza->from();
      const std::string& parentid = query->findAttribute( "node" );
      
      (*ith).second->handleNodeItemDiscovery( service, parentid, contentList );
      m_discoHandlerTrackMap.erase( ith );
    }

    void Manager::handleDiscoError( Stanza *stanza, int )
    {
      //const JID& service = stanza->from();
      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( stanza->id() );
      if( ith != m_discoHandlerTrackMap.end() )
      {
        //if( (*ith).second )
        //  (*ith).second->handleDiscoError( service, parentid, error );
        m_discoHandlerTrackMap.erase( ith );
      }
    }

    void Manager::requestSubscriptionOptions( const JID& service,
                                              const JID& jid,
                                              const std::string& node,
                                              NodeHandler * handler )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "options", "node", node );
      sub->addAttribute( "jid", jid.empty() ? m_parent->jid().bare() : jid.bare() );

      m_parent->trackID( this, id, RequestSubscriptionOptions );
      m_parent->send( iq );
    }

    void Manager::requestSubscriptionList( const JID& service, SubscriptionListHandler * slh )
    {
      if( !m_parent || !slh )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      new Tag( ps, "subscriptions" );

      m_parent->trackID( this, id, RequestSubscriptionList );
      m_subListTrackMap[id] = slh;
      m_parent->send( iq );
    }

    void Manager::requestAffiliationList( const JID& service, AffiliationListHandler * alh )
    {
      if( !m_parent || !alh  )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      new Tag( ps, "affiliations" );

      m_parent->trackID( this, id, RequestAffiliationList );
      m_affListTrackMap[id] = alh;
      m_parent->send( iq );
    }

    void Manager::subscribe( const JID& service,
                             const std::string& node,
			                       const JID& jid,
			                       SubscriptionObject type,
			                       int depth )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "subscribe", "node", node );
      sub->addAttribute( "jid", jid.empty() ? m_parent->jid().full() : jid.full() );

      if( type != SubscriptionNodes || depth != 1 )
      {
        Tag * options = new Tag( ps, "options" );
        DataForm df( DataForm::FormTypeSubmit );
        DataFormField *field = new DataFormField( DataFormField::FieldTypeHidden );
        field->setName( "FORM_TYPE" );
        field->setValue( XMLNS_PUBSUB_SUBSCRIBE_OPTIONS );
        df.addField( field );

        if( type == SubscriptionItems )
          df.addField( new DataFormField( "pubsub#subscription_type", "items", "", DataFormField::FieldTypeNone ) );

        if( depth != 1 )
        {
          field = new DataFormField( DataFormField::FieldTypeNone );
          field->setName( "pubsub#subscription_depth" );
          if( depth == 0 )
            field->setValue( "all" );
          //else
          //  field->setValue( depth );
          df.addField( field );
        }
        options->addChild( df.tag() );
      }

      m_parent->trackID( this, id, Subscription );
      //m_nopTrackMap[id] = StringPair( service, node );
      m_parent->send( iq );
    }

    void Manager::unsubscribe( const JID& service, const std::string& nodeid )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "unsubscribe", "node", nodeid );
      sub->addAttribute( "jid", m_parent->jid().bare() );

      m_parent->trackID( this, id, Unsubscription );
      m_parent->send( iq );
    }

    void Manager::publishItem( const JID& service, const std::string& node, const Tag& item )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq", "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag * publish = new Tag( pubsub, "publish", "node", node );
      publish->addChild( item.clone() );

      m_parent->trackID( this, id, PublishItem );
      //m_iopTrackMap[id] = TrackedItem(nodeid, itemid);
      m_parent->send( iq );
    }

    void Manager::deleteItem( const JID& service, const std::string& node, const std::string& itemid )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub" );
      pubsub->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag * retract = new Tag( pubsub, "retract" );
      retract->addAttribute( "node", node );
      Tag * item = new Tag( retract, "item" );
      item->addAttribute( "id", itemid );

      m_parent->trackID( this, id, DeleteItem );
      m_iopTrackMap[id] = TrackedItem( node, itemid );
      m_parent->send( iq );
    }

    void Manager::createNode( NodeType type,
                              const JID& service,
                              const std::string& node,
                              const std::string& name,
                              const std::string& parent,
                              const StringMap * config,
                              AccessModel access )
    {
      static LookupPair accessValues[] = {
        LookupPair( "open",      AccessOpen ),
        LookupPair( "presence",  AccessPresence ),
        LookupPair( "roster",    AccessRoster ),
        LookupPair( "authorize", AccessAuthorize ),
        LookupPair( "whitelist", AccessWhitelist )
      };
      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      new Tag( pubsub, "create", "node", node );
      Tag * configure = new Tag( pubsub, "configure" );

      if( !parent.empty() || config || type == NodeCollection || access != AccessDefault )
      {
        DataForm df( DataForm::FormTypeSubmit );
        DataFormField *field = new DataFormField( "FORM_TYPE", XMLNS_PUBSUB_NODE_CONFIG,
                                                  "", DataFormField::FieldTypeHidden );
        df.addField( field );

        if( !parent.empty() )
          df.addField( new DataFormField( "pubsub#collection", parent, "", DataFormField::FieldTypeNone ) );

        if( !name.empty() )
          df.addField( new DataFormField( "pubsub#title", name, "", DataFormField::FieldTypeNone ) );

        if( type == NodeCollection )
          df.addField( new DataFormField( "pubsub#node_type", "collection", "", DataFormField::FieldTypeNone ) );

        if( access != AccessDefault )
          df.addField( new DataFormField( "pubsub#access_model", lookup( access, accessValues ), "", DataFormField::FieldTypeNone ) );

        if( config )
        {
          StringMap::const_iterator it = config->begin();
          for( ; it != config->end(); ++it )
            df.addField( new DataFormField( (*it).first, (*it).first, "", DataFormField::FieldTypeNone ) );
        }
        configure->addChild( df.tag() );
      }

      m_parent->trackID( this, id, CreateNode );
      m_nopTrackMap[id] = make_pair( service, node );
      m_parent->send( iq );
    }

    void Manager::deleteNode( const JID& service, const std::string& nodeid )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
      new Tag( pubsub, "delete", "node", nodeid );

      m_parent->trackID( this, id, DeleteNode );
      m_nopTrackMap[id] = make_pair( service, nodeid );
      m_parent->send( iq );
    }
/*
    void Manager::getDefaultNodeConfig( const JID& service, const std::string& nodeid )
    {
      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag * create = new Tag( pubsub, "default" );
      if( !nodeid.empty() )
        create->addAttribute( "node", nodeid );
      m_parent->trackID( this, id, RequestConfig );
      m_nopTrackMap[id] = make_pair( service, nodeid );
      m_parent->send( iq );
    }
*/

    void Manager::nodeConfig( const JID& service, const std::string& node,
                                                  const DataForm * config,
                                                  NodeHandler * handler )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", config ? "set" : "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
      Tag *sub = new Tag( ps, "configure", "node", node );
      if( config )
        sub->addChild( config->tag() );

      m_parent->trackID( this, id, config ? SetNodeConfig : RequestNodeConfig );
      m_parent->send( iq );
    }

    void Manager::subscriberList( const JID& service,
                                  const std::string& node,
                                  const SubscriberList * list,
                                  NodeHandler * handler )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", list ? "set" : "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
      Tag *sub = new Tag( ps, "subscriptions", "node", node );
      if( list )
      {
        Tag * s;
        SubscriberList::const_iterator it = list->begin();
        for( ; it != list->end(); ++it )
        {
          s = new Tag( "subscription", "jid", (*it).jid.full() );
          s->addAttribute( "subscription", lookup( (*it).type, subscriptionValues ) );
          if( !(*it).subid.empty() )
            s->addAttribute( "subid", (*it).subid );
          sub->addChild( s );
        }
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
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", list ? "set" : "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
      Tag *aff = new Tag( ps, "affiliations", "node", node );
      if( list )
      {
        Tag * a;
        AffiliateList::const_iterator it = list->begin();
        for( ; it != list->end(); ++it )
        {
          a = new Tag( "affiliation", "jid", (*it).jid.full() );
          a->addAttribute( "affiliation", lookup( (*it).type, affiliationValues ) );
          aff->addChild( a );
        }
      }

      m_parent->trackID( this, id, list ? SetAffiliateList : RequestAffiliateList );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    void Manager::requestItems( const JID& service,
                                const std::string& nodeid,
                                ItemHandler * handler )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      new Tag( ps, "items", "node", nodeid );

      m_parent->trackID( this, id, RequestItemList );
      //m_itemHandlerList[id] = handler;
      m_parent->send( iq );
    }

    void Manager::purgeNodeItems( const JID& service, const std::string& nodeid,
                                                      NodeHandler * handler  )
    {
      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
      new Tag( pubsub, "purge", "node", nodeid );

      m_parent->trackID( this, id, PurgeNodeItems );
      m_nodeHandlerTrackMap[id] = handler;
      m_parent->send( iq );
    }

    bool Manager::handleIq( Stanza *stanza )
    {
      std::cout << __FUNCTION__ << ": " << stanza->xml() << std::endl;
      return 0;
    }

    /**
     * \bug Unsubscription does not retrieve the correct node id (needs a tracking map).
     */
    bool Manager::handleIqID( Stanza *stanza, int context )
    {
      const JID& service = stanza->from();
      const std::string& id = stanza->id();

      switch( stanza->subtype() )
      {
        case StanzaIqResult:
        {
          switch( context )
          {
            case Subscription:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              if( !ps )
                break;

              Tag *sub = ps->findChild( "subscription" );
              if( sub )
              {
                const std::string& nodeid = sub->findAttribute( "node" ),
                                   sid    = sub->findAttribute( "subid" ),
                                   jid    = sub->findAttribute( "jid" );
                SubscriptionType subType = subscriptionType( sub->findAttribute( "subsciption" ) );
                SubscriptionTrackList::iterator it = m_subscriptionTrackList.begin();
                for( ; it != m_subscriptionTrackList.end(); ++it )
                  (*it)->handleSubscriptionResult( service, nodeid, sid, subType, SubscriptionErrorNone );
              }
              break;
            }
            case Unsubscription:
            {
              std::cout << "UNSUBSCRIPTION" << std::endl;
              SubscriptionTrackList::iterator it = m_subscriptionTrackList.begin();
              //for( ; it != m_subscriptionTrackList.end(); ++it )
              //  (*it)->handleUnsubscriptionResult( service, jid, UnsubscriptionErrorNone );
              break;
            }
            case RequestSubscriptionList:
            {
              SubscriptionListTrackMap::iterator ith = m_subListTrackMap.find( stanza->id() );
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              Tag *subscription = ps->findChild( "subscriptions" );
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
                (*ith).second->handleSubscriptionListResult( service, subMap );
                m_subListTrackMap.erase( ith );
              }
              break;
            }
            case RequestAffiliationList:
            {
              AffiliationListTrackMap::iterator ith = m_affListTrackMap.find( stanza->id() );
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              Tag *affiliations = ps->findChild( "affiliations" );
              if( affiliations )
              {
                AffiliationMap affMap;
                Tag::TagList::const_iterator it = affiliations->children().begin();
                for( ; it != affiliations->children().end(); ++it )
                {
                  const std::string& node = affiliations->findAttribute( "node" ),
                                     aff  = affiliations->findAttribute( "affiliation" );
                  if( node.empty() || aff.empty() )
                    return 0;
                  affMap[node] = affiliationType( aff );
                }
                (*ith).second->handleAffiliationListResult( stanza->from(), affMap );
                m_affListTrackMap.erase( ith );
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
                  Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
                  Tag *options = ps->findChild( "options" );
                  Tag * x = options->findChild( "x" );
                  const DataForm df( x );
                  (*ith).second->handleSubscriptionOptions( stanza->from(),
                                         options->findAttribute( "jid" ),
                                         options->findAttribute( "node" ), df );
                  break;
                }
                case RequestSubscriberList:
                {
                  Tag * ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
                  Tag * subt = ps->findChild( "subscriptions" );
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
                  (*ith).second->handleSubscriberList( service, subt->findAttribute( "node" ), list );
                  break;
                }
                case SetSubscriptionOptions:
                case SetSubscriberList:
                case SetAffiliateList:
                case SetNodeConfig:
                {
                  NodeOperationTrackMap::iterator it = m_nopTrackMap.find( id );
                  if( it != m_nopTrackMap.end() )
                  {
                    const JID& service = (*it).second.first;
                    const std::string& node = (*it).second.second;
                    switch( context )
                    {
                      case SetSubscriptionOptions:
                        (*ith).second->handleSubscriptionOptionsResult( service, node );
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
                    }
                    m_nopTrackMap.erase( it );
                  }
                  break;
                }
                case RequestAffiliateList:
                {
                  
                  break;
                }
                case RequestNodeConfig:
                {
                  
                  break;
                }
                default:
                  break;
              }

              m_nodeHandlerTrackMap.erase( ith );
              break;
            }/*
            case SetSubscriberList:
            {
              break;
            }
            case RequestAffiliateList:
            {
              break;
            }
            case SetAffiliateList:
            {
              break;
            }
            case RequestNodeConfig:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              Tag *options = ps->findChild( "configure" );
              Tag * x = options->findChild( "x" );
              const DataForm * df = x ? new DataForm( x ) : 0;
              //handleNodeConfiguration( stanza->from(), options->findAttribute("node"), df, OptionRequestErrorNone );
            }
            case SetNodeConfig:
            {
            }*/
            case RequestItemList:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB_EVENT );
              if( !ps )
                break;
              Tag *items = ps->findChild( "items" );
              if( !items )
                break;

              const std::string& node = items->findAttribute( "node" );
              ItemHandlerList::iterator ith = m_itemHandlerList.begin();
              for( ; ith != m_itemHandlerList.end(); ++ith )
                (*ith)->handleItemList( stanza->from(), node, items->children() );
              break;
            }
            case PublishItem:
            {
              ItemOperationTrackMap::iterator it = m_iopTrackMap.find( id );
              if( it != m_iopTrackMap.end() )
              {
                //(*ith).second->handleItemPublicationResult( (*it).second.first,
                //                                            (*it).second.second );
                m_iopTrackMap.erase( it );
              }
              break;
            }
            case DeleteItem:
            {
              ItemOperationTrackMap::iterator it = m_iopTrackMap.find( id );
              if( it != m_iopTrackMap.end() )
              {
                //(*ith).second->handleItemDeletationResult( (*it).second.first,
                //                                           (*it).second.second );
                m_iopTrackMap.erase( it );
              }
              break;
            }
            case CreateNode:
            {
              NodeOperationTrackMap::iterator it = m_nopTrackMap.find( id );
              if( it != m_nopTrackMap.end() )
              {
                //(*ith).second->handleNodeCreationResult( (*it).second.first,
                //                                         (*it).second.second );
                m_nopTrackMap.erase( it );
              }
              break;
            }
            case DeleteNode:
            {
              NodeOperationTrackMap::iterator it = m_nopTrackMap.find( id );
              if( it != m_nopTrackMap.end() )
              {
                //(*ith).second->handleNodeDeletationResult( (*it).second.first,
                //                                           (*it).second.second );
                m_nopTrackMap.erase( it );
              }
              break;
            }
            case PurgeNodeItems:
            {
              NodeOperationTrackMap::iterator it = m_nopTrackMap.find( id );
              if( it != m_nopTrackMap.end() )
              {
                //(*ith).second->handleNodePurgeResult( (*it).second.first,
                //                                      (*it).second.second );
                m_nopTrackMap.erase( it );
              }
              break;
            }
          }
        }
        break;
        case StanzaIqError:
        {
          Tag* error = stanza->findChild( "error" );
          //Error error( stanza->findChild( "error" ) );
          
          switch( context )
          {
            case Subscription:
            {
              std::cout << "SUBSCRIPTION ERROR" << std::endl;
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              if( !ps )
                return false;
              Tag *sub = ps->findChild( "subscribe" );
              if( !sub )
                return false;

              const std::string& node = sub->findAttribute( "node" ),
                                 jid  = sub->findAttribute( "jid" );
              SubscriptionError errorType = SubscriptionErrorNone;

              if( error->hasChild( "not-authorized", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                if( error->hasChild( "pending-subscription", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = SubscriptionErrorPending;
                else if ( error->hasChild( "presence-subscription-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = SubscriptionErrorAccessPresence;
                else if ( error->hasChild( "not-in-roster-group", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = SubscriptionErrorAccessRoster;
              }
              else if( error->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = SubscriptionErrorItemNotFound;
              }
              else if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                       /*&& feature='subscribe'/> )*/
              {
                errorType = SubscriptionErrorUnsupported;
              }
              else if( error->hasChild( "not-allowed", "xmlns", XMLNS_XMPP_STANZAS ) &&
                      error->hasChild( "closed-node", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = SubscriptionErrorAccessWhiteList;
              }
              else if ( error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                const std::string& type = error->findAttribute( "type" );
                if( type == "cancel" )
                  errorType = SubscriptionErrorAnonymous;
                else if( type == "auth" )
                  errorType = SubscriptionErrorBlocked;
              }
              else if ( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                        error->hasChild( "invalid-jid", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = SubscriptionErrorJIDMismatch;
              }
              else if ( error->hasChild( "payment-required", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = SubscriptionErrorPayment;
              }
              else
                return false;

              //handleSubscriptionResult( service, node, jid, "", SubscriptionNone, errorType );
              break;
            }
            case Unsubscription:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              if( !ps )
                return false;
              Tag *subscription = ps->findChild( "subscribe" );
              if( !subscription )
                return false;
              const std::string& node = subscription->findAttribute( "node" ),
                                 jid  = subscription->findAttribute( "jid" );
              UnsubscriptionError errorType;
              if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                  error->hasChild( "subid-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = UnsubscriptionErrorMissingSID;
              }
              else if( error->hasChild( "unexpected-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "not-subscribed", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = UnsubscriptionErrorNotSubscriber;
              }
              else if( error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = UnsubscriptionErrorUnprivileged;
              }
              else if( error->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = UnsubscriptionErrorItemNotFound;
              }
              else if( error->hasChild( "not-acceptable", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "invalid-subid", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = UnsubscriptionErrorInvalidSID;
              }
              else
                return false;
              //handleUnsubscriptionResult( service, node, errorType );
              break;
            }
            case RequestSubscriptionList:
            {
              if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                  error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  /* feature='retrieve-subscriptions'/> */
              {
                //handleSubscriptionListError( service );
              }
              else
                return false;
              break;
            }
            case RequestAffiliationList:
            {
              if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                  error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  /* feature='retrieve-affiliations'/> */
              {
                //handleAffiliationListError( service );
              }
              else
                return false;
              break;
            }
            case RequestSubscriptionOptions:
            {
              Tag * pubsub = stanza->findChild( "pubsub" );
              if( !pubsub )
                return false;
              Tag * items = stanza->findChild( "items" );
              if( !items )
                return false;

              const std::string& node = items->findAttribute( "node" );
              OptionRequestError errorType = OptionRequestErrorNone;
              if( error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = OptionRequestUnprivileged;
              }
              else if( error->hasChild( "unexpected-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "not-subscribed", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = OptionRequestUnsubscribed;
              }
              else if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "jid-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = OptionRequestMissingJID;
              }
              else if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "subid-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = OptionRequestMissingSID;
              }
              else if( error->hasChild( "not-acceptable", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "invalid-subid", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = OptionRequestInvalidSID;
              }
              else if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                       // feature='subscription-options'
              {
                errorType = OptionRequestUnsupported;
              }
              else if( error->hasChild( "item-not-found", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = OptionRequestItemNotFound;
              }
              else
                return false;
              // handleOptionListError( "" );
              break;
            }
            case RequestNodeConfig:
            {
              
            }
            case RequestItemList:
            {
              Tag * pubsub = stanza->findChild( "pubsub" );
              if( !pubsub )
                return false;
              Tag * items = stanza->findChild( "items" );
              if( !items )
                return false;
              const std::string& node = items->findAttribute( "node" );
              ItemRetrivalError errorType = ItemRequestErrorNone;
              if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                  error->hasChild( "subid-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemRequestMissingSID;
              }
              else if( error->hasChild( "not-acceptable", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "invalid-subid", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemRequestInvalidSID;
              }
              else if( error->hasChild( "not-authorized", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                if( error->hasChild( "not-subscribed", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = ItemRequestNotSubscribed;
                else if( error->hasChild( "presence-subscription-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = ItemRequestAccessPresence;
                else if( error->hasChild( "not-in-roster-group", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = ItemRequestAccessRoster;
                else
                  return false;
              }
              else if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) )
              {  
                Tag * unsup = error->findChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS );
                if( !unsup )
                  return false;
                if( unsup->hasAttribute( "feature", "persistent-items" ) )
                  errorType = ItemRequestNoPersistent;
                else if( unsup->hasAttribute( "feature", "retrieve-items" ) )
                  errorType = ItemRequestUnsupported;
                else
                  return false;
              }
              else if( error->hasChild( "not-allowed", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "closed-node", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemRequestAccessWhiteList;
              }
              else if( error->hasChild( "payment-required", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemRequestPayment;
              }
              else if( error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemRequestBlocked;
              }
              else if( error->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemRetrievalItemNotFound;
              }
              else
                return false;
              //handleRequestItemListError( service, node, errorType );
              break;
            }
            case PublishItem:
            {
              ItemPublicationError errorType = ItemPublicationErrorNone;
              if( error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemPublicationUnprivileged;
              }  
              else if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                       // feature='publish'
              {
                errorType = ItemPublicationUnsupported;
              }  
              else if( error->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemPublicationNodeNotFound;
              }  
              else if( error->hasChild( "not-acceptable", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "payload-too-big", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemPublicationPayloadSize;
              }  
              else if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                /* Configuration errors needs to be specified */
                if( error->hasChild( "invalid-payload", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = ItemPublicationPayload;
                else if( error->hasChild( "item-required", "xmlns", XMLNS_PUBSUB_ERRORS) )
                  errorType = ItemPublicationConfiguration;
                else if( error->hasChild( "payload-required", "xmlns", XMLNS_PUBSUB_ERRORS) )
                  errorType = ItemPublicationConfiguration;
                else if( error->hasChild( "item-forbidden", "xmlns", XMLNS_PUBSUB_ERRORS) )
                  errorType = ItemPublicationConfiguration;
                else
                  return false;
              }
              else
                return false;
              //handle
              break;
            }
            case DeleteItem:
            {
              ItemDeletationError errorType = ItemDeletationErrorNone;
              if( error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemDeletationUnpriviledged;
              }  
              else if( error->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemDeletationItemNotFound;
              }  
              else if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "node-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemDeletationMissingNode;
              }  
              else if( error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       error->hasChild( "item-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemDeletationMissingItem;
              }  
              else if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                Tag * unsup = error->findChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS );
                if( !unsup )
                  return false;
                if( unsup->hasAttribute( "feature", "persistent-items" ) )
                  errorType = ItemDeletationNoPersistent;
                else if( unsup->hasAttribute( "feature", "delete-nodes" ) )
                  errorType = ItemDeletationUnsupported;
                else
                  return false;
              }
              else
                return false;
              //handle
              break;
            }
            case PurgeNodeItems:
            {
              break;
            }
            default:
              return false;
          }
          break;
        }
        default:
          return false;
      }
      return true;
    }
  }
}

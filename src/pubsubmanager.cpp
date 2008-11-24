/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
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
#include "pubsubitem.h"
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

    static const char* subscriptionValues[] = {
      "none", "subscribed", "pending", "unconfigured"
    };

    static inline SubscriptionType subscriptionType( const std::string& subscription )
    {
      return (SubscriptionType)util::lookup( subscription, subscriptionValues );
    }

    static inline const std::string subscriptionValue( SubscriptionType subscription )
    {
      return util::lookup( subscription, subscriptionValues );
    }

    static const char* affiliationValues[] = {
      "none", "publisher", "owner", "outcast"
    };

    static inline AffiliationType affiliationType( const std::string& affiliation )
    {
      return (AffiliationType)util::lookup( affiliation, affiliationValues );
    }

    static inline const std::string affiliationValue( AffiliationType affiliation )
    {
      return util::lookup( affiliation, affiliationValues );
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

    // ---- Manager::PubSub ----
    Manager::PubSub::PubSub( TrackContext context )
      : StanzaExtension( ExtPubSub ), m_ctx( context ), m_maxItems( 0 )
    {
      m_options.df = 0;
    }

    Manager::PubSub::PubSub( const Tag* tag )
      : StanzaExtension( ExtPubSub ), m_ctx( InvalidContext ), m_maxItems( 0 )
    {
      m_options.df = 0;
      if( !tag )
        return;

      ConstTagList l = tag->findTagList( "pubsub/subscriptions/subscription" );
      if( l.size() )
      {
        m_ctx = GetSubscriptionList;
        ConstTagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
        {
          const std::string& node = (*it)->findAttribute( "node" );
          const std::string& sub  = (*it)->findAttribute( "subscription" );
          SubscriptionInfo si;
          si.jid.setJID( (*it)->findAttribute( "jid" ) );
          si.type = subscriptionType( sub );
          m_subscriptionMap[node] = si;
        }
        return;
      }
      l = tag->findTagList( "pubsub/affiliations/affiliation" );
      if( l.size() )
      {
        m_ctx = GetAffiliationList;
        ConstTagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
        {
          const std::string& node = (*it)->findAttribute( "node" );
          const std::string& aff = (*it)->findAttribute( "affiliation" );
          m_affiliationMap[node] = affiliationType( aff );
        }
        return;
      }
      const Tag* s = tag->findTag( "pubsub/subscribe" );
      if( s )
      {
        m_ctx = Subscription;
        m_node = s->findAttribute( "node" );
        m_jid = s->findAttribute( "jid" );
      }
      const Tag* u = tag->findTag( "pubsub/unsubscribe" );
      if( u )
      {
        m_ctx = Unsubscription;
        m_node = u->findAttribute( "node" );
        m_jid = u->findAttribute( "jid" );
        m_subid = u->findAttribute( "subid" );
      }
      const Tag* o = tag->findTag( "pubsub/options" );
      if( o )
      {
        if( m_ctx == InvalidContext )
          m_ctx = GetSubscriptionOptions;
        m_options.jid.setJID( o->findAttribute( "jid" ) );
        m_options.node = o->findAttribute( "node" );
        m_options.df = new DataForm( o->findChild( "x", "xmlns", XMLNS_X_DATA ) );
      }
      const Tag* su = tag->findTag( "pubsub/subscription" );
      if( su )
      {
        SubscriptionInfo si;
        si.jid.setJID( su->findAttribute( "jid" ) );
        si.subid = su->findAttribute( "subid" );
        si.type = subscriptionType( su->findAttribute( "type" ) );
        m_subscriptionMap[su->findAttribute( "node" )] = si;
      }
      const Tag* i = tag->findTag( "pubsub/items" );
      {
        m_node = i->findAttribute( "node" );
        m_subid = i->findAttribute( "subid" );
        m_maxItems = atoi( i->findAttribute( "max_items" ).c_str() );
        const TagList& l = i->children();
        TagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
          m_items.push_back( new Item( (*it) ) );
      }
    }

    Manager::PubSub::~PubSub()
    {
      delete m_options.df;
    }

    const std::string& Manager::PubSub::filterString() const
    {
      static const std::string filter = "/iq/pubsub[@xmlns='" + XMLNS_PUBSUB + "']";
      return filter;
    }

    Tag* Manager::PubSub::tag() const
    {
      if( m_ctx == InvalidContext )
        return 0;

      Tag* t = new Tag( "pubsub" );
      t->setXmlns( XMLNS_PUBSUB );

      if( m_ctx == GetSubscriptionList && m_subscriptionMap.size() )
      {
        Tag* sub = new Tag( t, "subscriptions" );
        SubscriptionMap::const_iterator it = m_subscriptionMap.begin();
        for( ; it != m_subscriptionMap.end(); ++it )
        {
          Tag* s = new Tag( sub, "subscription" );
          s->addAttribute( "node", (*it).first );
          s->addAttribute( "jid", (*it).second.jid );
          s->addAttribute( "subscription", subscriptionValue( (*it).second.type ) );
        }
      }
      else if( m_ctx == GetAffiliationList && m_affiliationMap.size() )
      {

        Tag* aff = new Tag( t, "affiliations" );
        AffiliationMap::const_iterator it = m_affiliationMap.begin();
        for( ; it != m_affiliationMap.end(); ++it )
        {
          Tag* a = new Tag( aff, "affiliation" );
          a->addAttribute( "node", (*it).first );
          a->addAttribute( "affiliation", affiliationValue( (*it).second ) );
        }
      }
      else if( m_ctx == Subscription )
      {
        Tag* s = new Tag( t, "subscribe" );
        s->addAttribute( "node", m_node );
        s->addAttribute( "jid", m_jid.full() );
      }
      else if( m_ctx == Unsubscription )
      {
        Tag* u = new Tag( t, "unsubscribe" );
        u->addAttribute( "node", m_node );
        u->addAttribute( "jid", m_jid.full() );
        u->addAttribute( "subid", m_subid );
      }
      else if( m_ctx == GetSubscriptionOptions
               || m_ctx == SetSubscriptionOptions
               || ( m_ctx == Subscription && m_options.df ) )
      {
        Tag* o = new Tag( t, "options" );
        o->addAttribute( "node", m_node );
        o->addAttribute( "jid", m_jid.full() );
        if( m_options.df )
          o->addChild( m_options.df->tag() );
      }
      else if( m_ctx == RequestItems )
      {
        Tag* i = new Tag( t, "items" );
        i->addAttribute( "node", m_node );
        if( m_maxItems )
          i->addAttribute( "max_items", m_maxItems );
        i->addAttribute( "subid", m_subid );
        ItemList::const_iterator it = m_items.begin();
        for( ; it != m_items.end(); ++it )
          i->addChild( (*it)->tag() );
      }
      return t;
    }

    // ---- ~Manager::PubSub ----

    // ---- Manager ----
    Manager::Manager( ClientBase* parent )
      : m_parent( parent )
    {
      if( m_parent )
      {
        m_parent->registerStanzaExtension( new PubSub() );
      }
    }

    const std::string& Manager::getSubscriptionsOrAffiliations( const JID& service,
                                                                ResultHandler* handler,
                                                                TrackContext context )
    {
      if( !m_parent || !handler || !service || context == InvalidContext )
        return EmptyString;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id );
      iq.addExtension( new PubSub( context ) );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, context );
      return id;
    }

    const std::string& Manager::subscribe( const JID& service,
                             const std::string& node,
                             ResultHandler* handler,
                             const JID& jid,
                             SubscriptionObject type,
                             int depth )
    {
      if( !m_parent || !handler )
        return EmptyString;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id );
      PubSub* ps = new PubSub( Subscription );
      ps->setJID( jid ? jid : m_parent->jid() );
      ps->setNode( node );
      if( type != SubscriptionNodes || depth != 1 )
      {
        DataForm df( TypeSubmit );
        df.addField( DataFormField::TypeHidden, "FORM_TYPE", XMLNS_PUBSUB_SUBSCRIBE_OPTIONS );

        if( type == SubscriptionItems )
          df.addField( DataFormField::TypeNone, "pubsub#subscription_type", "items" );

        if( depth != 1 )
        {
          DataFormField* field = df.addField( DataFormField::TypeNone, "pubsub#subscription_depth" );
          if( depth == 0 )
            field->setValue( "all" );
          else
           field->setValue( util::int2string( depth ) );
        }
        ps->setOptions( jid, node, &df );
      }
      iq.addExtension( ps  );

      m_resultHandlerTrackMap[id] = handler;
      m_nopTrackMap[id] = node;
      m_parent->send( iq, this, Subscription );
      return id;
    }

    const std::string& Manager::unsubscribe( const JID& service,
                                              const std::string& node,
                                              const std::string& subid,
                                              ResultHandler* handler,
                                              const JID& jid )
    {
      if( !m_parent || !handler || !service )
        return EmptyString;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Set, service, id );
      PubSub* ps = new PubSub( Unsubscription );
      ps->setNode( node );
      ps->setJID( jid ? jid : m_parent->jid() );
      ps->setSubscriptionID( subid );
      iq.addExtension( ps );

      m_resultHandlerTrackMap[id] = handler;
      // need to track info for handler
      m_parent->send( iq, this, Unsubscription );
      return id;
    }

    const std::string& Manager::subscriptionOptions( TrackContext context,
                                       const JID& service,
                                       const JID& jid,
                                       const std::string& node,
                                       ResultHandler* handler,
                                       DataForm* df )
    {
      if( !m_parent || !handler || !service )
        return EmptyString;

      const std::string& id = m_parent->getID();
      IQ iq( df ? IQ::Set : IQ::Get, service, id );
      PubSub* ps = new PubSub( context );
      ps->setOptions( jid ? jid : m_parent->jid(), node, df );
      iq.addExtension( ps );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, context );
      return id;
    }

    const std::string& Manager::requestItems( const JID& service,
                                              const std::string& node,
                                              const std::string& subid,
                                              int maxItems,
                                              ResultHandler* handler )
    {
      if( !m_parent || !service || !handler )
        return EmptyString;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id );
      PubSub* ps = new PubSub( RequestItems );
      ps->setNode( node );
      ps->setSubscriptionID( subid );
      ps->setMaxItems( maxItems );
      iq.addExtension( ps );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, RequestItems );
      return id;
    }

    const std::string& Manager::requestItems( const JID& service,
                                              const std::string& node,
                                              const std::string& subid,
                                              const ItemList& items,
                                              ResultHandler* handler )
    {
      if( !m_parent || !service || !handler )
        return EmptyString;

      const std::string& id = m_parent->getID();
      IQ iq( IQ::Get, service, id );
      PubSub* ps = new PubSub( RequestItems );
      ps->setNode( node );
      ps->setSubscriptionID( subid );
      ps->setItems( items );
      iq.addExtension( ps );

      m_resultHandlerTrackMap[id] = handler;
      m_parent->send( iq, this, RequestItems );
      return id;
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
            df.addField( DataFormField::TypeNone, (*it).first, (*it).second );
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
      IQ iq( IQ::Get, service, id, XMLNS_PUBSUB_OWNER, "pubsub" );
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
    void Manager::handleIqID( const IQ& iq, int context )
    {
      const JID& service = iq.from();
      const Tag* query = iq.query();
      const std::string& id = iq.id();

      ResultHandlerTrackMap::iterator ith = m_resultHandlerTrackMap.find( id );
      if( ith == m_resultHandlerTrackMap.end() )
        return;

      ResultHandler* rh = (*ith).second;

      switch( iq.subtype() )
      {
        case IQ::Result:
        {
          switch( context )
          {
            case Subscription:
            {
              const PubSub* ps = iq.findExtension<PubSub>( ExtPubSub );
              if( ps )
              {
                SubscriptionMap sm = ps->subscriptionMap();
                SubscriptionMap::const_iterator it = sm.begin();
                rh->handleSubscriptionResult( service, (*it).first,
                                              (*it).second.subid,
                                              (*it).second.jid,
                                              (*it).second.type );
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
              const PubSub* ps = iq.findExtension<PubSub>( ExtPubSub );
              if( !ps )
                return;

              rh->handleSubscriptions( service, ps->subscriptions() );
              break;
            }
            case GetAffiliationList:
            {
              const PubSub* ps = iq.findExtension<PubSub>( ExtPubSub );
              if( !ps )
                return;

              rh->handleAffiliations( service, ps->affiliations() );
              break;
            }
            case GetSubscriptionOptions:
            case GetSubscriberList:
            case SetSubscriberList:
            case GetAffiliateList:
            case SetAffiliateList:
            case GetNodeConfig:
            case SetNodeConfig:
            case CreateNode:
            case DeleteNode:
            case PurgeNodeItems:
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
                        rh->handleSubscribersResult( service, node, 0 );
                        break;
                      case SetAffiliateList:
                        rh->handleAffiliatesResult( service, node, 0 );
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
                  Tag* ps = iq.query();
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
          const Error* error = iq.error();

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
              m_iopTrackMap.erase( iq.id() );

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
            case GetAffiliateList:
            {
              const Tag* aff = query->findChild( "affiliations" );
              AffiliateList list;
              const TagList& affiliates = aff->children();
              TagList::const_iterator it = affiliates.begin();
              for( ; it != affiliates.end(); ++it )
              {
                const std::string& jid = (*it)->findAttribute( "jid" );
                const std::string& afft = (*it)->findAttribute( "affiliation" );
                list.push_back( Affiliate( jid, affiliationType( afft ) ) );
              }
              rh->handleAffiliates( service, aff->findAttribute( "node" ), &list, error);
              break;
            }
            case SetAffiliateList:
            {
              const Tag* aff = query->findChild( "affiliations" );
              AffiliateList list;
              const TagList& affiliates = aff->children();
              TagList::const_iterator it = affiliates.begin();
              for( ; it != affiliates.end(); ++it )
              {
                const std::string& jid = (*it)->findAttribute( "jid" );
                const std::string& afft = (*it)->findAttribute( "affiliation" );
                list.push_back( Affiliate( jid, affiliationType( afft ) ) );
              }
              rh->handleAffiliatesResult( service, aff->findAttribute( "node" ), &list, error);
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
              rh->handleSubscribers( service, subt->findAttribute( "node" ), &list, error);
              break;
            }
            case SetSubscriberList:
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
              rh->handleSubscribersResult( service, subt->findAttribute( "node" ), &list, error);
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


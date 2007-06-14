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
#include "pubsubitemhandler.h"
#include "pubsubitem.h"
#include "pubsub.h"
#include "pubsubdiscohandler.h"
#include "disco.h"
#include <iostream>


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
      RequestSubscriptionList,
      RequestAffiliationList,
      RequestConfig,
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
      parent->disco()->registerDiscoHandler( this );
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

    typedef std::pair< const char *, PubSubFeature > featureStringPair;

    static const featureStringPair featureList [] = {
      featureStringPair( "collections", FeatureCollections ),
      featureStringPair( "config-node", FeatureConfigNode ),
      featureStringPair( "create-and-configure", FeatureCreateAndConfig ),
      featureStringPair( "create-nodes", FeatureCreateNodes ),
      featureStringPair( "delete-any", FeatureDeleteAny ),
      featureStringPair( "delete-nodes", FeatureDeleteNodes ),
      featureStringPair( "get-pending", FeatureGetPending ),
      featureStringPair( "instant-nodes", FeatureInstantNodes ),
      featureStringPair( "item-ids", FeatureItemIDs ),
      featureStringPair( "leased-subscription", FeatureLeasedSubscription ),
      featureStringPair( "manage-subscriptions", FeatureManageSubscriptions ),
      featureStringPair( "meta-data", FeatureMetaData ),
      featureStringPair( "modify-affiliations", FeatureModifyAffiliations ),
      featureStringPair( "multi-collection", FeatureMultiCollection ),
      featureStringPair( "multi-subscribe", FeatureMultiSubscribe ),
      featureStringPair( "outcast-affiliation", FeaturePutcastAffiliation ),
      featureStringPair( "persistent-items", FeaturePersistentItems ),
      featureStringPair( "presence-notifications", FeaturePresenceNotifications ),
      featureStringPair( "publish", FeaturePublish ),
      featureStringPair( "publisher-affiliation", FeaturePublisherAffiliation ),
      featureStringPair( "purge-nodes", FeaturePurgeNodes ),
      featureStringPair( "retract-items", FeatureRetractItems ),
      featureStringPair( "retrieve-affiliations", FeatureRetrieveAffiliations ),
      featureStringPair( "retrieve-default", FeatureRetrieveDefault ),
      featureStringPair( "retrieve-items", FeatureRetrieveItems ),
      featureStringPair( "retrieve-subscriptions", FeatureRetrieveSubscriptions ),
      featureStringPair( "subscribe", FeatureSubscribe ),
      featureStringPair( "subscription-options", FeatureSubscriptionOptions ),
      featureStringPair( "subscription-notifications", FeatureSubscriptionNotifs )
    };

    static const int featureListSize = sizeof( featureList ) / sizeof( featureStringPair );

    /**
     * Finds the associated PubSubFeature for a feature tag 'type' attribute,
     * as received from a disco info query on a pubsub service (XEP-0060 sect. 10).
     * @param feat Feature string to search for.
     * @return the associated PubSubFeature.
     */
    static PubSubFeature featureType( const std::string& feat )
    {
      int i=0;
      for( ; i < featureListSize && feat != featureList[i].first; ++i )
        ;
      return i != featureListSize ? featureList[i].second : FeatureUnknown;
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
          const Tag::TagList& qchildren = query->children();

          Tag::TagList::const_iterator it = qchildren.begin();
          for( ; it != qchildren.end(); ++it )
          {
            if( (*it)->name() == "feature" )
            {
              static std::string PUBSUB_PROTO_URI = "http://jabber.org/protocol/pubsub";
              const std::string& var = (*it)->findAttribute( "var" );

              if( var.size() > PUBSUB_PROTO_URI.size()
                  && var.find( PUBSUB_PROTO_URI ) == 0 )
              {
                unsigned fpos = var.rfind( '#' );
                if( fpos != std::string::npos )
                  features |= featureType( var.substr( fpos+1 ) );
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

    void Manager::handleDiscoItemsResult( Stanza *stanza, int context )
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

    void Manager::handleDiscoError( Stanza *stanza, int context )
    {
      const JID& service = stanza->from();
      DiscoHandlerTrackMap::iterator ith = m_discoHandlerTrackMap.find( stanza->id() );
      if( ith != m_discoHandlerTrackMap.end() )
      {
        //if( (*ith).second )
        //  (*ith).second->handleDiscoError( service, parentid, error );
        m_discoHandlerTrackMap.erase( ith );
      }
    }

    void Manager::requestSubscriptionList( const JID& service, SubscriptionListHandler * slh )
    {
      if( !m_parent )
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
      if( !m_parent )
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
                             const std::string& nodeid,
			                       const std::string& jid,
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
      Tag *sub = new Tag( ps, "subscribe", "node", nodeid );
      sub->addAttribute( "jid", jid.empty() ? m_parent->jid().bare() :jid );

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

    void Manager::publishItem( const JID& /*service*/, const std::string& /*nodeid*/, const Item& /*item*/ )
    {
      /*
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq", "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag * publish = new Tag( pubsub, "publish", "node", nodeid );

      Tag * item = new Tag( retract, "item" );
      item->addAttribute( "id", item.id );

      m_parent->trackID( this, id, PublishItem );
      m_iopTrackMap[id] = TrackedItem(nodeid, itemid);
      m_parent->send( iq );
      */
    }

    void Manager::deleteItem( const JID& service, const std::string& nodeid, const std::string& itemid )
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
      retract->addAttribute( "node", nodeid );
      Tag * item = new Tag( retract, "item" );
      item->addAttribute( "id", itemid );

      m_parent->trackID( this, id, DeleteItem );
      m_iopTrackMap[id] = TrackedItem(nodeid, itemid);
      m_parent->send( iq );
    }

    static const char * accessModelStrings[] = {
      "open",
      "presence",
      "roster",
      "authorize",
      "whitelist"
    };

    void Manager::createNode( NodeType type,
                              const JID& service,
                              const std::string& node,
                              const std::string& name,
                              const std::string& parent,
                              const StringMap * config,
                              AccessModel access )
    {
      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag * create = new Tag( pubsub, "create", "node", node );
      Tag * configure = new Tag( pubsub, "configure" );

      if( config || type == NodeCollection || access != AccessDefault )
      {
        DataForm df( DataForm::FormTypeSubmit );
        DataFormField *field = new DataFormField( DataFormField::FieldTypeHidden );
        field->setName( "FORM_TYPE" );
        field->setValue( XMLNS_PUBSUB_NODE_CONFIG );
        df.addField( field );

        if( type == NodeCollection )
          df.addField( new DataFormField( "pubsub#node_type", "collection", "", DataFormField::FieldTypeNone ) );

        if( access != AccessDefault )
          df.addField( new DataFormField( "pubsub#access_model", accessModelStrings[access], "", DataFormField::FieldTypeNone ) );

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
    void Manager::requestNodeConfig( const JID& service, const std::string& nodeid )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", service.full() );
      Tag *ps = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "options", "node", nodeid );
      sub->addAttribute( "jid", m_parent->jid().bare() );

      m_parent->trackID( this, id, RequestConfig );
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
      m_parent->send( iq );
    }

    void Manager::purgeNodeItems( const JID& service, const std::string& nodeid )
    {
      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "to", service.full() );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub", "xmlns", XMLNS_PUBSUB_OWNER );
      new Tag( pubsub, "purge", "node", nodeid );

      m_parent->trackID( this, id, PurgeNodeItems );
      m_parent->send( iq );
    }

    bool Manager::handleIq( Stanza */*stanza*/ )
    {
      return 0;
    }

    static SubscriptionType subscriptionType( const std::string& subscription )
    {
      SubscriptionType subType = SubscriptionInvalid;
      if( subscription == "pending" )
        subType = SubscriptionPending;
      else if( subscription == "subscribed" )
        subType = SubscriptionSubscribed;
      else if( subscription == "none" )
        subType = SubscriptionNone;
      else if( subscription == "unconfigured" )
        subType = SubscriptionUnconfigured;
      return subType;
    }

    static AffiliationType affiliationType( const std::string& affiliation )
    {
      AffiliationType affType = AffiliationInvalid;
      if( affiliation == "none" )
        affType = AffiliationNone;
      else if( affiliation == "publisher" )
        affType = AffiliationPublisher;
      else if( affiliation == "owner" )
        affType = AffiliationOwner;
      else if( affiliation == "outcast" )
        affType = AffiliationOutcast;
      return affType;
    }

    /**
     * \bug Unsubscription does not retrieve the correct node id (needs a tracking map).
     */
    bool Manager::handleIqID( Stanza *stanza, int context )
    {
      const JID& service = stanza->from();

      switch( stanza->subtype() )
      {
        case StanzaIqResult:
        {
          switch( context )
          {
            case Subscription:
            {
              std::cout << "SUBSCRIPTION" << std::endl;
	      Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
	      if( !ps )
	      {
	        std::cout << "no ps" << std::endl;
		break;
	      }

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
	      std::cout << __LINE__ << std::endl;
              break;
            }
            case Unsubscription:
            {
              //SubscriptionTrackList::iterator it = m_subscriptionTrackList.begin();
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
                  if( node.empty() || sub.empty() )
                    return 0;
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
            case RequestConfig:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              Tag *options = ps->findChild( "options" );
              const DataForm dataForm( options->findChild( "x" ) );
              //handleOptions( stanza->from(), options->findAttribute("node"), dataForm, OptionRequestErrorNone );
              break;
            }
            case RequestItemList:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns",
	      XMLNS_PUBSUB_EVENT );
	      if( !ps )
	      {
	        std::cout << "no pubsub!" << std::endl;
		break;
	      }
              Tag *items = ps->findChild( "items" );

	      if( !items )
	      {
	        std::cout << "no pubsub!" << std::endl;
		break;
	      }
              const std::string& node = items->findAttribute( "node" );
              ItemHandlerList::iterator ith = m_itemHandlerList.begin();
              for( ; ith != m_itemHandlerList.end();++ith )
	        (*ith)->handleItemList( stanza->from(), node, items->children() );
              break;
            }
            case PublishItem:
            {
              break;
            }
            case DeleteItem:
            {
              break;
            }
            case CreateNode:
            {
              break;
            }
            case DeleteNode:
            {
              
              break;
            }
            case PurgeNodeItems:
            {
              break;
            }
          }
        }
        break;
        case StanzaIqError:
        {
          Tag* error = stanza->findChild( "error" );
          
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
              std::cout << __LINE__ << std::endl;
              const std::string& node = sub->findAttribute( "node" ),
                                 jid  = sub->findAttribute( "jid" );
              SubscriptionError errorType = SubscriptionErrorNone;
              std::cout << __LINE__ << std::endl;
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
              std::cout << __LINE__ << std::endl;
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
            case RequestConfig:
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

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
#include "pubsub.h"

namespace gloox
{

  namespace PubSub
  {

    static const std::string XMLNS_PUBSUB = "http://jabber.org/protocol/pubsub";
    static const std::string XMLNS_PUBSUB_ERRORS = "http://jabber.org/protocol/pubsub#errors";

    enum Context {
      Subscription,
      Unsubscription,
      RequestSubscriptionList,
      RequestAffiliationList,
      RequestOptionList,
      RequestItemList,
      PublishItem,
      DeleteItem
    };

    void Manager::requestSubscriptionList( const std::string& jid, SubscriptionListHandler * slh )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", jid );
      Tag *ps = new Tag( iq, "pubsub" );
      ps->addAttribute( "xmlns", XMLNS_PUBSUB );
      new Tag( ps, "subscriptions" );

      m_parent->trackID( this, id, RequestSubscriptionList );
      m_subListTrackMap[id] = slh;
      m_parent->send( iq );
    }

    void Manager::requestAffiliationList( const std::string& jid, AffiliationListHandler * alh )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", jid );
      Tag *ps = new Tag( iq, "pubsub" );
      ps->addAttribute( "xmlns", XMLNS_PUBSUB );
      new Tag( ps, "affiliations" );

      m_parent->trackID( this, id, RequestAffiliationList );
      m_affListTrackMap[id] = alh;
      m_parent->send( iq );
    }

    void Manager::subscribe( const std::string& jid, const std::string& node )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", jid );
      Tag *ps = new Tag( iq, "pubsub" );
      ps->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "subscribe" );
      sub->addAttribute( "node", node );
      sub->addAttribute( "jid", m_parent->jid().bare() );

      m_parent->trackID( this, id, Subscription );
      m_parent->send( iq );
    }

    void Manager::unsubscribe( const std::string& jid, const std::string& node )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", jid );
      Tag *ps = new Tag( iq, "pubsub" );
      ps->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "unsubscribe" );
      sub->addAttribute( "node", node );
      sub->addAttribute( "jid", m_parent->jid().bare() );

      m_parent->trackID( this, id, Unsubscription );
      m_parent->send( iq );
    }

    void Manager::publishItem( const std::string& service, const std::string& node, const Item& item )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq", "type", "set" );
      iq->addAttribute( "to", service );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub" );
      pubsub->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag * publish = new Tag( pubsub, "publish" );
      publish->addAttribute( "node", node );
      /*
      Tag * item = new Tag( retract, "item" );
      item->addAttribute( "id", item.id );

      m_parent->trackID( this, id, PublishItem );
      m_parent->send( iq );
      */
    }

    void Manager::deleteItem( const std::string& service, const std::string& node, const std::string& itemID )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag * iq = new Tag( "iq", "type", "set" );
      iq->addAttribute( "to", service );
      iq->addAttribute( "id", id );
      Tag * pubsub = new Tag( iq, "pubsub" );
      pubsub->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag * retract = new Tag( pubsub, "retract" );
      retract->addAttribute( "node", node );
      Tag * item = new Tag( retract, "item" );
      item->addAttribute( "id", itemID );

      m_parent->trackID( this, id, DeleteItem );
      m_parent->send( iq );
    }
    
    void Manager::requestOptions( const std::string& jid, const std::string& node )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", jid );
      Tag *ps = new Tag( iq, "pubsub" );
      ps->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "options" );
      sub->addAttribute( "node", node );
      sub->addAttribute( "jid", m_parent->jid().bare() );

      m_parent->trackID( this, id, RequestOptionList );
      
      m_parent->send( iq );
    }

    void Manager::requestItems( const JID& node, ItemHandler * handler )
    {
      if( !m_parent )
        return;

      const std::string& id = m_parent->getID();
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "get" );
      iq->addAttribute( "id", id );
      iq->addAttribute( "to", node.bare() );
      Tag *ps = new Tag( iq, "pubsub" );
      ps->addAttribute( "xmlns", XMLNS_PUBSUB );
      Tag *sub = new Tag( ps, "items" );
      sub->addAttribute( "node", node.resource() );

      m_parent->trackID( this, id, RequestItemList );
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

    bool Manager::handleIqID( Stanza *stanza, int context )
    {
      switch( stanza->subtype() )
      {
        case StanzaIqResult:
        {
          switch( context )
          {
            case Subscription:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              Tag *subscription = ps->findChild( "subscription" );
              if( subscription )
              {
                const std::string& node = subscription->findAttribute( "node" ),
                                   jid  = subscription->findAttribute( "jid" ),
                                   sid  = subscription->findAttribute( "subid" ),
                                   sub  = subscription->findAttribute( "subsciption" );
                SubscriptionType subType = subscriptionType( sub );
                SubscriptionTrackList::iterator it = m_subscriptionTrackList.begin();
                for( ; it != m_subscriptionTrackList.end(); ++it )
                  (*it)->handleSubscriptionResult( jid, node, sid, subType, SubscriptionErrorNone );
              }
              break;
            }
            case Unsubscription:
            {
              const std::string& jid  = stanza->findAttribute( "to" ),
                                 srv  = stanza->findAttribute( "from" );
              if( jid.empty() || srv.empty() )
                return 0;
              SubscriptionTrackList::iterator it = m_subscriptionTrackList.begin();
              for( ; it != m_subscriptionTrackList.end(); ++it )
                (*it)->handleUnsubscriptionResult( jid, srv, UnsubscriptionErrorNone );
              break;
            }
            case RequestSubscriptionList:
            {
              SubscriptionListTrackMap::iterator ith = m_subListTrackMap.find( stanza->id() );
              if( ith == m_subListTrackMap.end() )
                return 0;
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
                (*ith).second->handleSubscriptionListResult( stanza->from(), subMap );
                m_subListTrackMap.erase( ith );
              }
              break;
            }
            case RequestAffiliationList:
            {
              AffiliationListTrackMap::iterator ith = m_affListTrackMap.find( stanza->id() );
              if( ith == m_affListTrackMap.end() )
                return 0;
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
            case RequestOptionList:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              Tag *options = ps->findChild( "options" );
              const DataForm dataForm( options->findChild( "x" ) );
              //handleOptions( stanza->from(), options->findAttribute("node"), dataForm, OptionRequestErrorNone );
              break;
            }
            case RequestItemList:
            {
              break;
            }
            case PublishItem:
            {
              break;
            }
            case DeleteItem:
            {
              // store the node id locally for the del notif ?
              break;
            }
            
          }
        }
        break;
        case StanzaIqError:
        {
          switch( context )
          {
            case Subscription:
            {
              Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
              if( !ps )
                return false;
              Tag *sub = ps->findChild( "subscribe" );
              if( !sub )
                return false;
              const std::string& service = stanza->findAttribute( "from" ),
                                 node = sub->findAttribute( "node" ),
                                 jid  = sub->findAttribute( "jid" );
              Tag* error = stanza->findChild( "error" );
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
              const std::string& service = stanza->findAttribute( "from" ),
                                 node = subscription->findAttribute( "node" ),
                                 jid  = subscription->findAttribute( "jid" );
              Tag* error = stanza->findChild( "error" );
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
              const std::string& jid  = stanza->findAttribute( "from" );
              Tag* error = stanza->findChild( "error" );
              if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                  error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  /* feature='retrieve-subscriptions'/> */
              {
                //handleSubscriptionListError( jid );
              }
              else
                return false;
              break;
            }
            case RequestAffiliationList:
            {
              const std::string& jid  = stanza->findAttribute( "from" );
              Tag* error = stanza->findChild( "error" );
              if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                  error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  /* feature='retrieve-affiliations'/> */
              {
                //handleAffiliationListError( jid );
              }
              else
                return false;
              break;
            }
            case RequestOptionList:
            {
              Tag * pubsub = stanza->findChild( "pubsub" );
              if( !pubsub )
                return false;
              Tag * items = stanza->findChild( "items" );
              if( !items )
                return false;
              const std::string& node = items->findAttribute( "node" ),
                                 service  = stanza->findAttribute( "from" );
              Tag* error = stanza->findChild( "error" );
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
              const std::string& node = items->findAttribute( "node" ),
                                 service  = stanza->findAttribute( "from" );
              Tag* error = stanza->findChild( "error" );

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
              if( stanza->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemPublicationUnprivileged;
              }  
              else if( stanza->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       stanza->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                       // feature='publish'
              {
                errorType = ItemPublicationUnsupported;
              }  
              else if( stanza->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemPublicationNodeNotFound;
              }  
              else if( stanza->hasChild( "not-acceptable", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       stanza->hasChild( "payload-too-big", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemPublicationPayloadSize;
              }  
              else if( stanza->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                /* Configuration errors needs to be specified */
                if( stanza->hasChild( "invalid-payload", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                  errorType = ItemPublicationPayload;
                else if( stanza->hasChild( "item-required", "xmlns", XMLNS_PUBSUB_ERRORS) )
                  errorType = ItemPublicationConfiguration;
                else if( stanza->hasChild( "payload-required", "xmlns", XMLNS_PUBSUB_ERRORS) )
                  errorType = ItemPublicationConfiguration;
                else if( stanza->hasChild( "item-forbidden", "xmlns", XMLNS_PUBSUB_ERRORS) )
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
              if( stanza->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemDeletationUnpriviledged;
              }  
              else if( stanza->hasChild( "item-not-found", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                errorType = ItemDeletationItemNotFound;
              }  
              else if( stanza->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       stanza->hasChild( "node-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemDeletationMissingNode;
              }  
              else if( stanza->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) &&
                       stanza->hasChild( "item-required", "xmlns", XMLNS_PUBSUB_ERRORS ) )
              {
                errorType = ItemDeletationMissingItem;
              }  
              else if( stanza->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) )
              {
                Tag * unsup = stanza->findChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS );
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

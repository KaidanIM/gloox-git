#include "pubsub.h"
#include "clientbase.h"
#include "dataform.h"
#include "tag.h"

namespace gloox
{

  static const std::string XMLNS_PUBSUB = "http://jabber.org/protocol/pubsub";
  static const std::string XMLNS_PUBSUB_ERRORS = "http://jabber.org/protocol/pubsub#errors";

  enum PubSubContext {
    Subscription,
    Unsubscription,
    RequestSubscriptionList,
    RequestAffiliationList,
    RequestOptions
  };

  void PubSubManager::requestSubscriptionList( const std::string& jid )
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
    m_parent->send( iq );
  }

  void PubSubManager::requestAffiliationList( const std::string& jid )
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
    m_parent->send( iq );
  }

  void PubSubManager::subscribe( const std::string& jid, const std::string& node )
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

  void PubSubManager::unsubscribe( const std::string& jid, const std::string& node )
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

  void PubSubManager::requestOptions( const std::string& jid, const std::string& node )
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

    m_parent->trackID( this, id, RequestOptions );
    m_parent->send( iq );
  }

  bool PubSubManager::handleIq( Stanza */*stanza*/ )
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

  static PubSubManager::AffiliationType affiliationType( const std::string& affiliation )
  {
    PubSubManager::AffiliationType affType = PubSubManager::AffiliationInvalid;
    if( affiliation == "none" )
      affType = PubSubManager::AffiliationNone;
    else if( affiliation == "publisher" )
      affType = PubSubManager::AffiliationPublisher;
    else if( affiliation == "owner" )
      affType = PubSubManager::AffiliationOwner;
    else if( affiliation == "outcast" )
      affType = PubSubManager::AffiliationOutcast;
    return affType;
  }

  bool PubSubManager::handleIqID( Stanza *stanza, int context )
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
              //handleSubscriptionResult( jid, node, sid, subType, SubscriptionErrorNone );
            }
            break;
          }
          case Unsubscription:
          {
            const std::string& jid  = stanza->findAttribute( "to" ),
                               srv  = stanza->findAttribute( "from" );
            if( jid.empty() || srv.empty() )
              return 0;
            //handleUnsubscriptionResult( jid, srv, UnsubscriptionErrorNone );
            break;
          }
          case RequestSubscriptionList:
          {
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
              //handleSubscriptionListResult( stanza->from(), subMap );
            }
            break;
          }
          case RequestAffiliationList:
          {
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
              //handleAffiliationListResult( stanza->from(), affMap );
            }
            break;
          }
          case RequestOptions:
          {
            Tag *ps = stanza->findChild( "pubsub", "xmlns", XMLNS_PUBSUB );
            Tag *options = ps->findChild( "options" );
            const DataForm dataForm( options->findChild( "x" ) );
            //handleOptions( stanza->from(), options->findAttribute("node"), dataForm, OptionRequestErrorNone );
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
            Tag *subscription = ps->findChild( "subscribe" );
            if( !subscription )
              return false;
            const std::string& node = subscription->findAttribute( "node" ),
                               jid  = subscription->findAttribute( "jid" );
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
            //handleSubscriptionResult( jid, node, "", SubscriptionNone, errorType );
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
            //handleUnsubscriptionResult( jid, node, errorType );
            break;
          }
          case RequestSubscriptionList:
          {
            const std::string& node = stanza->findAttribute( "node" ),
                               jid  = stanza->findAttribute( "jid" );
            Tag* error = stanza->findChild( "error" );
            if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                /* feature='retrieve-subscriptions'/> */
            {
              //handleSubscriptionListError( jid, node );
            }
            else
              return false;
            break;
          }
          case RequestAffiliationList:
          {
            const std::string& node = stanza->findAttribute( "node" ),
                               jid  = stanza->findAttribute( "jid" );
            Tag* error = stanza->findChild( "error" );
            if( error->hasChild( "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS ) &&
                error->hasChild( "unsupported", "xmlns", XMLNS_PUBSUB_ERRORS ) )
                /* feature='retrieve-affiliations'/> */
            {
              //handleAffiliationListError( jid, node );
            }
            else
              return false;
            break;
          }
          case RequestOptions:
          {
            /*
            if( )
              errorType = OptionRequestUnprivileged;
              errorType = OptionRequestUnsubscribed;
              errorType = OptionRequestNodeAndJID;
              errorType = OptionRequestMissingSID;
              errorType = OptionRequestInvalidSID;
              errorType = OptionRequestUnsupported;
              errorType = OptionRequestItemNotFound;
            */
            break;
          }
        }
        break;
      }
      default:
        return false;
    }
    return true;
  }

}

#include "error.h"
#include "tag.h"


/*
 - do we need tag() / how to save the XMLNS's (ie copy string or enumerated type) ?
 - std::pair / templates on all platforms ?
 - index based lookup offers short code as well as marking a clear
   relashionship between string value and error code. It also ease
   adding/modifying/removing values and is much less error prone.
*/

namespace gloox
{
  typedef const char * StrType;

  template< typename E >
  static inline int lookup( const std::string& str,
                            const std::pair< StrType, E > * tab,
                            int size )
  {
    int i=0;
    while( i < size && str != tab[i].first ) ++i;
    return i;
  }

  typedef std::pair< StrType, GenericErrorType > GenericErrorValue;

  /* "generic" error values */
  static const GenericErrorValue genericErrorValues[] = {
    GenericErrorValue("auth",   Auth),
    GenericErrorValue("cancel", Cancel),
    GenericErrorValue("modify", Modify)
  };

  typedef std::pair< StrType, ErrorType > ErrorValue;

  /* "first level" error values */
  static const ErrorValue errorValues[] = {
    ErrorValue( "bad-request",                BadRequest ),
    ErrorValue( "item-not-found",             ItemNotFound ),
    ErrorValue( "forbidden",                  Forbidden ),
    ErrorValue( "feature-not-implemented",    FeatureNotImplemented ),
    ErrorValue( "not-acceptable",             NotAcceptable ),
    ErrorValue( "not-allowed",                NotAllowed ),
    ErrorValue( "conflict",                   Conflict ),
    ErrorValue( "payment-required",           PaymentRequired ),
    ErrorValue( "registration-required",      RegistrationRequired ),
    ErrorValue( "service-unavailable",        ServiceUnavailable )
  };

  typedef std::pair< StrType, SubErrorType > SubErrorValue;

  /* "second level" error values */
  static const SubErrorValue subErrorValues[] = {
    SubErrorValue( "closed-node",                    ClosedNode ),
    SubErrorValue( "configuration-required",         ConfigurationRequired ),
    SubErrorValue( "invalid-jid",                    InvalidJid ),
    SubErrorValue( "invalid-options",                InvalidOptions ),
    SubErrorValue( "invalid-payload",                InvalidPayload ),
    SubErrorValue( "invalid-subid",                  InvalidSubid ),
    SubErrorValue( "item-forbidden",                 ItemForbidden ),
    SubErrorValue( "item-required",                  ItemRequired ),
    SubErrorValue( "jid-required",                   JidRequired ),
    SubErrorValue( "max-nodes-exceeded",             MaxNodesExceeded ),
    SubErrorValue( "nodeid-required",                NodeidRequired),
    SubErrorValue( "not-in-roster-group",            NotInRosterGroup),
    SubErrorValue( "not-subscribed",                 NotSubscribed),
    SubErrorValue( "payload-too-big",                PayloadTooBig),
    SubErrorValue( "payload-required",               PayloadRequired),
    SubErrorValue( "pending-subscription",           PendingSubscription),
    SubErrorValue( "presence-subscription-required", PresenceSubscriptionRequired),
    SubErrorValue( "subid-required",                 SubidRequired),
    SubErrorValue( "unsupported",                    Unsupported),
    SubErrorValue( "unsupported-access-model",       UnsupportedAccessModel)
  };

  typedef std::pair< StrType, FeatureType > FeatureValue;

  /* feature values */
  static const FeatureValue featureValues[] = {
    FeatureValue( "collections",                Collections ),
    FeatureValue( "config-node",                ConfigNode ),
    FeatureValue( "create-and-configure",       CreateAndconfigure ),
    FeatureValue( "create-nodes",               CreateNodes ),
    FeatureValue( "delete-any",                 DeleteAny ),
    FeatureValue( "delete-nodes",               DeleteNodes ),
    FeatureValue( "get-pending",                GetPending ),
    FeatureValue( "instant-nodes",              InstantNodes ),
    FeatureValue( "item-ids",                   ItemIds ),
    FeatureValue( "leased-subscription",        LeasedSubscription ),
    FeatureValue( "manage-subscriptions",       ManageSubscriptions ),
    FeatureValue( "meta-data",                  MetaData ),
    FeatureValue( "modify-affiliations",        ModifyAffiliations ),
    FeatureValue( "multi-collection",           MultiCollection ),
    FeatureValue( "multi-subscribe",            MultiSubscribe ),
    FeatureValue( "outcast-affiliation",        OutcastAffiliation ),
    FeatureValue( "persistent-items",           PersistentItems ),
    FeatureValue( "presence-notifications",     PresenceNotifications ),
    FeatureValue( "publish",                    Publish ),
    FeatureValue( "publisher-affiliation",      PublisherAffiliation ),
    FeatureValue( "purge-nodes",                PurgeNodes ),
    FeatureValue( "retract-items",              RetractItems ),
    FeatureValue( "retrieve-affiliations",      RetrieveAffiliations ),
    FeatureValue( "retrieve-default",           RetrieveDefault ),
    FeatureValue( "retrieve-items",             RetrieveItems ),
    FeatureValue( "retrieve-subscriptions",     RetrieveSubscriptions ),
    FeatureValue( "subscribe",                  Subscribe ),
    FeatureValue( "subscription-options",       SubscriptionOptions ),
    FeatureValue( "subscription-notifications", SubscriptionNotifications )
  };

  static const int nbErrors = sizeof(errorValues)/sizeof(ErrorValue);
  static const int nbFeatures = sizeof(featureValues)/sizeof(FeatureValue);
  static const int nbSubErrors = sizeof(subErrorValues)/sizeof(SubErrorValue);
  static const int nbGenericErrors = sizeof(genericErrorValues)/sizeof(GenericErrorValue);

  Error::Error( Tag * error )
    : StanzaExtension( ExtError ), m_genericType( GenericErrorNone ),
      m_type( ErrorNone ), m_subType( SubErrorNone ), m_feature( FeatureNone )
  {
    const std::string& genType = error->findAttribute( "type" );
    int i = lookup( genType, genericErrorValues, nbGenericErrors );
    if( i == nbGenericErrors )
      return;
    m_genericType = genericErrorValues[i].second;
    Tag::TagList::iterator it = error->children().begin();
    i = lookup( (*it)->name(), errorValues, nbErrors );
    if( i == nbErrors )
      return;
    m_type = errorValues[i].second;
    // xmlns...
    if( ++it != error->children().end() )
    {
      i = lookup( (*it)->name(), subErrorValues, nbSubErrors );
      if( i == nbSubErrors )
        return;
      m_subType = subErrorValues[i].second;
      // xmlns... 
      if( m_subType  == Unsupported )
      {
        i = lookup( (*it)->findAttribute( "feature" ), featureValues, nbFeatures );
        if( i == nbFeatures )
          return;
        m_feature = featureValues[i].second;
      }
    }
  }

  Tag * Error::tag() const
  {
    if( m_genericType == GenericErrorNone || m_type == ErrorNone )
      return 0;
    Tag * error = new Tag( "error", "type", genericErrorValues[m_genericType].first );
    new Tag( error, errorValues[m_type].first, "xmlns", "????????" );
    if( m_subType != SubErrorNone )
      new Tag( error, subErrorValues[m_subType].first, "xmlns", "????????" );
    return error;
  }

}

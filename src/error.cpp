#include "error.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  /* "generic" error values */
  static const char * genericErrorValues[] = {
    "auth",   
    "cancel", 
    "modify", 
  };

  /* "first level" error values */
  static const char * errorValues[] = {
     "bad-request",             
     "item-not-found",          
     "forbidden",               
     "feature-not-implemented", 
     "not-acceptable",          
     "not-allowed",             
     "conflict",                
     "payment-required",        
     "registration-required",   
     "service-unavailable",      
  };

  /* "second level" error values */
  static const char * subErrorValues[] = {
     "closed-node",                    
     "configuration-required",         
     "invalid-jid",                    
     "invalid-options",                
     "invalid-payload",                
     "invalid-subid",                  
     "item-forbidden",                 
     "item-required",                  
     "jid-required",                   
     "max-nodes-exceeded",             
     "nodeid-required",                
     "not-in-roster-group",            
     "not-subscribed",                 
     "payload-too-big",                
     "payload-required",               
     "pending-subscription",           
     "presence-subscription-required", 
     "subid-required",                 
     "unsupported",                    
     "unsupported-access-model",       
  };

  /* feature values */
  static const char * featureValues[] = {
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
  };

  Error::Error( const Tag * error )
    : StanzaExtension( ExtError ), m_genericType( GenericErrorNone ),
      m_type( ErrorNone ), m_subType( SubErrorNone ), m_feature( FeatureNone )
  {
    const std::string& genType = error->findAttribute( TYPE );
    m_genericType = (GenericErrorType)util::lookup( genType, genericErrorValues );
    Tag::TagList::const_iterator it = error->children().begin();
    m_type = (ErrorType)util::lookup( (*it)->name(), errorValues );
    m_xmlns1 = (*it)->findAttribute( XMLNS );
    if( ++it != error->children().end() )
    {
      m_subType = (SubErrorType)util::lookup( (*it)->name(), subErrorValues );
      m_xmlns2 = (*it)->findAttribute( XMLNS );
      const std::string& feat = (*it)->findAttribute( "feature" );
      m_feature = (FeatureType)util::lookup( feat, featureValues );
    }
  }
/*
  Error& Error::operator=( const Tag * error)
  {
    //setValues( error );
    const std::string& genType = error->findAttribute( TYPE );
    if( genType.empty() )
    {
      printf( "error: no basic error type\n" );
    }
    int i = util::lookup( genType, genericErrorValues, nbGenericErrors );
    if( i == nbGenericErrors )
      return;
    m_genericType = genericErrorValues[i].second;
    Tag::TagList::const_iterator it = error->children().begin();
    i = util::lookup( (*it)->name(), errorValues, nbErrors );
    if( i == nbErrors )
      return;
    m_type = errorValues[i].second;
    m_xmlns1 = (*it)->findAttribute( XMLNS );
    if( ++it != error->children().end() )
    {
      i = util::lookup( (*it)->name(), subErrorValues, nbSubErrors );
      if( i == nbSubErrors )
        return;
      m_subType = subErrorValues[i].second;
      m_xmlns2 = (*it)->findAttribute( XMLNS );
      const std::string& feat = (*it)->findAttribute( "feature" );
      if( feat.empty() )
        return;
      i = util::lookup( feat, featureValues, nbFeatures );
      if( i == nbFeatures )
        return;
      m_feature = featureValues[i].second;
    }
    return this;
  }

  void Error::setValues( const * tag )
  {
  }
*/
  Tag * Error::tag() const
  {
    if( m_genericType == GenericErrorNone || m_type == ErrorNone )
      return 0;
    Tag * error = new Tag( "error" );
    error->addAttribute( TYPE, util::lookup( m_genericType, genericErrorValues ) );
    new Tag( error, util::lookup( m_type, errorValues ), XMLNS, m_xmlns1 );
    if( m_subType != SubErrorNone )
    {
      Tag * tag = new Tag( error, util::lookup( m_subType, subErrorValues ), XMLNS, m_xmlns2 );
      if( m_subType == Unsupported )
        tag->addAttribute( "feature", m_feature );      
    }
    return error;
  }

}

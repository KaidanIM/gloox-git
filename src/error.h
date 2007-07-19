/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef __ERROR_H__
#define __ERROR_H__

#include "stanzaextension.h"
#include <string>

namespace gloox
{

  enum GenericErrorType
  {
    Auth,
    Cancel,
    Modify,
    GenericErrorNone
  };

  enum ErrorType
  {
    BadRequest,
    ItemNotFound,
    Forbidden,
    FeatureNotImplemented,
    NotAcceptable,
    NotAllowed,
    Conflict,
    PaymentRequired,
    RegistrationRequired,
    ServiceUnavailable,
    ErrorNone
  };

  enum SubErrorType
  {
    ClosedNode,
    ConfigurationRequired,
    InvalidJid,
    InvalidOptions,
    InvalidPayload,
    InvalidSubid,
    ItemForbidden,
    ItemRequired,
    JidRequired,
    MaxNodesExceeded,
    NodeidRequired,
    NotInRosterGroup,
    NotSubscribed,
    PayloadTooBig,
    PayloadRequired,
    PendingSubscription,
    PresenceSubscriptionRequired,
    SubidRequired,
    Unsupported,
    UnsupportedAccessModel,
    SubErrorNone
  };

  /**
   * For use with Unsupported SubErrorType
   */
  enum FeatureType
  {
    Collections,
    ConfigNode,
    CreateAndconfigure,
    CreateNodes,
    DeleteAny,
    DeleteNodes,
    GetPending,
    InstantNodes,
    ItemIds,
    LeasedSubscription,
    ManageSubscriptions,
    MetaData,
    ModifyAffiliations,
    MultiCollection,
    MultiSubscribe,
    OutcastAffiliation,
    PersistentItems,
    PresenceNotifications,
    Publish,
    PublisherAffiliation,
    PurgeNodes,
    RetractItems,
    RetrieveAffiliations,
    RetrieveDefault,
    RetrieveItems,
    RetrieveSubscriptions,
    Subscribe,
    SubscriptionOptions,
    SubscriptionNotifications,
    FeatureNone
  };

  /**
   *  \todo Move enums into Error (eg Error::Unsupported).
   *  \todo check for completeness of all supported error type, including unsupported stuff.
   *  \todo copy ctor (and assignment operator ?), just in case it ever gets used.
   */
  class Error : public StanzaExtension
  {
    public:

      Error()
        : StanzaExtension( ExtError ), m_genericType( GenericErrorNone ),
          m_type( ErrorNone ), m_subType( SubErrorNone ), m_feature( FeatureNone )
      {}

      Error( const Tag * );

      Error( GenericErrorType genType,
             ErrorType type,
             SubErrorType subType = SubErrorNone,
             FeatureType feat = FeatureNone )
        : StanzaExtension( ExtError ), m_genericType( genType ),
          m_type( type ), m_subType( subType ), m_feature( feat )
      {}

      //Error( const Error& error );
      //Error& operator=( const Tag * ) { return this; }

      GenericErrorType gentype() const { return m_genericType; }
      ErrorType type() const { return m_type; }
      SubErrorType subtype() const { return m_subType; }
      FeatureType feature() const { return m_feature; }
      Tag * tag() const;

    private:
      void setValues( const Tag * tag );
      GenericErrorType m_genericType;
      ErrorType m_type;
      SubErrorType m_subType;
      FeatureType m_feature;
      std::string m_xmlns1;
      std::string m_xmlns2;
  };

}

#endif /* __ERROR_H__ */

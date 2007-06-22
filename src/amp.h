/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef ADVANCEDMESSAGEPROCESSING_H__
#define ADVANCEDMESSAGEPROCESSING_H__

#include "stanzaextension.h"
#include "jid.h"
#include <time.h>

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an implementation of XEP-0079 (Advanced Message Processing).
   *
   * XEP Version: 1.2
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class AMP : public StanzaExtension
  {

    public:

      /**
       * @brief Constructs a new object from the given Tag.
       * @param tag The AMP Tag to parse.
       */
      AMP( const Tag *tag );

      /**
       * @brief Virtual Destructor.
       */
      virtual ~AMP() {}

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

      /**
       * @brief Possible types for a rule's condition.
       */
      enum ConditionType {
        ConditionDeliver,       /**< Ensures (non-)delivery of the message */
        ConditionExpireAt,      /**< Ensures delivery only before a certain time (UTC) */
        ConditionMatchResource, /**< Ensures delivery only to a specific resource type */
        ConditionInvalid        /**< Invalid condition */
      };

      /**
       * @brief Possible actions to take when the corresponding condition is met.
       */
      enum ActionType {

        ActionAlert,       /**< Sends back a message stanza with an 'alert' status */
        ActionError,       /**< Sends back a message stanza with an error type */
        ActionDrop,        /**< Silently ignore the message */
        ActionNotify,      /**< Sends back a message stanza with a 'notify' status */
        ActionInvalid      /**< Invalid action */
      };

      enum DeliverType {
        DeliverDirect,      /**< The message would be immediately delivered to the intended
                             *   recipient or routed to the next hop. */
        DeliverForward,     /**< The message would be forwarded to another XMPP address or
                             *   account. */
        DeliverGateway,     /**< The message would be sent through a gateway to an address
                             *   or account on a non-XMPP system. */
        DeliverNone,        /**< The message would not be delivered at all (e.g., because
                             *   the intended recipient is offline and message storage is
                             *   not enabled). */
        DeliverStored,      /**< The message would be stored offline for later delivery
                             *   to the intended recipient. */
        DeliverInvalid      /**< Invalid deliver value */
      };

      enum MatchResourceType {
        MatchResourceAny,         /**< Destination resource matches any value, effectively
                                   *   ignoring the intended resource. */
        MatchResourceExact,       /**< Destination resource exactly matches the intended
                                   *   resource. */
        MatchResourceOther,        /**< Destination resource matches any value except for
                                   *   the intended resource. */
        MatchResourceInvalid      /**< Invalid match-resource value */
      };

      /**
       * @brief Describes an advanced message processing rule.
       */
      struct Rule
      {
          Rule( const std::string& condition,
                const std::string& action,
                const std::string& value );

          Tag* tag() const;

          ConditionType m_condition;
          ActionType m_action;
          union {
            DeliverType deliver;
            MatchResourceType matchresource;
            time_t expireat;
          };
      };

      typedef std::list< Rule * > RuleList;

    private:
      bool m_valid;
      RuleList m_rules;
  };

}

#endif // ADVANCEDMESSAGEPROCESSING_H__

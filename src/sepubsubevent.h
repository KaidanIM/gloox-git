/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef SEPUBSUBEVENT_H__
#define SEPUBSUBEVENT_H__

#include "stanzaextension.h"
#include "pubsub.h"
#include "gloox.h"

namespace gloox
{
  class Tag;

  /**
   * @brief This is an implementation of a PubSub Notification StanzaExtension.
   *
   * @author Vincent Thomasset <vthomasset@gmail.com>
   * @since 1.0
   */
  class SEPubSubEvent : public StanzaExtension
  {
    public:

      /**
       *
       */
      struct ItemOperation
      {
        /**
         *
         */
        ItemOperation( bool _remove, const std::string& _item, const Tag* _payload = 0)
          : remove( _remove ), item( _item ), payload( _payload ) {}

        bool remove;
        std::string item;
        const Tag* payload;
      };

      /**
       * A list of ItemOperations.
       */
      typedef std::list<ItemOperation*> ItemOperationList;

      /**
       * PubSub event notification Stanza Extension.
       * @param event A tag to parse.
       */
      SEPubSubEvent( const Tag* event );

      /**
       * Virtual destructor.
       */
      virtual ~SEPubSubEvent();

      /**
       * Returns the event's type.
       * @return The event's type.
       */
      PubSub::EventType type() const { return m_type; }

      /**
       * Returns the list of subscription IDs for which this notification
       * is valid. May be 0.
       * @return The list of subscription IDs.
       */
      const StringList* subscriptions() const { return m_subscriptionIDs; }

      /**
       * Returns the list of ItemOperations for EventItems(Retract) notification.
       * May be 0.
       * @return The list of ItemOperations.
       */
      const ItemOperationList* items() const { return m_itemOperations; }

      /**
       * Returns the node's ID for which the notification is sent.
       * @return The node's ID.
       */
      const std::string& node() { return m_node; }

      // reimplemented from StanzaExtension
      const std::string& filterString() const;

      // reimplemented from StanzaExtension
      StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new SEPubSubEvent( tag );
      }

      // reimplemented from StanzaExtension
      Tag* tag() const;

    private:

      PubSub::EventType m_type;
      std::string m_node;
      StringList* m_subscriptionIDs;
      Tag* m_config;
      ItemOperationList* m_itemOperations;

  };

}

#endif // SEPPUBSUBEVENT_H__

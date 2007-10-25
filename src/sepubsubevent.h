#ifndef SEPUBSUBEVENT_H_
#define SEPUBSUBEVENT_H_

#include "stanzaextension.h"
#include "pubsub.h"
#include "gloox.h"

namespace gloox
{
  class Tag;

  class SEPubSubEvent : public StanzaExtension
  {
    public:

      struct ItemOperation
      {
        ItemOperation( bool _remove, const std::string& _item, const Tag* _payload = 0)
          : remove( _remove ), item( _item ), payload( _payload ) {}

        bool remove;
        std::string item;
        const Tag* payload;
      };

      typedef std::list< ItemOperation* > ItemOperationList;
      typedef std::list< std::string > StringList;

      /**
       * PubSub event notification Stanza Extension.
       */
      SEPubSubEvent( const Tag* event );

      /**
       * Virtual dtor.
       */
      virtual ~SEPubSubEvent();

      /**
       * Virtual dtor.
       */
      PubSub::EventType type() const { return m_type; }

      /**
       * Returns the list of subscription ID's for which this notification
       * is valid (maybe be null).
       */
      const StringList* subscriptions() const { return m_subscriptionIDs; }

      /**
       * Returns the list of ItemOperation's for EventItems(Retract) notification.
       */
      const ItemOperationList* items() const { return m_itemOperations; }

      /**
       * Returns the node ID of the node for which the notification is sent.
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

#endif /* SEPPUBSUBEVENT_H_ */

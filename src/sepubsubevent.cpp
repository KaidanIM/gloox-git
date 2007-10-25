#include "sepubsubevent.h"
#include "tag.h"
#include "util.h"

namespace gloox
{
  static const char* eventTypeValues[] = {
    "collection",
    "configure",
    "delete",
    "items",
    "items",
    "purge"
  };

  SEPubSubEvent::SEPubSubEvent( const Tag* event )
    : StanzaExtension( ExtPSEvent ), m_type( PubSub::EventUnknown ),
      m_subscriptionIDs( 0 ), m_config( 0 ), m_itemOperations( 0 )
  {
    const TagList& events = event->children();

    if( !event || event->name() != "event" || events.empty() )
      return;

    TagList::const_iterator it = events.begin();
    for( ; it != events.end(); ++it )
    {
      const Tag* tag = (*it);
      m_type = (PubSub::EventType)util::lookup( tag->name(), eventTypeValues );

      switch( m_type )
      {
        case PubSub::EventCollection:
          tag = tag->findChild( "node" );
          if( tag )
          {
            m_node = tag->findAttribute( "id" );
            m_config = tag->findChild( "x" );
          }
          break;

        case PubSub::EventConfigure:
        case PubSub::EventDelete:
        case PubSub::EventPurge:
          m_node = tag->findAttribute( "node" );
          m_config = tag->findChild( "x" );
          if( m_config )
            m_config = m_config->clone();
          break;

        case PubSub::EventItems:
        case PubSub::EventItemsRetract:
        {
          m_itemOperations = new ItemOperationList();

          m_node = tag->findAttribute( "node" );
          const TagList& items = tag->children();
          TagList::const_iterator itt = items.begin();
          for( ; itt != items.end(); ++itt )
          {
            tag = (*itt);

            const Tag* x = tag->findChild( "x" );
            ItemOperation* op = new ItemOperation( tag->name() == "retract",
                                                   tag->findAttribute( "id" ),
                                                   x ? x->clone() : 0 );
            m_itemOperations->push_back( op );
          }
          break;
        }

        case PubSub::EventUnknown:
        {
          if( m_type == PubSub::EventUnknown )
          {
            if( tag->name() != "headers" || m_subscriptionIDs != 0 )
            {
              m_valid = false;
              return;
            }

            m_subscriptionIDs = new StringList();

            const TagList& headers = tag->children();
            TagList::const_iterator ith = headers.begin();
            for( ; ith != headers.end(); ++ith )
            {
              if( (*ith)->hasAttribute( "name", "pubsub#subid" ) )
                m_subscriptionIDs->push_back( (*ith)->cdata() );
            }
          }
        }
      }
    }

    m_valid = true;
  }

  SEPubSubEvent::~SEPubSubEvent()
  {
    delete m_subscriptionIDs;
    delete m_config;
    if( m_itemOperations )
    {
      util::clearList( *m_itemOperations );
      delete m_itemOperations;
    }
  }

  const std::string SEPubSubEvent::filterString() const
  {
    static const std::string filter = "/message/event";
    return filter;
  }

  Tag* SEPubSubEvent::tag() const
  {
    if( !m_valid )
      return 0;

    Tag* event = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
    Tag* child = new Tag(  event , util::lookup( m_type, eventTypeValues ) );

    Tag* item;

    switch( m_type )
    {
      case PubSub::EventCollection:
      {
        item = new Tag( child, "node", "id", m_node );
        if( m_config )
          item->addChildCopy( item );
        break;
      }

      case PubSub::EventDelete:
      {
        child->addAttribute( "node", m_node );
        break;
      }

      case PubSub::EventPurge:
      {
        child->addAttribute( "node", m_node );
        break;
      }

      case PubSub::EventConfigure:
      {
        item = new Tag( child, "node", m_node );
        break;
      }

      case PubSub::EventItems:
      case PubSub::EventItemsRetract:
      {
        child->addAttribute( "node", m_node );
        if( m_itemOperations )
        {
          Tag* item;
          ItemOperation* op;
          ItemOperationList::const_iterator itt = m_itemOperations->begin();
          for( ; itt != m_itemOperations->end(); ++itt )
          {
            op = (*itt);
            item = new Tag( child, op->remove ? "retract" : "item", "id", op->item );
            if( op->payload )
              item->addChildCopy( op->payload );
          }
        }
        break;
      }

      default:
        delete event;
        return 0;
    }

    if( m_subscriptionIDs )
    {
      Tag* headers = new Tag( event, "headers", XMLNS, "http://jabber.org/protocol/shim" );
      StringList::const_iterator it = m_subscriptionIDs->begin();
      for( ; it != m_subscriptionIDs->end(); ++it )
      {
        (new Tag( headers, "header", "name", "pubsub#subid" ))->setCData( (*it) );
      }

    }

    return event;
  }

}

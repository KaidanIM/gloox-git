/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "disco.h"
#include "discohandler.h"
#include "clientbase.h"
#include "disconodehandler.h"
#include "seversion.h"
#include "util.h"


namespace gloox
{

  // ---- Disco::Identity ----
  Disco::Identity::Identity( const Tag* tag )
  {
    if( !tag || tag->name() != "identity" )
      return;

    m_category = tag->findAttribute( "category" );
    m_type = tag->findAttribute( "type" );
    m_name = tag->findAttribute( "name" );
  }

  Tag* Disco::Identity::tag() const
  {
    if( m_category.empty() || m_type.empty() )
      return 0;

    Tag* i = new Tag( "identity" );
    i->addAttribute( "category", m_category );
    i->addAttribute( "type", m_type );

    if( !m_name.empty() )
      i->addAttribute( "name", m_name );

    return i;
  }
  // ---- ~Disco::Identity ----

  // ---- Disco::Info ----
  Disco::Info::Info( const std::string& node )
    : StanzaExtension( ExtDiscoInfo ), m_node( node )
  {
  }

  Disco::Info::Info( const Tag* tag )
    : StanzaExtension( ExtDiscoInfo )
  {
    if( !tag || tag->name() != "query" || tag->xmlns() != XMLNS_DISCO_INFO )
      return;

    m_node = tag->findAttribute( "node" );

    const TagList& l = tag->children();
    TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      const std::string& name = (*it)->name();
      if( name == "identity" )
        m_identities.push_back( new Identity( (*it) ) );
      else if( name == "feature" && (*it)->hasAttribute( "var" ) )
        m_features.push_back( (*it)->findAttribute( "var" ) );
    }
  }

  Disco::Info::~Info()
  {
    util::clearList( m_identities );
  }

  const std::string& Disco::Info::filterString() const
  {
    static const std::string filter = "/iq/query[@xmlns='" + XMLNS_DISCO_INFO + "']";
    return filter;
  }

  Tag* Disco::Info::tag() const
  {
    Tag* t = new Tag( "query", XMLNS, XMLNS_DISCO_INFO );

    if( !m_node.empty() )
      t->addAttribute( "node", m_node );

    IdentityList::const_iterator it_i = m_identities.begin();
    for( ; it_i != m_identities.end(); ++it_i )
      t->addChild( (*it_i)->tag() );

    StringList::const_iterator it_f = m_features.begin();
    for( ; it_f != m_features.end(); ++it_f )
      new Tag( t, "feature", "var", (*it_f) );

    return t;
  }
  // ---- ~Disco::Info ----

  // ---- Disco::Item ----
  Disco::Item::Item( const Tag* tag )
  {
    if( !tag || tag->name() != "item" )
      return;

    m_jid = tag->findAttribute( "jid" );
    m_jid = tag->findAttribute( "node" );
    m_name = tag->findAttribute( "name" );
  }

  Tag* Disco::Item::tag() const
  {
    if( !m_jid )
      return 0;

    Tag* i = new Tag( "item" );
    i->addAttribute( "jid", m_jid.full() );

    if( !m_node.empty() )
      i->addAttribute( "node", m_node );
    if( !m_name.empty() )
      i->addAttribute( "name", m_name );

    return i;
  }
  // ---- Disco::Item ----

  // ---- Disco::Items ----
  Disco::Items::Items( const std::string& node )
  : StanzaExtension( ExtDiscoInfo ), m_node( node )
  {
  }

  Disco::Items::Items( const Tag* tag )
  : StanzaExtension( ExtDiscoItems )
  {
    if( !tag || tag->name() != "query" || tag->xmlns() != XMLNS_DISCO_ITEMS )
      return;

    m_node = tag->findAttribute( "node" );

    const TagList& l = tag->children();
    TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      const std::string& name = (*it)->name();
      if( name == "item" )
        m_items.push_back( new Item( (*it) ) );
    }
  }

  Disco::Items::~Items()
  {
    util::clearList( m_items );
  }

  const std::string& Disco::Items::filterString() const
  {
    static const std::string filter = "/iq/query[@xmlns='" + XMLNS_DISCO_ITEMS + "']";
    return filter;
  }

  Tag* Disco::Items::tag() const
  {
    Tag* t = new Tag( "query", XMLNS, XMLNS_DISCO_ITEMS );

    if( !m_node.empty() )
      t->addAttribute( "node", m_node );

    ItemList::const_iterator it_i = m_items.begin();
    for( ; it_i != m_items.end(); ++it_i )
      t->addChild( (*it_i)->tag() );

    return t;
  }
  // ---- Disco::Items ----

  // ---- Disco ----
  Disco::Disco( ClientBase* parent )
    : m_parent( parent )
  {
    addFeature( XMLNS_VERSION );
    addFeature( XMLNS_DISCO_INFO );
    addFeature( XMLNS_DISCO_ITEMS );
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_DISCO_INFO );
      m_parent->registerIqHandler( this, XMLNS_DISCO_ITEMS );
      m_parent->registerIqHandler( this, XMLNS_VERSION );
      m_parent->registerStanzaExtension( new Disco::Info() );
    }
  }

  Disco::~Disco()
  {
    if( m_parent )
    {
      m_parent->removeIqHandler( this, XMLNS_DISCO_INFO );
      m_parent->removeIqHandler( this, XMLNS_DISCO_ITEMS );
      m_parent->removeIqHandler( this, XMLNS_VERSION );
    }
  }

  static void addFeatures( const StringList& features, Tag* parent )
  {
    StringList::const_iterator it = features.begin();
    for( ; it != features.end(); ++it )
      new Tag( parent, "feature", "var", (*it) );
  }

  bool Disco::handleIq( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Get:
        if( iq->xmlns() == XMLNS_VERSION )
        {
          IQ re( IQ::Result, iq->from(), iq->id(), XMLNS_VERSION );
          re.addExtension( new SEVersion( m_versionName, m_versionVersion, m_versionOs ) );
          m_parent->send( re );
        }
        else if( iq->xmlns() == XMLNS_DISCO_INFO && iq->query() )
        {
          IQ re( IQ::Result, iq->from(), iq->id(), XMLNS_DISCO_INFO );
          Tag* query = re.query();

          Tag* q = iq->query();
          const std::string& node = q->findAttribute( "node" );
          if( !node.empty() )
          {
            query->addAttribute( "node", node );
            new Tag( query, "feature", "var", XMLNS_DISCO_INFO );

            DiscoNodeHandlerMap::const_iterator it = m_nodeHandlers.find( node );
            if( it != m_nodeHandlers.end() )
            {
              std::string name;
              DiscoNodeHandlerList::const_iterator in = (*it).second.begin();
              for( ; in != (*it).second.end(); ++in )
              {
                const StringMap& identities = (*in)->handleDiscoNodeIdentities( node, name );
                StringMap::const_iterator im = identities.begin();
                for( ; im != identities.end(); ++im )
                {
                  Tag* i = new Tag( query, "identity" );
                  i->addAttribute( "category", (*im).first );
                  i->addAttribute( TYPE, (*im).second );
                  i->addAttribute( "name", name );
                }
                addFeatures( (*in)->handleDiscoNodeFeatures( node ), query );
              }
            }
          }
          else
          {
            Tag* i = new Tag( query, "identity" );
            i->addAttribute( "category", m_identityCategory );
            i->addAttribute( TYPE, m_identityType );
            i->addAttribute( "name", m_versionName );

            addFeatures( m_features, query );
          }

          m_parent->send( re );
        }
        else if( iq->xmlns() == XMLNS_DISCO_ITEMS && iq->query() )
        {
          IQ re( IQ::Result, iq->from(), iq->id(), XMLNS_DISCO_ITEMS );
          Tag* query = re.query();
          const Tag* q = iq->query();
          const std::string& node = q->findAttribute( "node" );
          query->addAttribute( "node", node );
          DiscoNodeHandlerMap::const_iterator it = m_nodeHandlers.find( node );
          if( it != m_nodeHandlers.end() )
          {
            DiscoNodeHandlerList::const_iterator in = (*it).second.begin();
            for( ; in != (*it).second.end(); ++in )
            {
              const DiscoNodeItemList& items = (*in)->handleDiscoNodeItems( node );
              DiscoNodeItemList::const_iterator it = items.begin();
              for( ; it != items.end(); ++it )
              {
                const DiscoNodeItem& item = (*it);
                Tag* i = new Tag( query, "item" );
                i->addAttribute( "jid",  item.jid.empty() ? m_parent->jid().full() : item.jid );
                i->addAttribute( "node", item.node );
                i->addAttribute( "name", item.name );
              }
            }
          }

          m_parent->send( re );
        }
        return true;
        break;

      case IQ::Set:
      {
        bool res = false;
        DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
        for( ; it != m_discoHandlers.end(); ++it )
        {
          if( (*it)->handleDiscoSet( iq ) )
            res = true;
        }
        return res;
        break;
      }

      default:
        break;
    }
    return false;
  }

  void Disco::handleIqID( IQ* iq, int context )
  {
    DiscoHandlerMap::iterator it = m_track.find( iq->id() );
    if( it != m_track.end() )
    {
      switch( iq->subtype() )
      {
        case IQ::Result:
          switch( context )
          {
            case GET_DISCO_INFO:
              (*it).second.dh->handleDiscoInfoResult( iq, (*it).second.context );
              break;
            case GET_DISCO_ITEMS:
              (*it).second.dh->handleDiscoItemsResult( iq, (*it).second.context );
              break;
           }
        break;

        case IQ::Error:
          (*it).second.dh->handleDiscoError( iq, (*it).second.context );
          break;

        default:
          break;
      }

      m_track.erase( it );
    }
  }

  void Disco::getDisco( const JID& to, const std::string& node, DiscoHandler* dh, int context,
                        IdType idType, const std::string& tid )
  {
    const std::string& id = tid.empty() ? m_parent->getID() : tid;

    IQ iq( IQ::Get, to, id, idType == GET_DISCO_INFO ? XMLNS_DISCO_INFO : XMLNS_DISCO_ITEMS );
    if( !node.empty() )
      iq.query()->addAttribute( "node", node );

    DiscoHandlerContext ct;
    ct.dh = dh;
    ct.context = context;
    m_track[id] = ct;
    m_parent->send( iq, this, idType );
  }

  void Disco::setVersion( const std::string& name, const std::string& version, const std::string& os )
  {
    m_versionName = name;
    m_versionVersion = version;
    m_versionOs = os;
  }

  void Disco::setIdentity( const std::string& category, const std::string& type )
  {
    m_identityCategory = category;
    m_identityType = type;
  }

  void Disco::registerNodeHandler( DiscoNodeHandler* nh, const std::string& node )
  {
    m_nodeHandlers[node].push_back( nh );
  }

  void Disco::removeNodeHandler( DiscoNodeHandler* nh, const std::string& node )
  {
    DiscoNodeHandlerMap::iterator it = m_nodeHandlers.find( node );
    if( it != m_nodeHandlers.end() )
    {
      (*it).second.remove( nh );
      if( (*it).second.empty() )
        m_nodeHandlers.erase( it );
    }
  }

}

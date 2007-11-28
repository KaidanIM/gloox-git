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
#include "softwareversion.h"
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

  bool Disco::Info::hasFeature( const std::string& feature ) const
  {
    StringList::const_iterator it = m_features.begin();
    for( ; it != m_features.end() && (*it) != feature; ++it )
      ;
    return it != m_features.end();
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
  // ---- ~Disco::Item ----

  // ---- Disco::Items ----
  Disco::Items::Items( const std::string& node )
    : StanzaExtension( ExtDiscoItems ), m_node( node )
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
  // ---- ~Disco::Items ----

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
      m_parent->registerStanzaExtension( new Disco::Items() );
      m_parent->registerStanzaExtension( new SoftwareVersion() );
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

  bool Disco::handleIq( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Get:
      {
        IQ re( IQ::Result, iq->from(), iq->id() );

        const SoftwareVersion* sv = static_cast<const SoftwareVersion*>( iq->findExtension( ExtVersion ) );
        if( sv )
        {
          re.addExtension( new SoftwareVersion( m_versionName, m_versionVersion, m_versionOs ) );
          m_parent->send( re );
          return true;
        }

        const Info *info = static_cast<const Info*>( iq->findExtension( ExtDiscoInfo ) );
        if( info )
        {
          Info *i = new Info();
          if( !info->node().empty() )
          {
            i->setNode( info->node() );
            IdentityList identities;
            StringList features;
            DiscoNodeHandlerMap::const_iterator it = m_nodeHandlers.find( info->node() );
            if( it != m_nodeHandlers.end() )
            {
              DiscoNodeHandlerList::const_iterator in = (*it).second.begin();
              for( ; in != (*it).second.end(); ++in )
              {
                IdentityList il = (*in)->handleDiscoNodeIdentities( iq->from(), info->node() );
                identities.merge( il );
                StringList fl = (*in)->handleDiscoNodeFeatures( iq->from(), info->node() );
                features.merge( fl );
              }
            }
            i->setIdentities( identities );
            i->setFeatures( features );
          }
          else
          {
            i->setIdentities( m_identities );
            i->setFeatures( m_features );
          }

          re.addExtension( i );
          m_parent->send( re );
          return true;
        }

        const Items *items = static_cast<const Items*>( iq->findExtension( ExtDiscoItems ) );
        if( items )
        {
          Items *i = new Items( items->node() );
          ItemList itemlist;
          DiscoNodeHandlerMap::const_iterator it = m_nodeHandlers.find( items->node() );
          if( it != m_nodeHandlers.end() )
          {
            DiscoNodeHandlerList::const_iterator in = (*it).second.begin();
            for( ; in != (*it).second.end(); ++in )
            {
              ItemList il = (*in)->handleDiscoNodeItems( iq->from(), items->node() );
              itemlist.merge( il );
            }
          }

          i->setItems( itemlist );

          re.addExtension( i );
          m_parent->send( re );
          return true;
        }
        break;
      }

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
            case GetDiscoInfo:
            {
              const Info* di = static_cast<const Info*>( iq->findExtension( ExtDiscoInfo ) );
              if( di )
                (*it).second.dh->handleDiscoInfo( iq->from(), *di, (*it).second.context );
              (*it).second.dh->handleDiscoInfoResult( iq, (*it).second.context );
              break;
            }
            case GetDiscoItems:
            {
              const Items* di = static_cast<const Items*>( iq->findExtension( ExtDiscoItems ) );
              if( di )
                (*it).second.dh->handleDiscoItems( iq->from(), *di, (*it).second.context );
              (*it).second.dh->handleDiscoItemsResult( iq, (*it).second.context );
              break;
            }
          }
          break;

        case IQ::Error:
        {
          (*it).second.dh->handleDiscoError( iq->from(), iq->error(), (*it).second.context );
          (*it).second.dh->handleDiscoError( iq, (*it).second.context );
          break;
        }

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

    IQ iq( IQ::Get, to, id );
    if( idType == GetDiscoInfo )
      iq.addExtension( new Info( node ) );
    else
      iq.addExtension( new Items( node ) );

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

  void Disco::setIdentity( const std::string& category, const std::string& type,
                           const std::string& name )
  {
    util::clearList( m_identities );
    addIdentity( category, type, name );
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

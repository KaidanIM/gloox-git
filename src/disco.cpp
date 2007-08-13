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


namespace gloox
{

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
          IQ* re = new IQ( IQ::Result, iq->from(), iq->id(), XMLNS_VERSION );
          new Tag( re->query(), "name", m_versionName );
          new Tag( re->query(), "version", m_versionVersion );
          new Tag( re->query(), "os", m_versionOs );
          m_parent->send( re );
        }
        else if( iq->xmlns() == XMLNS_DISCO_INFO && iq->query() )
        {
          IQ* re = new IQ( IQ::Result, iq->from(), iq->id(), XMLNS_DISCO_INFO );
          Tag* query = re->query();

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
          IQ* re = new IQ( IQ::Result, iq->from(), iq->id(), XMLNS_DISCO_ITEMS );
          Tag* query = re->query();

          DiscoNodeHandlerMap::const_iterator it;
          Tag* q = iq->query();
          const std::string& node = q->findAttribute( "node" );
          query->addAttribute( "node", node );
          it = m_nodeHandlers.find( node );
          if( it != m_nodeHandlers.end() )
          {
            DiscoNodeHandlerList::const_iterator in = (*it).second.begin();
            for( ; in != (*it).second.end(); ++in )
            {
              const DiscoNodeItemList& items = (*in)->handleDiscoNodeItems( node );
              DiscoNodeItemList::const_iterator it = items.begin();
              for( ; it != items.end(); ++it )
              {
                Tag* i = new Tag( query, "item" );
                i->addAttribute( "jid",  (*it).jid.empty() ? m_parent->jid().full() : (*it).jid );
                i->addAttribute( "node", (*it).node );
                i->addAttribute( "name", (*it).name );
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

  void Disco::addFeature( const std::string& feature )
  {
    m_features.push_back( feature );
  }

  void Disco::removeFeature( const std::string& feature )
  {
    m_features.remove( feature );
  }

  void Disco::getDisco( const JID& to, const std::string& node, DiscoHandler* dh, int context,
                        IdType idType, const std::string& tid )
  {
    const std::string& id = tid.empty() ? m_parent->getID() : tid;

    IQ* iq = new IQ( IQ::Get, to, id, idType == GET_DISCO_INFO ? XMLNS_DISCO_INFO
                                                               : XMLNS_DISCO_ITEMS );
    if( !node.empty() )
      iq->query()->addAttribute( "node", node );

    DiscoHandlerContext ct;
    ct.dh = dh;
    ct.context = context;
    m_track[id] = ct;
    m_parent->trackID( this, id, idType );
    m_parent->send( iq );
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

  void Disco::registerDiscoHandler( DiscoHandler* dh )
  {
    m_discoHandlers.push_back( dh );
  }

  void Disco::removeDiscoHandler( DiscoHandler* dh )
  {
    m_discoHandlers.remove( dh );
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
      if( !(*it).second.size() )
      {
        m_nodeHandlers.erase( it );
      }
    }
  }

}

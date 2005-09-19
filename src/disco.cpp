/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "disco.h"
#include "discohandler.h"
#include "clientbase.h"
#include "disconodehandler.h"


namespace gloox
{

  Disco::Disco( ClientBase *parent )
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
      m_parent->removeIqHandler( XMLNS_DISCO_INFO );
      m_parent->removeIqHandler( XMLNS_DISCO_ITEMS );
      m_parent->removeIqHandler( XMLNS_VERSION );
    }
  }

  bool Disco::handleIq( Stanza *stanza )
  {
    switch( stanza->subtype() )
    {
      case STANZA_IQ_GET:
        if( stanza->xmlns() == XMLNS_VERSION )
        {
          Tag *iq = new Tag( "iq" );
          iq->addAttrib( "id", stanza->id() );
          iq->addAttrib( "from", m_parent->jid().full() );
          iq->addAttrib( "to", stanza->from().full() );
          iq->addAttrib( "type", "result" );
          Tag *query = new Tag( iq, "query" );
          query->addAttrib( "xmlns", XMLNS_VERSION );
          Tag *name = new Tag( query, "name", m_versionName );
          Tag *version = new Tag( query, "version", m_versionVersion );
          Tag *os = new Tag( query, "os", m_versionOs );

          m_parent->send( iq );
        }
        else if( stanza->xmlns() == XMLNS_DISCO_INFO )
        {
          Tag *iq = new Tag( "iq" );
          iq->addAttrib( "id", stanza->id() );
          iq->addAttrib( "from", m_parent->jid().full() );
          iq->addAttrib( "to", stanza->from().full() );
          iq->addAttrib( "type", "result" );
          Tag *query = new Tag( iq, "query" );
          query->addAttrib( "xmlns", XMLNS_DISCO_INFO );

          Tag *q = stanza->findChild( "query" );
          const std::string node = q->findAttribute( "node" );
          if( !node.empty() )
          {
            DiscoNodeHandlerMap::const_iterator it = m_nodeHandlers.find( node );
            if( it != m_nodeHandlers.end() )
            {
              std::string name;
              StringMap identities =
                  (*it).second->handleDiscoNodeIdentities( node, name );
              StringMap::const_iterator im = identities.begin();
              for( im; im != identities.end(); im++ )
              {
                Tag *i = new Tag( query, "identity" );
                i->addAttrib( "category", (*im).first );
                i->addAttrib( "type", (*im).second );
                i->addAttrib( "name", name );
              }
              StringList features = (*it).second->handleDiscoNodeFeatures( node );
              StringList::const_iterator fi = features.begin();
              for( fi; fi != features.end(); fi++ )
              {
                Tag *f = new Tag( query, "feature" );
                f->addAttrib( "var", (*fi) );
              }
            }
          }
          else
          {
            Tag *i = new Tag( query, "identity" );
            i->addAttrib( "category", m_identityCategory );
            i->addAttrib( "type", m_identityType );
            i->addAttrib( "name", m_versionName );

            StringList::const_iterator it = m_features.begin();
            for( it; it != m_features.end(); ++it )
            {
              Tag *f = new Tag( query, "feature" );
              f->addAttrib( "var", (*it).c_str() );
            }
          }

          m_parent->send( iq );
        }
        else if( stanza->xmlns() == XMLNS_DISCO_ITEMS )
        {
          Tag *iq = new Tag( "iq" );
          iq->addAttrib( "id", stanza->id() );
          iq->addAttrib( "to", stanza->from().full() );
          iq->addAttrib( "from", m_parent->jid().full() );
          iq->addAttrib( "type", "result" );
          Tag *query = new Tag( iq, "query" );
          query->addAttrib( "xmlns", XMLNS_DISCO_ITEMS );

          StringMap items;
          DiscoNodeHandlerMap::const_iterator it;
          Tag *q = stanza->findChild( "query" );
          const std::string node = q->findAttribute( "node" );
          if( !node.empty() )
          {
            it = m_nodeHandlers.find( node );
            if( it != m_nodeHandlers.end() )
            {
              items = (*it).second->handleDiscoNodeItems( node );
            }
          }
          else
          {
            it = m_nodeHandlers.begin();
            for( it; it != m_nodeHandlers.end(); it++ )
            {
              items = (*it).second->handleDiscoNodeItems( "" );
            }
          }

          if( items.size() )
          {
            StringMap::const_iterator it = items.begin();
            for( it; it != items.end(); it++ )
            {
              if( !(*it).first.empty() && !(*it).second.empty() )
              {
                Tag *i = new Tag( query, "item" );
                i->addAttrib( "jid",  m_parent->jid().full() );
                i->addAttrib( "node", (*it).first );
                i->addAttrib( "name", (*it).second );
              }
            }
          }

          m_parent->send( iq );
        }
        return true;
        break;

      case STANZA_IQ_SET:
      {
        bool res = false;
        DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
        for( it; it != m_discoHandlers.end(); it++ )
        {
          if( (*it)->handleDiscoSet( stanza->id(), stanza ) )
            res = true;
        }
        return res;
        break;
      }
    }
    return false;
  }

  bool Disco::handleIqID( Stanza *stanza, int context )
  {
    switch( stanza->subtype() )
    {
      case STANZA_IQ_RESULT:

        switch( context )
        {
          case GET_DISCO_INFO:
          {
            DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
            for( it; it != m_discoHandlers.end(); it++ )
            {
              (*it)->handleDiscoInfoResult( stanza->id(), stanza );
            }
            break;
          }
          case GET_DISCO_ITEMS:
          {
            DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
            for( it; it != m_discoHandlers.end(); it++ )
            {
              (*it)->handleDiscoItemsResult( stanza->id(), stanza );
            }
            break;
          }
        }
        break;

      case STANZA_IQ_ERROR:
        Tag *e = stanza->findChild( "error" );
        if( !e )
          return false;

        DiscoHandlerList::const_iterator it = m_discoHandlers.begin();
        for( it; it != m_discoHandlers.end(); it++ )
        {
          (*it)->handleDiscoError( stanza->id(), e->name() );
        }
        break;
    }

    return false;
  }

  void Disco::addFeature( const std::string& feature )
  {
    m_features.push_back( feature );
  }

  void Disco::getDiscoInfo( const std::string& to )
  {
    std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "id", id );
    iq->addAttrib( "to", to );
    iq->addAttrib( "from", m_parent->jid().full() );
    iq->addAttrib( "type", "get" );
    Tag *q = new Tag( iq, "query" );
    q->addAttrib( "xmlns", XMLNS_DISCO_INFO );

    m_parent->trackID( this, id, GET_DISCO_INFO );
    m_parent->send( iq );
  }

  void Disco::getDiscoItems( const std::string& to )
  {
    std::string id = m_parent->getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "id", id );
    iq->addAttrib( "to", to );
    iq->addAttrib( "from", m_parent->jid().full() );
    iq->addAttrib( "type", "get" );
    Tag *q = new Tag( iq, "query" );
    q->addAttrib( "xmlns", XMLNS_DISCO_ITEMS );

    m_parent->trackID( this, id, GET_DISCO_INFO );
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

  void Disco::registerDiscoHandler( DiscoHandler *dh )
  {
    m_discoHandlers.push_back( dh );
  }

  void Disco::removeDiscoHandler( DiscoHandler *dh )
  {
    m_discoHandlers.remove( dh );
  }

  void Disco::registerNodeHandler( DiscoNodeHandler *nh, const std::string& node )
  {
    m_nodeHandlers[node] = nh;
  }

  void Disco::removeNodeHandler( const std::string& node )
  {
    m_nodeHandlers.erase( node );
  }

};

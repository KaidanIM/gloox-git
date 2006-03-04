/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "inbandbytestreammanager.h"
#include "inbandbytestreamhandler.h"
#include "inbandbytestream.h"
#include "clientbase.h"

namespace gloox
{

  InBandBytestreamManager::InBandBytestreamManager( ClientBase *parent, Disco *disco )
    : m_parent( parent ), m_inbandBytestreamHandler( 0 ), m_blockSize( 4096 )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_IBB );

    if( disco )
      disco->addFeature( XMLNS_IBB );
  }

  InBandBytestreamManager::~InBandBytestreamManager()
  {
  }

  bool InBandBytestreamManager::requestInBandBytestream( const JID& to )
  {
    if( !m_parent )
      return false;

    if( m_trackMap.find( to.full() ) != m_trackMap.end() )
      return false;

    const std::string sid = m_parent->getID();
    const std::string id = m_parent->getID();
    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "type", "set" );
    iq->addAttribute( "to", to.full() );
    iq->addAttribute( "id", id );
    Tag *o = new Tag( iq, "open" );
    o->addAttribute( "sid", sid );
    o->addAttribute( "block-size", m_blockSize );
    o->addAttribute( "xmlns", XMLNS_IBB );

    m_trackMap[to.full()] = sid;
    m_parent->trackID( this, id, IBB_OPEN_STREAM );
    m_parent->send( iq );

    return true;
  }

  bool InBandBytestreamManager::handleIq( Stanza *stanza )
  {
    Tag *o = 0;
    if( ( stanza->subtype() == StanzaIqSet ) &&
          ( ( o = stanza->findChild( "open", "xmlns", XMLNS_IBB ) ) != 0 ) )
    {
      InBandBytestream *ibb = new InBandBytestream( 0, m_parent );
      ibb->setSid( o->findAttribute( "sid" ) );

      if( !m_inbandBytestreamHandler ||
           !m_inbandBytestreamHandler->handleIncomingInBandBytestream( stanza->from(), ibb ) )
      {
        delete ibb;
        Tag *iq = new Tag( "iq" );
        iq->addAttribute( "type", "error" );
        iq->addAttribute( "to", stanza->from().full() );
        iq->addAttribute( "id", stanza->id() );
        Tag *e = new Tag( iq, "error" );
        e->addAttribute( "code", "501" );
        e->addAttribute( "type", "cancel" );
        Tag *f = new Tag( e, "feature-not-implemented" );
        f->addAttribute( "xmlns", XMLNS_XMPP_STANZAS );
        m_parent->send( iq );
      }
      else
      {
        Tag *iq = new Tag( "iq" );
        iq->addAttribute( "type", "result" );
        iq->addAttribute( "to", stanza->from().full() );
        iq->addAttribute( "id", stanza->id() );
        m_parent->send( iq );
      }
    }
    else
    {
      return false;
    }

    return true;
  }

  bool InBandBytestreamManager::handleIqID( Stanza *stanza, int context )
  {
    if( !m_inbandBytestreamHandler )
      return false;

    switch( context )
    {
      case IBB_OPEN_STREAM:
        switch( stanza->subtype() )
        {
          case StanzaIqResult:
          {
            StringMap::iterator it = m_trackMap.find( stanza->from().full() );
            if( it != m_trackMap.end() )
            {
              InBandBytestream *ibb = new InBandBytestream( 0, m_parent );
              ibb->setSid( (*it).second );
              m_trackMap.erase( it );
              ibb->setBlockSize( m_blockSize );
              m_inbandBytestreamHandler->handleOutgoingInBandBytestream( stanza->from(), ibb );
            }
            break;
          }
          case StanzaIqError:
            m_inbandBytestreamHandler->handleInBandBytestreamError( stanza->from(), stanza->error() );
            break;
          default:
            break;
        }
        break;
      case IBB_CLOSE_STREAM:
        // ignore
        break;
    }

    return false;
  }

  void InBandBytestreamManager::registerInBandBytestreamHandler( InBandBytestreamHandler *ibbh )
  {
    m_inbandBytestreamHandler = ibbh;
  }

  void InBandBytestreamManager::removeInBandBytestreamHandler()
  {
    m_inbandBytestreamHandler = 0;
  }

}

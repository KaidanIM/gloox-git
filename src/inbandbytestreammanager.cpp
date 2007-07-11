/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
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
#include "disco.h"
#include "util.h"

namespace gloox
{

  InBandBytestreamManager::InBandBytestreamManager( ClientBase *parent )
    : m_parent( parent ), m_inbandBytestreamHandler( 0 ), m_syncInbandBytestreams( true ),
      m_blockSize( 4096 )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_IBB );
      m_parent->disco()->addFeature( XMLNS_IBB );
    }
  }

  InBandBytestreamManager::~InBandBytestreamManager()
  {
    if( m_parent )
    {
      m_parent->disco()->removeFeature( XMLNS_IBB );
      m_parent->removeIqHandler( XMLNS_IBB );
    }

    util::clear( m_ibbMap );
  }

  bool InBandBytestreamManager::requestInBandBytestream( const JID& to, InBandBytestreamHandler *ibbh,
                                                         const std::string& sid )
  {
    if( !m_parent || !ibbh )
      return false;

    const std::string& msid = sid.empty() ? m_parent->getID() : sid;
    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Set, to, id, XMLNS_IBB, "open" );
    Tag *o = iq->query();
    o->addAttribute( "sid", msid );
    o->addAttribute( "block-size", m_blockSize );

    TrackItem item;
    item.sid = msid;
    item.ibbh = ibbh;
    m_trackMap[id] = item;
    m_parent->trackID( this, id, IBBOpenStream );
    m_parent->send( iq );

    return true;
  }

  bool InBandBytestreamManager::handleIq( IQ* iq )
  {
    Tag *o = 0;
    if( ( iq->subtype() == IQ::Set ) &&
          ( ( o = iq->findChild( "open", "xmlns", XMLNS_IBB ) ) != 0 ) )
    {
      InBandBytestream *ibb = new InBandBytestream( 0, m_parent );
      const std::string& sid = o->findAttribute( "sid" );
      ibb->setSid( sid );

      if( !m_inbandBytestreamHandler )
        rejectInBandBytestream( ibb, iq->from(), iq->id() );

      if( !m_syncInbandBytestreams )
      {
        AsyncIBBItem item;
        item.ibb = ibb;
        item.from = iq->from();
        item.id = iq->id();
        m_asyncTrackMap[sid] = item;
      }

      bool t = m_inbandBytestreamHandler->handleIncomingInBandBytestream( iq->from(), ibb );
      if( m_syncInbandBytestreams && t )
      {
        acceptInBandBytestream( ibb, iq->from(), iq->id() );
      }
      else if( m_syncInbandBytestreams && !t )
      {
        rejectInBandBytestream( ibb, iq->from(), iq->id() );
      }
    }
    else if( ( iq->subtype() == IQ::Set ) &&
               ( ( o = iq->findChild( "close", "xmlns", XMLNS_IBB ) ) != 0 ) &&
               o->hasAttribute( "sid" ) )
    {
      IBBMap::iterator it = m_ibbMap.find( o->findAttribute( "sid" ) );
      if( it != m_ibbMap.end() )
      {
        (*it).second->closed();

        IQ* re = new IQ( IQ::Result, iq->from(), iq->id() );
        m_parent->send( re );
      }
    }
    else
    {
      return false;
    }

    return true;
  }

  void InBandBytestreamManager::acceptInBandBytestream( InBandBytestream *ibb )
  {
    if( m_syncInbandBytestreams )
      return;

    AsyncTrackMap::iterator it = m_asyncTrackMap.find( ibb->sid() );
    if( it != m_asyncTrackMap.end() )
    {
      acceptInBandBytestream( ibb, (*it).second.from, (*it).second.id );
      m_asyncTrackMap.erase( it );
    }
  }

  void InBandBytestreamManager::rejectInBandBytestream( InBandBytestream *ibb )
  {
    if( m_syncInbandBytestreams )
      return;

    AsyncTrackMap::iterator it = m_asyncTrackMap.find( ibb->sid() );
    if( it != m_asyncTrackMap.end() )
    {
      rejectInBandBytestream( ibb, (*it).second.from, (*it).second.id );
      m_asyncTrackMap.erase( it );
    }
  }

  void InBandBytestreamManager::acceptInBandBytestream( InBandBytestream *ibb,
      const JID& from, const std::string& id )
  {
    m_ibbMap[ibb->sid()] = ibb;
    IQ* iq = new IQ( IQ::Result, from, id );
    m_parent->send( iq );
  }

  void InBandBytestreamManager::rejectInBandBytestream( InBandBytestream *ibb,
      const JID& from, const std::string& id )
  {
    delete ibb;
    IQ* iq = new IQ( IQ::Error, from, id );
    Tag *e = new Tag( iq, "error" );
    e->addAttribute( "code", "501" );
    e->addAttribute( "type", "cancel" );
    Tag *f = new Tag( e, "feature-not-implemented" );
    f->addAttribute( "xmlns", XMLNS_XMPP_STANZAS );
    m_parent->send( iq );
  }

  void InBandBytestreamManager::handleIqID( IQ* iq, int context )
  {
    switch( context )
    {
      case IBBOpenStream:
      {
        TrackMap::iterator it = m_trackMap.find( iq->id() );
        if( it != m_trackMap.end() )
        {
          switch( iq->subtype() )
          {
            case IQ::Result:
            {
              InBandBytestream *ibb = new InBandBytestream( 0, m_parent );
              ibb->setSid( (*it).second.sid );
              ibb->setBlockSize( m_blockSize );
              m_ibbMap[(*it).second.sid] = ibb;
              InBandBytestreamHandler *t = (*it).second.ibbh;
              m_trackMap.erase( it );
              t->handleOutgoingInBandBytestream( iq->from(), ibb );
              break;
            }
            case IQ::Error:
              (*it).second.ibbh->handleInBandBytestreamError( iq );
              break;
            default:
              break;
          }
          m_trackMap.erase( it );
        }
        break;
      }
      default:
        break;
    }
  }

  bool InBandBytestreamManager::dispose( InBandBytestream *ibb )
  {
    IBBMap::iterator it = m_ibbMap.find( ibb->sid() );
    if( it != m_ibbMap.end() )
    {
      delete ibb;
      m_ibbMap.erase( it );
      return true;
    }

    return false;
  }

  void InBandBytestreamManager::registerInBandBytestreamHandler( InBandBytestreamHandler *ibbh,
      bool sync )
  {
    m_inbandBytestreamHandler = ibbh;
    m_syncInbandBytestreams = sync;
  }

  void InBandBytestreamManager::removeInBandBytestreamHandler()
  {
    m_inbandBytestreamHandler = 0;
  }

}

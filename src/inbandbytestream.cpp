/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "inbandbytestream.h"
#include "bytestreamdatahandler.h"
#include "disco.h"
#include "clientbase.h"
#include "base64.h"

namespace gloox
{

  InBandBytestream::InBandBytestream( ClientBase* clientbase, LogSink& logInstance, const JID& initiator,
                                      const JID& target, const std::string& sid )
    : Bytestream( Bytestream::IBB, logInstance, initiator, target, sid ),
      m_clientbase( clientbase ), m_blockSize( 4096 ), m_sequence( -1 ), m_lastChunkReceived( -1 )
  {
    if( m_clientbase )
      m_clientbase->registerIqHandler( this, XMLNS_IBB );

    m_open = false;
  }

  InBandBytestream::~InBandBytestream()
  {
    if( m_open )
      close();

    if( m_clientbase )
    {
      m_clientbase->removeIqHandler( this, XMLNS_IBB );
      m_clientbase->removeIDHandler( this );
    }
  }

  bool InBandBytestream::connect()
  {
    if( !m_clientbase )
      return false;

    if( m_target == m_clientbase->jid() )
      return true;

    const std::string& id = m_clientbase->getID();
    IQ* iq = new IQ( IQ::Set, m_target, id, XMLNS_IBB, "open" );
    iq->query()->addAttribute( "sid", m_sid );
    iq->query()->addAttribute( "block-size", m_blockSize );

    m_clientbase->trackID( this, id, IBBOpen );
    m_clientbase->send( iq );
    return true;
  }

  void InBandBytestream::handleIqID( IQ* iq, int context )
  {
    switch( iq->subtype() )
    {
      case IQ::Result:
        if( context == IBBOpen && m_handler )
        {
          m_handler->handleBytestreamOpen( this );
          m_open = true;
        }
        break;
      case IQ::Error:
        closed();
        break;
      default:
        break;
    }
  }

  bool InBandBytestream::handleIq( IQ* iq ) // data or open request, always 'set'
  {
    Tag* q = iq->query();
    if( !q || !q->hasAttribute( "sid", m_sid ) || !m_handler || iq->subtype() != IQ::Set )
      return false;

    if( !m_open )
    {
      if( q->name() == "open" )
      {
        returnResult( iq->from(), iq->id() );
        m_open = true;
        m_handler->handleBytestreamOpen( this );
        return true;
      }
      return false;
    }

    if( q->name() == "close" )
    {
      returnResult( iq->from(), iq->id() );
      closed();
      return true;
    }

    const std::string& seq = q->findAttribute( "seq" );
    if( seq.empty() || ++m_lastChunkReceived != atoi( seq.c_str() ) )
    {
      m_open = false;
      return false;
    }

    const std::string& data = q->cdata();
    if( data.empty() )
    {
      m_open = false;
      return false;
    }

    returnResult( iq->from(), iq->id() );
    m_handler->handleBytestreamData( this, Base64::decode64( data ) );
    return true;
  }

  void InBandBytestream::returnResult( const JID& to, const std::string& id )
  {
    IQ* iq = new IQ( IQ::Result, to, id );
    m_clientbase->send( iq );
  }

  bool InBandBytestream::send( const std::string& data )
  {
    if( !m_open || !m_clientbase )
      return false;

    int pos = 0;
    int len = data.length();
    do
    {
      const std::string& id = m_clientbase->getID();
      IQ* iq = new IQ( IQ::Set, m_target, id, XMLNS_IBB, "data" );
      iq->query()->setCData( Base64::encode64( data.substr( pos, m_blockSize ) ) );
      iq->query()->addAttribute( "sid", m_sid );
      iq->query()->addAttribute( "seq", ++m_sequence );

      m_clientbase->trackID( this, id, IBBData );
      m_clientbase->send( iq );

      pos += m_blockSize;
      if( m_sequence == 65535 )
        m_sequence = -1;
    }
    while( pos < len );

    return true;
  }

  void InBandBytestream::closed()
  {
    if( !m_open )
      return;

    m_open = false;

    if( m_handler )
      m_handler->handleBytestreamClose( this );
  }

  void InBandBytestream::close()
  {
    m_open = false;

    if( !m_clientbase )
      return;

    const std::string& id = m_clientbase->getID();
    IQ* iq = new IQ( IQ::Set, m_target, id, XMLNS_IBB, "close" );
    iq->query()->addAttribute( "sid", m_sid );

    m_clientbase->trackID( this, id, IBBClose );
    m_clientbase->send( iq );

    if( m_handler )
      m_handler->handleBytestreamClose( this );
  }

}

/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAM_H__
#define INBANDBYTESTREAM_H__

#include "bytestream.h"
#include "iqhandler.h"
#include "gloox.h"

namespace gloox
{

  class ClientBase;
  class BytestreamDataHandler;

  /**
   * @brief An implementation of a single In-Band Bytestream (XEP-0047).
   *
   * One instance of this class handles a single byte stream.
   *
   * See SIProfileFT for a detailed description on how to implement file transfer.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API InBandBytestream : public Bytestream, public IqHandler
  {
    friend class SIProfileFT;

    public:
      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestream();

      // re-implemented from Bytestream
      virtual ConnectionError recv( int timeout = -1 ) { return ConnNoError; }

      /**
       * Use this function to send a chunk of data over an open byte stream.
       * The negotiated block size is enforced. If the block is larger, nothing is sent
       * and @b false is returned. If the stream is not open or has been closed again
       * (by the remote entity or locally), nothing is sent and @b false is returned.
       * This function does the necessary base64 encoding for you.
       * @param data The block of data to send.
       * @return @b True if the data has been sent (no guarantee of receipt), @b false
       * in case of an error.
       */
      bool send( const std::string& data );

      /**
       * Lets you retrieve this bytestream's block-size.
       * @return The bytestream's block-size.
       */
      int blockSize() const { return m_blockSize; }

      /**
       * Sets the stream's block-size. Default: 4096
       * @param blockSize The new block size.
       * @note You should not change the block size once connect() has been called.
       */
      void setBlockSize( int blockSize ) { m_blockSize = blockSize; }

      // re-implemented from Bytestream
      virtual bool connect();

      // re-implemented from Bytestream
      virtual void close();

      // re-implemented from IqHandler
      virtual bool handleIq( IQ* iq );

      // re-implemented from IqHandler
      virtual void handleIqID( IQ *iq, int context );

    private:
      enum TrackEnum
      {
        IBBOpen,
        IBBData,
        IBBClose
      };

      InBandBytestream( ClientBase* clientbase, LogSink& logInstance, const JID& initiator,
                        const JID& target, const std::string& sid );
      void closed(); // by remote entity
      void returnResult( const JID& to, const std::string& id );

      ClientBase *m_clientbase;
      int m_blockSize;
      int m_sequence;
      int m_lastChunkReceived;

  };

}

#endif // INBANDBYTESTREAM_H__

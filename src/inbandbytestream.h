/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAM_H__
#define INBANDBYTESTREAM_H__

#include "messagefilter.h"
#include "iqhandler.h"
#include "gloox.h"

namespace gloox
{

  class ClientBase;
  class InBandBytestreamDataHandler;

  /**
   * @brief An implementation of JEP-0047 (In-Band Bytestreams).
   *
   * One instance of this class handles one byte stream. You can attach as many InBandBytestream
   * objects to a MessageSession as you like.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API InBandBytestream : public MessageFilter
  {
    public:
      /**
       * Constructs a new InBandBytestream object.
       * @param session The MessageSession that is used for data exchange with the remote
       * entity.
       * @param clientbase The ClientBase object that is used for establishing the
       * bytestream.
       */
      InBandBytestream( MessageSession *session, ClientBase *clientbase );

      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestream();

      /**
       *
       */
      bool open() const { return m_open; };

      /**
       *
       */
      void close();

      /**
       * Use this function to register an object that will receive any notifications from
       * the InBandBytestream instance. Only one InBandBytestreamDataHandler can be registered
       * at any one time.
       * @param ibbdh The InBandBytestreamDataHandler derived object to receive notifications.
       */
      void registerInBandBytestreamDataHandler( InBandBytestreamDataHandler *ibbdh );

      /**
       * Removes the registered InBandBytestreamDataHandler.
       */
      void removeInBandBytestreamDataHandler();

      /**
       * Use this function to send a chunk of data over an open byte stream.
       * The negotiated block size is enforced. If the block is larger, nothing is sent
       * and @b false is returned. If the stream is not open or has been closed again
       * (by the remote entity or locally), nothing is sent and @b false is returned.
       * This function does the base64 encoding for you.
       * @param data The block of data to send.
       * @return @b True if the data has been sent (no guarantee of receipt), @b false
       * in case of an error.
       */
      bool sendBlock( const std::string& data );

      /**
       * Sets the stream's stream id. This should not be changed manually once the stream has
       * been negotiated.
       * @param sid The new stream id.
       */
      void setSid( const std::string& sid ) { m_sid = sid; };

      /**
       * Sets the default block-size. Default: 4096
       * @param blockSize The default block-size in byte.
       */
      void setBlockSize( int blockSize ) { m_blockSize = blockSize; };

      // reimplemented from MessageFilter
      virtual void decorate( Tag *tag );

      // reimplemented from MessageFilter
      virtual void filter( Stanza *stanza );

    private:
      MessageSession *m_parent;
      ClientBase *m_clientbase;
      InBandBytestreamDataHandler *m_inbandBytestreamDataHandler;
      std::string m_sid;
      std::string::size_type m_blockSize;
      int m_sequence;
      int m_lastChunkReceived;
      bool m_open;

  };

}

#endif // INBANDBYTESTREAM_H__

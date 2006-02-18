/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
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
#include "gloox.h"

namespace gloox
{

  class ClientBase;
  class InBandBytestreamHandler;

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
       *
       */
      InBandBytestream( MessageSession *session, ClientBase *clientbase );

      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestream();

      /**
       * Use this function to register an object that will receive any notifications from
       * the InBandBytestream instance. Only one InBandBytestreamHandler can be registered
       * at any one time.
       * @param ibbh The InBandBytestreamHandler derived object to receive notifications.
       */
      void registerInBandBytestreamHandler( InBandBytestreamHandler *ibbh );

      /**
       * Removes the registered InBandBytestreamHandler.
       */
      void removeInBandBytestreamHandler();

      /**
       * Sets the desired block-size. Default: 4096
       * @param blockSize The desired block-size.
       */
      void setBlockSize( int blockSize ) { m_blockSize = blockSize; };

      // reimplemented from MessageFilter
      virtual void decorate( Tag *tag );

      // reimplemented from MessageFilter
      virtual void filter( Stanza *stanza );

    private:
      MessageSession *m_parent;
      ClientBase *m_clientbase;
      InBandBytestreamHandler *m_inbandBytestreamHandler;
      int m_blockSize;
  };

}

#endif // INBANDBYTESTREAM_H__

/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAMMANAGER_H__
#define INBANDBYTESTREAMMANAGER_H__

#include "iqhandler.h"
#include "disco.h"
#include "jid.h"

namespace gloox
{

  class InBandBytestreamHandler;

  /**
   * @brief An InBandBytestreamManager listens for incoming In-Band Bytestream
   * requests from remote entities. Upon an incoming request it notifies
   * a registered InBandBytestreamHandler. The return value of the handler
   * determines whether the stream shall be accepted or not.
   *
   * @note You should have only one InBandBytestreamManager per Client/ClientBase lying around.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class InBandBytestreamManager : public IqHandler
  {
    public:
      /**
       *
       */
      InBandBytestreamManager( ClientBase *parent, Disco *disco );

      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestreamManager();

      /**
       * This function initiates opening of a byte stream with the MessageSession's remote entity.
       * Data can only be sent over an open stream. Use open() to find out what the stream's
       * current state is. However, successful opening/initiation will be announced by means of the
       * InBandBytestreamHandler interface. Only one byte stream per JID can be initiated at once,
       * however, once a byte stream has been accepted (or declined), another one can be opened for
       * that JID. There is no such restriction for multiple byte streams to different JIDs.
       * @param to The recipient of the requested byte stream.
       * @return @b False in case of an error, @b true otherwise. A return value of @b true does
       * @b not indicate that the bytestream has been opened. This is announced by means of the
       * InBandBytestreamHandler.
       */
      bool requestInBandBytestream( const JID& to );

      /**
       * Sets the default block-size. Default: 4096
       * @param blockSize The default block-size in byte.
       */
      void setBlockSize( int blockSize ) { m_blockSize = blockSize; };

      /**
       * Returns the currently set block-size.
       * @return The currently set block-size.
       */
      int blockSize() const { return m_blockSize; };

      /**
       * Use this function to register an object that will receive new incoming bytestream requests
       * as well as internally requested bytestreams from the InBandBytestreamManager. Only one
       * InBandBytestreamHandler can be registered at any one time.
       * @param ibbh The InBandBytestreamHandler derived object to receive notifications.
       */
      void registerInBandBytestreamHandler( InBandBytestreamHandler *ibbh );

      /**
       * Removes the registered InBandBytestreamHandler.
       */
      void removeInBandBytestreamHandler();

      // reimplemented from IqHandler
      virtual bool handleIq( Stanza *stanza );

      // reimplemented from IqHandler
      virtual bool handleIqID( Stanza *stanza, int context );

    private:
      enum IBBActionType
      {
        IBB_OPEN_STREAM,
        IBB_CLOSE_STREAM
      };

      ClientBase *m_parent;
      InBandBytestreamHandler *m_inbandBytestreamHandler;
      int m_blockSize;
      StringMap m_trackMap;

  };

}

#endif // INBANDBYTESTREAMMANAGER_H__

/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAMHANDLER_H__
#define INBANDBYTESTREAMHANDLER_H__

#include "macros.h"
#include "jid.h"
#include "inbandbytestream.h"

namespace gloox
{

  /**
   * @brief A virtual interface that allows to receive new incoming In-Band Bytestream requests
   * from remote entities.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API InBandBytestreamHandler
  {
    public:
      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestreamHandler() {};

      /**
       * Notifies the implementor of a new incoming IBB request.
       * Attach the IBB to a MessageSession using InBandBytestream::attachTo().
       * If the return value is @b true the bytestream holds as accepted and the
       * InBandBytestreamHandler becomes the owner of the InBandBytestream. If @b false
       * is returned, the InBandBytestream is deleted by the InBandBytestreamManager and
       * the bytestream request will be declined.
       * @param to The remote initiator of the bytestream request.
       * @param ibb The bytestream.
       * @return @b True to accept the byte stream, @b false to reject.
       * @note You should @b not send any data over this bytestream from within this function.
       * The bytestream will only be accepted after this function returned.
       */
      virtual bool handleIncomingInBandBytestream( const JID& from, InBandBytestream *ibb ) = 0;

      /**
       * The In-Band Bytestream is already attached to the MessageSession provided to
       * InBandBytestreamManager::createInBandBytestream() earlier.
       * Also, the stream has been accepted by the remote entity and is ready to send data.
       * @param to The remote entity's JID.
       * @param ibb The new bytestream.
       */
      virtual bool handleOutgoingInBandBytestream( const JID& to, InBandBytestream *ibb ) = 0;

      /**
       * Notifies the implementor in case of an error.
       * @todo actually return errors.
       */
      virtual void handleInBandBytestreamError() = 0;

  };

}

#endif // INBANDBYTESTREAMHANDLER_H__

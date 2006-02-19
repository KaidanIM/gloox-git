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
   * @brief A virtual interface that allows to receive new incoming In-band Bytestream requests
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
       * Attach ibb to a MessageSession using InBandBytestream::attachTo().
       * @return @b True to accept the byte stream, @b false to reject.
       */
      virtual bool handleIncomingInBandBytestream( const JID& from, InBandBytestream *ibb ) = 0;

      /**
       * ibb is already attached to the MessageSession provided to
       * InBandBytestreamManager::createInBandBytestream() earlier.
       * Also, the stream has been accepted by the remote entity and is ready to send data.
       */
      virtual bool handleOutgoingInBandBytestream( const JID& to, InBandBytestream *ibb ) = 0;

      /**
       *
       */
      virtual void handleInBandBytestreamError() = 0;

  };

}

#endif // INBANDBYTESTREAMHANDLER_H__

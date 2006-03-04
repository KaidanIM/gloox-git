/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAMDATAHANDLER_H__
#define INBANDBYTESTREAMDATAHANDLER_H__

#include "macros.h"

#include <string>

namespace gloox
{

  /**
   * @brief A virtual interface that allows implementors to receive data
   * sent over a In-Band Bytestream as defined in JEP-0047.
   *
   * An InBandBytestreamDataHandler is registered with an InBandBytestream.
   *
   * See InBandBytestreamManager for a detailed description on how to implement In-Band Bytestreams.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API InBandBytestreamDataHandler
  {
    public:
      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestreamDataHandler() {};

      /**
       * Reimplement this function to receive data which is sent over the bytestream.
       * @param data The actual stream payload. Not base64 encoded.
       * @param sid The stream's ID.
       */
      virtual void handleInBandData( const std::string& data, const std::string& sid ) = 0;

      /**
       * Notifies about an error occuring when opening a stream.
       * @param sid The stream's ID.
       * @todo Actually return errors.
       */
      virtual void handleInBandError( const std::string& sid ) = 0;

  };

}

#endif // INBANDBYTESTREAMDATAHANDLER_H__

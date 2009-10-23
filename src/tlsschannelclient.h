/*
 * Copyright (c) 2007-2009 by Jakob Schroeter <js@camaya.net>
 * This file is part of the gloox library. http://camaya.net/gloox
 *
 * This software is distributed under a license. The full license
 * agreement can be found in the file LICENSE in this distribution.
 * This software may not be copied, modified, sold or distributed
 * other than expressed in the named license agreement.
 *
 * This software is distributed without any warranty.
 */

#ifndef TLSSCHANNELCLIENT_H__
#define TLSSCHANNELCLIENT_H__

#include "tlsschannelbase.h"

#include "config.h"

#ifdef HAVE_WINTLS

#include <ctime>

#define SECURITY_WIN32
#include <windows.h>
#include <security.h>
#include <schnlsp.h>

namespace gloox
{

  /**
   * This class implements a TLS backend using SChannel.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class SChannelClient : public SChannelBase
  {
    public:
      /**
       * Constructor.
       * @param th The TLSHandler to handle TLS-related events.
       * @param server The server to use in certificate verification.
       */
      SChannelClient( TLSHandler* th, const std::string& server );

      /**
       * Virtual destructor.
       */
      virtual ~SChannelClient();

      // reimplemented from TLSBase
      virtual bool handshake();

    private:
      void handshakeStage();

  };
}

#endif // HAVE_WINTLS

#endif // TLSSCHANNELCLIENT_H__

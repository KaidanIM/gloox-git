/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef SCHANNEL_H__
#define SCHANNEL_H__

#include "tlsbase.h"

#ifdef USE_WINTLS

#define SECURITY_WIN32
#include <windows.h>
#include <security.h>
#include <sspi.h>

namespace gloox
{

  /**
   *
   * @author Jakob Schröter <js@camaya.net>
   * @since 0.9
   */
  class SChannel : public TLSBase
  {
    public:
      /**
       * Constructor.
       */
      SChannel( TLSHandler *th );

      /**
       * Virtual destructor.
       */
      virtual ~SChannel();

      // reimplemented from TLSBase
      virtual bool encrypt( const std::string& data );

      // reimplemented from TLSBase
      virtual int decrypt( const std::string& data );

      // reimplemented from TLSBase
      virtual void cleanup();

      // reimplemented from TLSBase
      virtual bool handshake();

    private:
      bool handshakeLoop();

      SecurityFunctionTableA *m_securityFunc;
      CredHandle m_credentials;
      CtxtHandle m_context;
      SecBufferDesc m_imessage;
      SecBufferDesc m_omessage;
      SecBuffer m_ibuffers[4];
      SecBuffer m_obuffers[4];
      SecPkgContext_StreamSizes m_streamSizes;
      HMODULE m_lib;

      char *m_messageOffset;
      char *m_iBuffer;
      char *m_oBuffer;
      int m_bufferSize;
      int m_bufferOffset;
      int m_sspiFlags;

  };

}

#endif // USE_WINTLS

#endif // SCHANNEL_H__

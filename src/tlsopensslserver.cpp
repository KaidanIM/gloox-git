/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "tlsopensslserver.h"

#ifdef HAVE_OPENSSL

namespace gloox
{

  OpenSSLServer::OpenSSLServer( TLSHandler* th )
    : OpenSSLBase( th )
  {
  }

  OpenSSLServer::~OpenSSLServer()
  {
  }

  bool OpenSSLServer::setType()
  {
    m_ctx = SSL_CTX_new( TLSv1_server_method() );
    if( !m_ctx )
      return false;

    return true;
  }

  int OpenSSLServer::handshakeFunction()
  {
    return SSL_accept( m_ssl );
  }

}

#endif // HAVE_OPENSSL

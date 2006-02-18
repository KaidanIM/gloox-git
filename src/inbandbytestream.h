/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "inbandbytestream.h"
#include "inbandbytestreamhandler.h"
#include "messagesession.h"
#include "disco.h"

namespace gloox
{

  InBandBytestream::InBandBytestream( MessageSession *session, ClientBase *clientbase )
    : m_parent( session ), m_clientbase( clientbase ), m_inbandBytestreamHandler( 0 ),
      m_blockSize( 4096 )
  {
  }

  InBandBytestream::~InBandBytestream()
  {
  }

  virtual void InBandBytestream::decorate( Tag *tag )
  {
  }

  virtual void InBandBytestream::filter( Stanza *stanza )
  {
  }

}

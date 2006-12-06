/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MUCMESSAGESESSION_H__
#define MUCMESSAGESESSION_H__

#include "messagesession.h"

namespace gloox
{

  class ClientBase;

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class MUCMessageSession : public MessageSession
  {
    public:
      /**
       *
       */
      MUCMessageSession( ClientBase *parent, const JID& jid );

      /**
       *
       */
      virtual ~MUCMessageSession();

      /**
       *
       */
      virtual void send( const std::string& message );

      /**
       *
       */
      virtual void setSubject( const std::string& subject );

      // reimplemented from MessageSession
      virtual void handleMessage( Stanza *stanza );

  };

}

#endif // MUCMESSAGESESSION_H__

/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MESSAGESESSIONBASE_H__
#define MESSAGESESSIONBASE_H__

#include "messagehandler.h"
#include "session.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief The base of an abstraction of a message session between any two entities.
   *
   * You should not need to use this class directly.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_EXPORT MessageSessionBase : public Session, public MessageHandler
  {
    public:
      /**
       * Constructs a new MessageSession.
       */
      MessageSessionBase() {};

      /**
       * Virtual destructor.
       */
      virtual ~MessageSessionBase() {};

      // reimplemented from Session
      virtual void send( Tag *tag ) = 0;

      // reimplemented from MessageHandler
      virtual void handleMessage( Stanza *stanza ) = 0;

  };

}

#endif // MESSAGESESSIONBASE_H__

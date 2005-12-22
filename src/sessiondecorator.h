/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef SESSIONDECORATOR_H__
#define SESSIONDECORATOR_H__

#include "session.h"

namespace gloox
{

  class Tag;

  /**
   * @brief An abstract base class for Session decorators.
   *
   * You should not need to use this class directly.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_EXPORT SessionDecorator : public Session
  {
    public:
      /**
       * Creates a new SessionDecorator.
       * @param session The parent session to decorate.
       */
      SessionDecorator( Session *session ) : m_parent( session ) {};

      // reimplemented from Session
      virtual void send( Tag *tag ) { m_parent->send( tag ); };

    private:
      Session *m_parent;

  };

}

#endif // SESSIONDECORATOR_H__

/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef SESSION_H__
#define SESSION_H__


namespace gloox
{

  class Tag;

  /**
   * @brief An abstract base class for a message/IQ/whatever sessions between two entities.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class Session
  {
    public:
      /**
       * A wrapper around ClientBase::send().
       * @param tag A Tag to send.
       */
      virtual void send( Tag *tag ) = 0;

  };

}

#endif // SESSION_H__

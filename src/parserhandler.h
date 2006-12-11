/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef PARSERHANDLER_H__
#define PARSERHANDLER_H__

namespace gloox
{

  class Stanza;

  enum NodeType
  {
    NodeStreamStart,                /**< The &lt;stream:stream&gt; tag. */
    NodeStreamError,                /**< The &lt;stream:error&gt; tag. */
    NodeStreamClose,                /**< The &lt;/stream:stream&gt; tag. */
    NodeStreamChild                 /**< Everything else. */
  };

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API ParserHandler
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~ParserHandler() {};

      /**
       *
       */
      virtual void handleStanza( NodeType type, Stanza *stanza ) = 0;

  };

}

#endif // PARSERHANDLER_H__

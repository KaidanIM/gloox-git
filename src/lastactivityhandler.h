/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef LASTACTIVITYHANDLER_H__
#define LASTACTIVITYHANDLER_H__

namespace gloox
{

  /**
   *
   */
  class LastActivityHandler
  {
    public:
      /**
       *
       */
      virtual void handleLastActivityResult( const JID& jid, int seconds ) = 0;

      /**
       *
       */
      virtual void handleLastActivityError( const JID& jid, StanzaError error ) = 0;

  };

};

#endif // LASTACTIVITYHANDLER_H__

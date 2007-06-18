/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "iq.h"

namespace gloox
{

  static const std::string msgTypeStringValues[] = {
    "chat", "error", "groupchat", "headline", "normal"
  };

  static inline const std::string& typeString( Message::MessageType type )
    { return msgTypeStringValues[type]; }

  Message::Message( MessageType type, const std::string& id,
                                      const std::string& to,
                                      const std::string& from,
                                      const std::string& body,
                                      const std::string& thread,
                                      const std::string& xmllang,
                                      const std::string& subject )
    : Stanza( "message" )
  {
    addAttribute( "id", id );
    addAttribute( "to", to );
    addAttribute( "from", from );
    addAttribute( "type", typeString( type ) );
    if( !lang.empty() )
      addAttribute( "xml:lang", xmllang );
    new Tag( this, "body", body );
    if( !thread.empty() )
      new Tag( this, "thread", thread );
    if( !subject.empty() )
      new Tag( this, "subject", thread );
  }

}

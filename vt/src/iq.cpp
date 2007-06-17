/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
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

  static const std::string iqTypeStringValues[] = {
    "get", "set", "result", "error"
  };

  static inline const std::string& typeString( IQ::IqType type )
    { return iqTypeStringValues[type]; }

  IQ::IQ( IqType type, const std::string& id,
                       const std::string& to,
                       const std::string& from,
                       const std::string& childtag,
                       const std::string& xmlns )
    : Stanza( "iq" )
  {
    addAttribute( "type", typeString( type ) );
    addAttribute( "id", id );
    addAttribute( "to", to );
    if( !from.empty() )
    {
      addAttribute( "from", from );
      if( !childtag.empty() )
      {
        Tag *tag = new Tag( this, childtag );
        if( !xmlns.empty() )
          tag->addAttribute( "xmlns", xmlns );
      }
    }
  }

}

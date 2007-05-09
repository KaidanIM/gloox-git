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

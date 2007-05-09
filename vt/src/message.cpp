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
                       const std::string& from )
    : Tag( "iq" )
  {
    addAttribute( "id", id );
    addAttribute( "type", typeString( type ) );
    if( !to.empty() )
      addAttribute( "to", to );
    if( !from.empty() )
      addAttribute( "from", from );
  }

  IQ::IQ( IqType type, const std::string& id,
                       const std::string& to,
                       const std::string& from,
                       const std::string& xmlns,
                       const std::string& node )
    : Tag( "iq" )
  {
    addAttribute( "id", id );
    addAttribute( "type", typeString( type ) );
    addAttribute( "to", to );
    addAttribute( "from", from );
    Tag *tag = new Tag( this, "query" );
    tag->addAttribute( "xmlns", xmlns );
    if( !node.empty() )
      tag->addAttribute( "node", node );
  }

}

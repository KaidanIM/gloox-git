#include "iq.h"

namespace gloox
{

  static const std::string IQS = "iq",
                           ID = "id",
                           TO = "to",
                           FROM = "from",
                           TYPE = "type",
                           XMLNS = "xmlns";

  static const std::string iqTypeStringValues[] = {
    "get", "set", "result", "error"
  };

  static inline const std::string& typeString( IQ::IqType type )
    { return iqTypeStringValues[type]; }

  IQ::IQ( IqType type, const std::string& id,
                       const std::string& to,
                       const std::string& from )
    : Tag( IQS )
  {
    addAttribute( ID, id );
    addAttribute( TYPE, typeString( type ) );
    if( !to.empty() )
      addAttribute( TO, to );
    if( !from.empty() )
      addAttribute( FROM, from );
  }

  IQ::IQ( IqType type, const std::string& id,
                       const std::string& to,
                       const std::string& from,
                       const std::string& childtag,
                       const std::string& xmlns,
                       const std::string& node )
    : Tag( IQS )
  {
    addAttribute( ID, id );
    addAttribute( TYPE, typeString( type ) );
    addAttribute( TO, to );
    addAttribute( FROM, from );
    Tag *tag = new Tag( this, childtag );
    tag->addAttribute( XMLNS, xmlns );
    if( !node.empty() )
      tag->addAttribute( "node", node );
  }

}

#include "chatstate.h"
#include "error.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  /* chat state type values */
  static const char * stateValues [] = {
    "active",
    "composing",
    "paused",
    "inactive",
    "gone"
  };

  static inline ChatStateType chatStateType( const std::string& type )
  {
    return (ChatStateType)util::lookup2( type, stateValues );
  }

  ChatState::ChatState( const Tag* tag )
    : StanzaExtension( ExtChatState ),
      m_type( chatStateType( tag->name() ) )
  {
  }

  Tag * ChatState::tag() const
  {
    if( m_type == ChatStateInvalid )
      return 0;

    return new Tag( util::lookup( m_type, stateValues ) );
  }

}

#include "chatstate.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  /* chat state type values */
  static const char* stateValues [] = {
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
      m_state( chatStateType( tag->name() ) )
  {
  }

  Tag* ChatState::tag() const
  {
    if( m_state == ChatStateInvalid )
      return 0;

    return new Tag( util::lookup2( m_state, stateValues ), XMLNS, XMLNS_CHAT_STATES );
  }

}

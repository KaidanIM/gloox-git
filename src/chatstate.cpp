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
      m_csType( chatStateType( tag->name() ) )
  {
  }

  Tag* ChatState::tag() const
  {
    if( m_csType == ChatStateInvalid )
      return 0;

    return new Tag( util::lookup2( m_csType, stateValues ), XMLNS, XMLNS_CHAT_STATES );
  }

}

#include "error.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  /* Error type values */
  static const char * errValues [] = {
    "auth",
    "cancel",
    "continue",
    "modify",
    "wait"
  };

  /* Stanza error values */
  static const char* stanzaErrValues [] = {
    "bad-request",
    "conflict",
    "feature-not-implemented",
    "forbidden",
    "gone",
    "internal-server-error",
    "item-not-found",
    "jid-malformed",
    "not-acceptable",
    "not-allowed",
    "not-authorized",
    "not-modified",
    "payment-required",
    "recipient-unavailable",
    "redirect",
    "registration-required",
    "remote-server-not-found",
    "remote-server-timeout",
    "resource-constraint",
    "service-unavailable",
    "subscription-required",
    "undefined-condition",
    "unexpected-request",
    "unknown-sender"
  };

  static inline StanzaErrorType stanzaErrorType( const std::string& type )
  {
    return (StanzaErrorType)util::lookup( type, errValues );
  }

  static inline StanzaError stanzaError( const std::string& type )
  {
    return (StanzaError)util::lookup( type, stanzaErrValues );
  }

  Error::Error( const Tag* tag )
    : StanzaExtension( ExtError ),
      m_type( stanzaErrorType( tag->findAttribute( TYPE ) ) ),
      m_error( StanzaErrorUndefined ), m_appError( 0 )
  {
    TagList::const_iterator it = tag->children().begin();
    for( ; it != tag->children().end(); ++it )
    {
      StanzaError srt = gloox::stanzaError( (*it)->name() );
      if( srt != StanzaErrorUndefined )
        m_error = srt;
      else if( (*it)->name() == "text" )
        m_text[(*it)->findAttribute("xml:lang")] = (*it)->cdata();
      else
        m_appError = (*it)->clone();
    }
  }

  Error::Error( const Error& error )
        : StanzaExtension( ExtError ), m_type( error.m_type ),
          m_error( error.m_error ), m_appError( error.m_appError ? m_appError->clone() : 0)
  {}

  Error::~Error()
  {
    if( m_appError )
      delete m_appError;
  }

  Tag * Error::tag() const
  {
    if( m_type == StanzaErrorTypeUndefined || m_error == StanzaErrorUndefined )
      return 0;

    Tag * error = new Tag( "error", TYPE, util::lookup( m_type, errValues ) );
    new Tag( error, util::lookup( m_error, stanzaErrValues ), XMLNS, XMLNS_XMPP_STANZAS );

    StringMap::const_iterator it = m_text.begin();
    for( ; it != m_text.end(); ++it )
    {
      Tag * txt = new Tag( error, "text", XMLNS, XMLNS_XMPP_STANZAS );
      txt->addAttribute( "xml:lang", (*it).first );
      txt->setCData( (*it).second );
    }

    if( m_appError )
      error->addChild( m_appError->clone() );

    return error;
  }

  const std::string Error::text( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_text.find( lang );
    return it != m_text.end() ? (*it).second : std::string();
  }

}

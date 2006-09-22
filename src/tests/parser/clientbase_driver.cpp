#include "clientbase_driver.h"


namespace gloox
{

  ClientBase::ClientBase()
  : m_tag( 0 )
  {
  }

  ClientBase::~ClientBase()
  {
  }

  void ClientBase::filter( NodeType type, Stanza *stanza )
  {
    m_tag = stanza->clone();
  }

  Parser::ParserState ClientBase::setTest( Parser *parser, const std::string& str )
  {
    m_tag = 0;
    return parser->feed( str );
  }

  Stanza* ClientBase::getLastResult()
  {
    return m_tag;
  }
}


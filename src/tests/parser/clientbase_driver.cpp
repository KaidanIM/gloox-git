#include "clientbase_driver.h"

#include "stanza.h"
#include "parser.h"

namespace gloox
{

  ClientBase::ClientBase()
  : m_result( false ), m_testNum( 0 )
  {
  }

  ClientBase::~ClientBase()
  {
  }

  void ClientBase::filter( NodeType type, Stanza *stanza )
  {
    switch( m_testNum )
    {
      case 1:
        if( stanza->name() == "tag" )
          m_result = true;
        break;
      case 2:
        if( stanza->name() == "tag" )
          m_result = true;
        break;
      case 3:
        if( stanza->name() == "tag" )
          m_result = true;
        break;
      case 4:
        if( stanza->name() == "tag" )
          m_result = true;
        break;
      case 5:
        if( stanza->name() == "tag" )
          m_result = true;
        break;
      default:
        break;
    }
  }

  void ClientBase::setTest( Parser *parser, int num )
  {
    m_testNum = num;
    switch( m_testNum )
    {
      case 1:
        parser->feed( "<tag/>" );
        break;
      case 2:
        parser->feed( "<tag/>" );
        break;
      case 3:
        parser->feed( "<tag/>" );
        break;
      case 4:
        parser->feed( "<tag/>" );
        break;
      case 5:
        parser->feed( "<tag/>" );
        break;
      default:
        break;
    }
  }

  bool ClientBase::getLastResult()
  {
    bool t = m_result;
    m_result = false;
    return t;
  }
}


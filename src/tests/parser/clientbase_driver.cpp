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
        if( stanza->name() != "tag" )
          return;
        break;
      case 2:
        if( stanza->name() != "tag" )
          return;
        if( !stanza->hasChild( "child" ) )
          return;
        break;
      case 3:
        if( stanza->name() != "tag" )
          return;
        if( !stanza->hasAttribute( "attr", "val" ) )
          return;
        if( !stanza->hasChild( "child" ) )
          return;
        break;
      case 4:
        if( stanza->name() != "tag" )
          return;
        if( !stanza->hasChild( "child" ) )
          return;
        {
          Tag *c = stanza->findChild( "child" );
          if( !c->hasAttribute( "attr", "val" ) )
            return;
        }
        break;
      case 5:
        if( stanza->name() != "tag" )
          return;
        if( stanza->cdata() != "cdata" )
          return;
        break;
      default:
        m_result = false;
        break;
    }

    m_result = true;
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
        parser->feed( "<tag><child/></tag>" );
        break;
      case 3:
        parser->feed( "<tag attr='val'><child/></tag>" );
        break;
      case 4:
        parser->feed( "<tag><child attr='val'/></tag>" );
        break;
      case 5:
        parser->feed( "<tag>cdata</tag>" );
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


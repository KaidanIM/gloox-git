#include "parser.h"
#include "clientbase.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  ClientBase *c = new ClientBase();
  Parser *p = new Parser( c );


  // -------
  name = "simple: <tag/>";
  c->setTest( p, 1 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: <tag><child/></tag>";
  c->setTest( p, 2 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "attribute: <tag attr='val'><child/></tag>";
  c->setTest( p, 3 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "attribute in child: <tag><child attr='val'/></tag>";
  c->setTest( p, 4 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "cdata: <tag>cdata</tag>";
  c->setTest( p, 5 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }




  delete p;
  p = 0;

  if( fail == 0 )
  {
    printf( "Parser: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Parser: %d test(s) failed\n", fail );
    return 1;
  }

}

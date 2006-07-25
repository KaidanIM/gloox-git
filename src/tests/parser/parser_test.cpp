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
  name = "undefined tag";
  c->setTest( p, 1 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "undefined tag";
  c->setTest( p, 2 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "undefined tag";
  c->setTest( p, 3 );
  if( !c->getLastResult() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "undefined tag";
  c->setTest( p, 4 );
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

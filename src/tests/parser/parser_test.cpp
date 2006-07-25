#include "parser.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  Parser *p = 0;

  // -------
  name = "undefined tag";
//   t = new Tag();
//   if( t->type() != StanzaUndefined )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete p;
  p = 0;




  if( fail == 0 )
  {
    printf( "Tag: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Tag: %d test(s) failed\n", fail );
    return 1;
  }

}

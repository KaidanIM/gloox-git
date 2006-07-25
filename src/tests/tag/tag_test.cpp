#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  Tag *t;

  // -------
  name = "undefined tag";
  t = new Tag();
  if( t->type() != StanzaUndefined )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  t = 0;

  name = "empty tag";
  t = new Tag( "test" );
  if( t->name() != "test" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  t = 0;



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

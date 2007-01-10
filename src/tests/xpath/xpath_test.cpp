#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  Tag *aaa = new Tag( "aaa" );
  Tag *bbb = new Tag( aaa, "bbb" );
  Tag *ccc = new Tag( aaa, "ccc" );
  Tag *ddd = new Tag( ccc, "ddd" );
  Tag *eee = new Tag( ccc, "eee" );
  Tag *fff = new Tag( aaa, "fff" );
  Tag *ggg = new Tag( fff, "ggg" );

  // -------
  name = "get root: /";
  if( aaa->findByPath( "/" ) != 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "get root tag: /aaa";
  if( aaa->findByPath( "/aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: /aaa/bbb";
  if( aaa->findByPath( "/aaa/bbb" ) != bbb )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: /aaa/ccc";
  if( aaa->findByPath( "/aaa/ccc" ) != ccc )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: /aaa/ccc/ddd";
  if( aaa->findByPath( "/aaa/ccc/ddd" ) != ddd )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all: //aaa";
  if( aaa->findByPath( "//aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all: //bbb";
  if( aaa->findByPath( "//bbb" ) != bbb )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }






  delete aaa;

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

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


  //-------
  name = "clone test 1";
  t = new Tag( "hallo" );
  Tag *c = t->clone();
  if( *t != *c )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  delete c;
  t = 0;
  c = 0;

  //-------
  name = "clone test 2";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "hello" ) );
  c = t->clone();
  if( *t != *c )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  delete c;
  t = 0;
  c = 0;

  //-------
  name = "clone test 3";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "hello" ) );
  c = new Tag();
  if( *t == *c )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  delete c;
  t = 0;
  c = 0;

  //-------
  name = "clone test 4";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "hello" ) );
  c = new Tag( "test" );
  c->addAttribute( "me", "help" );
  c->addChild( new Tag( "yes" ) );

  if( *t == *c )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  delete c;
  t = 0;
  c = 0;

  //-------
  name = "findTags test";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  if( t->findTags( "test" ).size() != 4 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  delete c;
  t = 0;
  c = 0;









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

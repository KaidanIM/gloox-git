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
  name = "operator== test 1";
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
  name = "operator== test 2";
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
  name = "operator== test 3";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "hello" ) );
  c = new Tag( "hello" );
  c->addAttribute( "test", "bacd" );
  c->addChild( new Tag( "helloo" ) );

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
  name = "operator!= test 1";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "hello" ) );
  c = new Tag( "hello" );
  c->addAttribute( "test", "bacd" );
  c->addChild( new Tag( "hello" ) );

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
  name = "findChildren test";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  if( t->findChildren( "test" ).size() != 4 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete t;
  t = 0;

  //-------
  name = "escape";
  if ( Tag::escape( "&<>'\"" ) != "&amp;&lt;&gt;&apos;&quot;" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  //-------
  name = "relax";
  if ( Tag::relax( "&amp;&lt;&gt;&apos;&quot;&#60;&#62;&#39;&#34;""&#x3c;&#x3e;"
                   "&#x3C;&#x3E;&#x27;&#x22;&#X3c;&#x3e;&#X3C;&#X3E;&#X27;&#X22;" )
        != "&<>'\"<>'\"<><>'\"<><>'\"" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  //-------
  name = "xml() 1";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  if( t->xml() != "<hello test='bacd'><test/><test/><test/><test/></hello>" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), t->xml().c_str() );
  }
  delete t;
  t = 0;

  //-------
  name = "xml() 2 -- known failure";
  t = new Tag( "hello" );
  t->addAttribute( "test", "bacd" );
  t->addAttribute( "foo", "bar" );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  t->addChild( new Tag( "test" ) );
  if( t->xml() != "<hello test='bacd' foo='bar'><test/><test/><test/><test/></hello>" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), t->xml().c_str() );
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

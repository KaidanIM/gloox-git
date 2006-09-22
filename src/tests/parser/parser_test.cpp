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
  std::string data;
  Stanza *s = 0;
  bool tfail = false;
  ClientBase *c = new ClientBase();
  Parser *p = new Parser( c );


  // -------
  name = "simple";
  data = "<tag/>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag" )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

  // -------
  name = "simple child";
  data = "<tag1><child/></tag1>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag1" ||
        !s->hasChild( "child" ) )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

  // -------
  name = "attribute";
  data = "<tag2 attr='val'><child/></tag2>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag2" ||
        !s->hasAttribute( "attr", "val" ) ||
        !s->hasChild( "child" ) )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

  // -------
  name = "attribute in child";
  data = "<tag3><child attr='val'/></tag3>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag3" ||
        !s->hasChild( "child" ) )
  {
    tfail = true;
  }
  else
  {
    Tag *c = s->findChild( "child" );
    if( !c->hasAttribute( "attr", "val" ) )
    {
      tfail = true;
    }
  }
  if( tfail )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
    tfail = false;
  }
  delete s;
  s = 0;

  // -------
  name = "cdata";
  data = "<tag4>cdata</tag4>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag4" ||
        s->cdata() != "cdata" )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

  // -------
  name = "tag w/ whitespace 1";
  data = "< tag4 />";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag4" )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

  // -------
  name = "tag w/ whitespace 2";
  data = "< tag4/ >";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag4" )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

    // -------
  name = "tag w/ whitespace 3";
  data = "< tag4 / >";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag4" )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

//   // -------
//   name = "tag w/ cdata and child";
//   data = "< tag4 > cdata < tag/ ></tag4>";
//   if( c->setTest( p, data ) != Parser::PARSER_BADXML );
//   {
//     s = c->getLastResult();
//     printf( "xml: %s\n", s->xml().c_str() );
//     ++fail;
//     printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
//   }
//   delete s;
//   s = 0;


  // -------
  name = "simple child + white\tspace";
  data = "<tag1 ><child\t/ >< /  \ttag1>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag1" ||
        !s->hasChild( "child" ) )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
  delete s;
  s = 0;

  //-------
  name = "stream start";
  data = "<stream:stream version='1.0' to='example.org' xmlns='jabber:client' id='abcdef'>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "stream:stream" ||
        !s->hasAttribute( "version", "1.0" ) ||
        !s->hasAttribute( "id", "abcdef" ) ||
        !s->hasAttribute( "to", "example.org" ) ||
        !s->hasAttribute( "xmlns", "jabber:client" ) )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
//   printf( "stanza: %s\n", s->xml().c_str() );
  delete s;
  s = 0;

  name = "prolog";
  data = "<?xml version='1.0'?>";
  c->setTest( p, data );
  if( ( ( s = c->getLastResult() ) != 0 )/* ||
        s->name() != "xml" ||
        !s->hasAttribute( "version", "1.0" )*/ )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
  }
//   printf( "stanza: %s\n", s->xml().c_str() );
  delete s;
  s = 0;


  // -------
  name = "deeply nested";
  data = "<tag1 attr11='val11' attr12='val12'><tag2 attr21='val21' attr22='val22'/><tag3 attr31='val31'><tag4>cdata1</tag4><tag4>cdata2</tag4></tag3></tag1>";
  c->setTest( p, data );
  if( ( s = c->getLastResult() ) == 0 ||
        s->name() != "tag1" ||
        !s->hasAttribute( "attr11", "val11" ) ||
        !s->hasAttribute( "attr12", "val12" ) ||
        !s->hasChild( "tag2" ) ||
        !s->hasChild( "tag3" ) )
  {
    printf( "fail1\n" );
    tfail = true;
  }
  else
  {
    Tag *c = s->findChild( "tag2" );
    if( !c->hasAttribute( "attr21", "val21" ) ||
        !c->hasAttribute( "attr22", "val22" ) )
    {
      printf( "fail2\n" );
      tfail = true;
    }
    c = s->findChild( "tag3" );
    if( !c->hasAttribute( "attr31", "val31" ) ||
        !c->hasChild( "tag4" ) ||
        !c->hasChild( "tag4" ) )
    {
      printf( "fail3\n" );
      tfail = true;
    }
  }
  if( tfail )
  {
    ++fail;
    printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
    printf( "got: %s\n", s->xml().c_str() );
    tfail = false;
  }
  delete s;
  s = 0;







  delete p;
  p = 0;
  delete c;
  c = 0;

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

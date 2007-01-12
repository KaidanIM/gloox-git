#include "../../tag.h"
#include "../../xpath.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

void printResult( const std::string& name, Tag::TagList& result )
{
  printf( ">-- %s ------------------------------------------------\n", name.c_str() );
  int i = 0;
  Tag::TagList::const_iterator it = result.begin();
  for( ; it != result.end(); ++it, ++i )
  {
    printf( "tag #%d: %s\n", i, (*it)->xml().c_str() );
  }
  printf( "<-- %s ------------------------------------------------\n", name.c_str() );
}

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  Tag *aaa = new Tag( "aaa" );
  Tag *bbb = new Tag( aaa, "bbb" ); bbb->addAttribute( "name", "b1" );
  Tag *ccc = new Tag( aaa, "ccc" );
  Tag *ddd = new Tag( ccc, "ddd" );
  Tag *eee = new Tag( ccc, "eee" );
  Tag *fff = new Tag( aaa, "fff" );
  Tag *ggg = new Tag( fff, "ggg" );
  Tag *hhh = new Tag( bbb, "hhh" );
  Tag *iii = new Tag( bbb, "bbb" ); iii->addAttribute( "name", "b2" );
  Tag *jjj = new Tag( hhh, "bbb" ); jjj->addAttribute( "name", "b3" );
  Tag::TagList result;

  // -------
  name = "get root: /";
  if( XPath::findTag( aaa, "/" ) != 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "get root tag: /aaa";
  if( XPath::findTag( aaa, "/aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: /aaa/bbb";
  if( XPath::findTag( aaa, "/aaa/bbb" ) != bbb )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: /aaa/ccc";
  if( XPath::findTag( aaa, "/aaa/ccc" ) != ccc )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "simple child: /aaa/ccc/ddd";
  if( XPath::findTag( aaa, "/aaa/ccc/ddd" ) != ddd )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
//   printf( "------------------------------------------------\n" );

  // -------
  name = "find all: //aaa";
  if( XPath::findTag( aaa, "//aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
//   printf( "------------------------------------------------\n" );

  // -------
  name = "find all: //eee";
  if( XPath::findTag( aaa, "//eee" ) != eee )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
//   printf( "------------------------------------------------\n" );

  // -------
  name = "find all: //bbb";
  if( XPath::findTag( aaa, "//bbb" ) != bbb )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
//   printf( "------------------------------------------------\n" );

  // -------
  name = "get root tag from child: /aaa";
  if( XPath::findTag( bbb, "/aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "fail test 1: /abc";
  if( XPath::findTag( aaa, "/abc" ) != 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "fail test 2: /bbb";
  if( XPath::findTag( aaa, "/bbb" ) != 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "relative find 1: aaa";
  if( XPath::findTag( aaa, "aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "relative find 2: bbb";
  if( XPath::findTag( bbb, "bbb" ) != bbb )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find TagList: //bbb";
  result = XPath::findTagList( aaa, "//bbb" );
  if( result.size() != 3 || result.front() != bbb || result.back() != iii )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all: //*";
  result = XPath::findTagList( aaa, "//*" );
  if( result.size() != 10 || result.front() != aaa || result.back() != ggg )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find first level: /*";
  result = XPath::findTagList( aaa, "/*" );
  if( result.size() != 1 || result.front() != aaa )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find second level: /*/*";
  result = XPath::findTagList( aaa, "/*/*" );
  if( result.size() != 3 || result.front() != bbb || result.back() != fff )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find third level: /*/*/*";
  result = XPath::findTagList( aaa, "/*/*/*" );
  if( result.size() != 5 || result.front() != hhh || result.back() != ggg )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find fourth level: /*/*/*/*";
  result = XPath::findTagList( aaa, "/*/*/*/*" );
  if( result.size() != 1 || result.front() != jjj )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find fith level: /*/*/*/*/*";
  result = XPath::findTagList( aaa, "/*/*/*/*/*" );
  if( result.size() != 0 )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all sub-bbb: /*/*//bbb";
  result = XPath::findTagList( aaa, "/*/*//bbb" );
  if( result.size() != 2 || result.front() != jjj || result.back() != iii )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }





// <aaa>
//   <bbb name='b1'>
//     <hhh>
//       <bbb name='b3'/>
//     </hhh>
//     <bbb name='b2'/>
//   </bbb>
//   <ccc>
//     <ddd/>
//     <eee/>
//   </ccc>
//   <fff>
//     <ggg/>
//   </fff>
// </aaa>










  delete aaa;

  if( fail == 0 )
  {
    printf( "XPath: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "XPath: %d test(s) failed\n", fail );
    return 1;
  }

}

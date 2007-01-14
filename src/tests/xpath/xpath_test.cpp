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
  Tag::TagList::const_iterator it;

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

  // -------
  name = "get root: /";
  if( XPath::findTag( aaa, "/" ) != 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "get deeproot: //";
  if( XPath::findTag( aaa, "//" ) != 0 )
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

  // -------
  name = "find all: //aaa";
  if( XPath::findTag( aaa, "//aaa" ) != aaa )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all: //eee";
  if( XPath::findTag( aaa, "//eee" ) != eee )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all: //bbb";
  if( XPath::findTag( aaa, "//bbb" ) != bbb )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

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
  it = result.begin();
  if( result.size() != 3 || (*it) != bbb || (*++it) != jjj || (*++it) != iii )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find TagList: //ggg";
  result = XPath::findTagList( aaa, "//ggg" );
  it = result.begin();
  if( result.size() != 1 || (*it) != ggg )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find all: //*";
  result = XPath::findTagList( aaa, "//*" );
  it = result.begin();
  if( result.size() != 10 || (*it) != aaa || (*++it) != bbb || (*++it) != hhh ||
      (*++it) != jjj || (*++it) != iii || (*++it) != ccc || (*++it) != ddd ||
      (*++it) != eee || (*++it) != fff || (*++it) != ggg )
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
  it = result.begin();
  if( result.size() != 3 || (*it) != bbb || (*++it) != ccc || (*++it) != fff )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find third level: /*/*/*";
  result = XPath::findTagList( aaa, "/*/*/*" );
  it = result.begin();
  if( result.size() != 5 || (*it) != hhh || (*++it) != iii ||
      (*++it) != ddd || (*++it) != eee || (*++it) != ggg )
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

  // -------
  name = "find second level bbb: /*/bbb";
  result = XPath::findTagList( aaa, "/*/bbb" );
  if( result.size() != 1 || result.front() != bbb )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find second level via self/noop: /*/./*";
  result = XPath::findTagList( aaa, "/*/./*" );
  it = result.begin();
  if( result.size() != 3 || (*it) != bbb || (*++it) != ccc || (*++it) != fff )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "find second level via repeated self/noop: /*/././*";
  result = XPath::findTagList( aaa, "/*/././*" );
  it = result.begin();
  if( result.size() != 3 || (*it) != bbb || (*++it) != ccc || (*++it) != fff )
  {
    ++fail;
    printResult( name, result );
    printf( "test '%s' failed\n", name.c_str() );
  }

//   // -------
//   name = "find first level via parent: /*/../*";
//   result = XPath::findTagList( aaa, "/*/../*" );
//   it = result.begin();
//   if( result.size() != 1 || (*it) != aaa )
//   {
//     ++fail;
//     printResult( name, result );
//     printf( "test '%s' failed\n", name.c_str() );
//   }

//   // -------
//   name = "invalid parent: /../*";
//   result = XPath::findTagList( aaa, "/../*" );
//   it = result.begin();
//   if( result.size() != 0 )
//   {
//     ++fail;
//     printResult( name, result );
//     printf( "test '%s' failed\n", name.c_str() );
//   }

//   printf( "--------------------------------------------------------------\n" );
//   // -------
//   name = "select non-leaf elements: //..";
//   result = XPath::findTagList( aaa, "//.." );
//   it = result.begin();
//   if( result.size() != 5 || (*it) != aaa || (*++it) != bbb ||
//       (*++it) != hhh || (*++it) != ccc || (*++it) != fff )
//   {
//     ++fail;
//     printResult( name, result );
//     printf( "test '%s' failed\n", name.c_str() );
//   }
//   printf( "--------------------------------------------------------------\n" );

//   // -------
//   name = "deepsearch && .. combined 1: //ggg/..";
//   result = XPath::findTagList( aaa, "//ggg/.." );
//   it = result.begin();
//   if( result.size() != 1 || (*it) != fff )
//   {
//     ++fail;
//     printResult( name, result );
//     printf( "test '%s' failed\n", name.c_str() );
//   }
//   printf( "--------------------------------------------------------------\n" );

//   // -------
//   name = "deepsearch && .. combined 2: //ggg/../../bbb";
//   result = XPath::findTagList( aaa, "//ggg/../../bbb" );
//   it = result.begin();
//   if( result.size() != 1 || (*it) != bbb )
//   {
//     ++fail;
//     printResult( name, result );
//     printf( "test '%s' failed\n", name.c_str() );
//   }
//   printf( "--------------------------------------------------------------\n" );









//   Tag *c2 = new Tag( ddd, "ccc" );
//   Tag *c3 = new Tag( c2, "ccc" );
//   Tag *c4 = new Tag( eee, "ccc" );
//   Tag *c5 = new Tag( c4, "ccc" );

// <aaa>
//   <bbb name='b1'>
//     <hhh>
//       <bbb name='b3'/>
//     </hhh>
//     <bbb name='b2'/>
//   </bbb>
//   <ccc>
//     <ddd>
//       <ccc>
//         <ccc/>
//       </ccc>
//     </ddd>
//     <eee>
//       <ccc>
//         <ccc/>
//       </ccc>
//     </eee>
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

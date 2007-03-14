#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <sys/time.h>

static double divider = 1000000;
static int num = 100000;
static double t;

static Tag *tag;

static void printTime ( const char * testName, struct timeval tv1, struct timeval tv2 )
{
  t = tv2.tv_sec - tv1.tv_sec;
  t +=  ( tv2.tv_usec - tv1.tv_usec ) / divider;
  printf( "%d %s took %.02f seconds\n", num, testName, t );
  printf( "that's %.02f per sec\n", num / t );
}

static Tag * newTag ( const char *str )
{
  Tag *aaa = new Tag( str );
  Tag *bbb = new Tag( aaa, str ); bbb->addAttribute( str, str );
  Tag *ccc = new Tag( aaa, str ); ccc->addAttribute( str, str );
  Tag *ddd = new Tag( ccc, str ); ddd->addAttribute( str, str );
  Tag *eee = new Tag( ccc, str ); eee->addAttribute( str, str );
  Tag *fff = new Tag( aaa, str ); fff->addAttribute( str, str );
  Tag *ggg = new Tag( fff, str ); ggg->addAttribute( str, str );
  Tag *hhh = new Tag( bbb, str ); hhh->addAttribute( str, str );
  Tag *iii = new Tag( bbb, str ); iii->addAttribute( str, str );
  Tag *jjj = new Tag( hhh, str ); jjj->addAttribute( str, str );
  return aaa;
}

static const char * simpleString  = "azzaaaggaaaaqs dfqsdadddaads dfqsd faa";
static const char * escapedString = ">aa< < <w<w wx.'c <sdz& %)(>><<<<.\"''";
static const char * escapableString = "&amp;&lt;&gt;&apos;&quot;&#60;&#62;&#39;&#34;&#x3c;&#x3e;&#x3C;"
                                      "&#x3E;&#x27;&#x22;&#X3c;&#X3e;&#X3C;&#X3E;&#X27;&#X22;";

static inline Tag * newSimpleTag ()  { newTag(  simpleString ); }
static inline Tag * newEscapedTag () { newTag( escapedString ); }
static inline Tag * newEscapableTag () { newTag( escapableString ); }


int main( int /*argc*/, char* /*argv[]*/ )
{
  struct timeval tv1;
  struct timeval tv2;

  printf( "=== Testing without escaping ===\n" );

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    tag = newSimpleTag();
    delete tag;
  }
  gettimeofday( &tv2, 0 );
  printTime ("create/delete", tv1, tv2);

  // -----------------------------------------------------------------------

  tag = newSimpleTag();

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    tag->xml();
  }
  gettimeofday( &tv2, 0 );
  printTime ("xml()", tv1, tv2);


  // -----------------------------------------------------------------------

  printf( "=== Testing with escaping ===\n" );

  
  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    tag = newEscapableTag();
    delete tag;
  }
  gettimeofday( &tv2, 0 );
  printTime ("create/delete", tv1, tv2);


  // ---------------------------------------------------------------------

  tag = newEscapedTag();

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    tag->xml();
  }
  gettimeofday( &tv2, 0 );
  printTime ("xml()", tv1, tv2);


  // ---------------------------------------------------------------------

  printf( "=== Testing escaping independent functions ===\n" );

  tag = newSimpleTag();

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    delete tag->clone();
  }
  gettimeofday( &tv2, 0 );
  printTime ("clone/delete", tv1, tv2);



  delete tag;

  return 0;
}

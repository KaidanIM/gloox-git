#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <sys/time.h>

static double divider = 1000000;
static int num = 10000;
static double t;

static Tag *tag;

static void printTime ( const char * testName, struct timeval tv1, struct timeval tv2 )
{
  t = tv2.tv_sec - tv1.tv_sec;
  t +=  ( tv2.tv_usec - tv1.tv_usec ) / divider;
  printf( "%s: %.02f seconds (%.02f/s)\n", testName, t, num / t );
}

static Tag * newTag ( const char *str, bool incoming = false )
{
  Tag *aaa = new Tag( str, str, incoming );
  Tag *bbb = new Tag( aaa, str, str, incoming ); bbb->addAttribute( str, str );
  Tag *ccc = new Tag( aaa, str, str, incoming ); ccc->addAttribute( str, str );
  Tag *ddd = new Tag( ccc, str, str, incoming ); ddd->addAttribute( str, str );
  Tag *eee = new Tag( ccc, str, str, incoming ); eee->addAttribute( str, str );
  Tag *fff = new Tag( aaa, str, str, incoming ); fff->addAttribute( str, str );
  Tag *ggg = new Tag( fff, str, str, incoming ); ggg->addAttribute( str, str );
  Tag *hhh = new Tag( bbb, str, str, incoming ); hhh->addAttribute( str, str );
  Tag *iii = new Tag( bbb, str, str, incoming ); iii->addAttribute( str, str );
  Tag *jjj = new Tag( hhh, str, str, incoming ); jjj->addAttribute( str, str );
  return aaa;
}

static const char * simpleString  = "azzaaaggaaaaqs dfqsdadddaads dfqsd faaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
static const char * escapableString = ">aa< < <w<w wx.'c <sdz& %)(>><<<<.\"''";
static const char * escapedString = "&amp;&lt;&gt;&apos;&quot;&#60;&#62;&#39;&#34;&#x3c;&#x3e;&#x3C;"
                                      "&#x3E;&#x27;&#x22;&#X3c;&#X3e;&#X3C;&#X3E;&#X27;&#X22;";

static inline Tag * newSimpleTag ()    { return newTag( simpleString ); }
static inline Tag * newEscapedTag ()   { return newTag( escapedString ); }
static inline Tag * newEscapableTag () { return newTag( escapableString, true ); }


int main( int /*argc*/, char* /*argv[]*/ )
{
  struct timeval tv1;
  struct timeval tv2;

  printf( "Testing %d...\n", num );

  tag = newSimpleTag();
  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    tag->xml();
  }
  gettimeofday( &tv2, 0 );
  delete tag;
  printTime ("non escaping xml", tv1, tv2);


  // ---------------------------------------------------------------------

  tag = newEscapableTag();
  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    tag->xml();
  }
  gettimeofday( &tv2, 0 );
  delete tag;
  printTime ("escaping xml", tv1, tv2);


  // ---------------------------------------------------------------------

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    delete newSimpleTag();
  }
  gettimeofday( &tv2, 0 );
  printTime ("non relaxing create/delete", tv1, tv2);


  // -----------------------------------------------------------------------
  
  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    delete newEscapedTag();
  }
  gettimeofday( &tv2, 0 );
  printTime ("relaxing create/delete", tv1, tv2);


  // -----------------------------------------------------------------------

  tag = newSimpleTag();

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    delete tag->clone();
  }
  gettimeofday( &tv2, 0 );
  printTime ("clone/delete", tv1, tv2);

  // -----------------------------------------------------------------------

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

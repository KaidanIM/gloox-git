#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

#include <sys/time.h>

int main( int /*argc*/, char* /*argv[]*/ )
{
  struct timeval tv1;
  struct timeval tv2;
  gettimeofday( &tv1, 0 );
  int num = 100000;
  for( int i = 0; i < num; ++i )
  {
    Tag *aaa = new Tag( "aaa" );
    Tag *bbb = new Tag( aaa, "bbb" ); bbb->addAttribute( "name", "b1" );
    Tag *ccc = new Tag( aaa, "ccc" ); ccc->addAttribute( "name", "b1" );
    Tag *ddd = new Tag( ccc, "ddd" ); ddd->addAttribute( "name", "b1" );
    Tag *eee = new Tag( ccc, "eee" ); eee->addAttribute( "name", "b1" );
    Tag *fff = new Tag( aaa, "fff" ); fff->addAttribute( "name", "b1" );
    Tag *ggg = new Tag( fff, "ggg" ); ggg->addAttribute( "name", "b1" );
    Tag *hhh = new Tag( bbb, "hhh" ); hhh->addAttribute( "name", "b1" );
    Tag *iii = new Tag( bbb, "bbb" ); iii->addAttribute( "name", "b2" );
    Tag *jjj = new Tag( hhh, "bbb" ); jjj->addAttribute( "name", "b3" );

    std::string xml = aaa->xml();

    delete aaa;
  }
  gettimeofday( &tv2, 0 );

  long int t = tv2.tv_sec - tv1.tv_sec;
  printf( "%d times create/delete took %ld seconds\n", num, t );
  printf( "that's %ld per sec\n", num / t );

  // -----------------------------------------------------------------------

  Tag *aaa = new Tag( "aaa" );
  Tag *bbb = new Tag( aaa, "bbb" ); bbb->addAttribute( "name", "b1" );
  Tag *ccc = new Tag( aaa, "ccc" ); ccc->addAttribute( "name", "b1" );
  Tag *ddd = new Tag( ccc, "ddd" ); ddd->addAttribute( "name", "b1" );
  Tag *eee = new Tag( ccc, "eee" ); eee->addAttribute( "name", "b1" );
  Tag *fff = new Tag( aaa, "fff" ); fff->addAttribute( "name", "b1" );
  Tag *ggg = new Tag( fff, "ggg" ); ggg->addAttribute( "name", "b1" );
  Tag *hhh = new Tag( bbb, "hhh" ); hhh->addAttribute( "name", "b1" );
  Tag *iii = new Tag( bbb, "bbb" ); iii->addAttribute( "name", "b2" );
  Tag *jjj = new Tag( hhh, "bbb" ); jjj->addAttribute( "name", "b3" );

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    std::string xml = aaa->xml();
  }
  gettimeofday( &tv2, 0 );

  t = tv2.tv_sec - tv1.tv_sec;
  printf( "%d times xml() loop took %ld seconds\n", num, t );
  printf( "that's %ld per sec\n", num / t );

  // ---------------------------------------------------------------------

  Tag *z = 0;

  gettimeofday( &tv1, 0 );
  for( int i = 0; i < num; ++i )
  {
    z = aaa->clone();
    std::string xml = z->xml();
    delete z;
  }
  gettimeofday( &tv2, 0 );

  t = tv2.tv_sec - tv1.tv_sec;
  printf( "%d times clone() took %ld seconds\n", num, t );
  printf( "that's %ld per sec\n", num / t );

  delete aaa;

  return 0;
}

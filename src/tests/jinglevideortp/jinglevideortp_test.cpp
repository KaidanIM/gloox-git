#define JINGLE_TEST
#include "../../gloox.h"
#include "../../jid.h"
#include "../../tag.h"
#include "../../jinglevideortp.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>


int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;

  // -------
  {
    name = "invalid Jingle 1";
    Jingle::VideoRTP a;
    Tag* t = a.tag();
    if( t )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
    delete t;
  }




  printf( "Jingle::VideoRTP: " );
  if( fail == 0 )
  {
    printf( "OK\n" );
    return 0;
  }
  else
  {
    printf( "%d test(s) failed\n", fail );
    return 1;
  }

}

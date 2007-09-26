#include "../../tag.h"
#include "../../delayeddelivery.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  Tag *t;
  DelayedDelivery *d;
  JID j( "abc@def/ghi" );
  Tag *x = new Tag( "x", "reason" );
  x->addAttribute( "stamp", "invalidstamp" );
  x->addAttribute( "from", j.full() );
  x->addAttribute( "xmlns", XMLNS_X_DELAY );

  // -------
  name = "parsing 0 tag";
  d = new DelayedDelivery( 0 );
  if( d->tag() != 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete d;
  d = 0;

  // -------
  name = "from Tag/getters";
  d = new DelayedDelivery( x );
  if( d->reason() != "reason" || d->stamp() != "invalidstamp" || d->from() != j )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete d;
  d = 0;




  delete x;
  x = 0;


  if( fail == 0 )
  {
    printf( "DelayedDelivery: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "DelayedDelivery: %d test(s) failed\n", fail );
    return 1;
  }

}

#include "../../util.h"

#include <string>

using namespace gloox;

int main()
{
  enum { A, B, C, Inval };
  static const char * values[] = { "a", "b", "c" };
  enum { D = 1<<0, E = 1<<1, F = 1<<2, Inval2 = 1<<3 };
  static const char * values2[] = { "d", "e", "f" };
  int fail = 0;

  std::string name = "string lookup";
  if( util::lookup( "a", values ) != A )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "invalid string lookup";
  if( util::lookup( "", values ) != Inval )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "code lookup";
  if( util::lookup( A, values ) != "a" )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "invalid code lookup";
  if( util::lookup( Inval, values ) != 0 )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "out-of-range code lookup";
  if( util::lookup( 700, values ) != 0 )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "string lookup (ORable)";
  if( util::lookup2( "d", values2 ) != D )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "invalid string lookup (ORable)";
  if( util::lookup2( "", values2 ) != Inval2 )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "code lookup (ORable)";
  if( util::lookup2( D, values2 ) != "d" )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "invalid code lookup (ORable)";
  if( util::lookup2( Inval2, values2 ) != 0 )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }


  name = "out-of-range code lookup (ORable)";
  if( util::lookup2( 700, values2 ) != 0 )
  {
    printf( "test '%s' failed\n", name.c_str() );
    ++fail;
  }




  if( fail == 0 )
  {
    printf( "Util: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Util: %d test(s) failed\n", fail );
    return 1;
  }

}

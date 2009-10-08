#include "../../md4.h"
using namespace gloox;

#include <locale.h>
#include <string>
#include <cstdio> // [s]print[f]

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name, expected;
  MD4 md4;



  // -------
  name = "empty string";
  expected = "31d6cfe0d16ae931b73c59d7e0c089c0";
  md4.feed( "" );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "a";
  expected = "bde52cb31de33e46245e05fbdbd6fb24";
  md4.feed( name );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "abc";
  expected = "a448017aaf21d8525fc10ae87aa6729d";
  md4.feed( name );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "message digest";
  expected = "d9130a8164549fe818874806e1c7014b";
  md4.feed( name );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "abcdefghijklmnopqrstuvwxyz";
  expected = "d79e1c308aa5bbcdeea8ed63df412da9";
  md4.feed( name );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  expected = "043f8582f241db351ce627e153e7f0e4";
  md4.feed( name );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
  expected = "e33b4ddc9c38f2199c3e7b164fcc0536";
  md4.feed( name );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "two-step";
  expected = "e33b4ddc9c38f2199c3e7b164fcc0536";
  md4.feed( "123456789012345678901234567890123456789012");
  md4.feed( "34567890123456789012345678901234567890" );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "many-step";
  expected = "043f8582f241db351ce627e153e7f0e4";
  md4.feed( "ABCDEF" );
  md4.feed( "GHIJKLMNOPQRSTUVWXYZab" );
  md4.feed( "" );
  md4.feed( "cdefghijklmn" );
  md4.feed( "opqrstuvwxyz0123456789" );
  md4.finalize();
  if( md4.hex() != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md4.hex().c_str() );
  }
  md4.reset();

  // -------
  name = "static -- \"message digest\"";
  expected = "d9130a8164549fe818874806e1c7014b";
  if( MD4::md4( "message digest" ) != expected )
  {
    printf( "expected: %s\n", expected.c_str() );
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }


  printf( "MD4: " );
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

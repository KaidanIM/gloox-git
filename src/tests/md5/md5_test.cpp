#include "../../md5.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  MD5 md5;



  // -------
  name = "empty string";
  md5.feed( "", true );
  if( md5.pretty() != "d41d8cd98f00b204e9800998ecf8427e" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  md5.reset();

  // -------
  name = "The quick brown fox jumps over the lazy dog";
  md5.feed( name, true );
  if( md5.pretty() != "9e107d9d372bb6826bd81d3542a419d6" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md5.pretty().c_str() );
  }
  md5.reset();

  // -------
  name = "The quick brown fox jumps over the lazy cog";
  md5.feed( name, true );
  if( md5.pretty() != "1055d3e698d289f2af8663725127bd4b" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md5.pretty().c_str() );
  }
  md5.reset();

  // -------
  name = "two-step";
  md5.feed( "The quick brown fox ", false);
  md5.feed( "jumps over the lazy dog", true );
  if( md5.pretty() != "9e107d9d372bb6826bd81d3542a419d6" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md5.pretty().c_str() );
  }
  md5.reset();

  // -------
  name = "many-step";
  md5.feed( "The", false );
  md5.feed( " quick bro", false );
  md5.feed( "", false );
  md5.feed( "wn fox ", false);
  md5.feed( "jumps over the lazy dog", true );
  if( md5.pretty() != "9e107d9d372bb6826bd81d3542a419d6" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), md5.pretty().c_str() );
  }
  md5.reset();



  if( fail == 0 )
  {
    printf( "MD5: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "MD5: %d test(s) failed\n", fail );
    return 1;
  }


}

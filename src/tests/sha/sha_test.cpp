#include "../../sha.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  SHA sha;



  // -------
  name = "empty string";
  sha.feed( "", true );
  if( sha.pretty() != "da39a3ee5e6b4b0d3255bfef95601890afd80709" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  sha.reset();

  // -------
  name = "The quick brown fox jumps over the lazy dog";
  sha.feed( name, true );
  if( sha.pretty() != "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), sha.pretty().c_str() );
  }
  sha.reset();

  // -------
  name = "The quick brown fox jumps over the lazy cog";
  sha.feed( name, true );
  if( sha.pretty() != "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), sha.pretty().c_str() );
  }
  sha.reset();

  // -------
  name = "two-step";
  sha.feed( "The quick brown fox ", false);
  sha.feed( "jumps over the lazy dog", true );
  if( sha.pretty() != "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), sha.pretty().c_str() );
  }
  sha.reset();

  // -------
  name = "many-step";
  sha.feed( "The", false );
  sha.feed( " quick bro", false );
  sha.feed( "", false );
  sha.feed( "wn fox ", false);
  sha.feed( "jumps over the lazy dog", true );
  if( sha.pretty() != "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), sha.pretty().c_str() );
  }
  sha.reset();



  if( fail == 0 )
  {
    printf( "SHA: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "SHA: %d test(s) failed\n", fail );
    return 1;
  }


}

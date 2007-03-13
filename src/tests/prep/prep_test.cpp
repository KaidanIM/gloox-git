#include "../../prep.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;

  // -------
  name = "nodeprep oversized";
  const std::string t( 1200, 'x' );
  if( Prep::nodeprep( t ) != "" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "resourceprep oversized";
  if( Prep::resourceprep( t ) != "" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "nameprep oversized";
  if( Prep::nameprep( t ) != "" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "idna oversized";
  if( Prep::idna( t ) != "" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "nodeprep unchanged";
  const std::string t1( 10, 'x' );
  if( Prep::nodeprep( t1 ) != t1 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "resourceprep unchanged";
  if( Prep::resourceprep( t1 ) != t1 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "nameprep unchanged";
  if( Prep::nameprep( t1 ) != t1 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "idna unchanged";
  if( Prep::idna( t1 ) != t1 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "nodeprep simple casefolding";
  const std::string t2( "aBcDeFgH" );
  const std::string t3( "abcdefgh" );
  if( Prep::nodeprep( t2 ) != t3 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "resourceprep simple casefolding (none)";
  if( Prep::resourceprep( t2 ) != t2 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "nameprep simple casefolding";
  if( Prep::nameprep( t2 ) != t3 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "idna example";
  if( Prep::idna( "www.dömäin.de" ) != "www.xn--dmin-moa0i.de" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }












  if( fail == 0 )
  {
    printf( "Prep: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Prep: %d test(s) failed\n", fail );
    return 1;
  }

}

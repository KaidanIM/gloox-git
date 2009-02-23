#include "../../prep.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  std::string result;

  // -------
  name = "nodeprep oversized";
  const std::string t( 1200, 'x' );
  if( prep::nodeprep( t, result ) || !result.empty() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "resourceprep oversized";
  if( prep::resourceprep( t, result ) || !result.empty() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "nameprep oversized";
  if( prep::nameprep( t, result ) || !result.empty() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "idna oversized";
  if( prep::idna( t, result ) || !result.empty() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "nodeprep unchanged";
  const std::string t1( 10, 'x' );
  if( !( prep::nodeprep( t1, result ) && result == t1 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "resourceprep unchanged";
  if( !( prep::resourceprep( t1, result ) && result == t1 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "nameprep unchanged";
  if( !( prep::nameprep( t1, result ) && result == t1 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  // -------
  name = "idna unchanged";
  if( !( prep::idna( t1, result ) && result == t1 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";

  const std::string t2( "aBcDeFgH" );
  const std::string t3( "abcdefgh" );
  // -------
  name = "nodeprep simple casefolding";
#ifndef HAVE_LIBIDN
  printf( "Libidn not enabled. Skipped '%s' test\n", name.c_str() );
#else
  if( !( prep::nodeprep( t2, result ) && result == t3 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";
#endif
  // -------
  name = "resourceprep simple casefolding (none)";
#ifndef HAVE_LIBIDN
  printf( "Libidn not enabled. Skipped '%s' test\n", name.c_str() );
#else
  if( !( prep::resourceprep( t2, result ) && result == t2 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";
#endif

  // -------
  name = "nameprep simple casefolding";
#ifndef HAVE_LIBIDN
  printf( "Libidn not enabled. Skipped '%s' test\n", name.c_str() );
#else
  if( !( prep::nameprep( t2, result ) && result == t3 ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";
#endif

  // -------
  name = "idna example";
#ifndef HAVE_LIBIDN
  printf( "Libidn not enabled. Skipped '%s' test\n", name.c_str() );
#else
  if( !( prep::idna( "www.dömäin.de", result ) && result == "www.xn--dmin-moa0i.de" ) )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  result = "";
#endif











  if( fail == 0 )
  {
    printf( "Prep: OK\n" );
    return 0;
  }
  else
  {
    printf( "Prep: %d test(s) failed\n", fail );
    return 1;
  }

}

#include "../jclient.h"
#include "../prep.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int argc, char* argv[] )
{
  setlocale( LC_ALL, "" );

  JClient *j = new JClient();

  j->setServer( "ümlaut.domain.org" );
  printf( "domain: %s\n", j->server().c_str() );

  j->setUsername( "Stränge Name]_/%§_\\\"$" );
  printf( "username: %s\n", j->username().c_str() );

  j->setResource( "mö resource" );
  printf( "resource: %s\n", j->resource().c_str() );

//   printf( "punycode: haus-lüssow.de %s\n", Prep::punycode( "haus-lüssow.de" ) );

  std::string tmp( "www.ümlaut.domain.org" );
  std::string tmp2 = Prep::idna( tmp );
  printf( "idna: www.ümlaut.domain.org %s\n", tmp2.c_str() );

  delete( j );
}

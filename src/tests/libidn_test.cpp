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

  j->setServer( "�mlaut.domain.org" );
  printf( "domain: %s\n", j->server().c_str() );

  j->setUsername( "Str�nge Name]_/%�_\\\"$" );
  printf( "username: %s\n", j->username().c_str() );

  j->setResource( "m� resource" );
  printf( "resource: %s\n", j->resource().c_str() );

//   printf( "punycode: haus-l�ssow.de %s\n", Prep::punycode( "haus-l�ssow.de" ) );

  std::string tmp( "www.�mlaut.domain.org" );
  std::string tmp2 = Prep::idna( tmp );
  printf( "idna: www.�mlaut.domain.org %s\n", tmp2.c_str() );

  delete( j );
}

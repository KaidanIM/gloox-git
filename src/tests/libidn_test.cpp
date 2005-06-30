#include "../jclient.h"

#include <stdio.h>

int main( int argc, char* argv[] )
{
  JClient *j = new JClient();

  j->setServer( "�mlaut.domain.org" );
  printf( "domain: %s\n", j->server().c_str() );

  j->setUsername( "Str�nge Name$" );
  printf( "username: %s\n", j->username().c_str() );

  j->setResource( "m� resource" );
  printf( "resource: %s\n", j->resource().c_str() );
}

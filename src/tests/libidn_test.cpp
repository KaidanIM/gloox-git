#include "../jclient.h"

#include <stdio.h>

int main( int argc, char* argv[] )
{
  JClient *j = new JClient();

  j->setServer( "ümlaut.domain.org" );
  printf( "domain: %s\n", j->server().c_str() );

  j->setUsername( "Stränge Name$" );
  printf( "username: %s\n", j->username().c_str() );

  j->setResource( "mö resource" );
  printf( "resource: %s\n", j->resource().c_str() );
}

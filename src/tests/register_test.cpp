#include "../jclient.h"
#include "../prep.h"
#include "../registration.h"

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int argc, char* argv[] )
{
  JClient *j = new JClient();
  j->setServer( "jabber.cc" );
  j->setDebug( true );
  j->set_log_hook();


  Registration *r = new Registration( j );

  j->connect( false );

}

/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 * published under the GPL
 */


#include "connect.h"
#include "../jlib/jclient.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  Connect c;
  c.start();

  return 0;
}

Connect::Connect()
{
}

Connect::~Connect()
{
}

void Connect::start()
{
  c = new JClient( "mailmon", "test", "test", "jabber.cc" );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( true );
  c->setDebug( true );
  c->connect();
  c->disconnect();
}

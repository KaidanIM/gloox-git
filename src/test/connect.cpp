/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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
  printf("hello world\n");
  c->disconnect();
}

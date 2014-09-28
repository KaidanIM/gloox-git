/*
 *  Copyright (c) 2004-2014 by Jakob Schröter <js@camaya.net>
 *  This file is part of the gloox library. http://camaya.net/gloox
 *
 *  This software is distributed under a license. The full license
 *  agreement can be found in the file LICENSE in this distribution.
 *  This software may not be copied, modified, sold or distributed
 *  other than expressed in the named license agreement.
 *
 *  This software is distributed without any warranty.
 */

#include "../prep.h"
using namespace gloox;

#include <string>
#include <iostream>

#include <cstdio> // [s]print[f]

int main( int /*argc*/, char** /*argv*/ )
{
  std::string test = "OLD\\Administrator";
  printf( "orig: %s -- nodeprepped: %s\n", test.c_str(), Prep::nodeprep( test ).c_str() );
  printf( "orig: %s -- nameprepped: %s\n", test.c_str(), Prep::nameprep( test ).c_str() );
  printf( "orig: %s -- resourceprepped: %s\n", test.c_str(), Prep::resourceprep( test ).c_str() );
}

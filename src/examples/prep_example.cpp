#include "../prep.h"
using namespace gloox;

#include <string>
#include <iostream>

int main( int /*argc*/, char** /*argv*/ )
{
  std::string test = "OLD\\Administrator";
  printf( "orig: %s -- nodeprepped: %s\n", test.c_str(), Prep::nodeprep( test ).c_str() );
  printf( "orig: %s -- nameprepped: %s\n", test.c_str(), Prep::nameprep( test ).c_str() );
  printf( "orig: %s -- resourceprepped: %s\n", test.c_str(), Prep::resourceprep( test ).c_str() );
}

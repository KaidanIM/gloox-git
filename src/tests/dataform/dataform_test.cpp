#include "../../dataform.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  bool fail = false;
  std::string name;
  DataForm *f;

  // -------
  name = "empty form";
  f = new DataForm();
  if( f->type() != DataForm::FormTypeInvalid )
  {
    fail = true;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "empty form tag";
  f = new DataForm();
  if( f->tag() )
  {
    fail = true;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "form title";
  std::string title = "form test title";
  f = new DataForm();
  f->setTitle( title );
  if( f->title() != title )
  {
    fail = true;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "form instructions";
  StringList instructions;
  instructions.push_back( "form test instructions" );
  instructions.push_back( "line 2" );
  instructions.push_back( "line 3" );
  f = new DataForm();
  f->setInstructions( instructions );
  if( f->instructions() != instructions )
  {
    fail = true;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;



  if( !fail )
  {
    printf( "All tests passed\n" );
    return 0;
  }
  else
  {
    return 1;
  }

}

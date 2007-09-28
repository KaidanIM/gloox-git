#include "../../dataformbase.h"
#include "../../dataformfield.h"
#include "../../dataform.h"
#include "../../tag.h"
using namespace gloox;
using namespace gloox::DataForm;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  DataForm::FormBase *f;

  std::string title = "form test title";
  StringList instructions;
  instructions.push_back( "form test instructions" );
  instructions.push_back( "line 2" );
  instructions.push_back( "line 3" );
  // -------
  name = "form type, title, instructions";
  // using StringList instructions from previous test case
  // using std::string title from pre-previous test case
  f = new DataForm::Form( instructions, title );
  if( f->instructions() != instructions )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  if( f->title() != title )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  if( f->type() != DataForm::TypeForm )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;




  if( fail == 0 )
  {
    printf( "DataForm: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "DataForm: %d test(s) failed\n", fail );
    return 1;
  }

}

#include "../../dataformreported.h"
#include "../../dataformfield.h"
#include "../../dataform.h"
#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  DataForm::Reported *f;
  Tag* t;

  // -------
  name = "empty form";
  f = new DataForm::Reported();
  t = f->tag();
  if( t->xml() != "<reported/>" )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  delete t;
  f = 0;
  t = 0;

  // -------
  name = "one child";
  f = new DataForm::Reported();
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  t = f->tag();
  if( !t || t->xml() != "<reported><field type='text-single' var='name' label='label'>"
       "<value>value</value></field></reported>" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), t->xml().c_str() );
  }
  delete f;
  delete t;
  f = 0;
  t = 0;

  // -------
  name = "many children";
  f = new DataForm::Reported();
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeJidSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextMulti, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeJidMulti, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeListSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeListMulti, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  t = f->tag();
  if( !t || t->xml() != "<reported>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "<field type='jid-single' var='name' label='label'><value>value</value></field>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "<field type='text-multi' var='name' label='label'><value>value</value></field>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "<field type='jid-multi' var='name' label='label'><value>value</value></field>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "<field type='list-single' var='name' label='label'><value>value</value></field>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "<field type='list-multi' var='name' label='label'><value>value</value></field>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "<field type='text-single' var='name' label='label'><value>value</value></field>"
       "</reported>" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), t->xml().c_str() );
  }
  delete f;
  delete t;
  f = 0;
  t = 0;








  if( fail == 0 )
  {
    printf( "DataForm::Reported: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "DataForm::Reported: %d test(s) failed\n", fail );
    return 1;
  }

}

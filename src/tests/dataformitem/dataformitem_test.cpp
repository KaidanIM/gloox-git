#include "../../dataformitem.h"
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
  DataForm::Item *f;
  Tag* t;

  // -------
  name = "empty form";
  f = new DataForm::Item();
  t = f->tag();
  if( t->xml() != "<item/>" )
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
  f = new DataForm::Item();
  f->addField( DataForm::Field::TypeTextSingle, "name", "value", "label" );
  t = f->tag();
  if( !t || t->xml() != "<item><field type='text-single' var='name' label='label'>"
       "<value>value</value></field></item>" )
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
  f = new DataForm::Item();
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
  if( !t || t->xml() != "<item>"
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
       "</item>" )
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
    printf( "DataForm::Item: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "DataForm::Item: %d test(s) failed\n", fail );
    return 1;
  }

}

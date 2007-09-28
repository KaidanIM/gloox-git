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
  DataFormItem *f;
  Tag* t;

  // -------
  name = "empty form";
  f = new DataFormItem();
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
  f = new DataFormItem();
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
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
  f = new DataFormItem();
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeJidSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextMulti, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeJidMulti, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeListSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeListMulti, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
  f->addField( DataFormField::FieldTypeTextSingle, "name", "value", "label" );
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
    printf( "DataFormItem: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "DataFormItem: %d test(s) failed\n", fail );
    return 1;
  }

}

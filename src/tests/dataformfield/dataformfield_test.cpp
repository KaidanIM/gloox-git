#include "../../dataformfield.h"
#include "../../tag.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char* /*argv[]*/ )
{
  int fail = 0;
  std::string name;
  DataFormField *f;

  // -------
  name = "empty field";
  f = new DataFormField();
  if( f->type() != DataFormField::FieldTypeTextSingle )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "FieldTypeBoolean field";
  f = new DataFormField( DataFormField::FieldTypeBoolean );
  if( f->type() != DataFormField::FieldTypeBoolean )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "parse 0";
  f = new DataFormField( 0 );
  if( f->type() != DataFormField::FieldTypeInvalid )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "parse empty Tag";
  Tag *t = new Tag();
  f = new DataFormField( t );
  delete t;
  if( f->type() != DataFormField::FieldTypeInvalid )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "set name";
  f = new DataFormField();
  f->setName( name );
  if( f->name() != name )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "set required";
  f = new DataFormField();
  bool req = true;
  f->setRequired( req );
  if( f->required() != req )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "set label";
  f = new DataFormField();
  f->setLabel( name );
  if( f->label() != name )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "set value";
  f = new DataFormField();
  f->setValue( name );
  if( f->value() != name )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "set values";
  f = new DataFormField();
  StringList val;
  val.push_back( "val 1" );
  val.push_back( "val 2" );
  val.push_back( "val 3" );
  f->setValues( val );
  if( f->values() != val )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "set values";
  f = new DataFormField();
  StringMap opt;
  opt["lock"] = "1";
  opt["stock"] = "1";
  opt["smoking barrel"] = "2";
  f->setOptions( opt );
  if( f->options() != opt )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;

  // -------
  name = "parse Tag 1";
  t = new Tag( "field");
  t->addAttribute( "type", "fixed" );
  new Tag( "value", "abc" );
  f = new DataFormField( t );
  delete t;
  if( f->tag() != t )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;
  f = 0;













  if( fail == 0 )
  {
    printf( "All tests passed\n" );
    return 0;
  }
  else
  {
    printf( "%d test(s) failed\n", fail );
    return 1;
  }

}

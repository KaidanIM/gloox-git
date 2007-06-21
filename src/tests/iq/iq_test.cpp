#include "../../tag.h"
#include "../../iq.h"
#include "../../stanza.h"
#include "../../jid.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  Tag *iq = new Tag( "iq" );
  iq->addAttribute( "from", "me@example.net/gloox" );
  iq->addAttribute( "to", "you@example.net/gloox" );
  iq->addAttribute( "id", "id1" );
  iq->addAttribute( "type", "set" );
  Tag *q = new Tag( iq, "query" );
  q->addAttribute( "xmlns", "mynamespace" );

  IQ *i = 0;

  // -------
  name = "parse IQ set";
  i = new IQ( iq );
  if( !i->hasAttribute( "type", "set" ) || !i->hasAttribute( "id", "id1" )
       || !i->hasAttribute( "to", "you@example.net/gloox" ) || !i->hasChild( "query", "xmlns", "mynamespace" )
       || !i->hasAttribute( "from", "me@example.net/gloox" )
       || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
       || i->id() != "id1" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }

  // -------
  name = "parse IQ get";
  iq->addAttribute( "type", "get" );
  i = new IQ( iq );
  if( !i->hasAttribute( "type", "get" ) || !i->hasAttribute( "id", "id1" )
       || !i->hasAttribute( "to", "you@example.net/gloox" ) || !i->hasChild( "query", "xmlns", "mynamespace" )
       || !i->hasAttribute( "from", "me@example.net/gloox" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }

  // -------
  name = "parse IQ error";
  iq->addAttribute( "type", "error" );
  i = new IQ( iq );
  if( !i->hasAttribute( "type", "error" ) || !i->hasAttribute( "id", "id1" )
       || !i->hasAttribute( "to", "you@example.net/gloox" ) || !i->hasChild( "query", "xmlns", "mynamespace" )
       || !i->hasAttribute( "from", "me@example.net/gloox" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }

  // -------
  name = "parse IQ result";
  iq->addAttribute( "type", "result" );
  i = new IQ( iq );
  if( !i->hasAttribute( "type", "result" ) || !i->hasAttribute( "id", "id1" )
       || !i->hasAttribute( "to", "you@example.net/gloox" ) || !i->hasChild( "query", "xmlns", "mynamespace" )
       || !i->hasAttribute( "from", "me@example.net/gloox" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }

  // -------
  name = "new simple IQ error";
  i = new IQ( IQ::IqTypeError, "id2", "xyz@example.org/blah" );
  if( !i->hasAttribute( "type", "error" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple IQ result";
  i = new IQ( IQ::IqTypeResult, "id2", "xyz@example.org/blah" );
  if( !i->hasAttribute( "type", "result" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple IQ get";
  i = new IQ( IQ::IqTypeGet, "id2", "xyz@example.org/blah" );
  if( !i->hasAttribute( "type", "get" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple IQ set 1";
  i = new IQ( IQ::IqTypeSet, "id2", "xyz@example.org/blah" );
  if( !i->hasAttribute( "type", "set" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple IQ set 2";
  i = new IQ( IQ::IqTypeSet, "id2", "xyz@example.org/blah", "mynamespace" );
  if( !i->hasAttribute( "type", "set" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasChild( "query", "xmlns", "mynamespace" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple IQ set 3";
  i = new IQ( IQ::IqTypeSet, "id2", "xyz@example.org/blah", "mynamespace", "testtag" );
  if( !i->hasAttribute( "type", "set" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasChild( "testtag", "xmlns", "mynamespace" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple IQ set 4";
  i = new IQ( IQ::IqTypeSet, "id2", "xyz@example.org/blah", "mynamespace", "testtag", 0,
              "blah@example.net/foo" );
  if( !i->hasAttribute( "type", "set" ) || !i->hasAttribute( "id", "id2" )
       || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasChild( "testtag", "xmlns", "mynamespace" )
       || !i->hasAttribute( "from", "blah@example.net/foo" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "rip off";
  i = new IQ( iq, true );
  if( !i->hasAttribute( "type", "result" ) || !i->hasAttribute( "id", "id1" )
       || !i->hasAttribute( "to", "you@example.net/gloox" ) || !i->hasChild( "query", "xmlns", "mynamespace" )
       || !i->hasAttribute( "from", "me@example.net/gloox" )
       || iq->children().size() != 0 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;







  delete iq;
  iq = 0;

  if( fail == 0 )
  {
    printf( "IQ: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "IQ: %d test(s) failed\n", fail );
    return 1;
  }

}

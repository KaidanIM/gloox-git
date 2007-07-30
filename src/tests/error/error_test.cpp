#include "../../error.h"
#include "../../tag.h"
#include "../../gloox.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>


int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;

  Tag * error = new Tag( "error" );
  error->addAttribute( "type", "auth" );
  new Tag( error, "feature-not-implemented", "xmlns", XMLNS_XMPP_STANZAS );
  Tag * appErr = new Tag( error, "unsupported", "xmlns", "errorNS" );
  appErr->addAttribute( "feature", "f" );

  std::string name = "";
  Error *e = new Error( error );
  if( e->type() != StanzaErrorTypeAuth || e->error() != StanzaErrorFeatureNotImplemented
    || e->appError()->xml() != "<unsupported xmlns='errorNS' feature='f'/>" )
  {
    printf( "failed: '%s' test\n", name.c_str() );
    printf( "type == %d, should be %d\n", e->type(), StanzaErrorTypeAuth );
    printf( "err == %d, should be %d\n", e->error(), StanzaErrorFeatureNotImplemented );
    printf( "xml: %s\n", e->appError()->xml().c_str() );

    ++fail;
  }
  // -------

  
  if( fail == 0 )
  {
    printf( "Error: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Error: %d test(s) failed\n", fail );
    return 1;
  }

}

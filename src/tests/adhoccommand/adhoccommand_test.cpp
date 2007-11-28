#include "../../tag.h"
#include "../../adhoc.h"
#include "../../iq.h"
#include "../../stanzaextensionfactory.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  Tag *t;



  StanzaExtensionFactory sef;

  // -------
  name = "Adhoc::Command/SEFactory test";
  sef.registerExtension( new Adhoc::Command( "foo", Adhoc::Command::Execute) );
  Tag* f = new Tag( "iq" );
  Tag* b = new Tag( f, "command", "xmlns", XMLNS_ADHOC_COMMANDS );
  new Tag( b, "url", "url" );
  new Tag( b, "desc", "desc" );
  IQ iq( IQ::Set, JID(), "" );
  sef.addExtensions( iq, f );
  const Adhoc::Command* se = static_cast<const Adhoc::Command*>( iq.findExtension( ExtAdhocCommand ) );
  if( se == 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;


  if( fail == 0 )
  {
    printf( "Adhoc::Command: OK\n" );
    return 0;
  }
  else
  {
    printf( "Adhoc::Command: %d test(s) failed\n", fail );
    return 1;
  }

}

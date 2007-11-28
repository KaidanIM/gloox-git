#include "../../tag.h"
#include "../../disco.h"
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
  name = "Disco::Items/SEFactory test";
  sef.registerExtension( new Disco::Items() );
  Tag* f = new Tag( "iq" );
  Tag* b = new Tag( f, "query", "xmlns", XMLNS_DISCO_ITEMS );
  new Tag( b, "url", "url" );
  new Tag( b, "desc", "desc" );
  IQ iq( IQ::Set, JID(), "" );
  sef.addExtensions( iq, f );
  const Disco::Items* se = static_cast<const Disco::Items*>( iq.findExtension( ExtDiscoItems ) );
  if( se == 0 )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete f;


  if( fail == 0 )
  {
    printf( "Disco::Items: OK\n" );
    return 0;
  }
  else
  {
    printf( "Disco::Items: %d test(s) failed\n", fail );
    return 1;
  }

}

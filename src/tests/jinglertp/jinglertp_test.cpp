#define JINGLE_TEST
#include "../../gloox.h"
#include "../../jid.h"
#include "../../tag.h"
#include "../../jinglertp.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>


int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;

  // -------
  {
    name = "empty RTP";
    Jingle::RTP::PayloadList l;
    Jingle::RTP a( l );
    Tag* t = a.tag();
    if( !t || t->xml() != "<description xmlns='" + XMLNS_JINGLE_RTP + "'/>" )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
    delete t;
  }

  // -------
  {
    name = "RTP w/ simple payload";
    Jingle::RTP::PayloadList l;
    StringMap attrs;
    attrs.insert( std::make_pair( "id", "96" ) );
    attrs.insert( std::make_pair( "name", "speex" ) );
    attrs.insert( std::make_pair( "clockrate", "16000" ) );
    l.push_back( new Jingle::RTP::Payload( attrs, StringMap() ) );
    Jingle::RTP a( l );
    Tag* t = a.tag();
    if( !t || t->xml() != "<description xmlns='" + XMLNS_JINGLE_RTP + "'>"
         "<payload-type clockrate='16000' id='96' name='speex'/>"
         "</description>" )
    {
      ++fail;
      printf( "test '%s' failed: %s\n", name.c_str(), t->xml().c_str() );
    }
    delete t;
  }



  printf( "Jingle::RTP: " );
  if( fail == 0 )
  {
    printf( "OK\n" );
    return 0;
  }
  else
  {
    printf( "%d test(s) failed\n", fail );
    return 1;
  }

}
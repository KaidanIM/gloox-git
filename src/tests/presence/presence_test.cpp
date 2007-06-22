#include "../../tag.h"
#include "../../presence.h"
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
  Tag *pres = new Tag( "presence" );
  pres->addAttribute( "from", "me@example.net/gloox" );
  pres->addAttribute( "to", "you@example.net/gloox" );
  new Tag( pres, "status", "the status" );
  new Tag( pres, "priority", "10" );

  Tag* s = new Tag( "show" );

  Presence *i = 0;

  // -------
  name = "parse Presence implicit available";
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceAvailable
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence available";
  pres->addAttribute( "type", "available" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceAvailable
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence unavailable";
  pres->addAttribute( "type", "unavailable" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceUnavailable
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence error";
  pres->addAttribute( "type", "error" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceError
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence probe";
  pres->addAttribute( "type", "probe" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceProbe
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence available";
  pres->addAttribute( "type", "available" );
  pres->addChild( s );
  s->setCData( "chat" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceChat
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence away";
  pres->addAttribute( "type", "available" );
  s->setCData( "away" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceAway
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence dnd";
  pres->addAttribute( "type", "available" );
  s->setCData( "dnd" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceDnd
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Presence xa";
  pres->addAttribute( "type", "available" );
  s->setCData( "xa" );
  i = new Presence( pres );
  if( i->subtype() != Presence::PresenceXa
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->status() != "the status" || i->priority() != 10 )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence available";
  i = new Presence( Presence::PresenceAvailable, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence unavailable";
  i = new Presence( Presence::PresenceUnavailable, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "unavailable" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence chat";
  i = new Presence( Presence::PresenceChat, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" )
      || !i->hasChildWithCData( "show", "chat" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence away";
  i = new Presence( Presence::PresenceAway, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" )
      || !i->hasChildWithCData( "show", "away" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence dnd";
  i = new Presence( Presence::PresenceDnd, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" )
      || !i->hasChildWithCData( "show", "dnd" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence xa";
  i = new Presence( Presence::PresenceXa, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" )
      || !i->hasChildWithCData( "show", "xa" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence probe";
  i = new Presence( Presence::PresenceProbe, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "probe" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Presence error";
  i = new Presence( Presence::PresenceError, JID( "xyz@example.org/blah" ), "the status",
                        10, "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "error" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "status", "the status" )
      || !i->hasChild( "status", "xml:lang", "the xmllang" )
      || !i->hasChildWithCData( "priority", "10" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;



  delete pres;
  pres = 0;

  if( fail == 0 )
  {
    printf( "Presence: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Presence: %d test(s) failed\n", fail );
    return 1;
  }

}

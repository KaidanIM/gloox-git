#include "../../tag.h"
#include "../../message.h"
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
  Tag *msg = new Tag( "message" );
  msg->addAttribute( "from", "me@example.net/gloox" );
  msg->addAttribute( "to", "you@example.net/gloox" );
  msg->addAttribute( "id", "id1" );
  new Tag( msg, "body", "the body" );

  Message *i = 0;

  // -------
  name = "parse Message normal 1";
  i = new Message( msg );
  if( i->subtype() != Message::MessageNormal || !i->hasAttribute( "id", "id1" )
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->id() != "id1" || i->body() != "the body" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Message normal 2";
  msg->addAttribute( "type", "normal" );
  i = new Message( msg );
  if( i->subtype() != Message::MessageNormal || !i->hasAttribute( "type", "normal" )
      || !i->hasAttribute( "id", "id1" )
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->id() != "id1" || i->body() != "the body" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Message chat";
  msg->addAttribute( "type", "chat" );
  i = new Message( msg );
  if( i->subtype() != Message::MessageChat || !i->hasAttribute( "type", "chat" )
      || !i->hasAttribute( "id", "id1" )
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->id() != "id1" || i->body() != "the body" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Message groupchat";
  msg->addAttribute( "type", "groupchat" );
  i = new Message( msg );
  if( i->subtype() != Message::MessageGroupchat || !i->hasAttribute( "type", "groupchat" )
      || !i->hasAttribute( "id", "id1" )
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->id() != "id1" || i->body() != "the body" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "parse Message error";
  msg->addAttribute( "type", "error" );
  i = new Message( msg );
  if( i->subtype() != Message::MessageError || !i->hasAttribute( "type", "error" )
      || !i->hasAttribute( "id", "id1" )
      || !i->hasAttribute( "to", "you@example.net/gloox" )
      || !i->hasAttribute( "from", "me@example.net/gloox" )
      || i->from().full() != "me@example.net/gloox" || i->to().full() != "you@example.net/gloox"
      || i->id() != "id1" || i->body() != "the body" )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Message error";
  i = new Message( Message::MessageError, JID( "xyz@example.org/blah" ), "the body", "the subject",
                   "the thread", "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "error" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "body", "the body" ) || !i->hasChildWithCData( "thread", "the thread" )
      || !i->hasChildWithCData( "subject", "the subject" ) || !i->hasChild( "body", "xml:lang", "the xmllang" )
      || !i->hasChild( "subject", "xml:lang", "the xmllang" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Message chat";
  i = new Message( Message::MessageChat, JID( "xyz@example.org/blah" ), "the body", "the subject",
                   "the thread", "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "chat" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "body", "the body" ) || !i->hasChildWithCData( "thread", "the thread" )
      || !i->hasChildWithCData( "subject", "the subject" ) || !i->hasChild( "body", "xml:lang", "the xmllang" )
      || !i->hasChild( "subject", "xml:lang", "the xmllang" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Message normal";
  i = new Message( Message::MessageNormal, JID( "xyz@example.org/blah" ), "the body", "the subject",
                   "the thread", "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "normal" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "body", "the body" ) || !i->hasChildWithCData( "thread", "the thread" )
      || !i->hasChildWithCData( "subject", "the subject" ) || !i->hasChild( "body", "xml:lang", "the xmllang" )
      || !i->hasChild( "subject", "xml:lang", "the xmllang" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;

  // -------
  name = "new simple Message groupchat";
  i = new Message( Message::MessageGroupchat, JID( "xyz@example.org/blah" ), "the body", "the subject",
                   "the thread", "the xmllang", JID( "foo@bar.com" ) );
  if( !i->hasAttribute( "type", "groupchat" )
      || !i->hasAttribute( "to", "xyz@example.org/blah" ) || !i->hasAttribute( "from", "foo@bar.com" )
      || !i->hasChildWithCData( "body", "the body" ) || !i->hasChildWithCData( "thread", "the thread" )
      || !i->hasChildWithCData( "subject", "the subject" ) || !i->hasChild( "body", "xml:lang", "the xmllang" )
      || !i->hasChild( "subject", "xml:lang", "the xmllang" ) )
  {
    ++fail;
    printf( "test '%s' failed: %s\n", name.c_str(), i->xml().c_str() );
  }
  delete i;
  i = 0;




















  delete msg;
  msg = 0;

  if( fail == 0 )
  {
    printf( "Message: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Message: %d test(s) failed\n", fail );
    return 1;
  }

}

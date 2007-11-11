#include "../../sepubsubevent.h"
#include "../../tag.h"

static int failed = 0;

using namespace gloox;

int main()
{
  Tag* tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  Tag* t   = new Tag( tag, "items", "node", "princely_musings" );
  new Tag( t, "item", "id", "id" );

  SEPubSubEvent* pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "items test failed\n" );
  }
  delete tag;
  delete t;
  delete pse;
  pse = 0;
  t = 0;
  tag = 0;

  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t   = new Tag( tag, "items", "node", "princely_musings" );
  new Tag( t, "item", "id", "id" );
  t = new Tag( tag, "headers", XMLNS, "http://jabber.org/protocol/shim" );
  Tag* t3 = new Tag( t, "header", "name", "pubsub#collection" );
  t3->setCData( "collection" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    if( t )
      printf ( "%s\n", t->xml().c_str() );
      printf ( "%s\n", tag->xml().c_str() );
    printf( "items from collection test failed\n" );
  }
  delete tag;
  delete t;
  delete pse;
  pse = 0;
  t = 0;
  tag = 0;


  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t   = new Tag( tag, "items", "node", "princely_musings" );
  t   = new Tag( t, "item", "id", "id" );
  t   = new Tag( t, "entry", XMLNS, "http://www.w3.org" );
  new Tag( t, "title", "Soliloquy" );
  new Tag( t, "summary", "To be or not to be " );
  t3  = new Tag( t, "link", "rel", "alternate" );
  t3->addAttribute( "type", "text/html" );
  t3->addAttribute( "href", "http://denmark.lit/2003/12/13/atom03" );
  new Tag( t, "id", "tag:denmark.lit,2003:entry-32397" );
  new Tag( t, "published", "2003-12-13T18:30:02Z" );
  new Tag( t, "updated", "2003-12-13T18:30:02Z" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "items w/ payload test failed\n" );
  }
  delete tag;
  delete t;
  delete pse;
  pse = 0;
  t = 0;
  tag = 0;

  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t   = new Tag( tag, "items", "node", "princely_musings" );
  new Tag( t, "item", "id", "id" );
  t = new Tag( tag, "headers", XMLNS, "http://jabber.org/protocol/shim" );
  t3 = new Tag( t, "header", "name", "pubsub#subid" );
  t3->setCData( "123-abc" );
  t3 = new Tag( t, "header", "name", "pubsub#subid" );
  t3->setCData( "004-yyy" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "items w/ subscription id test failed\n" );
  }
  delete tag;
  delete t;
  delete pse;
  pse = 0;
  t = 0;
  tag = 0;


  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t   = new Tag( tag, "items", "node", "princely_musings" );
  new Tag( t, "retract", "id", "id" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "retract test failed\n" );
  }
  delete pse;
  delete t;
  pse = 0;
  t = 0;

  t = new Tag( tag, "headers", XMLNS, "http://jabber.org/protocol/shim" );
  t3 = new Tag( t, "header", "name", "pubsub#subid" );
  t3->setCData( "123-abc" );
  t3 = new Tag( t, "header", "name", "pubsub#subid" );
  t3->setCData( "004-yyy" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "retract w/ subscription id test failed\n" );
  }
  delete pse;
  delete t;
  delete tag;
  pse = 0;
  t = 0;
  tag = 0;

  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t = new Tag( tag, "configuration", "node", "princely_musings" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    if( t )
      printf( "t1: %s\n", t->xml().c_str() );
    printf( "t2: %s\n", tag->xml().c_str() );
    printf( "retract w/ subscription id test failed\n" );
  }
  delete pse;
  pse = 0;


  
  t = new Tag( t, "x", XMLNS, "jabber:x:data" );
  t->addAttribute( "type", "result" );
  t3 = new Tag( t, "field", "var", "FORM_TYPE" );
  t3->addAttribute( "type", "hidden" );
  new Tag( t3, "value", "http://jabber.org/protocol/pubsub#node_config" );
  t3 = new Tag( t, "field", "var", "pubsub#title" );
  new Tag( t3, "value", "Princely Musings (Atom)" );


  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "configuration w/ payload failed\n" );
  }
  delete t;
  delete pse;
  delete tag;
  pse = 0;
  t = 0;
  tag = 0;


  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t = new Tag( tag, "delete", "node", "princely_musings" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "delete node test failed\n" );
  }
  delete pse;
  delete tag;
  delete t;
  pse = 0;
  tag = 0;
  t = 0;


  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t = new Tag( tag, "purge", "node", "princely_musings" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "purge node id test failed\n" );
  }
  delete pse;
  delete tag;
  delete t;
  pse = 0;
  tag = 0;
  t = 0;


  tag = new Tag( "event", XMLNS, XMLNS_PUBSUB_EVENT );
  t = new Tag( tag, "collection" );
  t3 = new Tag( t, "node", "id", "princely_musings" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    if( t )
      printf( "t1: %s\n", t->xml().c_str() );
    printf( "t2: %s\n", tag->xml().c_str() );
    printf( "collection test failed\n" );
  }
  delete pse;
  pse = 0;


  
  t = new Tag( t3, "x", XMLNS, "jabber:x:data" );
  t->addAttribute( "type", "result" );
  t3 = new Tag( t, "field", "var", "FORM_TYPE" );
  t3->addAttribute( "type", "hidden" );
  new Tag( t3, "value", "http://jabber.org/protocol/pubsub#meta_data" );
  t3 = new Tag( t, "field", "var", "pubsub#description" );
  new Tag( t3, "value", "Atom feed for my blog" );

  pse = new SEPubSubEvent( tag );
  if( !pse || !(t = pse->tag()) || *t != *tag )
  {
    ++failed;
    printf( "collection w/ payload failed\n" );
  }
  delete t;
  delete pse;
  delete tag;
  pse = 0;
  t = 0;
  tag = 0;













  if( failed )
  {
    printf( "SEPubSubEvent: %d test(s) failed\n", failed );
    return 1;
  }
  else
  {
    printf( "SEPubSubEvent: OK\n" );
    return 0;
  }

}


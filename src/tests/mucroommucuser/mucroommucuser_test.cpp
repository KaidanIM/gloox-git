#include "../../tag.h"
#define MUCROOM_TEST
#include "../../mucroom.h"
#include "../../dataform.h"
#include "../../iq.h"
#include "../../message.h"
#include "../../stanzaextensionfactory.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  Tag* t = 0;

  // -------
  {
    name = "presence broadcast";
    Tag* x = new Tag( "x" );
    x->setXmlns( XMLNS_MUC_USER );
    Tag* i = new Tag( x, "item" );
    i->addAttribute( "jid", "foo@bar" );
    i->addAttribute( "role", "participant" );
    i->addAttribute( "affiliation", "member" );
    MUCRoom::MUCUser mu( x );
    t = mu.tag();
    if( !t || t->xml() != "<x xmlns='" + XMLNS_MUC_USER + "'>"
                          "<item jid='foo@bar' role='participant' affiliation='member'/>"
                          "</x>" )
    {
      ++fail;
      printf( "test '%s' failed:%s \n", name.c_str(), t->xml().c_str() );
    }
    delete t;
    delete x;
  }


  // -------
  {
    name = "presence broadcast incl status codes";
    Tag* x = new Tag( "x" );
    x->setXmlns( XMLNS_MUC_USER );
    Tag* i = new Tag( x, "item" );
    i->addAttribute( "jid", "foo@bar" );
    i->addAttribute( "role", "participant" );
    i->addAttribute( "affiliation", "member" );
    new Tag( x, "status", "code", "100" );
    new Tag( x, "status", "code", "110" );
    new Tag( x, "status", "code", "170" );
    new Tag( x, "status", "code", "201" );
    new Tag( x, "status", "code", "210" );
    new Tag( x, "status", "code", "301" );
    new Tag( x, "status", "code", "303" );
    new Tag( x, "status", "code", "307" );
    new Tag( x, "status", "code", "321" );
    new Tag( x, "status", "code", "322" );
    new Tag( x, "status", "code", "332" );
    MUCRoom::MUCUser mu( x );
    t = mu.tag();
    if( !t || t->xml() != "<x xmlns='" + XMLNS_MUC_USER + "'>"
         "<item jid='foo@bar' role='participant' affiliation='member'/>"
         "<status code='100'/>"
         "<status code='110'/>"
         "<status code='170'/>"
         "<status code='201'/>"
         "<status code='210'/>"
         "<status code='301'/>"
         "<status code='303'/>"
         "<status code='307'/>"
         "<status code='321'/>"
         "<status code='322'/>"
         "<status code='332'/>"
         "</x>" )
    {
      ++fail;
      printf( "test '%s' failed:%s \n", name.c_str(), t->xml().c_str() );
    }
    delete t;
    delete x;
  }


  // -------
  {
    name = "destroying a room";
    Tag* x = new Tag( "x" );
    x->setXmlns( XMLNS_MUC_USER );
    Tag* d = new Tag( x, "destroy" );
    d->addAttribute( "jid", "foo@bar" );
    new Tag( d, "reason", "fooreason" );
    MUCRoom::MUCUser mu( x );
    t = mu.tag();
    if( !t || t->xml() != "<x xmlns='" + XMLNS_MUC_USER + "'>"
         "<destroy jid='foo@bar'>"
         "<reason>fooreason</reason>"
         "</destroy>"
         "</x>" )
    {
      ++fail;
      printf( "test '%s' failed:%s \n", name.c_str(), t->xml().c_str() );
    }
    delete t;
    delete x;
  }

  // -------
  {
    name = "MUCRoom::MUCUser/SEFactory test (message)";
    StanzaExtensionFactory sef;
    sef.registerExtension( new MUCRoom::MUCUser() );
    Tag* f = new Tag( "message" );
    new Tag( f, "x", "xmlns", XMLNS_MUC_USER );
    Message msg( Message::Groupchat, JID(), "" );
    sef.addExtensions( msg, f );
    const MUCRoom::MUCUser* se = msg.findExtension<MUCRoom::MUCUser>( ExtMUCUser );
    if( se == 0 )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
    delete f;
  }

  // -------
  {
    name = "MUCRoom::MUCUser/SEFactory test (presence)";
    StanzaExtensionFactory sef;
    sef.registerExtension( new MUCRoom::MUCUser() );
    Tag* f = new Tag( "presence" );
    new Tag( f, "x", "xmlns", XMLNS_MUC_USER );
    Presence pres( Presence::Available, JID(), "" );
    sef.addExtensions( pres, f );
    const MUCRoom::MUCUser* se = pres.findExtension<MUCRoom::MUCUser>( ExtMUCUser );
    if( se == 0 )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
    delete f;
  }


  printf( "MUCRoom::MUCUser: " );
  if( !fail )
    printf( "OK\n" );
  else
    printf( "%d test(s) failed\n", fail );

  return fail;
}

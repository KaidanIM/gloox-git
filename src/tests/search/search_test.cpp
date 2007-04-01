#include "../../gloox.h"
#include "../../jid.h"
#include "../../dataform.h"
#include "../../stanza.h"
#include "../../tag.h"
#include "../../iqhandler.h"

#include <stdio.h>
#include <locale.h>
#include <string>

const std::string& g_dir = "test.dir";
const std::string& g_inst = "the instructions";

namespace gloox
{

  class ClientBase
  {
    public:
      ClientBase() {}
      virtual ~ClientBase() {}
      const std::string getID() { return "id"; }
      virtual void send( Tag *tag ) = 0;
      virtual void trackID( IqHandler *ih, const std::string& id, int context ) = 0;
  };

}

#define CLIENTBASE_H__
#include "../../search.h"
#include "../../search.cpp"
#include "../../searchhandler.h"

class SearchTest : public gloox::SearchHandler, public gloox::ClientBase
{
  public:
    SearchTest();
    ~SearchTest();
    virtual void handleSearchFields( const gloox::JID& directory, int fields,
                                     const std::string& instructions )
    {
      printf( "handleSearchFields being called\n ");
      switch( m_test )
      {
        case 2:
          if( directory.full() == g_dir && instructions == g_inst && fields == 15 )
            m_result = true;
          break;
        default:
          break;
      }
    };
    virtual void handleSearchFields( const gloox::JID& directory, gloox::DataForm *form ) {};
    virtual void handleSearchResult( const gloox::JID& directory, const gloox::SearchResultList& resultList ) {};
    virtual void handleSearchResult( const gloox::JID& directory, const gloox::DataForm *form ) {};
    virtual void handleSearchError( const gloox::JID& directory, gloox::Stanza *stanza ) {};
    virtual void send( gloox::Tag* tag )
    {
      switch( m_test )
      {
        case 1:
          if( tag && tag->hasAttribute( "id", "id" ) && tag->hasAttribute( "to", g_dir )
               && tag->hasAttribute( "type", "get" ) && tag->hasChild( "query", "xmlns", gloox::XMLNS_SEARCH ) )
            m_result = true;
          m_test = 0;
          break;
        default:
          break;
      }
    }
    void setTest( int test ) { m_test = test; }
    void fetchSearchFields() { m_search.fetchSearchFields( g_dir, this ); }
    bool result() { bool t = m_result; m_result = false; return t; }
    void feed( gloox::Stanza *s ) { m_search.handleIqID( s, m_context ); }
    virtual void trackID( gloox::IqHandler *ih, const std::string& id, int context ) { m_context = context; }
  private:
    gloox::Search m_search;
    int m_test;
    int m_context;
    bool m_result;
};

SearchTest::SearchTest() : m_search( this ), m_test( 0 ), m_context( -1 ), m_result( false ) {}
SearchTest::~SearchTest() {}

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;
  SearchTest t;

  // -------
  name = "fetch fields";
  t.setTest( 1 );
  t.fetchSearchFields();
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "receive fields";
  gloox::Stanza *iq = new gloox::Stanza( "iq" );
  iq->addAttribute( "from", g_dir );
  iq->addAttribute( "to", "searchtest" );
  iq->addAttribute( "id", "id" );
  iq->addAttribute( "type", "result" );
  gloox::Tag *q = new gloox::Tag( iq, "query" );
  q->addAttribute( "xmlns", gloox::XMLNS_SEARCH );
  new gloox::Tag( q, "instructions", g_inst );
  new gloox::Tag( q, "first" );
  new gloox::Tag( q, "last" );
  new gloox::Tag( q, "nick" );
  new gloox::Tag( q, "email" );
  iq->finalize();
  t.setTest( 2 );
  t.feed( iq );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete iq;
  iq = 0;

  // -------
  name = "old-style search";
  t.setTest( 3 );
  SearchFieldStruct sf;
  sf.first = "first";
  sf.last = "last";
  sf.nick = "nick";
  sf.email = "email";
  t.search();
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete iq;
  iq = 0;

















  if( fail == 0 )
  {
    printf( "Search: all tests passed\n" );
    return 0;
  }
  else
  {
    printf( "Search: %d test(s) failed\n", fail );
    return 1;
  }

}

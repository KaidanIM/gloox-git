#include "../../gloox.h"
#include "../../jid.h"
#include "../../dataform.h"
#include "../../stanza.h"
#include "../../tag.h"
#include "../../iqhandler.h"
#include "../../iq.h"

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
      if( m_test != 2 )
        return;

      if( directory.full() == g_dir && instructions == g_inst && fields == 15 )
        m_result = true;
    }
    virtual void handleSearchFields( const gloox::JID& directory, gloox::DataForm *form )
    {
      if( m_test != 6 )
        return;

      if( directory.full() == g_dir && form != 0 )
        m_result = true;

      delete form;
    }
    virtual void handleSearchResult( const gloox::JID& directory, const gloox::SearchResultList& resultList )
    {
      switch( m_test )
      {
        case 4:
        {
          gloox::SearchResultList::const_iterator it = resultList.begin();
          if( directory.full() == g_dir && resultList.size() == 2
              && (*it).first == "f1" && (*it).last == "l1" && (*it).nick == "n1" && (*it).email == "e1"
              && (*++it).first == "f2" && (*it).last == "l2" && (*it).nick == "n2" && (*it).email == "e2" )
            m_result = true;
          break;
        }
        case 5:
          if( directory.full() == g_dir && resultList.size() == 0 )
            m_result = true;
          break;
        default:
          break;
      }
    }
    virtual void handleSearchResult( const gloox::JID& directory, const gloox::DataForm *form )
    {
      if( m_test != 8 )
        return;

      if( directory.full() == g_dir && form != 0 )
        m_result = true;

      delete form;
    }
    virtual void handleSearchError( const gloox::JID& /*directory*/, gloox::Stanza* /*stanza*/ ) {}
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
        case 3:
        {
          gloox::Tag *t = 0;
          if( tag && tag->hasAttribute( "id", "id" ) && tag->hasAttribute( "to", g_dir )
               && tag->hasAttribute( "type", "set" )
               && ( ( t = tag->findChild( "query", "xmlns", gloox::XMLNS_SEARCH ) ) != 0 )
               && t->hasChildWithCData( "first", "first" ) && t->hasChildWithCData( "last", "last" )
               && t->hasChildWithCData( "nick", "nick" ) && t->hasChildWithCData( "email", "email" ) )
            m_result = true;
          break;
        }
        case 7:
        {
          gloox::Tag *t = 0;
          if( tag && tag->hasAttribute( "id", "id" ) && tag->hasAttribute( "to", g_dir )
               && tag->hasAttribute( "type", "set" )
               && ( ( t = tag->findChild( "query", "xmlns", gloox::XMLNS_SEARCH ) ) != 0 )
               && t->hasChild( "x", "xmlns", gloox::XMLNS_X_DATA ) )
            m_result = true;
          break;
        }
        default:
          break;
      }
      delete tag;
    }
    void setTest( int test ) { m_test = test; }
    void fetchSearchFields() { m_search.fetchSearchFields( g_dir, this ); }
    bool result() { bool t = m_result; m_result = false; return t; }
    void feed( gloox::IQ *s ) { m_search.handleIqID( s, m_context ); }
    virtual void trackID( gloox::IqHandler* /*ih*/, const std::string& /*id*/, int context )
      { m_context = context; }
    void search( const gloox::SearchFieldStruct& fields ) { m_search.search( g_dir, 15, fields, this ); }
    void search( const gloox::DataForm& form ) { m_search.search( g_dir, form, this ); }
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
  name = "fetch fields (old-style)";
  t.setTest( 1 );
  t.fetchSearchFields();
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "receive fields (old-style)";
  gloox::IQ *iq = new gloox::IQ( gloox::IQ::Result, gloox::JID( "searchtest" ), "id",
                                 gloox::XMLNS_SEARCH, "query", gloox::JID( g_dir ) );
  new gloox::Tag( iq->query(), "instructions", g_inst );
  new gloox::Tag( iq->query(), "first" );
  new gloox::Tag( iq->query(), "last" );
  new gloox::Tag( iq->query(), "nick" );
  new gloox::Tag( iq->query(), "email" );
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
  name = "search request (old-style)";
  t.setTest( 3 );
  gloox::SearchFieldStruct sf;
  sf.first = "first";
  sf.last = "last";
  sf.nick = "nick";
  sf.email = "email";
  t.search( sf );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "search result (old-style)";
  iq = new gloox::IQ( gloox::IQ::Result, gloox::JID( "searchtest" ), "id",
                      gloox::XMLNS_SEARCH, "query", gloox::JID( g_dir ) );
  gloox::Tag *i = new gloox::Tag( iq->query(), "item" );
  new gloox::Tag( i, "first", "f1" );
  new gloox::Tag( i, "last", "l1" );
  new gloox::Tag( i, "nick", "n1" );
  new gloox::Tag( i, "email", "e1" );
  i = new gloox::Tag( iq->query(), "item" );
  new gloox::Tag( i, "first", "f2" );
  new gloox::Tag( i, "last", "l2" );
  new gloox::Tag( i, "nick", "n2" );
  new gloox::Tag( i, "email", "e2" );
  t.setTest( 4 );
  t.feed( iq );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete iq;
  iq = 0;

  // -------
  name = "intermediary search request (old-style)";
  t.setTest( 3 );
  sf.first = "first";
  sf.last = "last";
  sf.nick = "nick";
  sf.email = "email";
  t.search( sf );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "search result (old-style), empty";
  iq = new gloox::IQ( gloox::IQ::Result, gloox::JID( "searchtest" ), "id",
                      gloox::XMLNS_SEARCH, "query", gloox::JID( g_dir ) );
  t.setTest( 5 );
  t.feed( iq );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete iq;
  iq = 0;

  // -------
  name = "fetch fields (dataform)";
  t.setTest( 1 );
  t.fetchSearchFields();
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "receive fields (dataform)";
  iq = new gloox::IQ( gloox::IQ::Result, gloox::JID( "searchtest" ), "id",
                      gloox::XMLNS_SEARCH, "query", gloox::JID( g_dir ) );
  gloox::DataForm df( gloox::DataForm::FormTypeForm );
  iq->query()->addChild( df.tag() );
  t.setTest( 6 );
  t.feed( iq );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }
  delete iq;
  iq = 0;

  // -------
  name = "search request (dataform)";
  t.setTest( 7 );
  t.search( df );
  if( !t.result() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  // -------
  name = "search result (dataform)";
  iq = new gloox::IQ( gloox::IQ::Result, gloox::JID( "searchtest" ), "id",
                      gloox::XMLNS_SEARCH, "query", gloox::JID( g_dir ) );
  gloox::DataForm df2( gloox::DataForm::FormTypeResult );
  iq->query()->addChild( df2.tag() );
  t.setTest( 8 );
  t.feed( iq );
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

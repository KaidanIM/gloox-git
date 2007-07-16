#include "../../parser.h"
#include "../../taghandler.h"
#include "../../util.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>

class ParserTest : private TagHandler
{
  public:
    ParserTest() : m_tag( 0 ), m_multiple( false ) {}
    virtual ~ParserTest() {}

    virtual void handleTag( Tag *tag )
    {
      if( m_multiple )
      {
        m_tags.push_back( tag->clone() );
      }
      else
        m_tag = tag->clone();
    }

    int run()
    {
      int fail = 0;
      std::string name;
      std::string data;
      bool tfail = false;
      Parser *p = new Parser( this );


      // -------
      name = "simple";
      data = "<tag/>";
      p->feed( data );
      if( m_tag == 0 || m_tag->name() != "tag" )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "simple child";
      data = "<tag1><child/></tag1>";
      p->feed( data );
      if( m_tag == 0 || m_tag->name() != "tag1" || !m_tag->hasChild( "child" ) )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "attribute";
      data = "<tag2 attr='val'><child/></tag2>";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag2" ||
            !m_tag->hasAttribute( "attr", "val" ) ||
            !m_tag->hasChild( "child" ) )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "attribute in child";
      data = "<tag3><child attr='val'/></tag3>";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag3" ||
            !m_tag->hasChild( "child" ) )
      {
        tfail = true;
      }
      else
      {
        Tag *c = m_tag->findChild( "child" );
        if( !c->hasAttribute( "attr", "val" ) )
        {
          tfail = true;
        }
      }
      if( tfail )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
        tfail = false;
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "cdata";
      data = "<tag4>cdata</tag4>";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag4" ||
            m_tag->cdata() != "cdata" )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "tag w/ whitespace 1";
      data = "< tag4 />";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag4" )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "tag w/ whitespace 2";
      data = "< tag4/ >";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag4" )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

        // -------
      name = "tag w/ whitespace 3";
      data = "< tag4 / >";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag4" )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "simple child + white\tspace";
      data = "<tag1 ><child\t/ >< /  \ttag1>";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag1" ||
            !m_tag->hasChild( "child" ) )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "stream start";
      data = "<stream:stream version='1.0' to='example.org' xmlns='jabber:client' id='abcdef'>";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "stream:stream" ||
            !m_tag->hasAttribute( "version", "1.0" ) ||
            !m_tag->hasAttribute( "id", "abcdef" ) ||
            !m_tag->hasAttribute( "to", "example.org" ) ||
            !m_tag->hasAttribute( "xmlns", "jabber:client" ) )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
    //   printf( "stanza: %s\n", m_tag->xml().c_str() );
      delete m_tag;
      m_tag = 0;


      // -------
      name = "prolog";
      data = "<?xml version='1.0'?>";
      p->feed( data );
      if( ( m_tag != 0 )/* ||
            m_tag->name() != "xml" ||
            !m_tag->hasAttribute( "version", "1.0" )*/ )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
    //   printf( "stanza: %s\n", m_tag->xml().c_str() );
      delete m_tag;
      m_tag = 0;


      // -------
      name = "deeply nested";
      data = "<tag1 attr11='val11' attr12='val12'><tag2 attr21='val21' attr22='val22'/><tag3 attr31='val31'><tag4>cdata1</tag4><tag4>cdata2</tag4></tag3></tag1>";
      p->feed( data );
      if( m_tag == 0 ||
            m_tag->name() != "tag1" ||
            !m_tag->hasAttribute( "attr11", "val11" ) ||
            !m_tag->hasAttribute( "attr12", "val12" ) ||
            !m_tag->hasChild( "tag2" ) ||
            !m_tag->hasChild( "tag3" ) )
      {
        printf( "fail1\n" );
        tfail = true;
      }
      else
      {
        Tag *c = m_tag->findChild( "tag2" );
        if( !c->hasAttribute( "attr21", "val21" ) ||
            !c->hasAttribute( "attr22", "val22" ) )
        {
          printf( "fail2\n" );
          tfail = true;
        }
        c = m_tag->findChild( "tag3" );
        if( !c->hasAttribute( "attr31", "val31" ) ||
            !c->hasChild( "tag4" ) ||
            !c->hasChild( "tag4" ) )
        {
          printf( "fail3\n" );
          tfail = true;
        }
      }
      if( tfail )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
        printf( "got: %s\n", m_tag->xml().c_str() );
        tfail = false;
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "mixed content 1";
      data = "<tag1>cdata1<tag2>cdata2</tag2>cdata3</tag1>";
      p->feed( data );
      if( m_tag == 0 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "mixed content 2";
      data = "<tag1>cdata1<tag2>cdata2</tag2>cdata3</tag1>";
      p->feed( data );
      if( m_tag == 0 || m_tag->xml() != data )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "mixed content 3";
      data = "<tag1>cdata1<tag2/>cdata2<tag3/>cdata3</tag1>";
      p->feed( data );
      if( m_tag == 0 || m_tag->xml() != data )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "apos inside quotes";
      data = "<tag1 name=\"foo'bar\">cdata3</tag1>";
      p->feed( data );
      if( m_tag == 0 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "apos inside apos";
      data = "<tag1 name='foo'bar'>cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "quote inside apos";
      data = "<tag1 name='foo\"bar'>cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "quote inside quotes";
      data = "<tag1 name=\"foo\"bar\">cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "< inside attrib value";
      data = "<tag1 name='foo<bar'>cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "> inside attrib value";
      data = "<tag1 name='foo>bar'>cdata3</tag1>";
      p->feed( data );
      if( m_tag == 0 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "> inside cdata";
      data = "<tag1 name='foobar'>cda>ta3</tag1>";
      p->feed( data );
      if( m_tag != 0 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "quote inside cdata";
      data = "<tag1 name='foobar'>cda\"ta3</tag1>";
      p->feed( data );
      if( m_tag != 0 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      //-------
      name = "relax";
      if ( Parser::relax( "&amp;&lt;&gt;&apos;&quot;&#60;&#62;&#39;&#34;""&#x3c;&#x3e;"
                      "&#x3C;&#x3E;&#x27;&#x22;&#X3c;&#x3e;&#X3C;&#X3E;&#X27;&#X22;" )
            != "&<>'\"<>'\"<><>'\"<><>'\"" )
      {
        ++fail;
        printf( "test '%s' failed\n", name.c_str() );
      }

      //-------
      name = "more than one tag at a time";
      m_multiple = true;
      data = "<tag1/><tag2/><tag3/>";
      int i = -1;
      if( ( i = p->feed( data ) ) >= 0 || m_tags.size() != 3 )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      m_multiple = false;
      util::clear( m_tags );

      delete p;
      p = 0;

      if( fail == 0 )
      {
        printf( "Parser: all tests passed\n" );
        return 0;
      }
      else
      {
        printf( "Parser: %d test(s) failed\n", fail );
        return 1;
      }

    }

  private:
    Tag *m_tag;
    Tag::TagList m_tags;
    bool m_multiple;

};

int main( int /*argc*/, char** /*argv*/ )
{
  ParserTest p;
  return p.run();
}

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
      name = "apos inside quotes in attrib value";
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
      name = "apos inside apos in attrib value";
      data = "<tag1 name='foo'bar'>cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "quote inside apos in attrib value";
      data = "<tag1 name='foo\"bar'>cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "quote inside quotes in attrib value";
      data = "<tag1 name=\"foo\"bar\">cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "lt inside attrib value";
      data = "<tag1 name='foo<bar'>cdata3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "gt inside attrib value";
      data = "<tag1 name='foo>bar'>cdata3</tag1>";
      p->feed( data );
      if( !m_tag )
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
      if( !m_tag )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "< inside cdata";
      data = "<tag1 name='foobar'>cda<ta3</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }

      // -------
      name = "quote inside cdata";
      data = "<tag1 name='foobar'>cda\"ta3</tag1>";
      p->feed( data );
      if( !m_tag )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      // -------
      name = "apos inside cdata";
      data = "<tag1 name='foobar'>cda'ta3</tag1>";
      p->feed( data );
      if( !m_tag )
      {
        ++fail;
        printf( "test '%s: %s' failed\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

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

      //-------
      name = "<![CDATA[ section 1";
      data = "<tag1><![CDATA[abcdefg]]></tag1>";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "abcdefg" )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      delete m_tag;
      m_tag = 0;


      //-------
      name = "<![CDATA[ section 2";
      data = "<tag1>123<![CDATA[abcdefg]]>456</tag1>";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "123abcdefg456" )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "<![CDATA[ section 3";
      data = "<tag1><![CDATA[abc&amp;&&lt;]]defg]]></tag1>";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "abc&amp;&&lt;]]defg" )
      {
        ++fail;
        printf( "test '%s' failed: %s\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "split <![CDATA[ section 1";
      data = "<tag1><![CDA";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || m_tag )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      data = "TA[abc&amp;&&lt;]]defg]]></tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "abc&amp;&&lt;]]defg" )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "split <![CDATA[ section 2";
      data = "<tag1><![C";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || m_tag )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      data = "DA";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || m_tag )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      data = "TA[abc&amp;&&lt;]]defg]]></tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "abc&amp;&&lt;]]defg" )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "split <![CDATA[ section 3";
      data = "<tag1><![CDA";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || m_tag )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      data = "TA[abc&amp;&&lt;]]defg]";
      i = -1;
      if( ( i = p->feed( data ) ) >= 0 || m_tag )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      data = "]></tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "abc&amp;&&lt;]]defg" )
      {
        ++fail;
        printf( "test '%s' failed at pos %d: %s\n", name.c_str(), i, data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid name 1";
      data = "<tag1><!></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid name 2";
      data = "<tag1><!abc/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s\n", name.c_str(), data.c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid name 3";
      data = "<tag1><ab!cd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid name 4";
      data = "<tag1><ab&amp;cd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid name 5";
      data = "<tag1><ab?cd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 6";
      data = "<tag1 a='b'a><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 7";
      data = "<tag1 a=a'b'><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 8";
      data = "<tag1 c=a><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 9";
      data = "<tag1 =a><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 10";
      data = "<tag1 =><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 11";
      data = "<tag1 a><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid tag 12";
      data = "<tag1 <><abcd/></tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 1";
      data = "<tag1>&ggggggt;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 2";
      data = "<tag1>&t;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 3";
      data = "<tag1>&tt;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 4";
      data = "<tag1>&lf;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 5";
      data = "<tag1>&gf;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 6";
      data = "<tag1>&add;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 7";
      data = "<tag1>&qitt;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 8";
      data = "<tag1>&apit;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 9";
      data = "<tag1>&#ABCD;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 10";
      data = "<tag1>&#xX123;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 11";
      data = "<tag1>&#x123X;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 12";
      data = "<tag1>&#1ABCD;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 13";
      data = "<tag1>&#2097152;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 14";
      data = "<tag1>&#x200000;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 15";
      data = "<tag1>&#X200000;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 16";
      data = "<tag1>&#0;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 17";
      data = "<tag1>&#X13;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 18";
      data = "<tag1>&#X110000;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 19";
      data = "<tag1>&#xFFFE;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 20";
      data = "<tag1>&#xFFFF;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 21";
      data = "<tag1>&#xD800;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "invalid escaping 22";
      data = "<tag1>&#xDFFF;</tag1>";
      if( p->feed( data ) == -1 )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 1";
      data = "<tag1>&lt;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "<" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 2";
      data = "<tag1>&gt;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != ">" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 3";
      data = "<tag1>&apos;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "'" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 4";
      data = "<tag1>&amp;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "&" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 5";
      data = "<tag1>&quot;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "\"" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 6";
      data = "<tag1>&#x1234;&gt;&lt;&apos;&amp;&quot;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "ሴ><'&\"" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 7";
      data = "<tag1>&#x1234;&#x43;&#34;&#43;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "ሴC\"+" )
      {
        ++fail;
        printf( "test '%s' failed: %s -- %s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;

      //-------
      name = "escaping 8";
      data = "<tag1>&amp;&lt;&gt;&apos;&quot;  &#60;&#62;&#39;&#34;  &#x3c;&#x3e;&#x3C;&#x3E;&#x27;&#x22; "
             " &#X3c;&#X3e;&#X3C;&#X3E;&#X27;&#X22;</tag1>";
      if( ( i = p->feed( data ) ) >= 0 || !m_tag || m_tag->cdata() != "&<>'\"  <>'\"  <><>'\"  <><>'\"" )
      {
        ++fail;
        printf( "test '%s' failed: \n%s\n%s\n", name.c_str(), data.c_str(), m_tag->xml().c_str() );
      }
      delete m_tag;
      m_tag = 0;



























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

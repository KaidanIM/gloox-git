/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "tag.h"

#include <stdlib.h>

#ifdef _WIN32_WCE
# include <cmath>
#else
# include <sstream>
#endif

#include <algorithm>
#include "util.h"

namespace gloox
{

  Tag::Tag()
    : m_parent( 0 ), m_children( new TagList() ), m_cdata( new StringPList() ),
      m_attribs( new AttributeList() ), m_nodes( new NodeList() ), m_type( StanzaUndefined )
  {
  }

  Tag::Tag( const std::string& name, const std::string& cdata )
    : m_parent( 0 ), m_children( new TagList() ), m_cdata( new StringPList() ),
      m_attribs( new AttributeList() ), m_nodes( new NodeList() ),
      m_type( StanzaUndefined ), m_name( name )
  {
    addCData( cdata );
  }

  Tag::Tag( Tag *parent, const std::string& name, const std::string& cdata )
    : m_parent( parent ), m_children( new TagList() ), m_cdata( new StringPList() ),
      m_attribs( new AttributeList() ), m_nodes( new NodeList() ),
      m_type( StanzaUndefined ), m_name( name )
  {
    if( m_parent )
      m_parent->addChild( this );
    addCData( cdata );
  }

  Tag::Tag( const std::string& name, const std::string& attrib, const std::string& value )
    : m_parent( 0 ), m_children( new TagList() ), m_cdata( new StringPList() ),
      m_attribs( new AttributeList() ), m_nodes( new NodeList() ),
      m_type( StanzaUndefined ), m_name( name )
  {
    addAttribute( attrib, value );
  }

  Tag::Tag( Tag *parent, const std::string& name, const std::string&  attrib, const std::string& value )
    : m_parent( parent ), m_children( new TagList() ), m_cdata( new StringPList() ),
      m_attribs( new AttributeList() ), m_nodes( new NodeList() ),
      m_type( StanzaUndefined ), m_name( name )
  {
    if( m_parent )
      m_parent->addChild( this );
    addAttribute( attrib, value );
  }

  Tag::~Tag()
  {
    util::clear( *m_cdata );
    util::clear( *m_attribs );
    util::clear( *m_children );
    util::clear( *m_nodes );
    delete m_cdata;
    delete m_attribs;
    delete m_children;
    delete m_nodes;
    m_parent = 0;
  }

  bool Tag::operator==( const Tag &right ) const
  {
    if( m_name != right.m_name ||
        m_xmlns != right.m_xmlns || m_children->size() != right.m_children->size() )
      return false;

    StringPList::const_iterator ct = m_cdata->begin();
    StringPList::const_iterator ct_r = right.m_cdata->begin();
    while( ct != m_cdata->end() && ct_r != right.m_cdata->end() && *(*ct) == *(*ct_r) )
    {
      ++ct;
      ++ct_r;
    }
    if( ct != m_cdata->end() )
      return false;

    TagList::const_iterator it = m_children->begin();
    TagList::const_iterator it_r = right.m_children->begin();
    while( it != m_children->end() && it_r != right.m_children->end() && *(*it) == *(*it_r) )
    {
      ++it;
      ++it_r;
    }
    if( it != m_children->end() )
      return false;

    AttributeList::const_iterator at = m_attribs->begin();
    AttributeList::const_iterator at_r = right.m_attribs->begin();
    while( at != m_attribs->end() && at_r != right.m_attribs->end() && *(*at) == *(*at_r) )
    {
      ++at;
      ++at_r;
    }
    return at == m_attribs->end();
  }

  const std::string Tag::xml() const
  {
    std::string xml = "<";
    xml += escape( m_name );
    if( !m_attribs->empty() )
    {
      AttributeList::const_iterator it_a = m_attribs->begin();
      for( ; it_a != m_attribs->end(); ++it_a )
      {
        xml += " ";
        xml += escape( (*it_a)->name() );
        xml += "='";
        xml += escape( (*it_a)->value() );
        xml += "'";
      }
    }

    if( m_nodes->empty() )
      xml += "/>";
    else
    {
      xml += ">";
      NodeList::const_iterator it_n = m_nodes->begin();
      for( ; it_n != m_nodes->end(); ++it_n )
      {
        switch( (*it_n)->type )
        {
          case TypeTag:
            xml += (*it_n)->tag->xml();
            break;
          case TypeString:
            xml += escape( *((*it_n)->str) );
            break;
        }
      }
      xml += "</";
      xml += escape( m_name );
      xml += ">";
    }

    return xml;
  }

  static const char escape_chars[] = { '&', '<', '>', '\'', '"', '<', '>',
  '\'', '"', '<', '>', '<', '>', '\'', '"', '<', '>', '<', '>', '\'', '"' };

  static const std::string escape_seqs[] = { "amp;", "lt;", "gt;", "apos;",
  "quot;", "#60;", "#62;", "#39;", "#34;", "#x3c;", "#x3e;", "#x3C;",
  "#x3E;", "#x27;", "#x22;", "#X3c;", "#X3e;", "#X3C;", "#X3E;", "#X27;",
  "#X22;" };

  static const unsigned nb_escape = sizeof( escape_chars )/sizeof( char );
  static const unsigned escape_size = 5;

  const std::string Tag::escape( std::string esc )
  {
    for( unsigned val, i = 0; i < esc.length(); ++i )
    {
      for( val = 0; val < escape_size; ++val )
      {
        if( esc[i] == escape_chars[val] )
        {
          esc[i] = '&';
          esc.insert( i+1, escape_seqs[val] );
          i += escape_seqs[val].length();
          break;
        }
      }
    }
    return esc;
  }

  void Tag::addAttribute( const std::string& name, const std::string& value )
  {
    if( name.empty() || value.empty() )
      return;

    AttributeList::iterator it = m_attribs->begin();
    for( ; it != m_attribs->end(); ++it )
    {
      if( (*it)->name() == name )
      {
        (*it)->setValue( value );
        return;
      }
    }

    m_attribs->push_back( new Attribute( name, value ) );
  }

  void Tag::addAttribute( const std::string& name, int value )
  {
    if( !name.empty() )
    {
#ifdef _WIN32_WCE
      const int len = 4 + (int)std::log10( value ? value : 1 ) + 1;
      char *tmp = new char[len];
      sprintf( tmp, "%d", value );
      std::string ret( tmp, len );
      addAttribute( name, ret );
      delete[] tmp;
#else
      std::ostringstream oss;
      oss << value;
      addAttribute( name, oss.str() );
#endif
    }
  }

  void Tag::addAttribute( const std::string& name, long value )
  {
    if( !name.empty() )
    {
#ifdef _WIN32_WCE
      const int len = 4 + (int)std::log10( value ? value : 1 ) + 1;
      char *tmp = new char[len];
      sprintf( tmp, "%ld", value );
      std::string ret( tmp, len );
      addAttribute( name, ret );
      delete[] tmp;
#else
      std::ostringstream oss;
      oss << value;
      addAttribute( name, oss.str() );
#endif
    }
  }

  void Tag::setAttributes( const AttributeList& attributes )
  {
    util::clear( *m_attribs );
    *m_attribs = attributes;
  }

  void Tag::addChild( Tag *child )
  {
    if( child )
    {
      m_children->push_back( child );
      child->m_parent = this;
      m_nodes->push_back( new Node( TypeTag, child ) );
    }
  }

  void Tag::addChildCopy( const Tag *child )
  {
    if( child )
    {
      Tag *t = child->clone();
      m_children->push_back( t );
      t->m_parent = this;
      m_nodes->push_back( new Node( TypeTag, t ) );
    }
  }

  void Tag::setCData( const std::string& cdata )
  {
    if( cdata.empty() )
      return;

    util::clear( *m_cdata );
    m_cdata->clear();
    NodeList::iterator it = m_nodes->begin();
    NodeList::iterator t = m_nodes->begin();
    while( it != m_nodes->end() )
    {
      if( (*it)->type == TypeString )
      {
        t = it;
        ++it;
        delete (*t);
        m_nodes->remove( (*t) );
      }
    }
    addCData( cdata );
  }

  void Tag::addCData( const std::string& cdata )
  {
    if( cdata.empty() )
      return;

    std::string* str = new std::string( cdata );
    m_cdata->push_back( str );
    m_nodes->push_back( new Node( TypeString, str ) );
  }

  const std::string Tag::cdata() const
  {
    std::string str;
    StringPList::const_iterator it = m_cdata->begin();
    for( ; it != m_cdata->end(); ++it )
      str += *(*it);

    return str;
  }

  const std::string Tag::findAttribute( const std::string& name ) const
  {
    AttributeList::const_iterator it = m_attribs->begin();
    for( ; it != m_attribs->end(); ++it )
      if( (*it)->name() == name )
        return (*it)->value();

    return std::string();
  }

  bool Tag::hasAttribute( const std::string& name, const std::string& value ) const
  {
    if( name.empty() )
      return true;

    AttributeList::const_iterator it = m_attribs->begin();
    for( ; it != m_attribs->end(); ++it )
      if( (*it)->name() == name && ( value.empty() || (*it)->value() == value ) )
        return true;

    return false;
  }

  Tag* Tag::findChild( const std::string& name ) const
  {
    TagList::const_iterator it = m_children->begin();
    while( it != m_children->end() && (*it)->name() != name )
      ++it;
    return it != m_children->end() ? (*it) : 0;
  }

  Tag* Tag::findChild( const std::string& name, const std::string& attr,
                       const std::string& value ) const
  {
    if( name.empty() )
      return 0;

    TagList::const_iterator it = m_children->begin();
    while( it != m_children->end() && ( (*it)->name() != name || !(*it)->hasAttribute( attr, value ) ) )
      ++it;
    return it != m_children->end() ? (*it) : 0;
  }

  bool Tag::hasChildWithCData( const std::string& name, const std::string& cdata ) const
  {
    TagList::const_iterator it = m_children->begin();
    while( it != m_children->end() && ( (*it)->name() != name
            || ( !cdata.empty() && (*it)->cdata() != cdata ) ) )
      ++it;
    return it != m_children->end();
  }

  Tag* Tag::findChildWithAttrib( const std::string& attr, const std::string& value ) const
  {
    TagList::const_iterator it = m_children->begin();
    while( it != m_children->end() && !(*it)->hasAttribute( attr, value ) )
      ++it;
    return it != m_children->end() ? (*it) : 0;
  }

  Tag* Tag::clone() const
  {
    Tag *t = new Tag( 0, name() );
    t->m_type = m_type;
    t->m_xmlns = m_xmlns;

    Tag::AttributeList::const_iterator at = m_attribs->begin();
    for( ; at != m_attribs->end(); ++at )
    {
      t->m_attribs->push_back( new Attribute( *(*at) ) );
    }

    Tag::NodeList::const_iterator nt = m_nodes->begin();
    for( ; nt != m_nodes->end(); ++nt )
    {
      switch( (*nt)->type )
      {
        case TypeTag:
          t->addChild( (*nt)->tag->clone() );
          break;
        case TypeString:
          t->addCData( *((*nt)->str) );
          break;
      }
    }

    return t;
  }

  Tag::TagList Tag::findChildren( const std::string& name ) const
  {
    return findChildren( *m_children, name );
  }

  Tag::TagList Tag::findChildren( const Tag::TagList& list, const std::string& name ) const
  {
    Tag::TagList ret;
    Tag::TagList::const_iterator it = list.begin();
    for( ; it != list.end(); ++it )
    {
      if( (*it)->name() == name )
        ret.push_back( (*it) );
    }
    return ret;
  }

  void Tag::removeChild( Tag *tag )
  {
    m_children->remove( tag );

    NodeList::iterator it = m_nodes->begin();
    for( ; it != m_nodes->end(); ++it )
    {
      if( (*it)->type == TypeTag && (*it)->tag == tag )
      {
        delete (*it);
        m_nodes->remove( (*it) );
        return;
      }
    }
  }

  void Tag::ripoff( Tag *tag )
  {
    delete m_nodes;
    m_nodes = tag->m_nodes;
    tag->m_nodes = new NodeList();

    util::clear( *m_cdata );
    delete m_cdata;
    m_cdata = tag->m_cdata;
    tag->m_cdata = new StringPList();

    util::clear( *m_attribs );
    delete m_attribs;
    m_attribs = tag->m_attribs;
    tag->m_attribs = new AttributeList();

    util::clear( *m_children );
    delete m_children;
    m_children = tag->m_children;
    tag->m_children = new TagList();
  }

  Tag* Tag::findTag( const std::string& expression )
  {
    const Tag::TagList& l = findTagList( expression );
    return !l.empty() ? l.front() : 0;
  }

  Tag::TagList Tag::findTagList( const std::string& expression )
  {
    Tag::TagList l;
    if( expression == "/" || expression == "//" )
      return l;

    if( m_parent && expression.length() >= 2 && expression[0] == '/'
                                             && expression[1] != '/' )
      return m_parent->findTagList( expression );

    unsigned len = 0;
    Tag *p = parse( expression, len );
//     if( p )
//       printf( "parsed tree: %s\n", p->xml().c_str() );
    l = evaluateTagList( p );
    delete p;
    return l;
  }

  Tag::TagList Tag::evaluateTagList( Tag *token )
  {
    Tag::TagList result;
    if( !token )
      return result;

//     printf( "evaluateTagList called in Tag %s and Token %s (type: %s)\n", name().c_str(),
//             token->name().c_str(), token->findAttribute( TYPE ).c_str() );

    TokenType tokenType = (TokenType)atoi( token->findAttribute( TYPE ).c_str() );
    switch( tokenType )
    {
      case XTUnion:
        add( result, evaluateUnion( token ) );
        break;
      case XTElement:
      {
//         printf( "in XTElement, token: %s\n", token->name().c_str() );
        if( token->name() == name() || token->name() == "*" )
        {
//           printf( "found %s\n", name().c_str() );
          const Tag::TagList& tokenChildren = token->children();
          if( tokenChildren.size() )
          {
            bool predicatesSucceeded = true;
            Tag::TagList::const_iterator cit = tokenChildren.begin();
            for( ; cit != tokenChildren.end(); ++cit )
            {
              if( (*cit)->hasAttribute( "predicate", "true" ) )
              {
                predicatesSucceeded = evaluatePredicate( (*cit) );
                if( !predicatesSucceeded )
                  return result;
              }
            }

            bool hasElementChildren = false;
            cit = tokenChildren.begin();
            for( ; cit != tokenChildren.end(); ++cit )
            {
              if( (*cit)->hasAttribute( "predicate", "true" ) ||
                  (*cit)->hasAttribute( "number", "true" ) )
                continue;

              hasElementChildren = true;

//               printf( "checking %d children of token %s\n", tokenChildren.size(), token->name().c_str() );
              if( !m_children->empty() )
              {
                Tag::TagList::const_iterator it = m_children->begin();
                for( ; it != m_children->end(); ++it )
                {
                  add( result, (*it)->evaluateTagList( (*cit) ) );
                }
              }
              else if( atoi( (*cit)->findAttribute( TYPE ).c_str() ) == XTDoubleDot && m_parent )
              {
                (*cit)->addAttribute( TYPE, XTDot );
                add( result, m_parent->evaluateTagList( (*cit) ) );
              }
            }

            if( !hasElementChildren )
              result.push_back( this );
          }
          else
          {
//             printf( "adding %s to result set\n", name().c_str() );
            result.push_back( this );
          }
        }
//         else
//           printf( "found %s != %s\n", token->name().c_str(), name().c_str() );

        break;
      }
      case XTDoubleSlash:
      {
//         printf( "in XTDoubleSlash\n" );
        Tag *t = token->clone();
//         printf( "original token: %s\ncloned token: %s\n", token->xml().c_str(), n->xml().c_str() );
        t->addAttribute( TYPE, XTElement );
        add( result, evaluateTagList( t ) );
        const Tag::TagList& res2 = allDescendants();
        Tag::TagList::const_iterator it = res2.begin();
        for( ; it != res2.end(); ++it )
        {
          add( result, (*it)->evaluateTagList( t ) );
        }
        delete t;
        break;
      }
      case XTDot:
      {
        const Tag::TagList& tokenChildren = token->children();
        if( !tokenChildren.empty() )
        {
          add( result, evaluateTagList( tokenChildren.front() ) );
        }
        else
          result.push_back( this );
        break;
      }
      case XTDoubleDot:
      {
//         printf( "in XTDoubleDot\n" );
        if( m_parent )
        {
          const Tag::TagList& tokenChildren = token->children();
          if( tokenChildren.size() )
          {
            Tag *testtoken = tokenChildren.front();
            if( testtoken->name() == "*" )
            {
              add( result, m_parent->evaluateTagList( testtoken ) );
            }
            else
            {
              Tag *t = token->clone();
              t->addAttribute( TYPE, XTElement );
              t->m_name = m_parent->m_name;
              add( result, m_parent->evaluateTagList( t ) );
              delete t;
            }
          }
          else
          {
            result.push_back( m_parent );
          }
        }
      }
      case XTInteger:
      {
        const Tag::TagList& l = token->children();
        if( !l.size() )
          break;

        const Tag::TagList& res = evaluateTagList( l.front() );

        int pos = atoi( token->name().c_str() );
//         printf( "checking index %d\n", pos );
        if( pos > 0 && pos <= (int)res.size() )
        {
          Tag::TagList::const_iterator it = res.begin();
          while ( --pos )
          {
            ++it;
          }
          result.push_back( *it );
        }
        break;
      }
      default:
        break;
    }
    return result;
  }

  bool Tag::evaluateBoolean( Tag *token )
  {
    if( !token )
      return false;

    bool result = false;
    TokenType tokenType = (TokenType)atoi( token->findAttribute( TYPE ).c_str() );
    switch( tokenType )
    {
      case XTAttribute:
        if( token->name() == "*" && m_attribs->size() )
          result = true;
        else
          result = hasAttribute( token->name() );
        break;
      case XTOperatorEq:
        result = evaluateEquals( token );
        break;
      case XTOperatorLt:
        break;
      case XTOperatorLtEq:
        break;
      case XTOperatorGtEq:
        break;
      case XTOperatorGt:
        break;
      case XTUnion:
      case XTElement:
      {
        Tag *t = new Tag( "." );
        t->addAttribute( TYPE, XTDot );
        t->addChild( token );
        result = !evaluateTagList( t ).empty();
        t->removeChild( token );
        delete t;
        break;
      }
      default:
        break;
    }

    return result;
  }

  bool Tag::evaluateEquals( Tag *token )
  {
    if( !token || token->children().size() != 2 )
      return false;

    bool result = false;
    Tag::TagList::const_iterator it = token->children().begin();
    Tag *ch1 = (*it);
    Tag *ch2 = (*++it);

    TokenType tt1 = (TokenType)atoi( ch1->findAttribute( TYPE ).c_str() );
    TokenType tt2 = (TokenType)atoi( ch2->findAttribute( TYPE ).c_str() );
    switch( tt1 )
    {
      case XTAttribute:
        switch( tt2 )
        {
          case XTInteger:
          case XTLiteral:
            result = ( findAttribute( ch1->name() ) == ch2->name() );
            break;
          case XTAttribute:
            result = ( hasAttribute( ch1->name() ) && hasAttribute( ch2->name() ) &&
                      findAttribute( ch1->name() ) == findAttribute( ch2->name() ) );
            break;
          default:
            break;
        }
        break;
      case XTInteger:
      case XTLiteral:
        switch( tt2 )
        {
          case XTAttribute:
            result = ( ch1->name() == findAttribute( ch2->name() ) );
            break;
          case XTLiteral:
          case XTInteger:
            result = ( ch1->name() == ch2->name() );
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }

    return result;
  }

  Tag::TagList Tag::allDescendants()
  {
    Tag::TagList result;
    Tag::TagList::const_iterator it = m_children->begin();
    for( ; it != m_children->end(); ++it )
    {
      result.push_back( (*it) );
      add( result, (*it)->allDescendants() );
    }
    return result;
  }

  Tag::TagList Tag::evaluateUnion( Tag *token )
  {
    Tag::TagList result;
    if( !token )
      return result;

    const Tag::TagList& l = token->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      add( result, evaluateTagList( (*it) ) );
    }
    return result;
  }

  void Tag::closePreviousToken( Tag** root, Tag** current, Tag::TokenType& type, std::string& tok )
  {
    if( !tok.empty() )
    {
      addToken( root, current, type, tok );
      type = XTElement;
      tok = "";
    }
  }

  Tag* Tag::parse( const std::string& expression, unsigned& len, Tag::TokenType border )
  {
    Tag *root = 0;
    Tag *current = root;
    std::string token;

//     XPathError error = XPNoError;
//     XPathState state = Init;
//     int expected = 0;
//     bool run = true;
//     bool ws = false;

    Tag::TokenType type  = XTElement;

    char c;
    for( ; len < expression.length(); ++len )
    {
      switch( c = expression[len] )
      {
        case '/':
          closePreviousToken( &root, &current, type, token );

          if( len < expression.length()-1 && expression[len+1] == '/' )
          {
//             addToken( &root, &current, XTDoubleSlash, "//" );
            type = XTDoubleSlash;
            ++len;
          }
//           else
//           {
//             if( !current )
//             addToken( &root, &current, XTSlash, "/" );
//           }
          break;
        case ']':
          closePreviousToken( &root, &current, type, token );
          ++len;
          return root;
        case '[':
        {
          closePreviousToken( &root, &current, type, token );
          Tag *t = parse( expression, ++len, XTRightBracket );
          if( !addPredicate( &root, &current, t ) )
            delete t;
          break;
        }
        case '(':
        {
          closePreviousToken( &root, &current, type, token );
          Tag *t = parse( expression, ++len, XTRightParenthesis );
          if( current )
          {
//             printf( "added %s to %s\n", t->xml().c_str(), current->xml().c_str() );
            t->addAttribute( "argument", "true" );
            current->addChild( t );
          }
          else
          {
            root = t;
//             printf( "made %s new root\n", t->xml().c_str() );
          }
          break;
        }
        case ')':
          closePreviousToken( &root, &current, type, token );
          ++len;
          return root;
        case '\'':
          type = XTLiteral;
          if( expression[len-1] == '\\' )
            token += c;
          break;
        case '@':
          type = XTAttribute;
          break;
        case '.':
          token += c;
          if( token.size() == 1 )
          {
            if( len < expression.length()-1 && expression[len+1] == '.' )
            {
              type = XTDoubleDot;
              ++len;
              token += c;
            }
            else
            {
              type = XTDot;
            }
          }
          break;
        case '*':
//           if( !root || ( current && ( current->tokenType() == XTSlash
//                                       || current->tokenType() == XTDoubleSlash ) ) )
//           {
//             addToken( &root, &current, type, "*" );
//             break;
//           }
          addToken( &root, &current, type, "*" );
          type = XTElement;
          break;
        case '+':
        case '>':
        case '<':
        case '=':
        case '|':
        {
          closePreviousToken( &root, &current, type, token );
          std::string s( 1, c );
          Tag::TokenType ttype = getType( s );
          if( ttype <= border )
            return root;
          Tag *t = parse( expression, ++len, ttype );
          addOperator( &root, &current, t, ttype, s );
          break;
        }
        default:
          token += c;
      }
    }

    if( !token.empty() )
      addToken( &root, &current, type, token );

//     if( error != XPNoError )
//       printf( "error: %d\n", error );
    return root;
  }

  void Tag::addToken( Tag **root, Tag **current, Tag::TokenType type,
                      const std::string& token )
  {
    Tag *t = new Tag( token );
    if( t->isNumber() && !t->children().size() )
      type = XTInteger;
    t->addAttribute( TYPE, type );

    if( *root )
    {
//       printf( "new current %s, type: %d\n", token.c_str(), type );
      (*current)->addChild( t );
      *current = t;
    }
    else
    {
//       printf( "new root %s, type: %d\n", token.c_str(), type );
      *current = *root = t;
    }
  }

  void Tag::addOperator( Tag **root, Tag **current, Tag *arg,
                           Tag::TokenType type, const std::string& token )
  {
    Tag *t = new Tag( token );
    t->addAttribute( TYPE, type );
//     printf( "new operator: %s (arg1: %s, arg2: %s)\n", t->name().c_str(), (*root)->xml().c_str(),
//                                                                           arg->xml().c_str() );
    t->addAttribute( "operator", "true" );
    t->addChild( *root );
    t->addChild( arg );
    *current = *root = t;
  }

  bool Tag::addPredicate( Tag **root, Tag **current, Tag *token )
  {
    if( !*root || !*current )
      return false;

    if( ( token->isNumber() && !token->children().size() ) || token->name() == "+" )
    {
//       printf( "found Index %s, full: %s\n", token->name().c_str(), token->xml().c_str() );
      if( !token->hasAttribute( "operator", "true" ) )
      {
        token->addAttribute( TYPE, XTInteger );
      }
      if( *root == *current )
      {
        *root = token;
//         printf( "made Index new root\n" );
      }
      else
      {
        (*root)->removeChild( *current );
        (*root)->addChild( token );
//         printf( "added Index somewhere between root and current\n" );
      }
      token->addChild( *current );
//       printf( "added Index %s, full: %s\n", token->name().c_str(), token->xml().c_str() );
    }
    else
    {
      token->addAttribute( "predicate", "true" );
      (*current)->addChild( token );
    }

    return true;
  }

  Tag::TokenType Tag::getType( const std::string& c )
  {
    if( c == "|" )
      return XTUnion;
    if( c == "<" )
      return XTOperatorLt;
    if( c == ">" )
      return XTOperatorGt;
    if( c == "*" )
      return XTOperatorMul;
    if( c == "+" )
      return XTOperatorPlus;
    if( c == "=" )
      return XTOperatorEq;

    return XTNone;
  }

  bool Tag::isWhitespace( const char c )
  {
    return ( c == 0x09 || c == 0x0a || c == 0x0d || c == 0x20 );
  }

  bool Tag::isNumber()
  {
    if( m_name.empty() )
      return false;

    std::string::size_type l = m_name.length();
    std::string::size_type i = 0;
    while( i < l && isdigit( m_name[i] ) )
      ++i;
    return i == l;
  }

  void Tag::add( Tag::TagList& one, const Tag::TagList& two )
  {
    Tag::TagList::const_iterator it = two.begin();
    for( ; it != two.end(); ++it )
      if( std::find( one.begin(), one.end(), (*it) ) == one.end() )
        one.push_back( (*it) );
  }

}

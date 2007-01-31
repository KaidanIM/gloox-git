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
# include <math.h>
#else
# include <sstream>
#endif

namespace gloox
{
  Tag::Tag()
    : m_parent( 0 ), m_type( StanzaUndefined ), m_incoming( false )
  {
  }

  Tag::Tag( const std::string& name, const std::string& cdata, bool incoming )
    : m_name( incoming ? relax( name ) : name ),
      m_cdata( incoming ? relax( cdata ) : cdata ),
      m_parent( 0 ), m_type( StanzaUndefined ), m_incoming( incoming )
  {
  }

  Tag::Tag( Tag *parent, const std::string& name, const std::string& cdata, bool incoming )
    : m_name( incoming ? relax( name ) : name ),
      m_cdata( incoming ? relax( cdata ) : cdata ),
      m_parent( parent ), m_type( StanzaUndefined ), m_incoming( incoming )
  {
    if( m_parent )
      m_parent->addChild( this );
  }

  Tag::~Tag()
  {
    TagList::iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      delete (*it);
      (*it) = 0;
    }
    m_children.clear();
    m_parent = 0;
  }

  bool Tag::operator==( const Tag &right ) const
  {
    if( m_name != right.m_name )
      return false;

    if( m_attribs != right.m_attribs )
      return false;

    if( m_children.empty() ^ right.m_children.empty() )
      return false;

    if( m_children.size() != right.m_children.size() )
      return false;

    TagList::const_iterator it_this = m_children.begin();
    TagList::const_iterator it_right = right.m_children.begin();
    for( ; it_this != m_children.end() && it_right != right.m_children.end(); ++it_this, ++it_right )
    {
      if( *(*it_this) != *(*it_right) )
        return false;
    }

    return true;
  }

  bool Tag::operator!=( const Tag &right ) const
  {
    if( *this == right )
      return false;

    return true;
  }

  void Tag::setCData( const std::string& cdata )
  {
    m_cdata = m_incoming ? relax( cdata ) : cdata;
  }

  void Tag::addCData( const std::string& cdata )
  {
    m_cdata += m_incoming ? relax( cdata ) : cdata;
  }

  const std::string Tag::xml() const
  {
    std::string xml;
    xml = "<" + escape( m_name );
    if( m_attribs.size() )
    {
      StringMap::const_iterator it_a = m_attribs.begin();
      for( ; it_a != m_attribs.end(); ++it_a )
      {
        xml += " " + escape( (*it_a).first ) + "='" + escape( (*it_a).second ) + "'";
      }
    }

    if( m_cdata.empty() && !m_children.size() )
      xml += "/>";
    else if( m_children.size() )
    {
      xml += ">";
      TagList::const_iterator it_c = m_children.begin();
      for( ; it_c != m_children.end(); ++it_c )
      {
        xml += (*it_c)->xml();
      }
      xml += "</" + escape( m_name ) + ">";
    }
    else if( !m_cdata.empty() )
      xml += ">" + escape( m_cdata ) + "</" + escape( m_name ) + ">";

    return xml;
  }

  void Tag::addAttribute( const std::string& name, const std::string& value )
  {
    if( !name.empty() && !value.empty() )
      m_attribs[m_incoming ? relax( name ) : name] = m_incoming ? relax( value ) : value;
  }

  void Tag::addAttribute( const std::string& name, int value )
  {
    if( !name.empty() )
    {
#ifdef _WIN32_WCE
	  int len = 4+(int)log10(value)+1;
	  char *tmp = new char[len];
      sprintf( tmp, "%d", value );
	  std::string ret;
	  ret.assign( tmp, len );
	  m_attribs[m_incoming ? relax( name ) : name] = ret;
#else
	  std::ostringstream oss;
      oss << value;
      m_attribs[m_incoming ? relax( name ) : name] = oss.str();
#endif
    }
  }

  void Tag::addChild( Tag *child )
  {
    if( child )
    {
      m_children.push_back( child );
      child->m_parent = this;
    }
  }

  void Tag::addChild( const Tag *child )
  {
    if( child )
    {
      Tag *t = child->clone();
      m_children.push_back( t );
      t->m_parent = this;
    }
  }

  const std::string Tag::cdata() const
  {
    return m_cdata;
  }

  StringMap& Tag::attributes()
  {
    return m_attribs;
  }

  Tag::TagList& Tag::children()
  {
    return m_children;
  }

  const std::string Tag::findAttribute( const std::string& name ) const
  {
    StringMap::const_iterator it = m_attribs.find( name );
    if( it != m_attribs.end() )
      return (*it).second;
    else
      return "";
  }

  bool Tag::hasAttribute( const std::string& name, const std::string& value ) const
  {
    if( name.empty() )
      return true;

    StringMap::const_iterator it = m_attribs.find( name );
    if( it != m_attribs.end() )
      return ( ( value.empty() )?( true ):( (*it).second == value ) );
    else
      return false;
  }

  Tag* Tag::findChild( const std::string& name )
  {
    TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( (*it)->name() == name )
        return (*it);
    }

    return 0;
  }

  Tag* Tag::findChild( const std::string& name, const std::string& attr,
                       const std::string& value )
  {
    TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( ( (*it)->name() == name ) && (*it)->hasAttribute( attr, value ) )
        return (*it);
    }

    return 0;
  }

  bool Tag::hasChild( const std::string& name,
                      const std::string& attr, const std::string& value ) const
  {
    if( name.empty() )
      return false;

    TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( ( (*it)->name() == name )
              && (*it)->hasAttribute( attr, value ) )
        return true;
    }

    return false;
  }

  bool Tag::hasChildWithCData( const std::string& name, const std::string& cdata ) const
  {
    TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( ( (*it)->name() == name ) && !cdata.empty() && ( (*it)->cdata() == cdata ) )
        return true;
      else if( ( (*it)->name() == name ) && cdata.empty() )
        return true;
    }

    return false;
  }

  bool Tag::hasChildWithAttrib( const std::string& attr, const std::string& value ) const
  {
    TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( (*it)->hasAttribute( attr, value ) )
        return true;
    }

    return false;
  }

  Tag* Tag::findChildWithAttrib( const std::string& attr, const std::string& value )
  {
    TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( (*it)->hasAttribute( attr, value ) )
        return (*it);
    }

    return 0;
  }

  const std::string Tag::replace( const std::string& what, const Duo& duo ) const
  {
    std::string esc = what;
    Duo::const_iterator it = duo.begin();
    for( ; it != duo.end(); ++it )
    {
      size_t lookHere = 0;
      size_t foundHere = 0;
      while( ( foundHere = esc.find( (*it).first, lookHere ) ) != std::string::npos )
      {
        esc.replace( foundHere, (*it).first.size(), (*it).second );
        lookHere = foundHere + (*it).second.size();
      }
    }
    return esc;
  }

  const std::string Tag::escape( const std::string& what ) const
  {
    Duo d;
    d.push_back( duo( "&", "&amp;" ) );
    d.push_back( duo( "<", "&lt;" ) );
    d.push_back( duo( ">", "&gt;" ) );
    d.push_back( duo( "'", "&apos;" ) );
    d.push_back( duo( "\"", "&quot;" ) );

    return replace( what, d );
  }

  const std::string Tag::relax( const std::string& what ) const
  {
    Duo d;
    d.push_back( duo( "&#60;", "<" ) );
    d.push_back( duo( "&#62;", ">" ) );
    d.push_back( duo( "&#39;", "'" ) );
    d.push_back( duo( "&#34;", "\"" ) );
    d.push_back( duo( "&#x3c;", "<" ) );
    d.push_back( duo( "&#x3e;", ">" ) );
    d.push_back( duo( "&#x3C;", "<" ) );
    d.push_back( duo( "&#x3E;", ">" ) );
    d.push_back( duo( "&#x27;", "'" ) );
    d.push_back( duo( "&#x22;", "\"" ) );
    d.push_back( duo( "&#X3c;", "<" ) );
    d.push_back( duo( "&#X3e;", ">" ) );
    d.push_back( duo( "&#X3C;", "<" ) );
    d.push_back( duo( "&#X3E;", ">" ) );
    d.push_back( duo( "&#X27;", "'" ) );
    d.push_back( duo( "&#X22;", "\"" ) );
    d.push_back( duo( "&lt;", "<" ) );
    d.push_back( duo( "&gt;", ">" ) );
    d.push_back( duo( "&apos;", "'" ) );
    d.push_back( duo( "&quot;", "\"" ) );
    d.push_back( duo( "&amp;", "&" ) );

    return replace( what, d );
  }

  Tag* Tag::clone() const
  {
    Tag *t = new Tag( name(), cdata() );
    t->m_attribs = m_attribs;

    Tag::TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      t->addChild( (*it)->clone() );
    }

    return t;
  }

  Tag::TagList Tag::findChildren( const std::string& name )
  {
    return findChildren( m_children, name );
  }

  Tag::TagList Tag::findChildren( Tag::TagList& list, const std::string& name )
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

  bool Tag::removeChild( Tag *tag )
  {
    TagList::iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      if( (*it) == tag )
      {
        m_children.erase( it );
        return true;
      }
    }

    return false;
  }

  Tag* Tag::findTag( const std::string& expression )
  {
    Tag::TagList l = findTagList( expression );
    if( l.size() )
      return (*(l.begin()));
    else
      return 0;
  }

  Tag::TagList Tag::findTagList( const std::string& expression )
  {
    Tag::TagList l;
    if( expression == "/" || expression == "//" )
      return l;

    if( m_parent && expression.length() >= 2 && expression.substr( 0, 1 ) == "/"
                                                  && expression.substr( 1, 1 ) != "/" )
      return m_parent->findTagList( expression );

    int len = 0;
    Tag *p = parse( expression, len );
    if( p )
      printf( "parsed tree: %s\n", p->xml().c_str() );
    l = evaluateTagList( p );
    delete p;
    return l;
  }

  Tag::TagList Tag::evaluateTagList( Tag *token )
  {
    Tag::TagList result;
    if( !token )
      return result;

    printf( "evaluateTagList called in Tag %s and Token %s (type: %s)\n", name().c_str(),
            token->name().c_str(), token->findAttribute( "type" ).c_str() );

    TokenType tokenType = (TokenType)atoi( token->findAttribute( "type" ).c_str() );
    switch( tokenType )
    {
      case XTUnion:
      {
        Tag::TagList res = evaluateUnion( token );
        add( result, res );
        break;
      }
      case XTElement:
      {
        printf( "in XTElement, token: %s\n", token->name().c_str() );
        if( token->name() == name() || token->name() == "*" )
        {
          printf( "found %s\n", name().c_str() );
          Tag::TagList& tokenChildren = token->children();
          if( tokenChildren.size() )
          {
            bool predicatesSucceeded = true;
            Tag::TagList::const_iterator cit = tokenChildren.begin();
            for( ; cit != tokenChildren.end(); ++cit )
            {
              if( !(*cit)->hasAttribute( "predicate", "true" ) )
                continue;

              predicatesSucceeded = evaluatePredicate( (*cit) );
              if( !predicatesSucceeded )
                break;
            }
            printf( "predicatesSucceeded is %d\n", predicatesSucceeded );
            if( !predicatesSucceeded )
              break;

            bool hasElementChildren = false;
            cit = tokenChildren.begin();
            for( ; cit != tokenChildren.end(); ++cit )
            {
              if( (*cit)->hasAttribute( "predicate", "true" ) ||
                  (*cit)->hasAttribute( "number", "true" ) )
                continue;

              hasElementChildren = true;

              Tag::TagList res;
              printf( "checking %d children of token %s\n", tokenChildren.size(), token->name().c_str() );
              if( m_children.size() )
              {
                Tag::TagList::const_iterator it = m_children.begin();
                for( ; it != m_children.end(); ++it )
                {
                  res = (*it)->evaluateTagList( (*cit) );
                  add( result, res );
                }
              }
              else if( atoi( (*cit)->findAttribute( "type" ).c_str() ) == XTDoubleDot && m_parent )
              {
                (*cit)->addAttribute( "type", XTDot );
                res = m_parent->evaluateTagList( (*cit) );
                add( result, res );
              }
            }

            if( !hasElementChildren )
              result.push_back( this );
          }
          else
          {
            printf( "adding %s to result set\n", name().c_str() );
            result.push_back( this );
          }
        }
        else
          printf( "found %s != %s\n", token->name().c_str(), name().c_str() );

        break;
      }
      case XTDoubleSlash:
      {
        printf( "in XTDoubleSlash\n" );
        Tag *n = token->clone();
        printf( "original token: %s\ncloned token: %s\n", token->xml().c_str(), n->xml().c_str() );
        n->addAttribute( "type", XTElement );
        Tag::TagList res = evaluateTagList( n );
        add( result, res );
        Tag::TagList res2 = allDescendants();
        Tag::TagList::const_iterator it = res2.begin();
        for( ; it != res2.end(); ++it )
        {
          res = (*it)->evaluateTagList( n );
          add( result, res );
        }
        delete n;
        break;
      }
      case XTDot:
      {
        Tag::TagList& tokenChildren = token->children();
        if( tokenChildren.size() )
        {
          Tag::TagList res = evaluateTagList( (*(tokenChildren.begin())) );
          add( result, res );
        }
        else
          result.push_back( this );
        break;
      }
      case XTDoubleDot:
      {
        printf( "in XTDoubleDot\n" );
        if( m_parent )
        {
          Tag::TagList& tokenChildren = token->children();
          if( tokenChildren.size() )
          {
            Tag *testtoken = (*(tokenChildren.begin()));
            if( testtoken->name() == "*" )
            {
              Tag::TagList res = m_parent->evaluateTagList( testtoken );
              add( result, res );
            }
            else
            {
              Tag *n = token->clone();
              n->addAttribute( "type", XTElement );
              n->m_name = m_parent->m_name;
              Tag::TagList res = m_parent->evaluateTagList( n );
              add( result, res );
              delete n;
            }
          }
          else
            result.push_back( m_parent );
        }
      }
      case XTInteger:
      {
        Tag::TagList l = token->children();
        if( !l.size() )
          break;

        Tag::TagList res = evaluateTagList( (*(l.begin())) );

        Tag *tag = 0;
        int pos = atoi( token->name().c_str() );
        printf( "checking index %d\n", pos );
        int i = 1;
        Tag::TagList::const_iterator it = res.begin();
        for( ; it != res.end(); ++it, ++i )
        {
          if( pos == i )
          {
            tag = (*it);
            break;
          }
        }
        if( tag )
          result.push_back( tag );
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
    TokenType tokenType = (TokenType)atoi( token->findAttribute( "type" ).c_str() );
    switch( tokenType )
    {
      case XTAttribute:
        if( token->name() == "*" )
          result = (bool)m_attribs.size();
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
        t->addAttribute( "type", XTDot );
        t->addChild( token );
        Tag::TagList l = evaluateTagList( t );
        if( l.size() )
          result = true;
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

    TokenType tt1 = (TokenType)atoi( ch1->findAttribute( "type" ).c_str() );
    TokenType tt2 = (TokenType)atoi( ch2->findAttribute( "type" ).c_str() );
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

  bool Tag::evaluatePredicate( Tag *token )
  {
    if( !token )
      return false;

    return evaluateBoolean( token );
  }

  Tag::TagList Tag::allDescendants()
  {
    Tag::TagList result;
    Tag::TagList res;
    Tag::TagList::const_iterator it = m_children.begin();
    for( ; it != m_children.end(); ++it )
    {
      result.push_back( (*it) );
      res = (*it)->allDescendants();
      add( result, res );
    }
    return result;
  }

  Tag::TagList Tag::evaluateUnion( Tag *token )
  {
    Tag::TagList result;
    if( !token )
      return result;

    Tag::TagList res;
    Tag::TagList l = token->children();
    Tag::TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      res = evaluateTagList( (*it) );
      add( result, res );
    }
    return result;
  }

  Tag* Tag::parse( const std::string& expression, int& len, Tag::TokenType border )
  {
    printf( "parse() called with '%s'\n", expression.c_str() );
    Tag *root = 0;
    Tag *current = root;
    std::string token;
    XPathError error = XPNoError;
//     XPathState state = Init;
//     int expected = 0;
//     bool run = true;
//     bool ws = false;
    Tag::TokenType type  = XTElement;

    int length = expression.length();
    int i = 1;
    bool last = false;
    bool prelast = false;
    std::string::const_iterator it = expression.begin();
    for( ; it != expression.end(); ++it, ++i, ++len )
    {
      if( i == length )
      {
        last = true;
        prelast = false;
      }
      if( i == length - 1 )
        prelast = true;

      printf( "current char: %c, current type: %d\n", (*it), type );

      switch( (*it) )
      {
        case '/':
          if( !token.empty() )
          {
            addToken( &root, &current, type, token );
            type = XTElement;
            token = "";
          }

          if( !last && (*(it + 1)) == '/' )
          {
//             addToken( &root, &current, XTDoubleSlash, "//" );
            type = XTDoubleSlash;
            ++it;
            ++i;
            ++len;
          }
//           else
//           {
//             if( !current )
//             addToken( &root, &current, XTSlash, "/" );
//           }

          break;
        case '[':
        {
          if( !token.empty() )
          {
            addToken( &root, &current, type, token );
            type = XTElement;
            token = "";
          }

          int sublen = 0;
          Tag *t = parse( expression.substr( i ), sublen, XTRightBracket );
          addPredicate( &root, &current, t );
          it += sublen;
          i += sublen;
          len += sublen;
          break;
        }
        case ']':
          if( !token.empty() )
          {
            addToken( &root, &current, type, token );
            type = XTElement;
            token = "";
          }
          ++len;
          return root;
          break;
        case '(':
        {
          if( !token.empty() )
          {
            addToken( &root, &current, type, token );
            type = XTElement;
            token = "";
          }

          int sublen = 0;
          Tag *t = parse( expression.substr( i ), sublen, XTRightParenthesis );
          if( current )
          {
            printf( "added %s to %s\n", t->xml().c_str(), current->xml().c_str() );
            t->addAttribute( "argument", "true" );
            current->addChild( t );
          }
          else
          {
            root = t;
            printf( "made %s new root\n", t->xml().c_str() );
          }
          i += sublen;
          it += sublen;
          len += sublen;
          break;
        }
        case ')':
          if( !token.empty() )
          {
            addToken( &root, &current, type, token );
            type = XTElement;
            token = "";
          }
          ++len;
          return root;
          break;
        case '\'':
          type = XTLiteral;
          if( (*(it - 1)) == '\\' )
            token += (*it);
          break;
        case '@':
          type = XTAttribute;
          break;
        case '.':
          if( !token.empty() )
          {
            token += (*it);
            break;
          }
          if( !last && (*(it + 1)) == '.' )
          {
            type = XTDoubleDot;
            token += (*it);
            ++i;
            ++it;
            ++len;
            token += (*it);
            if( prelast )
              addToken( &root, &current, type, token );
          }
          else
          {
            type = XTDot;
            token += (*it);
            if( last )
              addToken( &root, &current, type, token );
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
          if( !token.empty() )
          {
            addToken( &root, &current, type, token );
            type = XTElement;
            token = "";
          }
          std::string c( 1, (*it) );
          Tag::TokenType ttype = getType( c );
          if( ttype <= border )
            return root;

          int sublen = 0;
          Tag *t = parse( expression.substr( i ), sublen, ttype );
          addOperator( &root, &current, t, ttype, c );
          i += sublen;
          it += sublen;
          len += sublen;
          break;
        }
        default:
          token += (*it);
          if( last )
            addToken( &root, &current, type, token );
          break;
      }
    }

    if( error != XPNoError )
      printf( "error: %d\n", error );

    printf( "parse() finished\n" );
    return root;
  }

  void Tag::addToken( Tag **root, Tag **current, Tag::TokenType type,
                      const std::string& token )
  {
    Tag *t = new Tag( token );
    if( t->isNumber() && !t->children().size() )
      t->addAttribute( "type", XTInteger );
    else
      t->addAttribute( "type", type );

    if( *root )
    {
      printf( "new current %s, type: %d\n", token.c_str(), type );
      (*current)->addChild( t );
      *current = t;
    }
    else
    {
      printf( "new root %s, type: %d\n", token.c_str(), type );
      *root = t;
      *current = *root;
    }

//     while( t->parent() )
//     {
//       t->parent()->addAttribute( "type", type );
//       t = t->parent();
//     }

  }

  void Tag::addOperator( Tag **root, Tag **current, Tag *arg,
                           Tag::TokenType type, const std::string& token )
  {
    Tag *t = new Tag( token );
    t->addAttribute( "type", type );
//     printf( "new operator: %s (arg1: %s, arg2: %s)\n", t->name().c_str(), (*root)->xml().c_str(),
//                                                                           arg->xml().c_str() );
    t->addAttribute( "operator", "true" );
    t->addChild( *root );
    t->addChild( arg );
    *root = t;
    *current = t;
  }

  void Tag::addPredicate( Tag **root, Tag **current, Tag *token )
  {
    if( !*root || !*current )
      return;

    if( ( token->isNumber() && !token->children().size() ) || token->name() == "+" )
    {
      printf( "found Index %s, full: %s\n", token->name().c_str(), token->xml().c_str() );
      if( !token->hasAttribute( "operator", "true" ) )
      {
        token->addAttribute( "type", XTInteger );
      }
      if( *root == *current )
      {
        *root = token;
        printf( "made Index new root\n" );
      }
      else
      {
        (*root)->removeChild( *current );
        (*root)->addChild( token );
        printf( "added Index somewhere between root and current\n" );
      }
      token->addChild( *current );
      printf( "added Index %s, full: %s\n", token->name().c_str(), token->xml().c_str() );
    }
    else
    {
      token->addAttribute( "predicate", "true" );
      (*current)->addChild( token );
    }
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

  bool Tag::isWhitespace( const char& c )
  {
    if( c == 0x09 || c == 0x0a || c == 0x0d || c == 0x20 )
      return true;

    return false;
  }

  bool Tag::isNumber()
  {
    if( m_name.empty() )
      return false;

    std::string::size_type l = m_name.length();
    for( std::string::size_type i = 0; i < l; ++i )
    {
      if( m_name[i] < 0x30 || m_name[i] > 0x39 )
        return false;
    }

    return true;
  }

  void Tag::add( Tag::TagList& one, const Tag::TagList& two )
  {
    bool doit = true;
    Tag::TagList::const_iterator it = two.begin();
    for( ; it != two.end(); ++it )
    {
      Tag::TagList::const_iterator it2 = one.begin();
      for( ; it2 != one.end(); ++it2 )
      {
        if( (*it2) == (*it) )
        {
          doit = false;
          printf( "found %s in existing set\n", (*it2)->name().c_str() );
          break;
        }
        else
          doit = true;
      }
      if( doit )
      {
        printf( "seriously add()ing %s\n", (*it)->name().c_str() );
        one.push_back( (*it) );
      }
    }
  }

}

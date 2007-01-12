/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "xpath.h"

namespace gloox
{

  Tag* XPath::findTag( Tag *tag, const std::string& expression )
  {
//     printf( "findTag called with tag %s and exp %s\n", tag->name().c_str(), expression.c_str() );
    if( expression == "/" || expression == "//" )
      return 0;

    if( tag->parent() && expression.substr( 0, 1 ) == "/" )
      return findTag( tag->parent(), expression );

    int start = 0;
    if( expression[0] == '/' )
      start = 1;

    Tag::TagList l = findTagList( tag/*, tag->children()*/, expression.substr( start ), false );
    if( l.size() )
    {
//       printf( "found %d tag(s)\n", l.size() );
      Tag::TagList::iterator it = l.begin();
      return (*it);
    }
    else
      return 0;
  }

  Tag::TagList XPath::findTagList( Tag *tag, const std::string& expression )
  {
//     printf( "tag: %s\n", tag->xml().c_str() );
//     printf( "findTagList called with tag %s and exp %s\n", tag->name().c_str(), expression.c_str() );
    if( expression == "/" || expression == "//" )
    {
      Tag::TagList l;
      return l;
    }

    if( tag->parent() && expression.substr( 0, 1 ) == "/" )
      return findTagList( tag->parent(), expression );

    int start = 0;
    if( expression[0] == '/' )
      start = 1;

    return findTagList( tag, /*tag->children(), */expression.substr( start ), false );
  }

  Tag::TagList XPath::findTagList( Tag *tag/*, Tag::TagList& list*/, const std::string& expression,
                                   bool deepSearch )
  {
//     printf( "findTagList called with tag %s and exp %s: deepSearch: %d\n", tag->name().c_str(),
//                                                                   expression.c_str()/*, list.size()*/,
//                                                                   deepSearch );
    XPathState state = TokenName;
    std::string token;
    Tag::TagList result;
    bool matchAll = false;
    bool pathSearch = false;

    std::string::size_type i = 0;
    std::string::const_iterator it = expression.begin();
    for( ; it != expression.end(); ++it, ++i )
    {
      switch( state )
      {
        case TokenName:
          switch( (*it) )
          {
            case '/':
              if( token.empty() && !deepSearch )
              {
//                 printf( "deepSearch is now true\n" );
                deepSearch = true;
              }
              else if( tag->name() == token || matchAll )
              {
                if( deepSearch )
                  result.push_back( tag );

                Tag::TagList res = walkTagList( tag, expression.substr( i + 1 ), deepSearch, true );
                add( result, res );
                return result;
              }
              else
              {
                Tag::TagList l;
                return l;
              }
              break;
            case '*':
//               printf( "matchAll is now true\n" );
              matchAll = true;
              token += (*it);
              break;
            default:
              token += (*it);
//               printf( "token now: %s\n", token.c_str() );
              break;
          }
          break;
      }
    }

//     printf( "token is now %s\n", token.c_str() );
    if( token == tag->name() || matchAll )
    {
//       printf( "current token/tag is %s/%s, returning it\n", token.c_str(), tag->name().c_str() );
      result.push_back( tag );
    }

    if( deepSearch )
    {
//       printf( "last element in search string, and deepSearch is enabled\n" );
      Tag::TagList res = walkTagList( tag, expression, deepSearch, false );
      add( result, res );
    }

    return result;
  }

  Tag::TagList XPath::walkTagList( Tag *tag, const std::string& expression, bool deepSearch, bool leaveAlone )
  {
//     printf( "walking children of <%s> with exp '%s'\n", tag->name().c_str(), expression.c_str() );
    Tag::TagList result;
    Tag::TagList::const_iterator it = tag->children().begin();
    for( ; it != tag->children().end(); ++it )
    {
      int start = 0;
      if( !leaveAlone && expression[0] == '/' )
        start = 1;

      Tag::TagList res = findTagList( (*it), expression.substr( start ), deepSearch );
      add( result, res );
    }
    return result;
  }

  void XPath::add( Tag::TagList& one, const Tag::TagList& two )
  {
    Tag::TagList::const_iterator it = two.begin();
    for( ; it != two.end(); ++it )
    {
      one.push_back( (*it) );
    }
  }

//   int XPath::countFunction( Tag::TagList& list )
//   {
//     return list.size();
//   }

}

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
    Tag::TagList l = findTagList( tag, expression );
    if( l.size() )
      return (*(l.begin()));
    else
      return 0;
  }

  Tag::TagList XPath::findTagList( Tag *tag, const std::string& expression )
  {
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

    Tag::TagList l = findTagList( tag, expression.substr( start ), false );
    unique( l );
    return l;
  }

  Tag::TagList XPath::findTagList( Tag *tag, const std::string& expression, bool deepSearch )
  {
    std::string token;
    Tag::TagList result;
    bool matchAll = false;

    std::string::size_type i = 0;
    std::string::const_iterator it = expression.begin();
    for( ; it != expression.end(); ++it, ++i )
    {
      switch( (*it) )
      {
        case '/':
          if( token.empty() && !deepSearch )
          {
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
          else if( token == "." )
          {
            Tag::TagList res = findTagList( tag, expression.substr( i + 1 ), deepSearch );
            add( result, res );
            return result;
          }
          else if( deepSearch )
          {
            Tag::TagList res = walkTagList( tag, expression.substr( 1 ), deepSearch, true );
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
          matchAll = true;
          token += (*it);
          break;
        case '.':
          token += (*it);
          break;
        default:
          token += (*it);
          break;
      }
    }

    if( token == tag->name() || matchAll || token == "." )
    {
      result.push_back( tag );
    }


    if( deepSearch )
    {
      Tag::TagList res = walkTagList( tag, expression, deepSearch, false );
      add( result, res );
    }

    return result;
  }

  Tag::TagList XPath::walkTagList( Tag *tag, const std::string& expression, bool deepSearch, bool leaveAlone )
  {
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

  void XPath::unique( Tag::TagList& one )
  {
    Tag::TagList::iterator t;
    Tag::TagList::iterator t2;
    Tag::TagList::iterator it = one.begin();
    for( ; it != one.end(); ++it )
    {
      t = it;
      ++t;
      for( ; t != one.end(); ++t )
      {
        if( (*t) == (*it) )
        {
          one.erase( t );
          t = it;
        }
      }
    }
  }

}

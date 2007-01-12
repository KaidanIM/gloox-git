/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef XPATH_H__
#define XPATH_H__

#include "tag.h"

#include <string>

namespace gloox
{

  /**
   *
   */
  class XPath
  {
    public:
      /**
       * Evaluates the given XPath expression and returns the the result Tag. If more than one
       * Tag match, only the first is returned.
       * @param expression An XPath expression to evaluate.
       * @return The matched Tag, or 0.
       */
      static Tag* findTag( Tag *tag, const std::string& expression );

      /**
       * Evaluates the given XPath expression and returns the the matched Tags.
       * @param expression An XPath expression to evaluate.
       * @return The list of matched Tags, or an empty TagList.
       */
      static Tag::TagList findTagList( Tag *tag, const std::string& expression );


    private:
      enum XPathState
      {
        AtSign,
        TokenName,
        FunctionArg,
        Wildcard
      };

//       enum TokenType
//       {
//         Element,
//         CountFunction,
//         Slash,
//         AtSign,
//         Bracket
//       };

      static Tag::TagList findTagList( Tag *tag/*, Tag::TagList& list*/, const std::string& expression,
                                       bool deepSearch );
      static Tag::TagList walkTagList( Tag *tag, const std::string& expression,
                                       bool deepSearch, bool leaveAlone );
      static void add( Tag::TagList& one, const Tag::TagList& two );

//       static int countFunction( Tag::TagList& list );

//       typedef void (*XPathFunction)( void* );

//       class Token
//       {
//         public:
//           Token( Token *parent/*, XPathFunction func*/, TokenType type, const std::string& value )
//             : m_parent( parent ), m_child( 0 )/*, m_func( func )*/, m_type( type ), m_value( value ) {};
//           virtual ~Token() { delete m_child; };
//
//           Token *parent() const { return m_parent; };
//           Token *child() const { return m_child; };
//
//         private:
//           Token *m_parent;
//           Token *m_child;
// //           XPathFunction *m_func;
//           TokenType m_type;
//           std::string m_value;
//       };

  };

}

#endif // XPATH_H__

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
   * @brief An XPath interpreter.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API XPath
  {
    public:
      /**
       * Evaluates the given XPath expression and returns the result Tag. If more than one
       * Tag match, only the first one is returned.
       * @param expression An XPath expression to evaluate.
       * @return The matched Tag, or 0.
       */
      static Tag* findTag( Tag *tag, const std::string& expression );

      /**
       * Evaluates the given XPath expression and returns the matched Tags.
       * @param expression An XPath expression to evaluate.
       * @return The list of matched Tags, or an empty TagList.
       */
      static Tag::TagList findTagList( Tag *tag, const std::string& expression );


    private:
      /**
       * Tries to match the given expression (or relevant parts thereof) against the given Tag.
       * If the Tag's name match the expression, or if deepSearch is @b true, it semi-recursively
       * traverses through its children by means of walkTagList() (which uses findTagList()) even
       * if its name doesn't match the expression.
       * @param tag The Tag to check.
       * @param expression The XPath expression to evaluate.
       * @param deepSearch Whether any elements in the tree shall match.
       * @return A list of matched Tags.
       */
      static Tag::TagList findTagList( Tag *tag, const std::string& expression,
                                       bool deepSearch );

      /**
       * A convenience function that traverses through all of @p tag's children, calling
       * findTagList() for each of them.
       * @param tag The Tag to traverse.
       * @param expression The XPath expression to evaluate.
       * @param deepSearch Whether any elements in the tree shall match.
       * @param leaveAlone Whether to strip a leading slash (/).
       * @return An accumulated list of matched Tags.
       */
      static Tag::TagList walkTagList( Tag *tag, const std::string& expression,
                                       bool deepSearch, bool leaveAlone );
      /**
       * Creates the union of two TagLists by appending the second list to the first.
       * @param one The list to append list @p two to.
       * @param two The list to append to list @p one.
       */
      static void add( Tag::TagList& one, const Tag::TagList& two );

      /**
       * Searches the given list for duplicates and removes such duplicates.
       * Uses simple, non-optimized linear searching.
       * @param one The list to clean.
       */
      static void unique( Tag::TagList& one );

  };

}

#endif // XPATH_H__

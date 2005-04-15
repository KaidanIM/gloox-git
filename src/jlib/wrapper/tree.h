/*
   iksemelmm -- c++ wrapper for iksemel xml/xmpp library

   Copyright (C) 2004 Igor Goryachieff <igor@jahber.org>
   Copyright (C) 2005 Jakob Schroeter <js@camaya.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#ifndef TREE_H__
#define TREE_H__

#include <iksemel.h>

#include <string>

using namespace std;

/**
 * the namespace is documented in parser.h
 */
namespace Iksemel
{
  /**
   * This class encapsulates a XML tree from Iksemel.
   */
  class Tree
  {
    public:
      /**
       * Constructor.
       * Creates an empty tree containing an empty element.
       */
      Tree();

      /**
       * Constructor.
       * Creates a new element within the given iksstack.
       * @param s The stack the root element will be bcreated in.
       */
      Tree( ikstack* s );

      /**
       * Constructor.
       * Creates an empty tree and an initial elemnt from the given node.
       * @param node The node to create the initial element from.
       */
      Tree( iks* node );

      /**
       * Constructor
       * Creates an empty tree with an element with the given name.
       * @param name The name of the element.
       */
      Tree( const std::string& name );

      /**
       * Destructor.
       */
      ~Tree();

      /**
       * Inserts a new element into the given iks.
       * @param node The existing node the new element will be placed in.
       * @param name The name of the new element.
       * @return A pointer to the newly created element.
       * FIXME: static?
       */
      iks* tag( iks* node, const std::string& name );

      /**
       * Creates a new element within this tree.
       * @param name The name of the new element.
       * @return A pointer to the newly created element.
       */
      iks* tag( const std::string& name );

      /**
       * Inserts CDATA into the given node.
       * @param node The node the CDATA will be inerted into.
       * @param data The CDATA to insert.
       * @return A pointer to the inserted CDATA.
       */
      iks* cdata( iks* node, const std::string& data );

      /**
       * Inserts CDATA into the tree's root element.
       * @param data The CDATA to insert.
       * @return A pointer to the inserted CDATA.
       */
      iks* cdata( const std::string& data );

      /**
       * Inserts a new node and adds CDATA to that node.
       * @param tag The name of the new node.
       * @param data The CDATA to insert into the new node.
       * @return A pointer to the new CDATA.
       */
      iks* cdata( const std::string& tag, const std::string& data );

      iks* attrib( iks*, const std::string&, const std::string& );
      iks* attrib( const std::string&, const std::string& );
      iks* attrib( const std::string&, const std::string&, const std::string& );

      iks* next() const;
      iks* move_next();

      iks* prev() const;
      iks* move_prev();

      iks* parent() const;
      iks* move_parent();

      iks* child() const;
      iks* move_child();

      iks* attr() const;
      iks* move_attr();

      iks* root() const;
      iks* move_root();

      iks* next_tag() const;
      iks* move_next_tag();

      iks* prev_tag() const;
      iks* move_prev_tag();

      iks* first_tag() const;
      iks* move_first_tag();

      const enum ikstype type() const;

      const std::string name() const;
      const std::string cdata_name() const;

      const size_t size_cdata() const;

      const bool has_children() const;
      const bool has_attribs() const;

      iks* find_node( const std::string& ) const;
      const std::string find_cdata( const std::string& ) const;
      const std::string find_attrib( const std::string& ) const;

      iks* find_with_attrib( iks*, const std::string&, const std::string&, const std::string& ) const;
      iks* find_with_attrib( const std::string&, const std::string&, const std::string& ) const;

      const bool boolean() const;

      iks* get_iks() const;
      ikstack* get_stack() const;

      std::string string( ikstack*, iks* ) const;
      std::string string( iks* ) const;
      std::string string() const;

    protected:
      iks* X;

    private:
      Tree( const Tree& );
      Tree& operator=( const Tree& );
  };
}

#endif // TREE_H__

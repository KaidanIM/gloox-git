/*
   iksemelmm -- c++ wrapper for iksemel xml/xmpp library

   Copyright (C) 2004 Igor Goryachieff <igor@jahber.org>

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
 * documented in parser.h
 */
namespace Iksemel
{
  /**
   * This class encapsulates a tree from Iksemel.
   */
  class Tree
  {
    public:
      Tree();
      Tree( ikstack* );
      Tree( iks* );
      Tree( const std::string& );
      ~Tree();

      iks* tag( iks*, const std::string& );
      iks* tag( const std::string& );

      iks* cdata( iks*, const std::string& );
      iks* cdata( const std::string& );
      iks* cdata( const std::string&, const std::string& );

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

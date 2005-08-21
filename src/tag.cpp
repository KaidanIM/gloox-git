/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/


#include "tag.h"

namespace gloox
{
  Tag::Tag()
  {
  }

  Tag::Tag( const std::string& name, const std::string& cdata )
    : m_name( name ), m_cdata( cdata )
  {
  }

  Tag::~Tag()
  {
  }

  const std::string Tag::xml() const
  {
    std::string xml;
    xml = "<" + m_name;
    if( m_attribs.size() )
    {
      AttributeList::const_iterator it_a = m_attribs.begin();
      for( it_a; it_a != m_attribs.end(); it_a++ )
      {
        xml += " " + (*it_a).first + "='" + (*it_a).second + "'";
      }
    }

    if( m_cdata.empty() && !m_children.size() )
      xml += "/>";
    else if( !m_cdata.empty() )
      xml += ">" + m_cdata + "</" + m_name + ">";
    else if( m_children.size() )
    {
      xml += ">";
      TagList::const_iterator it_c = m_children.begin();
      for( it_c; it_c != m_children.end(); it_c++ )
      {
        xml += (*it_c).second.xml();
      }
      xml += "</" + m_name + ">";
    }

    return xml;
  }

  void Tag::addAttrib( const std::string& name, const std::string& value )
  {
    m_attribs[name] = value;
  }

  void Tag::addChild( const Tag& child )
  {
    m_children[child.name()] = child;
  }

  Tag::AttributeList& Tag::attributes()
  {
    return m_attribs;
  }

  Tag::TagList& Tag::children()
  {
    return m_children;
  }

  const std::string Tag::findAttribute( const std::string& name ) const
  {
    AttributeList::const_iterator it = m_attribs.find( name );
    if( it != m_attribs.end() )
      return (*it).second;
    else
      return "";
  }

  bool Tag::hasAttribute( const std::string& name, const std::string& value ) const
  {
    if( name.empty() )
      return false;

    AttributeList::const_iterator it = m_attribs.find( name );
    if( it != m_attribs.end() )
      return ( ( value.empty() )?( true ):( (*it).second == value ) );
    else
      return false;
  }

  const Tag Tag::findChild( const std::string& name ) const
  {
    TagList::const_iterator it = m_children.find( name );
    if( it != m_children.end() )
      return (*it).second;
    else
      return Tag();
  }

  bool Tag::hasChild( const std::string& name,
                      const std::string& attr, const std::string& value ) const
  {
    TagList::const_iterator it = m_children.find( name );
    if( it != m_children.end() )
      return (*it).second.hasAttribute( attr, value );
    else
      return false;
  }

  bool Tag::hasChildWithCData( const std::string& name, const std::string& cdata ) const
  {
    TagList::const_iterator it = m_children.find( name );
    if( it != m_children.end() )
      return ( (*it).second.cdata() == cdata );
    else
      return false;
  }

  bool Tag::hasChildWithAttrib( const std::string& attr, const std::string& value ) const
  {
    TagList::const_iterator it = m_children.begin();
    for( it; it != m_children.end(); it++ )
    {
      if( (*it).second.hasAttribute( attr, value ) )
        return true;
    }

      return false;
  }

};

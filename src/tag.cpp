/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "tag.h"

namespace gloox
{
  Tag::Tag()
  : m_parent( 0 ), m_type( STANZA_UNDEFINED )
  {
  }

  Tag::Tag( const std::string& name, const std::string& cdata )
  : m_name( name ), m_cdata( escape( cdata ) ), m_parent( 0 ), m_type( STANZA_UNDEFINED )
  {
  }

  Tag::Tag( Tag *parent, const std::string& name, const std::string& cdata )
  : m_parent( parent ), m_name( name ), m_cdata( escape( cdata ) ), m_type( STANZA_UNDEFINED )
  {
  }

  Tag::~Tag()
  {
    TagList::iterator it = m_children.begin();
    for( it; it != m_children.end(); it++ )
    {
      delete( (*it) );
      (*it) = 0;
    }
    m_children.clear();
  }

  void Tag::setCData( const std::string& cdata )
  {
    m_cdata = escape( cdata );
  }

  void Tag::addCData( const std::string& cdata )
  {
    m_cdata += escape( cdata );
  }

  const std::string Tag::xml() const
  {
    std::string xml;
    xml = "<" + m_name;
    if( m_attribs.size() )
    {
      StringMap::const_iterator it_a = m_attribs.begin();
      for( it_a; it_a != m_attribs.end(); it_a++ )
      {
        xml += " " + (*it_a).first + "='" + (*it_a).second + "'";
      }
    }

    if( m_cdata.empty() && !m_children.size() )
      xml += "/>";
    else if( m_children.size() )
    {
      xml += ">";
      TagList::const_iterator it_c = m_children.begin();
      for( it_c; it_c != m_children.end(); it_c++ )
      {
        xml += (*it_c)->xml();
      }
      xml += "</" + m_name + ">";
    }
    else if( !m_cdata.empty() )
      xml += ">" + m_cdata + "</" + m_name + ">";

    return xml;
  }

  void Tag::addAttrib( const std::string& name, const std::string& value )
  {
    if( !value.empty() )
      m_attribs[name] = value;
  }

  void Tag::addChild( Tag *child )
  {
    m_children.push_back( child );
    child->m_parent = this;
  }

  std::string Tag::cdata() const
  {
    return relax( m_cdata );
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
    for( it; it != m_children.end(); it++ )
    {
      if( (*it)->name() == name )
        return (*it);
    }

    return 0;
  }

  bool Tag::hasChild( const std::string& name,
                      const std::string& attr, const std::string& value ) const
  {
    TagList::const_iterator it = m_children.begin();
    for( it; it != m_children.end(); it++ )
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
    for( it; it != m_children.end(); it++ )
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
    for( it; it != m_children.end(); it++ )
    {
      if( (*it)->hasAttribute( attr, value ) )
        return true;
    }

    return false;
  }

  Tag* Tag::findChildWithAttrib( const std::string& attr, const std::string& value )
  {
    TagList::const_iterator it = m_children.begin();
    for( it; it != m_children.end(); it++ )
    {
      if( (*it)->hasAttribute( attr, value ) )
        return (*it);
    }

    return 0;
  }

  const std::string Tag::escape( const std::string& what ) const
  {
    std::string esc = what;
    StringMap map;
    map["&"] = "&amp;";
    map["<"] = "&lt;";
    map[">"] = "&gt;";
    map["'"] = "&apos;";
    map["\""] = "&quot;";

    StringMap::const_iterator it = map.begin();
    for( it; it != map.end(); ++it )
    {
      size_t lookHere = 0;
      size_t foundHere;
      while( ( foundHere = esc.find( (*it).first, lookHere ) ) != std::string::npos )
      {
        esc.replace( foundHere, (*it).first.size(), (*it).second );
        lookHere = foundHere + (*it).second.size();
      }
    }
    return esc;
  }

  const std::string Tag::relax( const std::string& what ) const
  {
    std::string esc = what;
    StringMap map;
    map["&"] = "&amp;";
    map["<"] = "&lt;";
    map[">"] = "&gt;";
    map["'"] = "&apos;";
    map["\""] = "&quot;";

    StringMap::const_iterator it = map.begin();
    for( it; it != map.end(); ++it )
    {
      size_t lookHere = 0;
      size_t foundHere;
      while( ( foundHere = esc.find( (*it).second, lookHere ) ) != std::string::npos )
      {
        esc.replace( foundHere, (*it).second.size(), (*it).first );
        lookHere = foundHere + (*it).first.size();
      }
    }
    return esc;
  }

};

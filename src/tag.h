/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef TAG_H__
#define TAG_H__

#include "gloox.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  /**
   * @brief This is an abstraction of an XML element.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class Tag
  {
    public:
      /**
       * A list of Tags.
       */
      typedef std::list<Tag*> TagList;

      /**
       * Creates an empty tag.
       */
      Tag();

      /**
       * Creates a new tag with agiven name (and XML character data, if given).
       * @param name The name of the element.
       * @param cdata The XML character data of the element.
       */
      Tag( const std::string& name, const std::string& cdata = "" );

      /**
       * Creates a new tag as a child tag of the given parent, with a given name (and
       * XML character data, if given).
       * @param parent The parent tag.
       * @param name The name of the element.
       * @param cdata The XML character data of the element.
       */
      Tag( Tag *parent, const std::string& name, const std::string& cdata = "" );

      /**
       * Virtual destructor.
       */
      virtual ~Tag();

      /**
       * This function can be used to retrieve the complete XML of a tag as a string.
       * It includes all the attributes, child nodes and character data.
       * @return The complete XML.
       */
      virtual const std::string xml() const;

      /**
       * Use this function to add a new attribute to the tag.
       * @param name The name of the attribute.
       * @param value The value of the attribute.
       */
      virtual void addAttrib( const std::string& name, const std::string& value );

      /**
       * Use this function to add a child node to the tag.
       * @param child The node to be inserted.
       */
      virtual void addChild( Tag *child );

      /**
       * Sets the XML character data for this Tag.
       * @param cdata The new cdata.
       */
      virtual void setCData( const std::string& cdata );

      /**
       * Adds the string to the existing XML character data for this Tag.
       * @param cdata The additional cdata.
       */
      virtual void addCData( const std::string& cdata );

      /**
       * Use this function to retrieve the name of an element.
       * @return The name of the tag.
       */
      virtual std::string name() const { return m_name; };

      /**
       * Use this function to retrieve the XML character data of an element.
       * @return The cdata the element contains.
       */
      virtual std::string cdata() const;

      /**
       * Use this function to manipulate the list of attributes.
       * @return A reference to the list of attributes.
       */
      virtual StringMap& attributes();

      /**
       * Use this function to manipulate the list of child elements.
       * @return A reference to the list of child elements.
       */
      virtual TagList& children();

      /**
       * This function can be used to retrieve the value of a Tag's attribute.
       * @param name The name of the attribute to look for.
       * @return The value of the attribute if found, an empty string otherwise.
       */
      virtual const std::string findAttribute( const std::string& name ) const;

      /**
       * Checks whether the tag has a attribute with given name and optional value.
       * @param name The name of the attribute to check for.
       * @param value The value of the attribute to check for.
       * @return Whether the attribute exists (optionally with the given value).
       */
      virtual bool hasAttribute( const std::string& name, const std::string& value = "" ) const;

      /**
       * This function finds and returns an element within the child elements of the current tag.
       * @param name The name of the element to search for.
       * @return The found Tag, or an empty (invalid) Tag.
       */
      virtual Tag* findChild( const std::string& name );

      /**
       * This function checks whether the Tag has a child element with a given name, and optionally
       * this child element is checked for having a given attribute with an optional value.
       * @param name The name of the child element.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return @b True if the given child element exists, @ false otherwise.
       */
      virtual bool hasChild( const std::string& name,
                             const std::string& attr = "", const std::string& value = "" ) const;

      /**
       * This function checks whether the Tag has a child element which posesses a given attribute
       * with an optional value. The name of the child element does not matter.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return The child if found, an empty Tag otherwise.
       */
      virtual Tag* findChildWithAttrib( const std::string& attr, const std::string& value = "" );

      /**
       * This function checks whether the Tag has a child element which posesses a given attribute
       * with an optional value. The name of the child element does not matter.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return @b True if any such child element exists, @ false otherwise.
       */
      virtual bool hasChildWithAttrib( const std::string& attr, const std::string& value = "" ) const;

      /**
       * Returns whether a Tag is considered empty, i.e. invalid.
       * @return @b True if the Tag is valid, @b false if not.
       */
      virtual bool empty() const { return m_name.empty(); };

      /**
       * This function checks whether a child element with given name exists and has
       * XML character data that equals the given cdata string.
       * @param name The name of the child element.
       * @param cdata The character data that has to exist in the child element.
       * @return @b True if a child element with given cdata exists, @b false otherwise.
       */
      bool hasChildWithCData( const std::string& name, const std::string& cdata ) const;

      /**
       * Returns the tag's parent Tag.
       * @return The Tag above the current Tag.
       */
      Tag* parent() { return m_parent; };

      /**
       * Returns the stanza type.
       * @return The type of the stanza.
       */
      virtual StanzaType type() const { return m_type; };

      /**
       * This function creates a deep copy of this Tag.
       * @return An independent copy of the Tag.
       * @since 0.7
       */
      virtual Tag* clone();

    protected:
      const std::string escape( const std::string& what ) const;
      const std::string relax( const std::string& what ) const;

      std::string m_name;
      StringMap m_attribs;
      std::string m_cdata;
      TagList m_children;
      Tag *m_parent;
      StanzaType m_type;

  };

}

#endif // TAG_H__

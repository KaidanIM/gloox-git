/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
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
  class GLOOX_API Tag
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
       * Creates a new tag with a given name (and XML character data, if given).
       * @param name The name of the element.
       * @param cdata The XML character data of the element.
       * @param incoming Indicates whether tag names, attributes, attribute values, and cdata shall
       * be escaped (false, default) or not (true).
       */
      Tag( const std::string& name, const std::string& cdata = "", bool incoming = false );

      /**
       * Creates a new tag as a child tag of the given parent, with a given name (and
       * XML character data, if given).
       * @param parent The parent tag.
       * @param name The name of the element.
       * @param cdata The XML character data of the element.
       * @param incoming Indicates whether tag names, attributes, attribute values, and cdata shall
       * be escaped (false, default) or not (true).
       */
      Tag( Tag *parent, const std::string& name, const std::string& cdata = "", bool incoming = false );

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
      virtual void addAttribute( const std::string& name, const std::string& value );

      /**
       * Use this function to add a new attribute to the tag. Tha value is an @c int here.
       * @param name The name of the attribute.
       * @param value The value of the attribute.
       * @since 0.8
       */
      virtual void addAttribute( const std::string& name, int value );

      /**
       * Sets the given attributes. Any existing attributes are lost.
       * @param attributes The attributes to set.
       * @since 0.9
       */
      virtual void setAttributes( const StringMap& attributes ) { m_attribs = attributes; };

      /**
       * Use this function to add a child node to the tag. The Tag will be owned by Tag.
       * @param child The node to be inserted.
       */
      virtual void addChild( Tag *child );

      /**
       * Use this function to add a child node to the tag. The Tag will be copied.
       * @param child The node to be inserted.
       * @since 0.9
       */
      virtual void addChild( const Tag *child );

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
      virtual const std::string& name() const { return m_name; };

      /**
       * Use this function to retrieve the XML character data of an element.
       * @return The cdata the element contains.
       */
      virtual const std::string cdata() const;

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
       * This function finds and returns the @b first element within the child elements of the current tag
       * that has a matching tag name.
       * @param name The name of the element to search for.
       * @return The found Tag, or NULL.
       */
      virtual Tag* findChild( const std::string& name );

      /**
       * This function finds and returns the @b first element within the child elements of the current tag,
       * that has a certain name, and a certain attribute with a certain value.
       * @param name The name of the element to search for.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return The found Tag, or NULL.
       */
      virtual Tag* findChild( const std::string& name, const std::string& attr,
                               const std::string& value = "" );

      /**
       * This function checks whether the Tag has a child element with a given name, and optionally
       * this child element is checked for having a given attribute with an optional value.
       * @param name The name of the child element.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return @b True if the given child element exists, @b false otherwise.
       */
      virtual bool hasChild( const std::string& name, const std::string& attr = "",
                             const std::string& value = "" ) const;

      /**
       * This function checks whether the Tag has a child element which posesses a given attribute
       * with an optional value. The name of the child element does not matter.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return The child if found, NULL otherwise.
       */
      virtual Tag* findChildWithAttrib( const std::string& attr, const std::string& value = "" );

      /**
       * This function checks whether the Tag has a child element which posesses a given attribute
       * with an optional value. The name of the child element does not matter.
       * @param attr The name of the attribute of the child element.
       * @param value The value of the attribute of the child element.
       * @return @b True if any such child element exists, @b false otherwise.
       */
      virtual bool hasChildWithAttrib( const std::string& attr, const std::string& value = "" ) const;

      /**
       * Returns a list of child tags of the current tag with the given name.
       * @param name The name of the tags to look for.
       * @return A list of tags with the given name.
       * @note The tags are still linked to the current Tag and should not be deleted from the TagList.
       * @since 0.9
       */
      TagList findChildren( const std::string& name );

      /**
       * Returns whether the Tag is considered empty, i.e. invalid.
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
       * @return The Tag above the current Tag. May be @b 0.
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
      virtual Tag* clone() const;

      /**
       * Checks two Tags for equality. Order of attributes and child tags does matter.
       * @param right The Tag to check against the current Tag.
       * @since 0.9
       */
      bool operator==( const Tag &right ) const;

      /**
       * Checks two Tags for inequality. Order of attributes and child tags does matter.
       * @param right The Tag to check against the current Tag.
       * @since 0.9
       */
      bool operator!=( const Tag &right ) const;

    protected:
      std::string m_name;
      StringMap m_attribs;
      std::string m_cdata;
      TagList m_children;
      Tag *m_parent;
      StanzaType m_type;
      bool m_incoming;

    private:
      struct duo
      {
        duo( std::string f, std::string s ) : first( f), second( s ) {};
        std::string first;
        std::string second;
      };
      typedef std::list<duo> Duo;

      const std::string escape( const std::string& what ) const;
      const std::string relax( const std::string& what ) const;
      const std::string replace( const std::string& what, const Duo& duo ) const;
      TagList findChildren( TagList& list, const std::string& name );

  };

}

#endif // TAG_H__

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef DATAFORMFIELD_H__
#define DATAFORMFIELD_H__

#include "gloox.h"
#include "tag.h"

#include <string>

namespace gloox
{

  /**
   * An abstraction of a single field in a JEP-0004 Data Form.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.7
   */
  class DataFormField
  {
    public:
      /**
       * Describes the possible type of a Data Form Field.
       */
      enum DataFormFieldType
      {
        FIELD_TYPE_BOOLEAN,         /**< The field enables an entity to gather or provide an either-or choice
                                     * between two options. The default value is "false". */
        FIELD_TYPE_FIXED,           /**< The field is intended for data description (e.g., human-readable text
                                     * such as "section" headers) rather than data gathering or provision. The
                                     * &lt;value/&gt; child SHOULD NOT contain newlines (the \n and \r
                                     * characters); instead an application SHOULD generate multiple fixed
                                     * fields, each with one &lt;value/&gt; child. */
        FIELD_TYPE_HIDDEN,          /**< The field is not shown to the entity providing information, but
                                     * instead is returned with the form. */
        FIELD_TYPE_JID_MULTI,       /**< The field enables an entity to gather or provide multiple Jabber
                                     * IDs.*/
        FIELD_TYPE_JID_SINGLE,      /**< The field enables an entity to gather or provide a single Jabber
                                     * ID.*/
        FIELD_TYPE_LIST_MULTI,      /**< The field enables an entity to gather or provide one or more options
                                     * from among many. */
        FIELD_TYPE_LIST_SINGLE,     /**< The field enables an entity to gather or provide one option from
                                     * among many. */
        FIELD_TYPE_TEXT_MULTI,      /**< The field enables an entity to gather or provide multiple lines of
                                     * text. */
        FIELD_TYPE_TEXT_PRIVATE,    /**< The field enables an entity to gather or provide a single line or
                                     * word of text, which shall be obscured in an interface (e.g., *****). */
        FIELD_TYPE_TEXT_SINGLE,     /**< The field enables an entity to gather or provide a single line or
                                     * word of text, which may be shown in an interface. This field type is
                                     * the default and MUST be assumed if an entity receives a field type it
                                     * does not understand.*/
      };

      /**
       *
       */
      DataFormField( DataFormFieldType type = FIELD_TYPE_TEXT_SINGLE );

      /**
       * Virtual destructor.
       */
      virtual ~DataFormField();

      /**
       *
       */
      Tag* tag();

      /**
       *
       */
      StringMap& options() { return m_options; };

      /**
       *
       */
      void setOptions( const StringMap& options ) { m_options = options; };

      /**
       *
       */
      bool required() const { return m_required; };

      /**
       *
       */
      void setRequired( bool required ) { m_required = required; };

      /**
       *
       */
      const std::string& fieldName() const { return m_fieldName; };

      /**
       *
       */
      void setFieldName( const std::string& fieldName ) { m_fieldName = fieldName; };

      /**
       *
       */
      DataFormFieldType type() const { return m_type; };

      /**
       *
       */
      const std::string& label() const { return m_label; };

      /**
       *
       */
      void setLabel( const std::string& label ) { m_label = label; };

      /**
       *
       */
      const std::string& value() const { return m_value; };

      /**
       *
       */
      void setValue( const std::string& value ) { m_value = value; };

    private:
      StringMap m_options;

      std::string m_value;
      std::string m_desc;
      std::string m_label;
      std::string m_fieldName;
      DataFormFieldType m_type;
      bool m_required;
  };

};

#endif // DATAFORMFIELD_H__

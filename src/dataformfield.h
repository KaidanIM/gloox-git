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
       * Constructs a new Data Form Field.
       * @param type The type of the field. Default: text-single.
       */
      DataFormField( DataFormFieldType type = FIELD_TYPE_TEXT_SINGLE );

      /**
       * Virtual destructor.
       */
      virtual ~DataFormField();

      /**
       * Use this function to create a Tag representation of the form field. This is usually called by
       * DataForm.
       * @return A Tag hierarchically describing the form field.
       */
      Tag* tag() const;

      /**
       * Use this function to retrieve the optional values of a field.
       * @return The options of a field.
       */
      StringMap& options() { return m_options; };

      /**
       * Use this function to set the optional values of a field.
       * @param options The optional values of a list* or *multi type of field.
       */
      void setOptions( const StringMap& options ) { m_options = options; };

      /**
       * Use this function to determine whether or not this field is required.
       * @return Whether or not this field is required.
       */
      bool required() const { return m_required; };

      /**
       * Use this field to set this field to be required.
       * @param required Whether or not this field is required.
       */
      void setRequired( bool required ) { m_required = required; };

      /**
       * Use this function to retrieve the name of the field (the content of the 'var' attribute).
       * @return The name of the field.
       */
      const std::string& name() const { return m_name; };

      /**
       * Sets the name of the field.
       * @param name The new name of the field.
       * @note Fields of type other than 'fixed' SHOULD have a name.
       */
      void setFieldName( const std::string& name ) { m_name = name; };

      /**
       * Use this function to retrieve the type of this field.
       * @return The type of this field.
       */
      DataFormFieldType type() const { return m_type; };

      /**
       * Use this function to retrieve the describing label of this field.
       * @return The describing label of this field.
       */
      const std::string& label() const { return m_label; };

      /**
       * Use this function to set the describing label of this field.
       * @param label The describing label of thsi field.
       */
      void setLabel( const std::string& label ) { m_label = label; };

      /**
       * Use this function to retrieve the value of this field.
       * @return The value of this field.
       */
      const std::string& value() const { return m_value; };

      /**
       * Use this function to set the value of this field.
       * @param value The new value of this field.
       */
      void setValue( const std::string& value ) { m_value = value; };

    private:
      StringMap m_options;

      std::string m_value;
      std::string m_desc;
      std::string m_label;
      std::string m_name;
      DataFormFieldType m_type;
      bool m_required;
  };

};

#endif // DATAFORMFIELD_H__

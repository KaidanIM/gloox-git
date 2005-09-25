/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef DATAFORM_H__
#define DATAFORM_H__

#include "dataformfield.h"

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  /**
   * @brief An abstraction of a JEP-0004 Data Form.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.7
   * @note This implementation lacks support for Section 3.4 of JEP-0004 (Multiple Items in Form Results),
   * i.e. the &lt;reported&gt; and &lt;item&gt; elements.
   */
  class DataForm
  {
    public:
      /**
       * Describes the possible Form Types.
       */
      enum DataFormType
      {
        FORM_TYPE_FORM,             /**< The forms-processing entity is asking the forms-submitting entity to
                                     * complete a form. */
        FORM_TYPE_SUBMIT,           /**< The forms-submitting entity is submitting data to the
                                     * forms-processing entity. */
        FORM_TYPE_CANCEL,           /**< The forms-submitting entity has cancelled submission of data to the
                                     * forms-processing entity. */
        FORM_TYPE_RESULT,           /**< The forms-processing entity is returning data (e.g., search results)
                                     * to the forms-submitting entity, or the data is a generic data set. */
        FORM_TYPE_INVALID,          /**< The form is invalid. Only possible if the form was created from an
                                     * Tag which doesn't correctly describe a Data Form. */
      };

      /**
       * A list of Data Form Fields.
       */
      typedef std::list<DataFormField> FieldList;

      /**
       * Contsructs a new, empty form.
       * @param type The form type.
       * @param title The natural-language title of the form. Should not contain newlines (\n, \r).
       * @param instructions Natural-language instructions for filling out the form. Should not contain
       * newlines (\\n, \\r).
       */
      DataForm( DataFormType type, const std::string& title = "", const std::string& instructions = "" );

      /**
       * Constructs a new DataForm from an existing Tag/XML representation.
       * @param tag The existing form to parse.
       */
      DataForm( Tag *tag );

      /**
       * Virtual destructor.
       */
      virtual ~DataForm();

      /**
       * Use this function to create a Tag representation of the form.
       * @return A Tag hierarchically describing the form, or NULL if the form is invalid (i.e.
       * created from a Tag not correctly describing a Data Form).
       */
      Tag* tag();

      /**
       * Use this function to check whether  this form contains a field with the given name.
       * @param field The name of the field (the content of the 'var' attribute).
       * @return Whether or not the form contains the named field.
       */
      bool hasField( const std::string& field );

      /**
       * Use this function to fetch a copy of a field of the form. If no such field exists, an empty
       * (invalid) field is returned.
       * @param field The name of the field (the content of the 'var' attribute).
       * @return A copy of the field with the given name if it exists, an empty (invalid) field otherwise.
       */
      DataFormField field( const std::string& field );

      /**
       * Use this function to retrieve the title of the form.
       * @return The title of the form.
       */
      const std::string& title() const { return m_title; };

      /**
       * Use this function to set the title of the form.
       * @param title The new title of the form.
       * @note The title should not contain newlines (\\n, \\r).
       */
      void setTitle( const std::string& title ) { m_title = title; };

      /**
       * Retrieves the natural-language instructions for the form.
       * @return The fill-in instructions for the form.
       */
      const std::string& instructions() const { return m_instructions; };

      /**
       * Use this function to set natural-language instructions for the form.
       * @param instructions The instructions for the form.
       * @note The instructions should not contain newlines (\\n, \\r).
       */
      void setInstructions( const std::string& instructions ) { m_instructions = instructions; };

      /**
       * Use this function to set the fields the form contains.
       * @param fields The list of fields.
       * @note Any previously set fields will be deleted. Always set all fields, not a delta.
       */
      void setFields( FieldList& fields ) { m_fields = fields; };

      /**
       * Use this function to retrieve the list of fields of a form.
       * @return The list of fields the form contains.
       */
      FieldList& fields() { return m_fields; };

    private:
      FieldList m_fields;
      DataFormType m_type;
      std::string m_title;
      std::string m_instructions;
  };

};

#endif // DATAFORM_H__

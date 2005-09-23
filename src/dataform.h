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
   * An abstarction of a JEP-0004 Data Form.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.7
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
      };

      /**
       * A list of Data Form Fields.
       */
      typedef std::list<DataFormField*> FieldList;

      /**
       *
       */
      DataForm( DataFormType type, const std::string& element = "x", const std::string& title = "",
                const std::string& instructions = "" );

      /**
       * Virtual destructor.
       */
      virtual ~DataForm();

      /**
       *
       */
      Tag* tag();

      /**
       *
       */
      const std::string& title() const { return m_title; };

      /**
       *
       */
      void setTitle( const std::string& title ) { m_title = title; };

      /**
       *
       */
      const std::string& instructions() const { return m_instructions; };

      /**
       *
       */
      void setInstructions( const std::string& instructions ) { m_instructions = instructions; };

      /**
       *
       */
      void setFields( FieldList& fields ) { m_fields = fields; };

      /**
       *
       */
      FieldList& fields() { return m_fields; };

    private:
      FieldList m_fields;
      DataFormType m_type;
      std::string m_element;
      std::string m_title;
      std::string m_instructions;
  };

};

#endif // DATAFORM_H__

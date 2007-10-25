/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef SEVERSION_H__
#define SEVERSION_H__

#include "stanzaextension.h"
#include "tag.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an implementation of XEP-0092 as a StanzaExtension.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class SEVersion : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object with the given resource string.
       * @param tag The Tag to parse.
       */
      SEVersion( const std::string& name, const std::string& version, const std::string& os );

      /**
       * Constructs a new object from the given Tag.
       * @param tag The Tag to parse.
       */
      SEVersion( const Tag* tag );

      /**
       * Returns the application's name.
       * @return The application's name.
       */
      const std::string& name() const { return m_name; }

      /**
       * Returns the application's version.
       * @return The application's version.
       */
      const std::string& version() const { return m_version; }

      /**
       * Returns the application's Operating System.
       * @return The application's OS.
       */
      const std::string& os() const { return m_os; }

      /**
       * Virtual Destructor.
       */
      virtual ~SEVersion();

      // reimplemented from StanzaExtension
      virtual const std::string& filterString() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new SEVersion( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

    private:
      std::string m_name;
      std::string m_version;
      std::string m_os;
  };

}

#endif // SEVERSION_H__

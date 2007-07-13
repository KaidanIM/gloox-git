/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef CAPABILITIES_H__
#define CAPABILITIES_H__

#include "stanzaextension.h"
#include "tag.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an implementation of XEP-0115 (Entity Capabilities).
   *
   * XEP Version: 1.3
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class Capabilities : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object and fills it according to the parameters.
       * @param node The node identifying the client.
       * @param stamp The client's version.
       * @param reason A whitespace-separated list of supported extensions.
       */
      Capabilities( const std::string& node, const std::string& version, const std::string& ext = "" );

      /**
       * Constructs a new object from the given Tag.
       * @param tag The Tag to parse.
       */
      Capabilities( Tag *tag );

      /**
       * Virtual Destructor.
       */
      virtual ~Capabilities();

      /**
       * Returns the client's identifying node.
       * @return The node.
       */
      const std::string& node() const { return m_node; }

      /**
       * Returns the client's identifying version.
       * @return The version.
       */
      const std::string& version() const { return m_version; }

      /**
       * Returns the client's extensions (whitespace-separated).
       * @return The extensions.
       */
      const std::string& ext() const { return m_ext; }

      /**
       * Sets the client's extensions (whitespace-separated).
       * @param node The extensions (whitespace-separated).
       */
      void setExt( const std::string& ext ) { m_ext = ext; }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

    private:
      std::string m_node;
      std::string m_version;
      std::string m_ext;
      bool m_valid;
  };

}

#endif // CAPABILITIES_H__

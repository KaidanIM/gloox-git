/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef VCARDUPDATE_H__
#define VCARDUPDATE_H__

#include "stanzaextension.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an abstraction of a vcard-temp:x:update namespace element, as used in XEP-0153
   * (vCard-Based Avatars).
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API VCardUpdate : public StanzaExtension
  {
    public:
      /**
       * Constructs an empty VCardUpdate object.
       */
      VCardUpdate();

      /**
       * Constructs an VCardUpdate object from the given Tag. To be recognized properly, the Tag should
       * have a name 'x' of in the vcard-temp:x:update namespace.
       * @param tag The Tag to parse.
       */
      VCardUpdate( Tag *tag );

      /**
       * Virtual destructor.
       */
      virtual ~VCardUpdate();

      /**
       *
       */
      void setHash( const std::string& hash ) { m_hash = hash; };

      /**
       *
       */
      const std::string& hash() const { return m_hash; };

      // reimplemented from StanzaExtension
      Tag* tag() const;

    private:
      std::string m_hash;
      bool m_notReady;
      bool m_noImage;

  };

}

#endif // VCARDUPDATE_H__

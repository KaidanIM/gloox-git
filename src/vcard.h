/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef VCARD_H__
#define VCARD_H__

#include "gloox.h"

namespace gloox
{

  class Tag;

  /**
   * @brief A VCard abstraction.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API VCard
  {
    public:
      /**
       * Constructor.
       */
      VCard();

      /**
       * Constructs a new VCard from a given Tag containing appropriate fields.
       * @param vcard The vcard-tag.
       */
      VCard( const Tag* vcard );

      /**
       * Virtual destructor.
       */
      virtual ~VCard();

      /**
       * Returns a Tag representation of the VCard. The caller becomes the owner of the Tag.
       * @return A Tag containing the VCard, or @b 0 if the VCard data is invalid.
       */
      Tag* tag() const;

  };

}

#endif // VCARD_H__

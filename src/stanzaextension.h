/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef STANZAEXTENSION_H__
#define STANZAEXTENSION_H__

#include "macros.h"

namespace gloox
{

  class GLOOX_API Tag;

  /**
   * @brief This class abstracts a stanza extension (usually an 'x' element in a specific
   * namespace.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class StanzaExtension
  {
    public:
      /**
       *
       */
      StanzaExtension() {};

      /**
       * Virtual destructor.
       */
      virtual ~StanzaExtension() {};

  };
}

#endif // STANZAEXTENSION_H__

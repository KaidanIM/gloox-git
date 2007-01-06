/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef STANZAEXTENSIONFACTORY_H__
#define STANZAEXTENSIONFACTORY_H__

#include <string>

namespace gloox
{

  class StanzaExtension;
  class Tag;

  /**
   * @brief A Factory that creates StanzaExtensions from Tags.
   *
   * You should not need to use this class directly.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class StanzaExtensionFactory
  {
    public:
      /**
       *
       */
      static StanzaExtension* create( Tag* tag );

  };

}

#endif // STANZAEXTENSIONFACTORY_H__

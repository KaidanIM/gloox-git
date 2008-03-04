/*
  Copyright (c) 2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEDESCRIPTION_H__
#define JINGLEDESCRIPTION_H__


#include "jingleplugin.h"

namespace gloox
{

  namespace Jingle
  {

    /**
     * @brief An abstraction of a Jingle Description (Application Format).
     *
     * You should not need to use this class directly. See
     * @link gloox::Jingle::Session Jingle::Session @endlink for more info on Jingle.
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class Description : public Plugin
    {
    };

  }

}

#endif // JINGLEDESCRIPTION_H__

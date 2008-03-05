/*
  Copyright (c) 2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEICEUDP_H__
#define JINGLEICEUDP_H__

#include "jingletransport.h"

#include <string>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle ICE-UDP Transport Method (XEP-0176).
     *
     * XEP Version: 0.16
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class ICEUDP : public Transport
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~ICEUDP() {}

        // reimplemented from Plugin
        virtual const std::string& filterString() const { return EmptyString; }

        // reimplemented from Plugin
        virtual Tag* tag() const { return 0; }

    };

  }

}

#endif // JINGLEICEUDP_H__

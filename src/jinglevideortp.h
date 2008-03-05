/*
  Copyright (c) 2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEVIDEORTP_H__
#define JINGLEVIDEORTP_H__

#include "jingledescription.h"

#include <string>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle Video via RTP (XEP-0180).
     *
     * XEP Version: 0.11
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class VideoRTP : public Description
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~VideoRTP() {}

        // reimplemented from Plugin
        virtual const std::string& filterString() const { return EmptyString; }

        // reimplemented from Plugin
        virtual Tag* tag() const { return 0; }

    };

  }

}

#endif // JINGLEVIDEORTP_H__

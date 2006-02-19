/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAMDATAHANDLER_H__
#define INBANDBYTESTREAMDATAHANDLER_H__

#include "macros.h"

#include <string>

namespace gloox
{

  class GLOOX_API InBandBytestreamDataHandler
  {
    public:
      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestreamDataHandler() {};

      /**
       *
       */
      virtual void handleInBandData( const std::string& data, const std::string& sid ) = 0;

      /**
       *
       */
      virtual void handleInBandError( const std::string& sid ) = 0;

      /**
       *
       */
      virtual bool handleInBandRequest( const std::string& sid ) = 0;

  };

}

#endif // INBANDBYTESTREAMDATAHANDLER_H__

/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAMHANDLER_H__
#define INBANDBYTESTREAMHANDLER_H__

#include "macros.h"

namespace gloox
{

  class GLOOX_API InBandBytestreamHandler
  {
    public:
      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestreamHandler();

      /**
       *
       */
      virtual void handleInBandData( const std::string& data, const std::string& sid ) = 0;

      /**
       *
       */
      virtual void handleInBandError( ) = 0;

      /**
       *
       */
      virtual std::string handleInBandFetchData( const std::string& sid ) = 0;

  };

};

#endif // INBANDBYTESTREAMHANDLER_H__

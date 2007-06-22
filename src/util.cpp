/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "util.h"
#include <math.h>

namespace gloox
{

  namespace util
  {

    unsigned _lookup( const std::string& str, const char * values[], unsigned size )
    {
      unsigned i = 0;
      for( ; i < size && str != values[i]; ++i ) ;
      return i;
    }

  }

}


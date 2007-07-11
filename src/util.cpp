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
#include <cmath>

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

    const std::string _lookup( unsigned code, const char * values[], unsigned size )
    {
      return code < size ? std::string(values[code]) : std::string();
    }

    const std::string _lookup2( unsigned code, const char * values[], unsigned size )
    {
      const unsigned i = (unsigned)log2(code);
      return i < size ? std::string(values[i]) : std::string();
    }

  }

}


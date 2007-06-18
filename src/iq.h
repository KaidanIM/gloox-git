/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef IQ_H__
#define IQ_H__

#include "stanza.h"
#include <string>

namespace gloox
{

  class IQ : public Stanza
  {

  public:

    enum IqType
    {
      IqTypeGet,    /**< The stanza is a request for information or requirements. */
      IqTypeSet,    /**< The stanza provides required data, sets new values, or
                     *   replaces existing values. */
      IqTypeResult, /**< The stanza is a response to a successful get or set request. */
      IqTypeError   /**< An error has occurred regarding processing or delivery of a
                     *   previously-sent get or set (see Stanza Errors (Section 9.3)). */
    };

    /**
     * Creates an IQ from a tag.
     */
    IQ( Tag * tag ) : Stanza( tag ) {}

    /**
     * Creates an IQ Query.
     */
    IQ ( IqType type, const std::string& id,
                      const std::string& to,
                      const std::string& from = "",
                      const std::string& childtag = "",
                      const std::string& xmlns = "");

  };

}

#endif // IQ_H__

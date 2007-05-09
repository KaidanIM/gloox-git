#ifndef IQ_H
#define IQ_H

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
    IQ::IQ( Tag * tag ) : Stanza( tag ) {}

    /**
     * Creates a simple IQ.
     */
    IQ ( IqType type, const std::string& id,
                      const std::string& to = "",
                      const std::string& from = "" );

    /**
     * Creates an IQ Query.
     */
    IQ ( IqType type, const std::string& id,
                      const std::string& to,
                      const std::string& from,
                      const std::string& xmlns,
                      const std::string& node = "" );

  };

}

#endif // IQ_H

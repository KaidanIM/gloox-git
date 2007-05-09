#ifndef IQ_H
#define IQ_H

#include "tag.h"
#include <string>

namespace gloox
{

  class IQ : public Tag
  {

  public:
  
    enum IqType
    {
      IqTypeSet,
      IqTypeGet,
      IqTypeResult,
      IqTypeError
    };

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

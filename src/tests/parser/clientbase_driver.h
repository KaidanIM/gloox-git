
#ifndef CLIENTBASE_DRIVER_H__
#define CLIENTBASE_DRIVER_H__

#include "stanza.h"
#include "parser.h"

namespace gloox
{
  class Stanza;
  class Parser;

  class ClientBase
  {
    public:
      enum NodeType
      {
        NODE_STREAM_START,             /**< The &lt;stream:stream&gt; tag. */
        NODE_STREAM_ERROR,             /**< The &lt;stream:error&gt; tag. */
        NODE_STREAM_CLOSE,             /**< The &lt;/stream:stream&gt; tag. */
        NODE_STREAM_CHILD              /**< Everything else. */
      };


      ClientBase();
      virtual ~ClientBase();

      void filter( NodeType type, Stanza *stanza );

      Parser::ParserState setTest( Parser *parser, const std::string& str );
      Stanza* getLastResult();

    private:
      Stanza *m_tag;
  };

}

#endif // CLIENTBASE_DRIVER_H__


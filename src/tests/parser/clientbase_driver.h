
#ifndef CLIENTBASE_DRIVER_H__
#define CLIENTBASE_DRIVER_H__

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

      void setTest( Parser *parser, int num );
      bool getLastResult();

    private:
      bool m_result;
      int m_testNum;
  };

}

#endif // CLIENTBASE_DRIVER_H__


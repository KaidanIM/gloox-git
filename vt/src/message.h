#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <list>
#include "tag.h"
#include "stanzaextension.h"

namespace gloox
{

  class Message : public Tag
  {

    public:

      /**
       * @brief Describes the different valid message types.
       */
      enum MessageType
      {
        MessageChat,
        MessageError,
        MessageGroupChat,
        MessageHeadline,
        MessageNormal
      };

      /**
       * @brief Creates a chat Message.
       */
      Message ( const std::string& id,
                const std::string& to = "",
                const std::string& from = "" );

      /**
       * @brief Creates a Message.
       */
      Message ( MessageType type, const std::string& id,
                                  const std::string& to = "",
                                  const std::string& from = "" );

      /**
       * @brief Creates a Message.
       */
      Message ( MessageType type, const std::string& id,
                                  const std::string& to,
                                  const std::string& from,
                                  const std::string& xmlns,
                                  const std::string& node = "" );

      /**
       * @brief Virtual destructor.
       */
      ~Message();

      /**
       * @brief Adds an extension to the message.
       * @arg ext Extension to add. The extension must be a legal message extension.
       */
      void addExtension( StanzaExtension * ext ) // + switch ext->type() to ensure legal extension is added
        { m_extensions.push_back( extension ); }

      MessageType type() const { return m_messageType; }
      void type( MessageType type ) { m_messageType = type; }

    private:
      MessageType m_messageType;
      std::list< StanzaExtension * > m_extensions;
  };

}

#endif // MESSAGE_H

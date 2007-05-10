/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef MESSAGE_H__
#define MESSAGE_H__

#include <string>
#include <list>
#include "stanza.h"

namespace gloox
{

  class Message : public Stanza
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

      Message( Tag * tag ) : Stanza( tag ) {}

      /**
       * @brief Creates a Message.
       */
      Message ( MessageType type, const std::string& id,
                                  const std::string& to,
                                  const std::string& from,
                                  const std::string& body,
                                  const std::string& thread = "",
                                  const std::string& xmllang = "",
                                  const std::string& subject = "" );
      /**
       * @brief Virtual destructor.
       */
      ~Message();

      /**
       * @brief Adds an extension to the message.
       * @arg ext Extension to add. The extension must be a legal message extension.
       */
      void addExtension( StanzaExtension * ext ) // + switch ext->type() to ensure legal extension is added ?
        { m_extensions.push_back( extension ); }

      MessageType type() const { return m_messageType; }
      void type( MessageType type ) { m_messageType = type; }

    private:
      MessageType m_messageType;
  };

}

#endif // MESSAGE_H__

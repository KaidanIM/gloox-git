/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef MESSAGE_H__
#define MESSAGE_H__

#include "stanza.h"

#include <string>

namespace gloox
{

  class JID;

  /**
   *
   * @author Vincent Thomasset
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class Message : public Stanza
  {

    public:

      /**
       * Describes the different valid message types.
       */
      enum MessageType
      {
        Chat           =  1,        /**< */
        MessageError   =  2,        /**< */
        Groupchat      =  4,        /**< */
        Headline       =  8,        /**< */
        Normal         = 16,        /**< */
        Invalid        = 32         /**< */
      };

      /**
       * Creates a message Stanza from the given Tag.
       * @param tag The Tag to parse.
       */
      Message( Tag *tag, bool rip = false );

      /**
       * Creates a Message.
       */
      Message( MessageType type, const JID& to,
               const std::string& body = "", const std::string& subject = "",
               const std::string& thread = "", const std::string& xmllang = "",
               const JID& from = JID() );
      /**
       * Destructor.
       */
      virtual ~Message();

      /**
       * Adds an extension to the message.
       * @param ext Extension to add. The extension must be a legal message extension.
       */
      void addExtension( StanzaExtension *extension ) // + switch ext->type() to ensure legal extension is added ?
        { m_extensionList.push_back( extension ); }

      /**
       *
       */
      MessageType subtype() const { return m_subtype; }

      /**
       * Returns the body of a message stanza for the given language if available.
       * If the requested language is not available, the default body (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default body
       * will be returned, if any.
       * @return The body of a message stanza.
       */
      virtual const std::string body( const std::string& lang = "default" ) const
      { return findLang( m_body, lang ); }

      /**
       * Returns the subject of a message stanza for the given language if available.
       * If the requested language is not available, the default subject (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default subject
       * will be returned, if any.
       * @return The subject of a message stanza.
       */
      virtual const std::string subject( const std::string& lang = "default" ) const
      { return findLang( m_subject, lang ); }

      /**
       * Returns the thread ID of a message stanza.
       * @return The thread ID of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string& thread() const { return m_thread; }

      /**
       * Sets the Stanza's thread ID. Only useful for message stanzas.
       * @param thread The thread ID.
       * @since 0.9
       */
      void setThread( const std::string& thread ) { m_thread = thread; }

    private:
      MessageType m_subtype;
      StringMap m_body;
      StringMap m_subject;
      std::string m_thread;
  };

}

#endif // MESSAGE_H__

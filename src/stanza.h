/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef STANZA_H__
#define STANZA_H__

#include "gloox.h"
#include "tag.h"
#include "jid.h"

namespace gloox
{

  /**
   * @brief This is an abstraction of a XMPP stanza.
   *
   * You can create a new Stanza from an existing Tag (or another stanza).
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class Stanza : public Tag
  {
    public:

      /**
       * Creates a new Stanza from the given Tag.
       * @note This creates a shallow copy, i.e. pointers to child tags are shared with the original
       * Tag. You will ahve problems freeing the allocated memory.
       * @param tag The Tag to create the stanza from.
       * @deprecated
       */
      Stanza( Tag *tag );

      /**
       * Creates an ew Stanza with given name and optional CData.
       * This will probably be replaced with special subclasses fro IQ, Message, Subscription
       * and Presenc e stanzas. Direct usage of Tags is encouraged.
       * @param name The name of the root tag.
       * @param cdata Initial XML character data for the tag.
       * @param xmllang The value of the xmllang attribute. The stanza's primary language.
       */
      Stanza( const std::string& name, const std::string& cdata = "",
              const std::string& xmllang = "default" );

      /**
       * Virtual destructor.
       */
      virtual ~Stanza() {};

      /**
       * Returns the sub-type of the stanza.
       * @return The sub-type of the stanza.
       */
      virtual StanzaSubType subtype() const { return m_subtype; };

      /**
       * Returns the JID the stanza comes from.
       * @return The origin of the stanza.
       */
      virtual const JID& from() const { return m_from; };

      /**
       * Returns the receiver of the stanza.
       * @return The stanza's destination.
       */
      virtual const JID& to() const { return m_to; };

      /**
       * Returns the id of the stanza, if set.
       * @return The ID of the stanza.
       */
      virtual const std::string id() const { return m_id; };

      /**
       * Returns the value of the xmlns attribute of the first child node.
       * @return The namespace of the IQ stanza.
       */
      virtual const std::string& xmlns() const { return m_xmlns; };

      /**
       * Returns the presence 'show' type of a presence stanza.
       * @return The presence type of the sender.
       */
      virtual PresenceStatus show() const { return m_show; };

      /**
       * Returns the status text of a presence stanza for the given language if available.
       * If the requested language is not available, the default status text (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default body
       * will be returned, if any.
       * @return The status text set by the sender.
       */
      virtual const std::string status( const std::string& lang = "default" ) const;

      /**
       * Returns the body of a message stanza for the given language if available.
       * If the requested language is not available, the default body (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default body
       * will be returned, if any.
       * @return The body of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string body( const std::string& lang = "default" ) const;

      /**
       * Returns the subject of a message stanza for the given language if available.
       * If the requested language is not available, the default subject (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default subject
       * will be returned, if any.
       * @return The subject of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string subject( const std::string& lang = "default" ) const;

      /**
       * Returns the text of a error stanza for the given language if available.
       * If the requested language is not available, the default text (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default subject
       * will be returned, if any.
       * @return The text of an error stanza. Empty for non-error stanzas.
       */
      virtual const std::string errorText( const std::string& lang = "default" ) const;

      /**
       * Returnes the stanza error condition, if any.
       * @return The stanza error condition.
       */
      virtual StanzaError error() const { return m_stanzaError; };

      /**
       * This function can be used to retrieve the application-specific error condition of a stanza error.
       * @return The application-specific error element of a stanza error. 0 if no respective element was
       * found or no error occured.
       */
      Tag* errorAppCondition() { return m_stanzaErrorAppCondition; };

      /**
       * Returns the thread ID of a message stanza.
       * @return The thread ID of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string thread() const { return m_thread; };

      /**
       * Retrieves the value of the xml:lang attribute of this stanza.
       * Default is 'en'.
       */
      const std::string& xmlLang() const { return m_xmllang; };

      /**
       * Use this function to parse the content of the Tag and determine type, etc.
       * of the Stanza. This feels kind of hackish...
       * You only need to call this if you are constructing a bare Stanza from scratch.
       * Stanzas provided by gloox are fully parsed.
       * @deprecated
       */
      void finalize() { init(); };

      /**
       * This function creates a deep copy of this Stanza.
       * @return An independent copy of the stanza.
       * @since 0.7
       */
      virtual Stanza* clone();

    protected:
      void init();

      StanzaSubType m_subtype;
      PresenceStatus m_show;
      StanzaError m_stanzaError;
      StanzaErrorType m_stanzaErrorType;
      Tag *m_stanzaErrorAppCondition;
      StringMap m_errorText;
      StringMap m_body;
      StringMap m_subject;
      StringMap m_status;
      JID m_from;
      JID m_to;
      std::string m_xmlns;
      std::string m_id;
      std::string m_thread;
      std::string m_xmllang;
      int m_priority;
  };

};

#endif // STANZA_H__

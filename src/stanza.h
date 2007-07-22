/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef STANZA_H__
#define STANZA_H__

#include "gloox.h"
#include "tag.h"
#include "jid.h"

namespace gloox
{

  class StanzaExtension;

  /**
   * A list of StanzaExtensions.
   */
  typedef std::list<StanzaExtension*> StanzaExtensionList;

  /**
   * @brief This is a base class for XMPP stanza abstractions.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class GLOOX_API Stanza : public Tag
  {
    public:
      /**
       * Virtual destructor.
       */
      virtual ~Stanza();

      /**
       * Returns the JID the stanza comes from.
       * @return The origin of the stanza.
       */
      virtual const JID& from() const { return m_from; }

      /**
       * Returns the receiver of the stanza.
       * @return The stanza's destination.
       */
      virtual const JID& to() const { return m_to; }

      /**
       * Returns the id of the stanza, if set.
       * @return The ID of the stanza.
       */
      virtual const std::string& id() const { return m_id; }

      /**
       * Returns the text of a error stanza for the given language if available.
       * If the requested language is not available, the default text (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default subject
       * will be returned, if any.
       * @return The text of an error stanza. Empty for non-error stanzas.
       */
      virtual const std::string errorText( const std::string& lang = "default" ) const
        { return findLang( m_errorText, lang ); }

      /**
       * Returns the stanza error condition, if any.
       * @return The stanza error condition.
       */
      virtual StanzaError error() const { return m_stanzaError; }

      /**
       * This function can be used to retrieve the application-specific error condition of a stanza error.
       * @return The application-specific error element of a stanza error. 0 if no respective element was
       * found or no error occured.
       */
      Tag* errorAppCondition() { return m_stanzaErrorAppCondition; }

      /**
       * Retrieves the value of the xml:lang attribute of this stanza.
       * Default is 'en'.
       */
      const std::string& xmlLang() const { return m_xmllang; }

      /**
       * Use this function to add a StanzaExtension to this Stanza.
       * @param se The StanzaExtension to add.
       * @note The Stanza will become the owner of the StanzaExtension and will delete it
       * after using it.
       * @since 0.9
       */
      void addExtension( StanzaExtension *se );

      /**
       * Returns the list of the Stanza's extensions.
       * @return The list of the Stanza's extensions.
       */
      const StanzaExtensionList& extensions() const { return m_extensionList; }

    protected:
      /**
       * Creates a new Stanza from a deep copy of the given Tag.
       * @param tag The Tag to create the Stanza from.
       * @param rip Whether to rip off the original Tag.
       * @since 1.0
       */
      Stanza( Tag *tag, bool rip = false );

      /**
       * Creates a new Stanza with given name.
       * @param name The name of the root tag.
       * @since 1.0
       */
      Stanza( const std::string& name, const JID& to, const JID& from );

      StanzaExtensionList m_extensionList;
      StanzaError m_stanzaError;
      StanzaErrorType m_stanzaErrorType;
      Tag *m_stanzaErrorAppCondition;
      StringMap m_errorText;
      std::string m_id;
      std::string m_xmllang;
      JID m_from;
      JID m_to;

      static const std::string findLang( const StringMap& map, const std::string& lang );
      static void setLang( StringMap& map, const Tag *tag );
  };

}

#endif // STANZA_H__

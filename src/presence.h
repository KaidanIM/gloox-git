/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PRESENCE_H__
#define PRESENCE_H__

#include "stanza.h"

#include <string>

namespace gloox
{

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class Presence : public Stanza
  {

    public:

      /**
       * Describes the different valid presence types.
       */
      enum PresenceType
      {
        PresenceInvalid,
        PresenceAvailable,
        PresenceChat,
        PresenceAway,
        PresenceDnd,
        PresenceXa,
        PresenceUnavailable,
        PresenceProbe,
        PresenceError
      };

      /**
       * Creates a Presence request from the given Tag.
       * @param tag The Tag to parse.
       */
      Presence( Tag *tag, bool rip = false );

      /**
       * Creates a Presence request.
       */
      Presence( PresenceType type, const std::string& to, const std::string& status = "",
                int priority = 0, const std::string& xmllang = "", const std::string& from = "" );
      /**
       * Destructor.
       */
      virtual ~Presence();

      /**
       *
       */
      PresenceType subtype() const { return m_subtype; }

      /**
       *
       */
#warning FIXME return something useful (only 'show' values?) or kill this func
      PresenceType presence() const { return m_subtype; }

      /**
       * Returns the status text of a presence stanza for the given language if available.
       * If the requested language is not available, the default status text (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default body
       * will be returned, if any.
       * @return The status text set by the sender.
       */
      const std::string status( const std::string& lang = "default" ) const
        { return findLang( m_status, lang ); }

      /**
         * Returns the remote entity resource's presence priority if the stanza is a presence stanza.
         * If the stanza is not a presence stanza or if no priority information was included, a value
         * below -128 is returned, which is an illegal value for the priority. Legal range is between
         * -128 and +127.
         * @return The priority information contained in the stanza, if any, or a value below -128.
       */
      int priority() const { return m_priority; }

    private:
      PresenceType m_subtype;
      StringMap m_status;
      int m_priority;

  };

}

#endif // PRESENCE_H__

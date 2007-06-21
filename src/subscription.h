/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef SUBSCRIPTION_H__
#define SUBSCRIPTION_H__

#include "stanza.h"

#include <string>

namespace gloox
{

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class Subscription : public Stanza
  {

    public:

      /**
       * Describes the different valid message types.
       */
      enum S10nType
      {
        S10nInvalid,
        S10nSubscribe,
        S10nSubscribed,
        S10nUnsubscribe,
        S10nUnsubscribed
      };

      /**
       * Creates a Subscription request from the given Tag.
       * @param tag The Tag to parse.
       */
      Subscription( Tag *tag );

      /**
       * Creates a Subscription request.
       */
      Subscription( S10nType type, const std::string& to, const std::string& status,
                     const std::string& xmllang = "", const std::string& from = "" );
      /**
       * Destructor.
       */
      virtual ~Subscription();

      /**
       *
       */
      S10nType subtype() const { return m_subtype; }

      /**
       * Returns the status text of a presence stanza for the given language if available.
       * If the requested language is not available, the default status text (without a xml:lang
       * attribute) will be returned.
       * @param lang The language identifier for the desired language. It must conform to
       * section 2.12 of the XML specification and RFC 3066. If empty, the default body
       * will be returned, if any.
       * @return The status text set by the sender.
       */
      virtual const std::string status( const std::string& lang = "default" ) const
        { return findLang( m_status, lang ); }

    private:
      S10nType m_subtype;
      StringMap m_status;

  };

}

#endif // SUBSCRIPTION_H__

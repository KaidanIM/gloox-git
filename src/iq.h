/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef IQ_H__
#define IQ_H__

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
  class IQ : public Stanza
  {

    public:

      /**
       *
       */
      enum IqType
      {
        Get = 0,              /**< The stanza is a request for information or requirements. */
        Set,                  /**< The stanza provides required data, sets new values, or
                                     * replaces existing values. */
        Result,               /**< The stanza is a response to a successful get or set request. */
        Error,                /**< An error has occurred regarding processing or delivery of a
                                     * previously-sent get or set (see Stanza Errors (Section 9.3)). */
        IqTypeInvalid               /**< The stanza is invalid */
      };

      /**
       * Creates an IQ from a tag.
       * @param tag The Tag to parse.
       * @param rip Whether to rip off the original Tag.
       */
      IQ( Tag *tag, bool rip = false );

      /**
       * Creates an IQ Query.
       */
      IQ ( IqType type, const JID& to, const std::string& id, const std::string& xmlns = "",
           const std::string& childtag = "query", const JID& from = JID() );

      /**
       * Virtual destructor.
       */
      virtual ~IQ();

      /**
       *
       */
      Tag *query() const { return m_query; }

      /**
       *
       */
      IqType subtype() const { return m_subtype; }

      /**
       * Returns the value of the xmlns attribute of the first child node.
       * @return The namespace of the IQ stanza.
       */
      virtual const std::string& xmlns() const { return m_xmlns; }

    private:
      Tag *m_query;
      std::string m_xmlns;
      IqType m_subtype;
  };

}

#endif // IQ_H__

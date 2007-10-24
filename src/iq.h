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
   * @brief An abstraction of an IQ stanza.
   *
   * @author Vincent Thomasset
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class IQ : public Stanza
  {

    friend class ClientBase;

    public:

      /**
       * Describes the different valid IQ types.
       */
      enum IqType
      {
        Get = 0,                    /**< The stanza is a request for information or requirements. */
        Set,                        /**< The stanza provides required data, sets new values, or
                                     * replaces existing values. */
        Result,                     /**< The stanza is a response to a successful get or set request. */
        Error,                      /**< An error has occurred regarding processing or delivery of a
                                     * previously-sent get or set (see Stanza Errors (Section 9.3)). */
        Invalid                     /**< The stanza is invalid */
      };

      /**
       * Creates an IQ Query.
       * @param type The desired IqType.
       * @param to The intended receiver.
       * @param id The request's ID. Usually obtained from ClientBase::getID()
       * @param xmlns The child tag's xmlns. Optional. If omitted, no child tag is added.
       * @param childtag The child tag's name. Defaults to "query".
       * @param from An optional sender address. Usually not needed. Cannot be forged.
       */
      IQ ( IqType type, const JID& to, const std::string& id, const std::string& xmlns = EmptyString,
           const std::string& childtag = "query", const JID& from = JID() );

      /**
       * Virtual destructor.
       */
      virtual ~IQ();

      /**
       * Returns the IQ's child tag (Only one is allowed, unless the IQ is of type 'error').
       * @return The IQ's child tag.
       */
      Tag* query() const { return m_query; }

      /**
       * Returns the IQ's type.
       * @return The IQ's type.
       */
      IqType subtype() const { return m_subtype; }

      /**
       * Returns the value of the xmlns attribute of the first child node.
       * @return The namespace of the IQ stanza.
       */
      const std::string& xmlns() const { return m_xmlns; }

      // reimplemented from Stanza
      virtual Tag* tag() const;

    private:
      /**
       * Creates an IQ from a tag. The original Tag will be ripped off.
       * @param tag The Tag to parse.
       * @param rip Whether to rip off the original Tag.
       */
      IQ( Tag* tag );

      Tag* m_query;
      std::string m_xmlns;
      IqType m_subtype;
  };

}

#endif // IQ_H__

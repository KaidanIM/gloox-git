/*
   Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This file is distributed under a license. The exact licensing terms
  can be found in the file LICENSE in the distribution.
*/


#ifndef STANZA_H__
#define STANZA_H__

#include "gloox.h"
#include "tag.h"
#include "jid.h"

namespace gloox
{

  /**
   * This is an abstraction of a XMPP stanza.
   * You can create a new Stanza from an existing Tag (or another stanza).
   * Or use the derived classes (Iq, Message, Presence, Subscription) or a Tag directly.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class Stanza : public Tag
  {
    public:

      /**
       * Creates a new Stanza from the given Tag.
       * @param tag The Tag to create the stanza from.
       */
      Stanza( const Tag& tag );

      /**
       * Virtual destructor.
       */
      virtual ~Stanza() {};

      /**
       * Returns the stanza type.
       * @return The type of the stanza.
       */
      virtual StanzaType type() const { return m_type; };

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

      virtual std::string id() const { return m_id; };

    private:
      StanzaType m_type;
      StanzaSubType m_subtype;
      JID m_from;
      JID m_to;
      std::string m_id;
  };

};

#endif // STANZA_H__

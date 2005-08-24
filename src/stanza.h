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
      Stanza( Tag *tag );

      /**
       * Creates an ew Stanza with given name and optional CData.
       * @param name The name of the root tag.
       * @param cdata Initial XML caharcter data for the tag.
       */
      Stanza( const std::string& name, const std::string& cdata = "" );

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
       * Returns the status text of a presence stanza.
       * @return The status text set by the sender.
       */
      virtual const std::string status() const { return m_status; };

      /**
       * Returns the message body of a message stanza.
       * @return The message body of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string message() const { return m_message; };

      /**
       * Returns the subject of a message stanza.
       * @return The subject of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string subject() const { return m_subject; };

      /**
       * Returns the thread ID of a message stanza.
       * @return The thread ID of a message stanza. Empty for non-message stanzas.
       */
      virtual const std::string thread() const { return m_thread; };

      /**
       * Use this function to parse the content of the Tag and determine type, etc.
       * of the Stanza. This feels kind of hackish...
       * You only need to call this if you are constructing a bare Stanza from scratch.
       * Stanzas provided by gloox are fully parsed.
       */
      void finalize() { init(); };

    private:
      void init();

      StanzaSubType m_subtype;
      PresenceStatus m_show;
      JID m_from;
      JID m_to;
      std::string m_xmlns;
      std::string m_id;
      std::string m_status;
      std::string m_message;
      std::string m_subject;
      std::string m_thread;
      int m_priority;
  };

};

#endif // STANZA_H__

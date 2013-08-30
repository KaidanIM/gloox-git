/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef FORWARD_H__
#define FORWARD_H__

#include "delayeddelivery.h"
#include "stanza.h"

#include <string>

namespace gloox
{
  /**
   * @brief This is an implementation of Stanza Forwarding (@xep{0297}) as a StanzaExtension.
   *
   * XEP-Version: 0.5
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @author Fernando Sanchez
   * @since 1.0.5
   */
  class GLOOX_API Forward : public StanzaExtension
  {
    public:

      /**
       * Creates a forwarding Stanza, embedding the given Stanza and DelayedDelivery objects.
       * @param stanza The forwarded Stanza.
       * @param delay The date/time the forwarded stanza was received at by the forwarder.
       */
      Forward( Stanza* stanza, DelayedDelivery* delay );
      
      /**
       * Creates a forwarding Stanza from the given Tag. The original Tag will be ripped off.
       * @param tag The Tag to parse.
       */
      Forward( const Tag* tag );

      /**
       * Virtual destructor.
       */
      virtual ~Forward();

      /**
       * This function returns a pointer to a DelayedDelivery StanzaExtension which indicates
       * when the forwarder originally received the forwarded stanza.
       * 
       * @return A pointer to a DelayedDelivery object.
       */
      const DelayedDelivery* when() const { return m_delay; }

      // reimplemented from Stanza
      virtual Stanza* embeddedStanza() const { return m_stanza; }
      
      // reimplemented from Stanza
      virtual Tag* embeddedTag() const { return m_tag; }
      
      // reimplemented from Stanza
      virtual Tag* tag() const;

      /**
       * Returns an XPath expression that describes a path to child elements of a
       * stanza that an extension handles.
       *
       * @return The extension's filter string.
       */
      const std::string& filterString() const;
      
      StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new Forward( tag );
      }

      StanzaExtension* clone() const
      {
        return new Forward( this->m_stanza, this->m_delay );
      }

    private:
      Stanza* m_stanza;
      Tag* m_tag;
      DelayedDelivery* m_delay;

  };

}

#endif // FORWARD_H__

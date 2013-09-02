/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef CARBONS_H__
#define CARBONS_H__

#include "macros.h"

#include "stanzaextension.h"
#include "tag.h"

namespace gloox
{

  class Forward;

  class GLOOX_API Carbons : public StanzaExtension
  {
    public:
      /**
       *
       */
      enum Type
      {
        Received,                   /**< Indicates that the message received has been sent by a third party. */
        Sent,                       /**< Indicates that the message received has been sent by one of the user's own resources. */
        Enable,                     /**< Indicates that the sender wishes to enable carbon copies. */
        Disable,                    /**< Indicates that the sender wishes to disable carbon copies. */
        Private,                    /**< Indicates that the sender does not want carbon copies to be sent for this message. */
        Invalid                     /**< Invalid type. */
      };

      /**
       *
       */
      Carbons( Type type );

      /**
       *
       */
      Carbons( const Tag* tag );

      /**
       * Virtual destructor.
       */
      virtual ~Carbons();

      /**
       *
       */
      Type type() const { return m_type; }

      // reimplemented from StanzaExtension
      virtual Stanza* embeddedStanza() const;

      // reimplemented from StanzaExtension
      virtual Tag* embeddedTag() const;

      // reimplemented from StanzaExtension
      virtual const std::string& filterString() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new Carbons( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* clone() const;

    private:
      Forward* m_forward;
      Type m_type;

  };

}

#endif // CARBONS_H__

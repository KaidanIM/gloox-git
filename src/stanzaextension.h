/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef STANZAEXTENSION_H__
#define STANZAEXTENSION_H__

#include "macros.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * Supported Stanza extension types.
   */
  enum StanzaExtensionType
  {
    ExtNone                = 1<< 1, /**< Invalid StanzaExtension. */
    ExtVCardUpdate         = 1<< 2, /**< Extension in the vcard-temp:x:update namespace, advertising
                                     * a user avatar's SHA1 hash (XEP-0153). */
    ExtOOB                 = 1<< 3, /**< An extension in the jabber:iq:oob or jabber:x:oob namespaces
                                     * (XEP-0066). */
    ExtGPGSigned           = 1<< 4, /**< An extension containing a GPG/PGP signature (XEP-0027). */
    ExtGPGEncrypted        = 1<< 5, /**< An extension containing a GPG/PGP encrypted message (XEP-0027). */
    ExtReceipt             = 1<< 6, /**< An extension containing a Message Receipt/Request (XEP-0184). */
    ExtDelay               = 1<< 7, /**< An extension containing notice of delayed delivery (XEP-0203
                                     * & XEP-0091). */
    ExtAMP                 = 1<< 8, /**< An extension containing advanced message processing rules
                                     * (XEP-0079). */
    ExtError               = 1<< 9, /**< An extension containing an error. */
    ExtCaps                = 1<<10, /**< An extension containing Entity Capabilities (XEP-0115). */
    ExtChatState           = 1<<11, /**< An extension containing a chat state (XEP-0085). */
    ExtMessageEvent        = 1<<12, /**< An extension containing a message event (XEP-0022). */
    ExtDataForm            = 1<<13, /**< An extension containing a Data Form (XEP-0004). */
    ExtNickname            = 1<<14, /**< An extension containing a User Nickname (XEP-0172). */
    ExtResourceBind        = 1<<15, /**< A resource bind SE (RFC3921). */
    ExtVersion             = 1<<16, /**< An extension containing a Version request/reply (XEP-0092). */
    ExtUser                = 1<<17  /**< User-supplied extensions must use IDs above this. Do not
                                      * hard-code ExtUser's value anywhere, it is subject to change. */
  };

  /**
   * @brief This class abstracts a stanza extension, which is usually (but not necessarily) an 'x'
   * element in a specific namespace.
   *
   * You should not need to use this class directly.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API StanzaExtension
  {
    public:
      /**
       * Constructs an empty StanzaExtension.
       * @param type Designates the extension's type. It should be one of StanzaExtensionType
       * for built-in extensions, and it should be higher than ExtUser for custom types.
       */
      StanzaExtension( int type ) : m_extensionType( type ), m_valid( false ) {}

      /**
       * Virtual destructor.
       */
      virtual ~StanzaExtension() {}

      /**
       * Returns an XPath expression that describes a path to child elements of a
       * stanza that an extension handles.
       *
       * @return The extension's type.
       */
      virtual const std::string filterString() const = 0;

      /**
       * Returns a new Instance of the derived type. Usually, for a derived class FooExtension,
       * the implementation of this function looks like:
       * @code
       * StanzaExtension* FooExtension::newInstance( const Tag* tag ) const
       * {
       *   return new FooExtension( tag );
       * }
       * @endcode
       * @return The derived extension's new instance.
       */
      virtual StanzaExtension* newInstance( const Tag* tag ) const = 0;

      /**
       * Returns a Tag representation of the extension.
       * @return A Tag representation of the extension.
       */
      virtual Tag* tag() const = 0;

      /**
       * Returns the extension's type.
       * @return The extension's type.
       */
      int extensionType() const { return m_extensionType; }

    protected:
      bool m_valid;

    private:
      int m_extensionType;

  };

}

#endif // STANZAEXTENSION_H__

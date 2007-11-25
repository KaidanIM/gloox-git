/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef XHTMLIM_H__
#define XHTMLIM_H__

#include "jid.h"
#include "stanzaextension.h"
#include "tag.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is a wrapper of an XHTML-IM (XEP-0071) message body.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class XHtmlIM : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object with the given resource string.
       * @param xhtml XHTML to include.
       */
      XHtmlIM( const Tag* xhtml );

      /**
       * Virtual Destructor.
       */
      virtual ~XHtmlIM();

      // reimplemented from StanzaExtension
      virtual const std::string& filterString() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new XHtmlIM( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

    private:
      Tag* m_xhtml;

  };

}

#endif // XHTMLIM_H__

/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "receipt.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  /* chat state type values */
  static const char* receiptValues [] = {
    "request",
    "received"
  };

  static inline ReceiptType receiptType( const std::string& type )
  {
    return (ReceiptType)util::lookup2( type, receiptValues );
  }

  Receipt::Receipt( const Tag* tag )
    : StanzaExtension( ExtReceipt ),
      m_rcpt( receiptType( tag->name() ) )
  {
  }

  const std::string& Receipt::filterString() const
  {
    static const std::string filter =
           "/message/request[@xmlns='" + XMLNS_RECEIPTS + "']"
           "|/message/received[@xmlns='" + XMLNS_RECEIPTS + "']";
    return filter;
  }

  Tag* Receipt::tag() const
  {
    if( m_rcpt == ReceiptInvalid )
      return 0;

    return new Tag( util::lookup2( m_rcpt, receiptValues ), XMLNS, XMLNS_RECEIPTS );
  }

}

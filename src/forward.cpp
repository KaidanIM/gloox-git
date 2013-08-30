/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "util.h"
#include "forward.h"

namespace gloox
{

  Forward::Forward( Stanza *stanza, DelayedDelivery *delay )
    : StanzaExtension( ExtForward ),
      m_stanza( stanza ), m_tag( 0 ), m_delay( delay )
  {
    
  }
  
  Forward::Forward( const Tag* tag )
    : StanzaExtension( ExtForward ), 
      m_stanza( 0 ), m_tag( 0 ), m_delay( 0 )
  {
    
  }

  Forward::~Forward()
  {
    delete m_delay;
    delete m_stanza;
    delete m_tag;
  }

  const std::string& Forward::filterString() const
  {
    static const std::string filter = "/message/forward[@xmlns='" + XMLNS_STANZA_FORWARDING + "']"
                                      "|/iq/forward[@xmlns='" + XMLNS_STANZA_FORWARDING + "']"
                                      "|/presence/forward[@xmlns='" + XMLNS_STANZA_FORWARDING + "']";
    return filter;
  }
  
  Tag* Forward::tag() const
  {
    Tag* f = new Tag( "forwarded" );
    f->setXmlns( XMLNS_STANZA_FORWARDING );
    if( m_delay )
      f->addChild( m_delay->tag() );
    if( m_stanza )
      f->addChild( m_stanza->tag() );

    return f;
  }

}

/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "seresourcebind.h"

#include "tag.h"

namespace gloox
{

  SEResourceBind::SEResourceBind( const std::string& resource )
    : StanzaExtension( ExtResourceBind ), m_resource( resource ), m_jid( JID() )
  {
  }

  SEResourceBind::SEResourceBind( const Tag* tag )
    : StanzaExtension( ExtResourceBind ), m_resource( EmptyString )
  {
    if( tag && tag->hasChild( "jid" ))
      m_jid.setJID( tag->findChild( "jid" )->cdata() );
  }

  SEResourceBind::~SEResourceBind()
  {
  }

  const std::string& SEResourceBind::filterString() const
  {
    static const std::string filter = "/iq/bind[@xmlns='" + XMLNS_STREAM_BIND + "']";
    return filter;
  }

  Tag* SEResourceBind::tag() const
  {
    if( m_resource.empty() )
    {
      if( !m_jid)
        return 0;

      Tag* t = new Tag( "bind" );
      t->setXmlns( XMLNS_STREAM_BIND );
      new Tag( t, "jid", m_jid.full() );
      return t;
    }

    Tag* t = new Tag( "bind", m_resource );
    t->setXmlns( XMLNS_STREAM_BIND );
    return t;
  }

}

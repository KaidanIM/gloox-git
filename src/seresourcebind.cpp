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

#include "prep.h"
#include "tag.h"

namespace gloox
{

  SEResourceBind::SEResourceBind( const std::string& resource, bool bind )
    : StanzaExtension( ExtResourceBind ), m_jid( JID() ), m_bind( bind )
  {
    prep::resourceprep( resource, m_resource );
    m_valid = true;
  }

  SEResourceBind::SEResourceBind( const Tag* tag )
    : StanzaExtension( ExtResourceBind ), m_resource( EmptyString ), m_bind( true )
  {
    if( !tag )
      return;

    if( tag->name() == "unbind" )
      m_bind = false;
    else if( tag->name() == "bind" )
      m_bind = true;
    else
      return;

    if( tag->hasChild( "jid" ) )
      m_jid.setJID( tag->findChild( "jid" )->cdata() );
    else if( tag->hasChild( "resource" ) )
      m_resource = tag->findChild( "resource" )->cdata();

    m_valid = true;
  }

  SEResourceBind::~SEResourceBind()
  {
  }

  const std::string& SEResourceBind::filterString() const
  {
    static const std::string filter = "/iq/bind[@xmlns='" + XMLNS_STREAM_BIND + "']"
                                      "|/iq/unbind[@xmlns='" + XMLNS_STREAM_BIND + "']";
    return filter;
  }

  Tag* SEResourceBind::tag() const
  {
    if( !m_valid )
      return 0;

    Tag* t = new Tag( m_bind ? "bind" : "unbind" );
    t->setXmlns( XMLNS_STREAM_BIND );

    if( m_bind && m_resource.empty() && m_jid )
      new Tag( t, "jid", m_jid.full() );
    else
      new Tag( t, "resource", m_resource );

    return t;
  }

}

/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "vcard.h"
#include "tag.h"

namespace gloox
{

  VCard::VCard()
    : m_N( false )
  {
  }

  VCard::VCard( Tag *vcard )
    : m_N( false )
  {
    if( vcard->hasChild( "FN" ) )
      m_formattedName = vcard->findChild( "FN" )->cdata();

    Tag *n = vcard->findChild( "N" );
    if( n )
    {
      m_N = true;
      if( n->hasChild( "FAMILY" ) )
        m_family = n->findChild( "FAMILY" )->cdata();
      if( n->hasChild( "GIVEN" ) )
        m_family = n->findChild( "GIVEN" )->cdata();
      if( n->hasChild( "MIDDLE" ) )
        m_family = n->findChild( "MIDDLE" )->cdata();
      if( n->hasChild( "PREFIX" ) )
        m_family = n->findChild( "PREFIX" )->cdata();
      if( n->hasChild( "SUFFIX" ) )
        m_family = n->findChild( "SUFFIX" )->cdata();
    }

    if( vcard->hasChild( "NICKNAME" ) )
      m_nickname = vcard->findChild( "NICKNAME" )->cdata();

    if( vcard->hasChild( "URL" ) )
      m_url = vcard->findChild( "URL" )->cdata();

    if( vcard->hasChild( "BDAY" ) )
      m_bday = vcard->findChild( "BDAY" )->cdata();

    if( vcard->hasChild( "JABBERID" ) )
      m_jabberid = vcard->findChild( "JABBERID" )->cdata();

    if( vcard->hasChild( "TITLE" ) )
      m_title = vcard->findChild( "TITLE" )->cdata();

    if( vcard->hasChild( "ROLE" ) )
      m_role = vcard->findChild( "ROLE" )->cdata();

    if( vcard->hasChild( "NOTE" ) )
      m_note = vcard->findChild( "NOTE" )->cdata();

    if( vcard->hasChild( "DESC" ) )
      m_desc = vcard->findChild( "DESC" )->cdata();

  }
  VCard::~VCard()
  {
  }

  void VCard::setName( const std::string& family, const std::string& given, const std::string& middle,
                  const std::string& prefix, const std::string& suffix )
  {
    m_family = family;
    m_given = given;
    m_middle = middle;
    m_prefix = prefix;
    m_suffix = suffix;
    m_N = true;
  }

  Tag* VCard::tag() const
  {
    Tag *v = new Tag( "vcard" );
    v->addAttribute( "xmlns", XMLNS_VCARD_TEMP );
    v->addAttribute( "version", "3.0" );
    if( !m_formattedName.empty() )
      new Tag( v, "FN", m_formattedName );
    if( m_N )
    {
      Tag *n = new Tag( v, "N" );
      if( !m_family.empty() )
        new Tag( n, "FAMILY", m_family );
      if( !m_given.empty() )
        new Tag( n, "GIVEN", m_given );
      if( !m_middle.empty() )
        new Tag( n, "MIDDLE", m_middle );
      if( !m_prefix.empty() )
        new Tag( n, "PREFIX", m_prefix );
      if( !m_suffix.empty() )
        new Tag( n, "SUFFIX", m_suffix );
    }

    if( !m_nickname.empty() )
      new Tag( v, "NICKNAME", m_nickname );

    if( !m_url.empty() )
      new Tag( v, "URL", m_url );

    if( !m_bday.empty() )
      new Tag( v, "BDAY", m_bday );

    if( !m_jabberid.empty() )
      new Tag( v, "JABBERID", m_jabberid );

    if( !m_title.empty() )
      new Tag( v, "TITLE", m_title );

    if( !m_role.empty() )
      new Tag( v, "ROLE", m_role );

    if( !m_note.empty() )
      new Tag( v, "NOTE", m_note );

    if( !m_desc.empty() )
      new Tag( v, "DESC", m_desc );

    return v;
  }

}

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
#include "base64.h"

namespace gloox
{

  VCard::VCard()
    : m_N( false ), m_PHOTO( false ), m_LOGO( false )
  {
  }

  VCard::VCard( Tag *vcard )
    : m_N( false ), m_PHOTO( false ), m_LOGO( false )
  {
    checkField( vcard, "FN", m_formattedname );
    checkField( vcard, "NICKNAME", m_nickname );
    checkField( vcard, "URL", m_url );
    checkField( vcard, "BDAY", m_bday );
    checkField( vcard, "JABBERID", m_jabberid );
    checkField( vcard, "TITLE", m_title );
    checkField( vcard, "ROLE", m_role );
    checkField( vcard, "NOTE", m_note );
    checkField( vcard, "DESC", m_desc );
    checkField( vcard, "MAILER", m_mailer );
    checkField( vcard, "TZ", m_tz );
    checkField( vcard, "PRODID", m_prodid );
    checkField( vcard, "REV", m_rev );
    checkField( vcard, "SORT-STRING", m_sortstring );
    checkField( vcard, "UID", m_uid );

    Tag *t = vcard->findChild( "N" );
    if( t )
    {
      m_N = true;
      if( t->hasChild( "FAMILY" ) )
        m_family = t->findChild( "FAMILY" )->cdata();
      if( t->hasChild( "GIVEN" ) )
        m_family = t->findChild( "GIVEN" )->cdata();
      if( t->hasChild( "MIDDLE" ) )
        m_family = t->findChild( "MIDDLE" )->cdata();
      if( t->hasChild( "PREFIX" ) )
        m_family = t->findChild( "PREFIX" )->cdata();
      if( t->hasChild( "SUFFIX" ) )
        m_family = t->findChild( "SUFFIX" )->cdata();
    }

    t = vcard->findChild( "PHOTO" );
    if( t )
    {
      if( t->hasChild( "EXTVAL" ) )
      {
        m_photoext = t->findChild( "EXTVAL" )->cdata();
        m_PHOTO = true;
      }
      else if( t->hasChild( "TYPE" ) && t->hasChild( "BINVAL" ) )
      {
        m_phototype = t->findChild( "TYPE" )->cdata();
        m_photobin = Base64::decode64( t->findChild( "BINVAL" )->cdata() );
        m_PHOTO = true;
      }
    }

    t = vcard->findChild( "LOGO" );
    if( t )
    {
      if( t->hasChild( "EXTVAL" ) )
      {
        m_logoext = t->findChild( "EXTVAL" )->cdata();
        m_LOGO = true;
      }
      else if( t->hasChild( "TYPE" ) && t->hasChild( "BINVAL" ) )
      {
        m_logotype = t->findChild( "TYPE" )->cdata();
        m_logobin = Base64::decode64( t->findChild( "BINVAL" )->cdata() );
        m_LOGO = true;
      }
    }
  }

  VCard::~VCard()
  {
  }

  void VCard::checkField( Tag *vcard, const std::string& field, std::string& var )
  {
    if( vcard->hasChild( field ) )
      var = vcard->findChild( field )->cdata();
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

  void VCard::setPhoto( const std::string& extval )
  {
    if( !extval.empty() )
    {
      m_photoext = extval;
      m_PHOTO = true;
    }
  }

  void VCard::setPhoto( const std::string& type, const std::string& binval )
  {
    if( !type.empty() && !binval.empty() )
    {
      m_phototype = type;
      m_photobin = Base64::encode64( binval );
      m_PHOTO = true;
    }
  }

  void VCard::setLogo( const std::string& extval )
  {
    if( !extval.empty() )
    {
      m_logoext = extval;
      m_LOGO = true;
    }
  }

  void VCard::setLogo( const std::string& type, const std::string& binval )
  {
    if( !type.empty() && !binval.empty() )
    {
      m_logotype = type;
      m_logobin = Base64::encode64( binval );
      m_LOGO = true;
    }
  }

  Tag* VCard::tag() const
  {
    Tag *v = new Tag( "vcard" );
    v->addAttribute( "xmlns", XMLNS_VCARD_TEMP );
    v->addAttribute( "version", "3.0" );

    insertField( v, "FN", m_formattedname );
    insertField( v, "NICKNAME", m_nickname );
    insertField( v, "URL", m_url );
    insertField( v, "BDAY", m_bday );
    insertField( v, "JABBERID", m_jabberid );
    insertField( v, "TITLE", m_title );
    insertField( v, "ROLE", m_role );
    insertField( v, "NOTE", m_note );
    insertField( v, "DESC", m_desc );
    insertField( v, "MAILER", m_mailer );
    insertField( v, "TZ", m_tz );
    insertField( v, "REV", m_rev );
    insertField( v, "SORT_STRING", m_sortstring );
    insertField( v, "UID", m_uid );

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

    if( m_PHOTO )
    {
      Tag *p = new Tag( v, "PHOTO" );
      if( !m_photoext.empty() )
      {
        new Tag( p, "EXTVAL", m_photoext );
      }
      else if( !m_phototype.empty() && !m_photobin.empty() )
      {
        new Tag( p, "TYPE", m_phototype );
        new Tag( p, "BINVAL", m_photobin );
      }
    }

    if( m_LOGO )
    {
      Tag *l = new Tag( v, "LOGO" );
      if( !m_logoext.empty() )
      {
        new Tag( l, "EXTVAL", m_logoext );
      }
      else if( !m_logotype.empty() && !m_logobin.empty() )
      {
        new Tag( l, "TYPE", m_logotype );
        new Tag( l, "BINVAL", m_logobin );
      }
    }

    return v;
  }

  void VCard::insertField( Tag *vcard, const std::string& field, const std::string& var ) const
  {
    if( !var.empty() )
      new Tag( vcard, field, var );
  }

}

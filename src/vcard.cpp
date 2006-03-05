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

    Tag::TagList::const_iterator it = vcard->children().begin();
    for( ; it != vcard->children().end(); ++it )
    {
      if( (*it)->name() == "N" )
      {
        m_N = true;
        if( (*it)->hasChild( "FAMILY" ) )
          m_family = (*it)->findChild( "FAMILY" )->cdata();
        if( (*it)->hasChild( "GIVEN" ) )
          m_family = (*it)->findChild( "GIVEN" )->cdata();
        if( (*it)->hasChild( "MIDDLE" ) )
          m_family = (*it)->findChild( "MIDDLE" )->cdata();
        if( (*it)->hasChild( "PREFIX" ) )
          m_family = (*it)->findChild( "PREFIX" )->cdata();
        if( (*it)->hasChild( "SUFFIX" ) )
          m_family = (*it)->findChild( "SUFFIX" )->cdata();
      }

      if( (*it)->name() == "PHOTO" )
      {
        if( (*it)->hasChild( "EXTVAL" ) )
        {
          m_photoext = (*it)->findChild( "EXTVAL" )->cdata();
          m_PHOTO = true;
        }
        else if( (*it)->hasChild( "TYPE" ) && (*it)->hasChild( "BINVAL" ) )
        {
          m_phototype = (*it)->findChild( "TYPE" )->cdata();
          m_photobin = Base64::decode64( (*it)->findChild( "BINVAL" )->cdata() );
          m_PHOTO = true;
        }
      }

      if( (*it)->name() == "LOGO" )
      {
        if( (*it)->hasChild( "EXTVAL" ) )
        {
          m_logoext = (*it)->findChild( "EXTVAL" )->cdata();
          m_LOGO = true;
        }
        else if( (*it)->hasChild( "TYPE" ) && (*it)->hasChild( "BINVAL" ) )
        {
          m_logotype = (*it)->findChild( "TYPE" )->cdata();
          m_logobin = Base64::decode64( (*it)->findChild( "BINVAL" )->cdata() );
          m_LOGO = true;
        }
      }

      if( (*it)->name() == "EMAIL" && (*it)->hasChild( "USERID" ) )
      {
        Email item;
        item.userid = (*it)->findChild( "USERID" )->cdata();
        item.internet = ( (*it)->hasChild( "INTERNET" ) )?( true ):( false );
        item.x400 = ( (*it)->hasChild( "X400" ) )?( true ):( false );
        item.work = ( (*it)->hasChild( "WORK" ) )?( true ):( false );
        item.home = ( (*it)->hasChild( "HOME" ) )?( true ):( false );
        item.pref = ( (*it)->hasChild( "PREF" ) )?( true ):( false );
        m_emailList.push_back( item );
      }

      if( (*it)->name() == "ADR" )
      {
        Address item;
        checkField( (*it), "POBOX", item.pobox );
        checkField( (*it), "EXTADD", item.extadd );
        checkField( (*it), "STREET", item.street );
        checkField( (*it), "LOCALITY", item.locality );
        checkField( (*it), "REGION", item.region );
        checkField( (*it), "PCODE", item.pcode );
        checkField( (*it), "CTRY", item.ctry );
        item.postal = ( (*it)->hasChild( "POSTAL" ) )?( true ):( false );
        item.parcel = ( (*it)->hasChild( "PARCEL" ) )?( true ):( false );
        item.work = ( (*it)->hasChild( "WORK" ) )?( true ):( false );
        item.home = ( (*it)->hasChild( "HOME" ) )?( true ):( false );
        item.pref = ( (*it)->hasChild( "PREF" ) )?( true ):( false );
        item.dom = ( (*it)->hasChild( "DOM" ) )?( true ):( false );
        item.intl = ( !item.dom && (*it)->hasChild( "INTL" ) )?( true ):( false );
        m_addressList.push_back( item );
      }

      if( (*it)->name() == "TEL" && (*it)->hasChild( "NUMBER" ) )
      {
        Telephone item;
        item.number = (*it)->findChild( "NUMBER" )->cdata();
        item.work = ( (*it)->hasChild( "WORK" ) )?( true ):( false );
        item.home = ( (*it)->hasChild( "HOME" ) )?( true ):( false );
        item.voice = ( (*it)->hasChild( "VOICE" ) )?( true ):( false );
        item.fax = ( (*it)->hasChild( "FAX" ) )?( true ):( false );
        item.pager = ( (*it)->hasChild( "PAGER" ) )?( true ):( false );
        item.msg = ( (*it)->hasChild( "MSG" ) )?( true ):( false );
        item.cell = ( (*it)->hasChild( "CELL" ) )?( true ):( false );
        item.video = ( (*it)->hasChild( "VIDEO" ) )?( true ):( false );
        item.bbs = ( (*it)->hasChild( "BBS" ) )?( true ):( false );
        item.modem = ( (*it)->hasChild( "MODEM" ) )?( true ):( false );
        item.isdn = ( (*it)->hasChild( "ISDN" ) )?( true ):( false );
        item.pcs = ( (*it)->hasChild( "PCS" ) )?( true ):( false );
        item.pref = ( (*it)->hasChild( "PREF" ) )?( true ):( false );
        m_telephoneList.push_back( item );
      }

      if( (*it)->name() == "ORG" )
      {
        Tag::TagList::const_iterator ito = (*it)->children().begin();
        for( ; ito != (*it)->children().end(); ++ito )
        {
          if( (*ito)->name() == "ORGNAME" )
            m_orgname = (*ito)->cdata();
          else if( (*ito)->name() == "ORGUNIT" )
            m_orgunits.push_back( (*ito)->cdata() );
        }
      }

      if( (*it)->name() == "CLASS" )
      {
        if( (*it)->hasChild( "PRIVATE" ) )
          m_class = ClassPrivate;
        else if( (*it)->hasChild( "PUBLIC" ) )
          m_class = ClassPublic;
        else if( (*it)->hasChild( "CONFIDENTIAL" ) )
          m_class = ClassConfidential;
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

  void VCard::addEmail( const std::string& userid, int type )
  {
    if( userid.empty() )
      return;

    Email item;
    item.userid = userid;
    item.internet = ( type & AddrTypeInet )?( true ):( false );
    item.x400 = ( type & AddrTypeX400 )?( true ):( false );
    item.work = ( type & AddrTypeWork )?( true ):( false );
    item.home = ( type & AddrTypeHome )?( true ):( false );
    item.pref = ( type & AddrTypePref )?( true ):( false );

    m_emailList.push_back( item );
  }

  void VCard::addAddress( const std::string& pobox, const std::string& extadd,
                          const std::string& street, const std::string& locality,
                          const std::string& region, const std::string& pcode,
                          const std::string& ctry, int type )
  {
    if( pobox.empty() && extadd.empty() && street.empty() &&
        locality.empty() && region.empty() && pcode.empty() && ctry.empty() )
      return;

    Address item;
    item.pobox = pobox;
    item.extadd = extadd;
    item.street = street;
    item.locality = locality;
    item.region = region;
    item.pcode = pcode;
    item.ctry = ctry;
    item.home = ( type & AddrTypeHome )?( true ):( false );
    item.work = ( type & AddrTypeWork )?( true ):( false );
    item.parcel = ( type & AddrTypeParcel )?( true ):( false );
    item.postal = ( type & AddrTypePostal )?( true ):( false );
    item.dom = ( type & AddrTypeDom )?( true ):( false );
    item.intl = ( !item.dom && type & AddrTypeDom )?( true ):( false );
    item.pref = ( type & AddrTypePref )?( true ):( false );

    m_addressList.push_back( item );
  }

  void VCard::addTelephone( const std::string& number, int type )
  {
    if( number.empty() )
      return;

    Telephone item;
    item.number = number;
    item.work = ( type & AddrTypeWork )?( true ):( false );
    item.home = ( type & AddrTypeHome )?( true ):( false );
    item.voice = ( type & AddrTypeVoice )?( true ):( false );
    item.fax = ( type & AddrTypeFax )?( true ):( false );
    item.pager = ( type & AddrTypePager )?( true ):( false );
    item.msg = ( type & AddrTypeMsg )?( true ):( false );
    item.cell = ( type & AddrTypeCell )?( true ):( false );
    item.video = ( type & AddrTypeVideo )?( true ):( false );
    item.bbs = ( type & AddrTypeBbs )?( true ):( false );
    item.modem = ( type & AddrTypeModem )?( true ):( false );
    item.isdn = ( type & AddrTypeIsdn )?( true ):( false );
    item.pcs = ( type & AddrTypePcs )?( true ):( false );
    item.pref = ( type & AddrTypePref )?( true ):( false );

    m_telephoneList.push_back( item );
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

  void VCard::setGeo( const std::string& lat, const std::string& lon )
  {
    if( !lat.empty() && !lon.empty() )
    {
      m_geolat = lat;
      m_geolon = lon;
    }
  }

  void VCard::setOrganization( const std::string& orgname, const StringList& orgunits )
  {
    if( !orgname.empty() )
    {
      m_orgname = orgname;
      m_orgunits = orgunits;
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
      insertField( n, "FAMILY", m_family );
      insertField( n, "GIVEN", m_given );
      insertField( n, "MIDDLE", m_middle );
      insertField( n, "PREFIX", m_prefix );
      insertField( n, "SUFFIX", m_suffix );
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

    EmailList::const_iterator ite = m_emailList.begin();
    for( ; ite != m_emailList.end(); ++ite )
    {
      Tag *e = new Tag( v, "EMAIL" );
      insertField( e, "INTERNET", (*ite).internet );
      insertField( e, "WORK", (*ite).work );
      insertField( e, "HOME", (*ite).home );
      insertField( e, "X400", (*ite).x400 );
      insertField( e, "PREF", (*ite).pref );
      insertField( e, "USERID", (*ite).userid );
    }

    AddressList::const_iterator ita = m_addressList.begin();
    for( ; ita != m_addressList.end(); ++ita )
    {
      Tag *a = new Tag( v, "ADR" );
      insertField( a, "POSTAL", (*ita).postal );
      insertField( a, "PARCEL", (*ita).parcel );
      insertField( a, "HOME", (*ita).home );
      insertField( a, "WORK", (*ita).work );
      insertField( a, "PREF", (*ita).pref );
      insertField( a, "DOM", (*ita).dom );
      if( !(*ita).dom )
        insertField( a, "INTL", (*ita).intl );

      insertField( a, "POBOX", (*ita).pobox );
      insertField( a, "EXTADD", (*ita).extadd );
      insertField( a, "STREET", (*ita).street );
      insertField( a, "LOCALITY", (*ita).locality );
      insertField( a, "REGION", (*ita).region );
      insertField( a, "PCODE", (*ita).pcode );
      insertField( a, "CTRY", (*ita).ctry );
    }

    TelephoneList::const_iterator itt = m_telephoneList.begin();
    for( ; itt != m_telephoneList.end(); ++itt )
    {
      Tag *t = new Tag( v, "TEL" );
      insertField( t, "NUMBER", (*itt).number );
      insertField( t, "HOME", (*itt).home );
      insertField( t, "WORK", (*itt).work );
      insertField( t, "VOICE", (*itt).voice );
      insertField( t, "FAX", (*itt).fax );
      insertField( t, "PAGER", (*itt).pager );
      insertField( t, "MSG", (*itt).msg );
      insertField( t, "CELL", (*itt).cell );
      insertField( t, "VIDEO", (*itt).video );
      insertField( t, "BBS", (*itt).bbs );
      insertField( t, "MODEM", (*itt).modem );
      insertField( t, "ISDN", (*itt).isdn );
      insertField( t, "PCS", (*itt).pcs );
      insertField( t, "PREF", (*itt).pref );
    }

    if( !m_geolat.empty() && !m_geolon.empty() )
    {
      Tag *g = new Tag( v, "GEO" );
      new Tag( g, "LAT", m_geolat );
      new Tag( g, "LON", m_geolon );
    }

    if( !m_orgname.empty() )
    {
      Tag *o = new Tag( v, "ORG" );
      new Tag( o, "ORGNAME", m_orgname );
      StringList::const_iterator ito = m_orgunits.begin();
      for( ; ito != m_orgunits.end(); ++ito )
        new Tag( o, "ORGUNITS", (*ito) );
    }

    if( m_class != ClassNone )
    {
      Tag *c = new Tag( v, "CLASS" );
      switch( m_class )
      {
        case ClassPublic:
          new Tag( c, "PUBLIC" );
          break;
        case ClassPrivate:
          new Tag( c, "PRIVATE" );
          break;
        case ClassConfidential:
          new Tag( c, "CONFIDENTIAL" );
          break;
        default:
          break;
      }
    }

    return v;
  }

  void VCard::insertField( Tag *vcard, const std::string& field, const std::string& var ) const
  {
    if( !var.empty() )
      new Tag( vcard, field, var );
  }

  void VCard::insertField( Tag *vcard, const std::string& field, bool var ) const
  {
    if( var )
      new Tag( vcard, field );
  }

}

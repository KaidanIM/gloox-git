/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef VCARD_H__
#define VCARD_H__

#include "gloox.h"

namespace gloox
{

  class Tag;

  /**
   * @brief A VCard abstraction.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API VCard
  {
    public:
      /**
       * Addressing type indicators.
       * @note @c AddrTypeDom and @c AddrTypeIntl are mutually exclusive. If both are present,
       * @c AddrTypeDom takes precendence.
       * @note Also note that not all adress types are applicable everywhere. For example,
       * @c AddrTypeIsdn does not make sense for a postal address. Check JEP-0054
       * for details.
       */
      enum AddressType
      {
        AddrTypeHome   =      1,    /**< */
        AddrTypeWork   =      2,    /**< */
        AddrTypePref   =      4,    /**< */
        AddrTypeX400   =      8,    /**< */
        AddrTypeInet   =     16,    /**< */
        AddrTypeParcel =     32,    /**< */
        AddrTypePostal =     64,    /**< */
        AddrTypeDom    =    128,    /**< */
        AddrTypeIntl   =    256,    /**< */
        AddrTypeVoice  =    512,    /**< */
        AddrTypeFax    =   1024,    /**< */
        AddrTypePager  =   2048,    /**< */
        AddrTypeMsg    =   4096,    /**< */
        AddrTypeCell   =   8192,    /**< */
        AddrTypeVideo  =  16384,    /**< */
        AddrTypeBbs    =  32768,    /**< */
        AddrTypeModem  =  65536,    /**< */
        AddrTypeIsdn   = 131072,    /**< */
        AddrTypePcs    = 262144     /**< */
      };

      /**
       * Classifies the VCard.
       */
      enum VCardClassification
      {
        ClassNone         = 0,      /**< */
        ClassPublic       = 1,      /**< */
        ClassPrivate      = 2,      /**< */
        ClassConfidential = 4       /**< */
      };

      /**
       * Constructor.
       */
      VCard();

      /**
       * Constructs a new VCard from a given Tag containing appropriate fields.
       * @param vcard The vcard-tag.
       */
      VCard( Tag* vcard );

      /**
       * Virtual destructor.
       */
      virtual ~VCard();

      /**
       * Returns a Tag representation of the VCard. The caller becomes the owner of the Tag.
       * @return A Tag containing the VCard, or @b 0 if the VCard data is invalid.
       */
      Tag* tag() const;

      /**
       *
       */
      void setFormattedname( const std::string& name ) { m_formattedname = name; };

      /**
       *
       */
      void setName( const std::string& family, const std::string& given, const std::string& middle,
                    const std::string& prefix, const std::string& suffix );
      /**
       *
       */
      void setNickname( const std::string& nickname ) { m_nickname = nickname; };

      /**
       *
       */
      void setUrl( const std::string& url ) { m_url = url; };

      /**
       *
       */
      void setBday( const std::string& bday ) { m_bday = bday; };

      /**
       *
       */
      void setJabberid( const std::string& jabberid ) { m_jabberid = jabberid; };

      /**
       *
       */
      void setTitle( const std::string& title ) { m_title = title; };

      /**
       *
       */
      void setRole( const std::string& role ) { m_role = role; };

      /**
       *
       */
      void setNote( const std::string& note ) { m_note = note; };

      /**
       *
       */
      void setDesc( const std::string& desc ) { m_desc = desc; };

      /**
       *
       */
      void setMailer( const std::string& mailer ) { m_mailer = mailer; };

      /**
       *
       */
      void setRev( const std::string& rev ) { m_rev = rev; };

      /**
       *
       */
      void setUid( const std::string& uid ) { m_uid = uid; };

      /**
       *
       */
      void setTz( const std::string& tz ) { m_tz = tz; };

      /**
       *
       */
      void setProdid( const std::string& prodid ) { m_prodid = prodid; };

      /**
       *
       */
      void setSortstring( const std::string& sortstring ) { m_sortstring = sortstring; };

      /**
       *
       */
      void setPhoto( const std::string& extval );

      /**
       *
       */
      void setPhoto( const std::string& type, const std::string& binval );

      /**
       *
       */
      void setLogo( const std::string& extval );

      /**
       *
       */
      void setLogo( const std::string& type, const std::string& binval );

      /**
       *
       */
      void addEmail( const std::string& userid, int type );

      /**
       *
       */
      void addAddress( const std::string& pobox, const std::string& extadd,
                       const std::string& street, const std::string& locality,
                       const std::string& region, const std::string& pcode,
                       const std::string& ctry, int type );

      /**
       *
       */
      void addTelephone( const std::string& number, int type );

      /**
       *
       */
      void setGeo( const std::string& lat, const std::string& lon );

      /**
       *
       */
      void setOrganization( const std::string& orgname, const StringList& orgunits );

      /**
       *
       */
      void setClass( VCardClassification vclass ) { m_class = vclass; };

    private:
      void checkField( Tag *vcard, const std::string& field, std::string& var );
      void insertField( Tag *vcard, const std::string& field, const std::string& var ) const;
      void insertField( Tag *vcard, const std::string& field, bool var ) const;

      struct Email
      {
        std::string userid;
        bool home;
        bool work;
        bool internet;
        bool pref;
        bool x400;
      };
      typedef std::list<Email> EmailList;
      EmailList m_emailList;

      struct Telephone
      {
        std::string number;
        bool home;
        bool work;
        bool voice;
        bool fax;
        bool pager;
        bool msg;
        bool cell;
        bool video;
        bool bbs;
        bool modem;
        bool isdn;
        bool pcs;
        bool pref;
      };
      typedef std::list<Telephone> TelephoneList;
      TelephoneList m_telephoneList;

      struct Address
      {
        std::string pobox;
        std::string extadd;
        std::string street;
        std::string locality;
        std::string region;
        std::string pcode;
        std::string ctry;
        bool home;
        bool work;
        bool postal;
        bool parcel;
        bool pref;
        bool dom;
        bool intl;
      };
      typedef std::list<Address> AddressList;
      AddressList m_addressList;

      StringList m_orgunits;

      std::string m_formattedname;
      std::string m_family;
      std::string m_given;
      std::string m_middle;
      std::string m_prefix;
      std::string m_suffix;
      std::string m_nickname;
      std::string m_url;
      std::string m_bday;
      std::string m_jabberid;
      std::string m_title;
      std::string m_role;
      std::string m_note;
      std::string m_desc;
      std::string m_phototype;
      std::string m_photobin;
      std::string m_photoext;
      std::string m_logotype;
      std::string m_logobin;
      std::string m_logoext;
      std::string m_mailer;
      std::string m_tz;
      std::string m_prodid;
      std::string m_rev;
      std::string m_sortstring;
      std::string m_uid;
      std::string m_geolat;
      std::string m_geolon;
      std::string m_orgname;

      VCardClassification m_class;

      bool m_N;
      bool m_PHOTO;
      bool m_LOGO;
  };

}

#endif // VCARD_H__

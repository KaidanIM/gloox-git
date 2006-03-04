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
      void setFormattedName( const std::string& name ) { m_formattedName = name; };

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

    private:
      std::string m_formattedName;
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

      bool m_N;
  };

}

#endif // VCARD_H__

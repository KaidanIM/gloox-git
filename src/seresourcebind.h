/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef SERESOURCEBIND_H__
#define SERESOURCEBIND_H__

#include "jid.h"
#include "stanzaextension.h"
#include "tag.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an implementation of a resource binding StanzaExtension.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class SEResourceBind : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object with the given resource string.
       * @param tag The Tag to parse.
       */
      SEResourceBind( const std::string& resource );

      /**
       * Constructs a new object from the given Tag.
       * @param tag The Tag to parse.
       */
      SEResourceBind( const Tag* tag );

      /**
       * Returns the requested resource.
       * @return The requested resource.
       */
      const std::string& resource() const { return m_resource; }

      /**
       * Returns the assigned JID.
       * @return The assigned JID.
       */
      const JID& jid() const { return m_jid; }

      /**
       * Virtual Destructor.
       */
      virtual ~SEResourceBind();

      // reimplemented from StanzaExtension
      virtual const std::string& filterString() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new SEResourceBind( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

    private:
      const std::string m_resource;
      JID m_jid;
  };

}

#endif // SERESOURCEBIND_H__

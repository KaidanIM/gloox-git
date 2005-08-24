/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#ifndef PRIVATEXMLHANDLER_H__
#define PRIVATEXMLHANDLER_H__

#include <iksemel.h>

#include <string>

namespace gloox
{

  /**
   * A virtual interface which can be reimplemented to receive data stored in private XML.
   * Derived classes can be registered as PrivateXMLHandlers with the PrivateXML object.
   * Upon an incoming PrivateXML packet @ref handlePrivateXML() will be called.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class PrivateXMLHandler
  {
    public:
      /**
       * Reimplement this function to receive the private XML that was requested earlier using
       * @c PrivateXML::requestXML().
       * @param tag The tag of the storage packet.
       * @param xmlns The XML namespace of the storage packet.
       * @param xml The private xml, i.e. the first child of the &lt;query&gt; tag.
       */
      virtual void handlePrivateXML( const std::string& tag, const std::string& xmlns, Tag *xml ) {};

  };

};

#endif // PRIVATEXMLHANDLER_H__

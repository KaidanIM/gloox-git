/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/



#ifndef PRIVATEXML_H__
#define PRIVATEXML_H__

#include "iqhandler.h"
#include "privatexmlhandler.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  class ClientBase;
  class Tag;
  class Stanza;

  /**
   * This class implements JEP-0049 (Private XML Storage).
   * @author Jakob Schroeter <js@camaya.net>
   */
  class PrivateXML : public IqHandler
  {
    public:
      /**
       * Constructor.
       * This function creates a new PrivateXML client that registers as IqHandler
       * with @c ClientBase.
       * @param parent The ClientBase used for XMPP communication
       */
      PrivateXML( ClientBase *parent );

      /**
       * Virtual destructor.
       */
      virtual ~PrivateXML();

      /**
       * Use this function to request the private XML stored in the given namespace.
       * @param tag Child element of the query element used to identify the rrquested XML fragment.
       * @param xmlns The namespace which qualifies the tag.
       */
      void requestXML( const std::string& tag, const std::string& xmlns );

      /**
       * Use this function to store private XML stored in the given namespace.
       * @param xml The XML to store. This is the complete tag including the unique namespace.
       * It is deleted after sending it.
       * @param xmlns The is the namespace, again, in which the element @c is stored.
       */
      void storeXML( const Tag& tag, const std::string& xmlns );

      /**
       * Use this function to register an object that shall receive incoming Private XML packets.
       * @param pxh The handler to register.
       * @param tag The tag to look for and associate with this handler.
       * @param xmlns The namespace of the tag.
       */
      void registerPrivateXMLHandler( PrivateXMLHandler *pxh, const std::string& tag,
                                      const std::string& xmlns );

      /**
       * Use this function to un-register an PrivateXMLHandler.
       * @param xmlns The namespace for which the handler shall be removed.
       */
      void removePrivateXMLHandler( const std::string& xmlns );

      // reimplemented from IqHandler.
      virtual bool handleIq( const Stanza& stanza );

      // reimplemented from IqHandler.
      virtual bool handleIqID( const Stanza& stanza, int context );

    protected:
      ClientBase *m_parent;

    private:
      enum IdType
      {
        REQUEST_XML,
        STORE_XML
      };

      struct XMLHandlerStruct
      {
        std::string xmlns;
        std::string tag;
        PrivateXMLHandler *pxh;
      };
      typedef std::map<std::string, XMLHandlerStruct> PrivateXMLHandlers;

      PrivateXMLHandlers m_privateXMLHandlers;
  };

};

#endif // PRIVATEXML_H__

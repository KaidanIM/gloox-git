/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
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
       * Creates a new PrivateXML client that registers as IqHandler
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
       * @param tag Child element of the query element used to identify the requested XML fragment.
       * @param xmlns The namespace which qualifies the tag.
       * @return The ID of the sent query.
       */
      std::string requestXML( const std::string& tag, const std::string& xmlns );

      /**
       * Use this function to store private XML stored in the given namespace.
       * @param tag The XML to store. This is the complete tag including the unique namespace.
       * It is deleted after sending it.
       * @param xmlns The namespace again, in which the element @c is stored.
       * @return The ID of the sent query.
       */
      std::string storeXML( Tag *tag, const std::string& xmlns );

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
      virtual bool handleIq( Stanza *stanza );

      // reimplemented from IqHandler.
      virtual bool handleIqID( Stanza *stanza, int context );

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

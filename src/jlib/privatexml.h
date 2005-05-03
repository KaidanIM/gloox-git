/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/



#ifndef PRIVATEXML_H__
#define PRIVATEXML_H__

#include "iqhandler.h"
#include "privatexmlhandler.h"

#include <string>
#include <list>
#include <map>
using namespace std;

class JClient;

/**
 * This class implements JEP-0049 (Private XML Storage).
 * 
 */
class PrivateXML : public IqHandler
{
  public:
    /**
     * Constructor.
     * You should access the Adhoc object through the @c JClient object.
     * This function creates a new Adhoc client that registers as IqHandler
     * with @c JClient.
     * @param parent The JClient used for XMPP communication
     */
    PrivateXML( JClient* parent );

    /**
     * Virtual destructor.
     */
    virtual ~PrivateXML();

    /**
     * Use this function to request the private XML stored in the given namespace.
     * @param xmlns The namespace which should be retrieved.
     */
    void requestXML( const string& tag, const string& xmlns );

    /**
     * Use this function to store private XML stored in the given namespace.
     * @param xml The XML to store. This is the complete tag including the unique namespace.
     * It is deleted after sending it.
     */
    void storeXML( iks* xml );

    /**
     * Use this function to register an object that shall receive incoming Private XML packets.
     * @param pxh The handler to register.
     * @param tag The tag to look for and associate with this handler.
     * @param xmlns The namespace of the tag.
     */
    void registerPrivateXMLHandler( PrivateXMLHandler* pxh, const string& tag, const string& xmlns );

    // reimplemented from IqHandler.
    virtual void handleIq( const char* xmlns, ikspak* pak );

  private:
    JClient* m_parent;

    typedef struct XMLHandlerStruct
    {
      const string* xmlns;
      const string* tag;
      PrivateXMLHandler* pxh;
    } HandlerStruct;
    typedef list<HandlerStruct*> PrivateXMLHandlers;

    PrivateXMLHandlers m_privateXMLHandlers;

};

#endif // PRIVATEXML_H__

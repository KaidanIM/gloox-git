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



#ifndef DISCO_H__
#define DISCO_H__

#include "iqhandler.h"

#include <string>
#include <list>
using namespace std;

class JClient;

/**
 * This class implements JEP-0030 (Service Discovery).
 * 
 */
class Disco : public IqHandler
{
  public:
    typedef list<string> StringList;

    /**
     * Constructor.
     * You should access the disco object through the @c JClient object.
     * Creates a new Disco client that registers as IqHandler with @c JClient.
     * @param parent The JClient used for XMPP communication
     */
    Disco( JClient* parent );

    /**
     * Virtual destructor.
     */
    virtual ~Disco();

    /**
     * Adds a feature to the list of supported Jabber features.
     * The list will be posted as an answer to IQ queries in the
     * "http://jabber.org/protocol/disco#info" namespace.
     * These IQ packets will also be forwarded to the
     * application's IqHandler, if it listens to the disco#info namespace.
     * You can call @ref disableDisco() to disable automatic disco
     * entirely. By default, disco(very) queries are handled by the library.
     * By default, all supported, not disabled features are announced.
     * @param feature A feature (namespace) the host app supports.
     */
    void addFeature( const string& feature );

    /**
     * Queries the given JID for general infomation according to
     * JEP-0030 (Service Discovery).
     * @param to The destination-JID of the query.
     * @return A list of capabilities.
     */
    Disco::StringList getDiscoInfo( const string& to );

    /**
     * Queries the given JID for its items according to
     * JEP-0030 (Service Discovery).
     * @param to The destination-JID of the query.
     * @return A list of items.
     */
    Disco::StringList getDiscoItems( const string& to );

    /**
     * Sets the version of the host application using this library.
     * The library takes care of jabber:iq:version requests. These
     * IQ packets will not be forwarded to the IqHandlers.
     * @param name The name to be returned to inquireing clients.
     * @param version The version to be returned to inquireing clients.
     */
    void setVersion( const string& name, const string& version );

    /**
     * Sets the identity of this entity.
     * The library uses this information to answer disco#info requests
     * with a correct identity.
     * JEP-0030 requires an entity to have at least one identity. See JEP-0030
     * for more information on categories and types.
     * @param category The entity category of this client. Default: client
     * @param type The type of this entity. Default: bot
     */
    void setIdentity( const string& category, const string& type );

    // reimplemented from IqHandler.
    virtual void handleIq( const char* xmlns, ikspak* pak );

  private:
    JClient* m_parent;

    StringList m_features;

    string m_versionName;
    string m_versionVersion;
    string m_identityCategory;
    string m_identityType;

};

#endif // DISCO_H__

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



#ifndef ADHOC_H__
#define ADHOC_H__

#include "nodehandler.h"

#include <string>
#include <list>
#include <map>
using namespace std;

class JClient;

/**
 * This class implements JEP-0030 (Service Discovery).
 * 
 */
class Adhoc : public NodeHandler
{
  public:
    /**
   * Constructor.
   * You should access the Adhoc object through the @c JClient object.
   * Creates a new Adhoc client that registers as IqHandler with @c JClient.
   * @param parent The JClient used for XMPP communication
     */
    Adhoc( JClient* parent );

    /**
     * Virtual destructor.
     */
    virtual ~Adhoc();

    /**
     * reimplemented from NodeHandler
     */
    virtual FeatureList handleNodeFeatures( const char* node );

    /**
     * reimplemented from NodeHandler
     */
    virtual IdentityMap handleNodeIdentities( const char* node );

    /**
     * reimplemented from NodeHandler
     */
    virtual ItemMap handleNodeItems( const char* node );

  private:
    typedef map<string, NodeHandler::IdentityMap> NodeIdentityMap;
    typedef map<string, NodeHandler::ItemMap>     NodeItemMap;

    JClient* m_parent;

    NodeIdentityMap m_identities;
    NodeItemMap m_items;

};

#endif // ADHOC_H__

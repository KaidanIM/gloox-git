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



#ifndef NODEHANDLER_H__
#define NODEHANDLER_H__

#include <list>
#include <map>
#include <string>
using namespace std;

/**
 * A virtual interface.
 * Derived classes can be registered as NodeHandlers for certain nodes
 * with the Disco object of JClient.
 * Incoming disco#info and disco#items queries are delegated to their
 * respective handlers.
 * @author Jakob Schroeter <js@camaya.net>
 */
class NodeHandler
{
  public:
    /**
     * Holds a list of items. The first string is the item's node specifier,
     * the second string is the items natural-language name.
     */
    typedef map<string, string> ItemMap;

    /**
     * Holds a list of features.
     */
    typedef list<string> FeatureList;

    /**
     * Holds a list of identities for a given node. The first string is the
     * category specifier, the second string is the type specifier.
     */
    typedef map<string, string> IdentityMap;

    /**
     * In addition to @c handleNodeIdentities, this function is used to gather
     * more information on a specific node. It is called when a disco#info query
     * arrives with a node attribute that matches the one registered for this handler.
     * @param node The node this handler is supposed to handle.
     * @return A list of features supported by this node.
     */
    virtual FeatureList handleNodeFeatures( const char* node ) {};

    /**
     * In addition to @c handleNodeFeatures, this function is used to gather
     * more information on a specific node. It is called when a disco#info query
     * arrives with a node attribute that matches the one registered for this handler.
     * @param node The node this handler is supposed to handle.
     * @return A list of identities for this node.
     */
    virtual IdentityMap handleNodeIdentities( const char* node ) {};

    /**
     * This function is used to gather more information on a specific node.
     * It is called when a disco#items query arrives with a node attribute that
     * matches the one registered for this handler. If node is 0, items for the
     * root node (no node) shall be returned.
     * @param node The node this handler is supposed to handle.
     * @return A map of items supported by this node.
     */
    virtual ItemMap handleNodeItems( const char* node = 0 ) {};

};

#endif // NODEHANDLER_H__

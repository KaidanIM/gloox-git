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



#ifndef DISCONODEHANDLER_H__
#define DISCONODEHANDLER_H__

#include <list>
#include <map>
#include <string>

namespace gloox
{

  /**
   * A virtual interface.
   * Derived classes can be registered as NodeHandlers for certain nodes
   * with the Disco object of Client.
   * Incoming disco#info and disco#items queries are delegated to their
   * respective handlers.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class DiscoNodeHandler
  {
    public:
      /**
       * Holds a list of items. The first std::string is the item's node specifier,
       * the second std::string is the items natural-language name.
       */
      typedef std::map<std::string, std::string> ItemMap;

      /**
       * Holds a list of features.
       */
      typedef std::list<std::string> FeatureList;

      /**
       * Holds a list of identities for a given node. The first std::string is the
       * category specifier, the second std::string is the type specifier.
       */
      typedef std::map<std::string, std::string> IdentityMap;

      /**
       * In addition to @c handleDiscoNodeIdentities, this function is used to gather
       * more information on a specific node. It is called when a disco#info query
       * arrives with a node attribute that matches the one registered for this handler.
       * @param node The node this handler is supposed to handle.
       * @return A list of features supported by this node.
       */
      virtual FeatureList handleDiscoNodeFeatures( const std::string& node ) {};

      /**
       * In addition to @c handleDiscoNodeFeatures, this function is used to gather
       * more information on a specific node. It is called when a disco#info query
       * arrives with a node attribute that matches the one registered for this handler.
       * @param node The node this handler is supposed to handle.
       * @return A list of identities for this node.
       */
      virtual IdentityMap handleDiscoNodeIdentities( const std::string& node ) {};

      /**
       * This function is used to gather more information on a specific node.
       * It is called when a disco#items query arrives with a node attribute that
       * matches the one registered for this handler. If node is empty, items for the
       * root node (no node) shall be returned.
       * @param node The node this handler is supposed to handle.
       * @return A map of items supported by this node.
       */
      virtual ItemMap handleDiscoNodeItems( const std::string& node = "" ) {};

  };

};

#endif // DISCONODEHANDLER_H__

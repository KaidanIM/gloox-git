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


#include "adhoc.h"
#include "disco.h"
#include "discohandler.h"
#include "jclient.h"


Adhoc::Adhoc( JClient* parent )
  : m_parent( parent )
{
  m_parent->disco()->addFeature( XMLNS_ADHOC_COMMANDS );

  NodeHandler::IdentityMap ident;
  ident["automation"] = "command-list";
  m_identities[XMLNS_ADHOC_COMMANDS] = ident;
  ident["automation"] = "command-node";
  m_identities["config"] = ident;

  m_parent->disco()->registerNodeHandler( this, XMLNS_ADHOC_COMMANDS );
  m_parent->disco()->registerNodeHandler( this, "config" );

  NodeHandler::ItemMap item;
  item["config"] = "Configure Service";
  m_items[XMLNS_ADHOC_COMMANDS] = item;
}

Adhoc::~Adhoc()
{

}

NodeHandler::FeatureList Adhoc::handleNodeFeatures( const char* node )
{
  NodeHandler::FeatureList features;
  printf( "received feature request for node %s\n", node );
  return features;
}

NodeHandler::ItemMap Adhoc::handleNodeItems( const char* node )
{
  printf( "received items request for node %s\n", node );
  if( node )
  {
    return m_items[node];
  }
  else
  {
    NodeHandler::ItemMap items;
    items[XMLNS_ADHOC_COMMANDS] = "Ad-Hoc Commands";
    return items;
  }
}

NodeHandler::IdentityMap Adhoc::handleNodeIdentities( const char* node )
{
  return m_identities[node];
}

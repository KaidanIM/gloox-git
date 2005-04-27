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
  m_parent->disco()->registerNodeHandler( this, XMLNS_ADHOC_COMMANDS );
}

Adhoc::~Adhoc()
{

}

DiscoNodeHandler::FeatureList Adhoc::handleDiscoNodeFeatures( const char* node )
{
  DiscoNodeHandler::FeatureList features;
  printf( "received feature request for node %s\n", node );
  return features;
}

DiscoNodeHandler::ItemMap Adhoc::handleDiscoNodeItems( const char* node )
{
  printf( "received items request for node %s\n", node );
  if( !node )
    return m_items;
  else if( iks_strncmp( XMLNS_ADHOC_COMMANDS, node, iks_strlen( XMLNS_ADHOC_COMMANDS ) ) == 0 )
  {
    DiscoNodeHandler::ItemMap item;
    item[XMLNS_ADHOC_COMMANDS] = "Ad-Hoc Commands";
    return item;
  }
  else
  {
    DiscoNodeHandler::ItemMap item;
    return item;
  }
}

DiscoNodeHandler::IdentityMap Adhoc::handleDiscoNodeIdentities( const char* node )
{
  DiscoNodeHandler::IdentityMap ident;
  if( iks_strncmp( XMLNS_ADHOC_COMMANDS, node, iks_strlen( XMLNS_ADHOC_COMMANDS ) ) == 0 )
    ident["automation"] = "command-list";
  else
    ident["automation"] = "command-node";
  return ident;
}

void Adhoc::registerAdhocCommandProvider( AdhocCommandProvider* acp, const string& command, const string& name )
{
  m_parent->disco()->registerNodeHandler( this, command );
  m_adhocCommandProviders[command] = acp;
  m_items[command] = name;
}

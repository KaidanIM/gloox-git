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


#include "adhoc.h"
#include "disco.h"
#include "discohandler.h"
#include "jclient.h"


namespace gloox
{

  Adhoc::Adhoc( ClientBase *parent, Disco *disco )
    : m_parent( parent ), m_disco( disco )
  {
    if( m_parent && m_disco )
    {
      m_parent->registerIqHandler( this, XMLNS_ADHOC_COMMANDS );
      m_disco->addFeature( XMLNS_ADHOC_COMMANDS );
      m_disco->registerNodeHandler( this, XMLNS_ADHOC_COMMANDS );
    }
  }

  Adhoc::~Adhoc()
  {
    if( m_parent )
    {
      m_parent->removeIqHandler( XMLNS_ADHOC_COMMANDS );
      m_disco->removeNodeHandler( XMLNS_ADHOC_COMMANDS );
    }
  }

  DiscoNodeHandler::FeatureList Adhoc::handleDiscoNodeFeatures( const char *node )
  {
    DiscoNodeHandler::FeatureList features;
    features.push_back( XMLNS_ADHOC_COMMANDS );
    return features;
  }

  DiscoNodeHandler::ItemMap Adhoc::handleDiscoNodeItems( const char *node )
  {
    if( !node )
    {
      DiscoNodeHandler::ItemMap item;
      item[XMLNS_ADHOC_COMMANDS] = "Ad-Hoc Commands";
      return item;
    }
    else if( iks_strncmp( XMLNS_ADHOC_COMMANDS, node, iks_strlen( XMLNS_ADHOC_COMMANDS ) ) == 0 )
    {
      return m_items;
    }
    else
    {
      DiscoNodeHandler::ItemMap item;
      return item;
    }
  }

  DiscoNodeHandler::IdentityMap Adhoc::handleDiscoNodeIdentities( const char *node )
  {
    DiscoNodeHandler::IdentityMap ident;
    if( iks_strncmp( XMLNS_ADHOC_COMMANDS, node, iks_strlen( XMLNS_ADHOC_COMMANDS ) ) == 0 )
      ident["automation"] = "command-list";
    else
      ident["automation"] = "command-node";
    return ident;
  }

  void Adhoc::handleIq( const char *tag, const char *xmlns, ikspak *pak )
  {
    if( iks_strncmp( "command", tag, 7 ) == 0 )
    {
      iks *x = iks_first_tag( pak->x );
      char *node = iks_find_attrib( x, "node" );
      AdhocCommandProviderMap::const_iterator it = m_adhocCommandProviders.find( node );
      if( node && ( it != m_adhocCommandProviders.end() ) )
        (*it).second->handleAdhocCommand( node, x );
    }
  }

  void Adhoc::registerAdhocCommandProvider( AdhocCommandProvider *acp, const string& command,
                                            const string& name )
  {
    m_disco->registerNodeHandler( this, command );
    m_adhocCommandProviders[command] = acp;
    m_items[command] = name;
  }

};

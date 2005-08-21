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



#include "rosteritem.h"

namespace gloox
{

  RosterItem::RosterItem( const std::string& jid, const std::string& name )
    : m_jid( jid ), m_name( name ), m_changed( false ),
    m_directPresence( 0 ), m_status( 0 )
  {
  }

  RosterItem::~RosterItem()
  {
  }

  void RosterItem::setName( const std::string& name )
  {
    m_name = name;
    m_changed = true;
  }

  void RosterItem::setDirectPresence( const int presence )
  {
    m_directPresence = presence;
    m_changed = true;
  }

  void RosterItem::setStatus( int status )
  {
    m_status = status;
    m_changed = true;
  }

  void RosterItem::setStatusMsg( const std::string& msg )
  {
    m_statusMessage = msg;
    m_changed = true;
  }

  void RosterItem::setSubscription( const std::string& subscription, bool ask )
  {
    if( subscription == "from" && !ask )
      m_subscription = S10N_FROM;
    else if( subscription == "from" && ask )
      m_subscription = S10N_FROM_OUT;
    else if( subscription == "to" && !ask )
      m_subscription = S10N_TO;
    else if( subscription == "to" && ask )
      m_subscription = S10N_TO_IN;
    else if( subscription == "none" && !ask )
      m_subscription = S10N_NONE;
    else if( subscription == "none" && ask )
      m_subscription = S10N_NONE_OUT;
    else if( subscription == "both" )
      m_subscription = S10N_BOTH;
  }

  void RosterItem::setGroups( const GroupList& groups )
  {
    m_groups = groups;
    m_changed = true;
  }

};

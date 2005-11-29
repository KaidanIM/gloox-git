/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#include "rosteritem.h"

namespace gloox
{

  RosterItem::RosterItem( const std::string& jid, const std::string& name )
    : m_name( name ), m_jid( jid ), m_changed( false )
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

  void RosterItem::setStatus( const std::string& resource, PresenceStatus status )
  {
    if( m_resources.find( resource ) == m_resources.end() )
    {
      ResourceStruct rs;
	  rs.priority = 0;
	  rs.message = "";
	  rs.status = status;
      m_resources[resource] = rs;
    }
    else
      m_resources[resource].status = status;
  }

  void RosterItem::setStatusMsg( const std::string& resource, const std::string& msg )
  {
    if( m_resources.find( resource ) == m_resources.end() )
    {
      ResourceStruct rs;
	  rs.priority = 0;
	  rs.message = msg;
	  rs.status = PRESENCE_UNAVAILABLE;
      m_resources[resource] = rs;
    }
    else
      m_resources[resource].message = msg;
  }

  void RosterItem::setPriority( const std::string& resource, int priority )
  {
    if( m_resources.find( resource ) == m_resources.end() )
    {
      ResourceStruct rs;
	  rs.priority = priority;
	  rs.message = "";
	  rs.status = PRESENCE_UNAVAILABLE;
      m_resources[resource] = rs;
    }
    else
      m_resources[resource].priority = priority;
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

  void RosterItem::setGroups( const StringList& groups )
  {
    m_groups = groups;
    m_changed = true;
  }

  void RosterItem::removeResource( const std::string& resource )
  {
    ResourceMap::iterator it = m_resources.find( resource );
    if( it != m_resources.end() )
      m_resources.erase( it );
  }

  bool RosterItem::online() const
  {
    if( m_resources.size() )
      return true;
    else
      return false;
  }
}

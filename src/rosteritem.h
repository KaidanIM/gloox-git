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


#ifndef ROSTERITEM_H__
#define ROSTERITEM_H__

#include "gloox.h"

#include <string>
#include <list>


namespace gloox
{

  /**
   * An implementation of a roster item.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class RosterItem
  {
    friend class RosterManager;

    public:
      /**
       * A list of groups a contact belongs to.
       */
      typedef std::list<std::string> GroupList;

      /**
       * Describes possible subscribtion types according to RFC 3921, Section 9.
       */
      enum SubscriptionEnum
      {
        S10N_NONE,            /**< contact and user are not subscribed to each other, and
                               * neither has requested a subscription from the other */
        S10N_NONE_OUT,        /**< contact and user are not subscribed to each other, and
                               * user has sent contact a subscription request but contact
                               * has not replied yet */
        S10N_NONE_IN,         /**< contact and user are not subscribed to each other, and
                               * contact has sent user a subscription request but user has
                               * not replied yet (note: contact's server SHOULD NOT push or
                               * deliver roster items in this state, but instead SHOULD wait
                               * until contact has approved subscription request from user) */
        S10N_NONE_OUT_IN,     /**< contact and user are not subscribed to each other, contact
                               * has sent user a subscription request but user has not replied
                               * yet, and user has sent contact a subscription request but
                               * contact has not replied yet */
        S10N_TO,              /**< user is subscribed to contact (one-way) */
        S10N_TO_IN,           /**< user is subscribed to contact, and contact has sent user a
                               * subscription request but user has not replied yet */
        S10N_FROM,            /**< contact is subscribed to user (one-way) */
        S10N_FROM_OUT,        /**< contact is subscribed to user, and user has sent contact a
                               * subscription request but contact has not replied yet */
        S10N_BOTH             /**< user and contact are subscribed to each other (two-way) */
      };

      /**
       * Constructor
       * @param jid The JID of the contact.
       * @param name The displayed name of the contact.
       */
      RosterItem( const std::string& jid, const std::string& name = "" );

      /**
       * Virtual Destructor.
       */
      virtual ~RosterItem();

      /**
       * Sets the displayed name of a contact/roster item.
       * @param name The contact's new name.
       */
      virtual void setName( const std::string& name );

      /**
       * Retrieves the displayed name of a contact/roster item.
       * If none is set the JID is returned.
       * @return The contact's name.
       */
      virtual const std::string name() { return m_name; };

      /**
       * Returns the contact's JID.
       * @return The contact's JID.
       */
      virtual const std::string jid() { return m_jid; };

      /**
       * Sets the presence the remote contact sees for the local entity.
       * @param presence The presence the remote entity sees.
       */
      virtual void setDirectPresence( const int presence );

      /**
       * Returns the presence the remote contact sees for the local entity.
       * @return The presence the remote contact sees.
       */
      virtual const int directPresence() { return m_directPresence; };

      /**
       * Returns the current subscription type between the remote and the local entity.
       * @return The subscription type.
       */
      virtual SubscriptionEnum subscription() { return m_subscription; };

      /**
       * Sets the groups this RosterItem belongs to.
       * @param groups The groups to set for this item.
       */
      virtual void setGroups( const GroupList& groups );

      /**
       * Returns the groups this RosterItem belongs to.
       * @return The groups this item belongs to.
       */
      virtual GroupList& groups() { return m_groups; };

      /**
       * Whether the item has unsynchronized changes.
       * @return @b True if the item has unsynchronized changes, @b false otherwise.
       */
      virtual bool changed() { return m_changed; };

    protected:
      /**
       * Sets the current status of the contact.
       * @param status The current status, i.e. presence info.
       */
      virtual void setStatus( int status );

      /**
       * Sets the current status message of the contact.
       * @param msg The current status message, i.e. from the presence info.
       */
      virtual void setStatusMsg( const std::string& msg );

      /**
       * Sets the current subscription status of the contact.
       * @param subscription The current subscription.
       * @param ask Whether a subscription request is pending.
       */
      virtual void setSubscription( const std::string& subscription, bool ask );

      /**
       * Removes the 'changed' flag from the item.
       */
      virtual void setSynchronized() { m_changed = false; };

    private:
      GroupList m_groups;
      SubscriptionEnum m_subscription;
      std::string m_jid;
      std::string m_name;
      std::string m_statusMessage;
      bool m_changed;
      int m_directPresence;
      int m_status;
  };

};

#endif // ROSTERITEM_H__

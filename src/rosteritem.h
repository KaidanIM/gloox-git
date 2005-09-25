/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef ROSTERITEM_H__
#define ROSTERITEM_H__

#include "gloox.h"

#include <string>
#include <list>


namespace gloox
{

  /**
   * @brief An abstraction of a roster item.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class RosterItem
  {
    friend class RosterManager;

    public:
      /**
       * Describes possible subscribtion types according to RFC 3921, Section 9.
       */
      enum SubscriptionEnum
      {
        S10N_NONE,            /**< Contact and user are not subscribed to each other, and
                               * neither has requested a subscription from the other. */
        S10N_NONE_OUT,        /**< Contact and user are not subscribed to each other, and
                               * user has sent contact a subscription request but contact
                               * has not replied yet. */
        S10N_NONE_IN,         /**< Contact and user are not subscribed to each other, and
                               * contact has sent user a subscription request but user has
                               * not replied yet (note: contact's server SHOULD NOT push or
                               * deliver roster items in this state, but instead SHOULD wait
                               * until contact has approved subscription request from user). */
        S10N_NONE_OUT_IN,     /**< Contact and user are not subscribed to each other, contact
                               * has sent user a subscription request but user has not replied
                               * yet, and user has sent contact a subscription request but
                               * contact has not replied yet. */
        S10N_TO,              /**< User is subscribed to contact (one-way). */
        S10N_TO_IN,           /**< User is subscribed to contact, and contact has sent user a
                               * subscription request but user has not replied yet. */
        S10N_FROM,            /**< Contact is subscribed to user (one-way). */
        S10N_FROM_OUT,        /**< Contact is subscribed to user, and user has sent contact a
                               * subscription request but contact has not replied yet. */
        S10N_BOTH             /**< User and contact are subscribed to each other (two-way). */
      };

      /**
       * Constructs a new item of the roster.
       * @param jid The JID of the contact.
       * @param name The displayed name of the contact.
       */
      RosterItem( const std::string& jid, const std::string& name = "" );

      /**
       * Virtual destructor.
       */
      virtual ~RosterItem();

      /**
       * Sets the displayed name of a contact/roster item.
       * @param name The contact's new name.
       */
      virtual void setName( const std::string& name );

      /**
       * Retrieves the displayed name of a contact/roster item.
       * @return The contact's name.
       */
      virtual const std::string name() const { return m_name; };

      /**
       * Returns the contact's JID.
       * @return The contact's JID.
       */
      virtual const std::string jid() const { return m_jid; };

      /**
       * Returns the current subscription type between the remote and the local entity.
       * @return The subscription type.
       */
      virtual SubscriptionEnum subscription() const { return m_subscription; };

      /**
       * Sets the groups this RosterItem belongs to.
       * @param groups The groups to set for this item.
       */
      virtual void setGroups( const StringList& groups );

      /**
       * Returns the groups this RosterItem belongs to.
       * @return The groups this item belongs to.
       */
      virtual StringList& groups() { return m_groups; };

      /**
       * Returns the contact's status.
       * @return The status of the contact.
       */
      virtual PresenceStatus status() const { return m_status; };

      /**
       * Whether the item has unsynchronized changes.
       * @return @b True if the item has unsynchronized changes, @b false otherwise.
       */
      virtual bool changed() const { return m_changed; };

    protected:
      /**
       * Sets the current status of the contact.
       * @param status The current status, i.e. presence info.
       */
      virtual void setStatus( PresenceStatus status );

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
      StringList m_groups;
      SubscriptionEnum m_subscription;
      PresenceStatus m_status;
      std::string m_jid;
      std::string m_name;
      std::string m_statusMessage;
      bool m_changed;
  };

};

#endif // ROSTERITEM_H__

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef ROSTERLISTENER_H__
#define ROSTERLISTENER_H__

#include "rosteritem.h"

#include <string>
#include <map>

namespace gloox
{

  /**
   * @brief A virtual interface which can be reimplemented to receive roster updates.
   * A class implementing this interface and being registered as RosterListener with the Roster
   * object receives notifications about all the changes in the server-side roster.
   * Only one RosterListener per Roster at a time is possible.
   * @author Jakob Schroeter <js@camaya.net>
   */
  class RosterListener
  {
    public:
      /**
       * A map of JID/RosterItem pairs.
       * @todo Derive from JID.
       */
      typedef std::map<const std::string, RosterItem*> Roster;

      /**
       * Reimplement this function if you want to be notified about new items
       * on the server-side roster (items subject to a so-called Roster Push).
       * @param jid The new item's full address.
       */
      virtual void itemAdded( const std::string& jid ) {};

      /**
       * Reimplement this function if you want to be notified about items
       * which authorised subscription.
       * @param jid The authorising item's full address.
       */
      virtual void itemSubscribed( const std::string& jid ) {};

      /**
       * Reimplement this function if you want to be notified about items that
       * were removed from the server-side roster (items subject to a so-called Roster Push).
       * @param jid The removed item's full address.
       */
      virtual void itemRemoved( const std::string& jid ) {};

      /**
       * Reimplement this function if you want to be notified about items that
       * were modified on the server-side roster (items subject to a so-called Roster Push).
       * @param jid The modified item's full address.
       */
      virtual void itemUpdated( const std::string& jid ) {};

      /**
       * Reimplement this function if you want to be notified about items which
       * removed subscription authorization.
       * @param jid The item's full address.
       */
      virtual void itemUnsubscribed( const std::string& jid ) {};

      /**
       * Reimplement this function if you want to receive the whole server-side roster
       * on the initial roster push. The roster item status is set to unavailable.
       * @param roster The full roster.
       */
      virtual void roster( Roster& roster ) {};

      /**
       * This function is called on every status change of an item in the roster.
       * @note This function is not called for status changes from or to Unavailable. In these cases,
       * @ref itemAvailable() and @ref itemUnavailable() are called, respectively.
       * @param item The roster item.
       * @param status The item's new status.
       * @param msg The status change message.
       */
      virtual void itemChanged( RosterItem& item, int status, const std::string& msg ) {};

      /**
       * This function is called whenever a roster item comes online (is available).
       * @param item The changed roster item.
       * @param msg The status change message.
       */
      virtual void itemAvailable( RosterItem& item, const std::string& msg ) {};

      /**
       * This function is called whenever a roster item goes offline (is unavailable).
       * @param item The roster item.
       * @param msg The status change message.
       */
      virtual void itemUnavailable( RosterItem& item, const std::string& msg ) {};

      /**
       * This function is called when an entity wishes to subscribe to this entity's presence.
       * @param jid The requesting item's address.
       * @param msg A message sent along with the request.
       * @return Return @b true to allow subscription and subscribe to the remote entities
       * presence, @b false to ignore the request.
       */
      virtual bool subscriptionRequest( const std::string& jid, const std::string& msg ) {};

      /**
       * This function is called when an entity unsubscribes from this entity's presence.
       * @param jid The item's address.
       * @param msg A message sent along with the request.
       * @return Return @b true to unsubscribe from the remote entity and remove the entity
       * from the roster, @b false to ignore.
       */
      virtual bool unsubscriptionRequest( const std::string& jid, const std::string& msg ) {};

  };

};

#endif // ROSTERLISTENER_H__

/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef MUCROOMLISTENER_H__
#define MUCROOMLISTENER_H__

namespace gloox
{

  class JID;
  class MUCRoom;

  /**
   * Describes a participant in a MUC room.
   */
  struct MUCRoomParticipant
  {
    JID *nick;                      /**< Pointer to a JID holding the participant's full JID
                                     * in the form of @c room@service/nick. <br/>
                                     * @note The MUC server @b may change the chosen nickname.
                                     * If the @b self member of this struct is true, one should
                                     * check the resource of of this member if the actual nickname
                                     * is important. */
    MUCRoomAffiliation affiliation; /**< The participant's affiliation with room. */
    MUCRoomRole role;               /**< The participant's role with the room. */
    bool self;                      /**< This is true if the struct refers to this instance's user.
                                     * (MUC servers send presence to all room occupants, including
                                     * the originator of the presence.) */
  };

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class MUCRoomListener
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~MUCRoomListener() {};

      /**
       *
       */
      virtual void handleMUCParticipantPresence( MUCRoom *room, const MUCRoomParticipant participant,
                                              Presence presence ) = 0;

      /**
       *
       */
      virtual void handleMUCMessage( MUCRoom *room, const std::string& nick,
                                     const std::string& message, bool history,
                                     const std::string& when ) = 0;

      /**
       *
       * @note With some MUC services the nick may be empty when a room is first entered.
       */
      virtual void handleMUCSubject( MUCRoom *room, const std::string& nick,
                                     const std::string& subject ) = 0;

      /**
       *
       * @note Only the following error conditions are specified for MUC:
       * @li @b Not @b Authorized: Password required.
       * @li @b Forbidden: Access denied, user is banned.
       * @li @b Item @b Not @b Found: The room does not exist.
       * @li @b Not @b Allowed: Room creation is restricted.
       * @li @b Not @b Acceptable: Room nicks are locked down.
       * @li @b Registration @b Required: User is not on the member list.
       * @li @b Conflict: Desired room nickname is in use or registered by another user.
       * @li @b Service @b Unavailable: Maximum number of users has been reached.
       */
      virtual void handleMUCError( MUCRoom *room, StanzaError error ) = 0;

  };

}

#endif// MUCROOMLISTENER_H__

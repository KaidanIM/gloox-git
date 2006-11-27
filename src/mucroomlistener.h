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
      virtual void handleParticipantPresence( const MUCRoomParticipant participant, Presence presence ) = 0;

  };

}

#endif// MUCROOMLISTENER_H__

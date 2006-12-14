/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef MUCROOMCONFIGHANDLER_H__
#define MUCROOMCONFIGHANDLER_H__

#include "gloox.h"

#include <string>
#include <list>

namespace gloox
{

  /**
   * An item in a list of MUC room users. Lists of these items are
   * used when manipulating the lists of members, admins, owners, etc.
   * of a room.
   */
  struct MUCListItem
  {
    JID *jid;                       /**< Pointer to the occupant's JID if available, 0 otherwise. */
    std::string nick;               /**< The occupant's nick in the room. */
    MUCRoomAffiliation affiliation; /**< The occupant's affiliation. */
    MUCRoomRole role;               /**< The occupant's role. */
  };

  /**
   * A list of MUCListItems.
   */
  typedef std::list<MUCListItem> MUCListItemList;

  /**
   *
   */
  enum MUCOperation
  {
    RequestUniqueName,              /**< */
    CreateInstantRoom,              /**< */
    CancelRoomCreation,             /**< */
    RequestRoomConfig,              /**< */
    DestroyRoom,                    /**< */
    GetRoomInfo,                    /**< */
    GetRoomItems,                   /**< */
    SetRNone,                       /**< */
    SetVisitor,                     /**< */
    SetParticipant,                 /**< */
    SetModerator,                   /**< */
    SetANone,                       /**< */
    SetOutcast,                     /**< */
    SetMember,                      /**< */
    SetAdmin,                       /**< */
    SetOwner,                       /**< */
    RequestVoiceList,               /**< */
    StoreVoiceList,                 /**< */
    RequestBanList,                 /**< */
    StoreBanList,                   /**< */
    RequestMemberList,              /**< */
    StoreMemberList,                /**< */
    RequestModeratorList,           /**< */
    StoreModeratorList,             /**< */
    RequestOwnerList,               /**< */
    StoreOwnerList,                 /**< */
    RequestAdminList,               /**< */
    StoreAdminList                  /**< */
  };

  /**
   *
   */
  enum MUCListType
  {
    MUCListParticipants,            /**< */
    MUCListModerators,              /**< */
    MUCListAdmins,                  /**< */
    MUCListOwners,                  /**< */
    MUCListMembers,                 /**< */
    MUCListBanned                   /**< */
  };

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API MUCRoomConfigHandler
  {
    public:
      /**
       * Virtual Destructor.
       */
      virtual ~MUCRoomConfigHandler() {};

      /**
       *
       */
      virtual void handleMUCConfigList( MUCRoom *room, const MUCListItemList& items, MUCListType type ) = 0;

      /**
       *
       */
      virtual void handleMUCConfigForm( MUCRoom *room, const DataForm *form ) = 0;

      /**
       *
       */
      virtual void handleMUCConfigResult( MUCRoom *room, bool success, MUCOperation operation ) = 0;

      /**
       *
       */
      virtual void handleMUCVoiceRequest( MUCRoom *room, const JID& requester, const DataForm *form ) = 0;

      /**
       *
       */
      virtual void handleMUCRegistrationRequest( MUCRoom *room, const JID& requester,
                                                 const DataForm *form ) = 0;

  };

}

#endif // MUCROOMCONFIGHANDLER_H__

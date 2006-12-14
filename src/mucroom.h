/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef MUCROOM_H__
#define MUCROOM_H__

#include "discohandler.h"
#include "disconodehandler.h"
#include "presencehandler.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "mucroomlistener.h"
#include "jid.h"

#include <string>

namespace gloox
{

  class ClientBase;
  class MUCMessageSession;
  class Stanza;

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API MUCRoom : private DiscoHandler, private PresenceHandler,
                            private IqHandler, private MessageHandler, private DiscoNodeHandler
  {
    public:
      /**
       * Allowable history request types. To disable sending of history, use any value except
       * HistoryUnknown and specify a zero-length time span (using setRequestHistory()).
       */
      enum HistoryRequestType
      {
        HistoryUnknown,             /**< It is up to the service to decide how much history to send.
                                     * This is the default. */
        HistoryMaxChars,            /**< Limit the total number of characters in the history to "X"
                                     * (where the character count is the characters of the complete
                                     * XML stanzas, not only their XML character data). */
        HistoryMaxStanzas,          /**< Limit the total number of messages in the history to "X". */
        HistorySeconds,             /**< Send only the messages received in the last "X" seconds. */
        HistorySince                /**< Send only the messages received since the datetime specified
                                     * (which MUST conform to the DateTime profile specified in Jabber
                                     * Date and Time Profiles (XEP-0082)). */
      };

      /**
       * Creates a new abstraction of a Multi-User Chat room. The room is not joined automatically.
       * Use join() to join the room, use leave() to leave it.
       * @param parent The ClientBase object to use for the communication.
       * @param nick The room's name and service plus the desired nickname in the form
       * room@service/nick.
       * @param mrl The MUCRoomListener that will listen to room events.
       */
      MUCRoom( ClientBase *parent, const JID& nick, MUCRoomListener *mrl );

      /**
       * Virtual Destructor.
       */
      virtual ~MUCRoom();

      /**
       * Use this function to set a password to use when joining a (password protected)
       * room.
       * @param password The password to use for this room.
       * @note This function does not password-protect a room.
       */
      void setPassword( const std::string& password ) { m_password = password; };

      /**
       * A convenience function that returns the room's name.
       * @return The room's name.
       */
      const std::string name() const { return m_nick.username(); };

      /**
       * A convenience function that returns the name/address of the MUC service the room is running on
       * (e.g., conference.jabber.org).
       * @return The MUC service's name/address.
       */
      const std::string service() const { return m_nick.server(); };

      /**
       * A convenience function that returns the user's nickname in the room.
       * @return The user's nickname.
       */
      const std::string nick() const { return m_nick.resource(); };

      /**
       * Join this room.
       */
      void join();

      /**
       * Leave this room.
       * @param msg An optional msg indicating the reason for leaving the room. Default: empty.
       */
      void leave( const std::string& msg = "" );

      /**
       * Sends a chat message to the room.
       * @param message The message to send.
       */
      void send( const std::string& message );

      /**
       * Sets the subject of the room to the given string.
       * The MUC service may decline the request to set a new subject. You should
       * not assume the subject was set successfully util it is acknowledged via the MUCRoomListener.
       * @param subject The new subject.
       */
      void setSubject( const std::string& subject );

      /**
       * Returns the user's current affiliation with this room.
       * @return The user's current affiliation.
       */
      MUCRoomAffiliation affiliation() const { return m_affiliation; };

      /**
       * Returns the user's current role in this room.
       * @return The user's current role.
       */
      MUCRoomRole role() const { return m_role; };

      /**
       *
       */
//       void setUnique( bool unique );

      /**
       * Use this function to change the user's nickname in the room.
       * The MUC service may decline the request to set a new nickname. You should not assume
       * the nick change was successful until it is acknowledged via the MUCRoomListener.
       * @param nick The user's new nickname.
       */
      void setNick( const std::string& nick );

      /**
       * Use this function to set the user's presence in this room. It is not possible to
       * use PresenceUnavailable with this function.
       * @param presence The user's new presence.
       * @param msg An optional status message. Default: empty.
       */
      void setPresence( Presence presence, const std::string& msg = "" );

      /**
       * Use this function to invite another user to this room.
       * @param invitee The (bare) JID of the user to invite.
       * @param reason The user-supplied reason for the invitation.
       * @param cont Whether this invitation is part of a transformation of a
       * one-to-one chat to a MUC. Default: false.
       */
      void invite( const JID& invitee, const std::string& reason, bool cont = false );

      /**
       * Use this function to request basic room info, possibly prior to joining it.
       * Results are announced using the MUCRoomListener.
       */
      void getRoomInfo();

      /**
       * Use this function to request information about the current room occupants,
       * possibly prior to joining it. The room ay be configured not to disclose such
       * information.
       * Results are announced using the MUCRoomListener.
       */
      void getRoomItems();

      /**
       * The MUC spec enables other entities to discover via Service Discovery which rooms
       * an entity is in. By default, gloox does not publish such info for privacy reasons.
       * This function can be used to enable publishing the info for @b this room.
       * @param publish Whether to enable other entities to discover the user's presence in
       * @b this room.
       * @param publishNick Whether to publish the nickname used in the room. This parameter
       * is ignored if @c publish is @b false.
       */
      void setPublish( bool publish, bool publishNick );

      /**
       * Use this function to register a (new) MUCRoomListener with this room. There can be only one
       * MUCRoomListener per room at any one time.
       * @param mrl The MUCRoomListener to register.
       */
      void registerMUCRoomListener( MUCRoomListener *mrl ) { m_roomListener = mrl; };

      /**
       * Use this function to add history to a (newly created) room. The use case from the MUC spec
       * is to add history to a room that was created in the process of a transformation of a
       * one-to-one chat to a multi-user chat.
       * @param message A reason for declining the invitation.
       * @param from The JID of the original author of this part of the history.
       * @param stamp The datetime of the original message in the format: 20061224T12:15:23
       * @note You should not attempt to use this function before
       * MUCRoomListener::handleMUCParticipantPresence() was called for the first time.
       */
      void addHistory( const std::string& message, const JID& from, const std::string& stamp );

      /**
       * Use this function to request room history. Set @c value to zero to disable the room
       * history request. You should not use HistorySince type with this function.
       * History is sent only once after entering a room. You should use this function before joining.
       * @param value Represents either the number of requested characters, the number of requested
       * message stanzas, or the number seconds, depending on the value of @c type.
       * @param type
       * @note If this function is not used to request a specific amount of room history, it is up
       * to the MUC service to decide how much history to send.
       */
      void setRequestHistory( int value, HistoryRequestType type );

      /**
       * Use this function to request room history since specific datetime.
       * History is sent only once after entering a room. You should use this function before joining.
       * @param since A string representing a datetime conforming to the DateTime profile specified
       * in Jabber Date and Time Profiles (XEP-0082).
       * @note If this function is not used to request a specific amount of room history, it is up
       * to the MUC service to decide how much history to send.
       */
      void setRequestHistory( const std::string& since );

      /**
       * This static function allows to formally decline a MUC invitation received via the
       * MUCInvitationListener.
       * @param message A reason for declining the invitation.
       * @param from The JID of the invitor.
       * @param
       */
      static Stanza* declineInvitation( const JID& room, const JID& invitee, const std::string& reason );

      /**
       * It is not possible for a visitor to speak in a moderated room. Use this function to request
       * voice from the moderator.
       */
      void requestVoice();

      /**
       * Use this function to kick a user from the room.
       * Depending on service and/or room configuration and role/affiliation
       * this may not always succeed. Usually, a role of 'moderator' is necessary.
       * @note This is a convenience function. It directly uses setRole() with a MUCRoomRole of RoleNone.
       * @param nick The nick of the user to be kicked.
       * @param reason An optional reason for the kick.
       */
      void kick( const std::string& nick, const std::string& reason = "" );

      /**
       * Use this function to ban a user from the room.
       * Depending on service and/or room configuration and role/affiliation
       * this may not always succeed. Usually, an affiliation of admin is necessary.
       * @note This is a convenience function. It directly uses setAffiliation() with a MUCRoomAffiliation
       * of RoleOutcast.
       * @param nick The nick of the user to be banned.
       * @param reason An optional reason for the ban.
       */
      void ban( const std::string& nick, const std::string& reason );

      /**
       * Use this function to grant voice to a user in a moderated room.
       * Depending on service and/or room configuration and role/affiliation
       * this may not always succeed. Usually, a role of 'moderator' is necessary.
       * @note This is a convenience function. It directly uses setRole() with a MUCRoomRole
       * of RoleParticipant.
       * @param nick The nick of the user to be granted voice.
       * @param reason An optional reason for the grant.
       */
      void grantVoice( const std::string& nick, const std::string& reason );

      /**
       * Use this function to revoke voice from a user in a moderated room.
       * Depending on service and/or room configuration and role/affiliation
       * this may not always succeed. Usually, a role of 'moderator' is necessary.
       * @note This is a convenience function. It directly uses setRole() with a MUCRoomRole
       * of RoleVisitor.
       * @param nick The nick of the user.
       * @param reason An optional reason for the revoke.
       */
      void revokeVoice( const std::string& nick, const std::string& reason );

      /**
       * Use this function to change the role of a user in the room.
       * Usually, at least moderator privileges are required to succeed.
       * @param nick The nick of the user who's role shall be modfified.
       * @param role The user's new role in the room.
       * @param reason An optional reason for the role change.
       */
      void setRole( const std::string& nick, MUCRoomRole role, const std::string& reason = "" );

      /**
       * Use this function to change the affiliation of a user in the room.
       * Usually, at least admin privileges are required to succeed.
       * @param nick The nick of the user who's affiliation shall be modfified.
       * @param role The user's new affiliation in the room.
       * @param reason An optional reason for the affiliation change.
       */
      void setAffiliation( const std::string& nick, MUCRoomAffiliation affiliation,
                           const std::string& reason );

      /**
       * Use this function to request the room's configuration form.
       * It can be used either after MUCRoomListener::handleMUCRoomCreation() was called,
       * or at any later time.
       *
       * Usually owner priviledges are required for this action to succeed.
       */
      void requestRoomConfig();

      /**
       * Use this function to accept the room's default configuration. This function is useful
       * only after MUCRoomListener::handleMUCRoomCreation() was called. This is a NOOP at
       * any other time.
       */
      void acknowledgeInstantRoom();

      /**
       * Use this function to cancel the creation of a room. This function is useful only after
       * MUCRoomListener::handleMUCRoomCreation() was called. This is a NOOP at any other time.
       */
      void cancelRoomCreation();

      /**
       * Use this function to destroy the room. All the occupants will be removed from the room.
       * @param reason An optional reason for the destruction.
       * @param alternate A pointer to a JID of an alternate venue (e.g., another MUC room).
       * May be 0.
       * @param password An optional password for the alternate venue.
       *
       * Usually owner priviledges are required for this action to succeed.
       */
      void destroy( const std::string& reason = "",
                    const JID* alternate = 0, const std::string& password = "" );

      // reimplemented from DiscoHandler
      virtual void handleDiscoInfoResult( Stanza *stanza, int context );

      // reimplemented from DiscoHandler
      virtual void handleDiscoItemsResult( Stanza *stanza, int context );

      // reimplemented from DiscoHandler
      virtual void handleDiscoError( Stanza *stanza, int context );

      // reimplemented from PresenceHandler
      virtual void handlePresence( Stanza *stanza );

      // reimplemented from MessageHandler
      virtual void handleMessage( Stanza *stanza );

      // reimplemented from IqHandler
      virtual bool handleIq( Stanza *stanza ) { return false; };

      // reimplemented from IqHandler
      virtual bool handleIqID( Stanza *stanza, int context );

      // reimplemented from DiscoNodeHandler
      virtual StringList handleDiscoNodeFeatures( const std::string& node );

      // reimplemented from DiscoNodeHandler
      virtual StringMap handleDiscoNodeIdentities( const std::string& node, std::string& name );

      // reimplemented from DiscoNodeHandler
      virtual StringMap handleDiscoNodeItems( const std::string& node = "" );

    private:
      bool handleIqGet( Stanza *stanza, int context );
      bool handleIqSet( Stanza *stanza, int context );
      bool handleIqResult( Stanza *stanza, int context );
      bool handleIqError( Stanza *stanza, int context );
      void setNonAnonymous();
      void setSemiAnonymous();
      void setFullyAnonymous();
      void modifyOccupant( const std::string& nick, int state, const std::string roa,
                           const std::string& reason );
      void acknowledgeRoomCreation();

      enum TrackEnum
      {
        RequestUniqueName,
        CreateInstantRoom,
        CancelRoomCreation,
        RequestRoomConfig,
        DestroyRoom,
        GetRoomInfo,
        GetRoomItems,
        SetRNone,
        SetVisitor,
        SetParticipant,
        SetModerator,
        SetANone,
        SetOutcast,
        SetMember,
        SetAdmin,
        SetOwner
      };

      ClientBase *m_parent;
      JID m_nick;
      MUCRoomListener *m_roomListener;
      MUCMessageSession *m_session;

      typedef std::list<MUCRoomParticipant> ParticipantList;
      ParticipantList m_participants;

      std::string m_password;
      std::string m_newNick;

      MUCRoomAffiliation m_affiliation;
      MUCRoomRole m_role;

      HistoryRequestType m_historyType;

      std::string m_historySince;
      int m_historyValue;
      int m_flags;
      bool m_creationInProgress;
      bool m_configChanged;
      bool m_publishNick;
      bool m_publish;
      bool m_joined;
      bool m_unique;

  };

}

#endif // MUCROOM_H__

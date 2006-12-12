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
       * Alloowable history request types. To disable sending of history, use any value except
       * HistoryUnknwon and specify a zero-length time span (using setRequestHistory()).
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
       * @param from The JID of the original author auf this part of the history.
       * @param stamp The datetime of the original message in the format: 20061224T12:15:23
       * @note You should not attempt to use this function before
       * MUCRoomListener::handleMUCParticipantPresence() was called for the first time.
       */
      void addHistory( const std::string& message, const JID& from, const std::string& stamp );

      /**
       * Use this function to request room history. Set @c value to zero to disable the room
       * history request. You should not use HistorySince with this function.
       * @param value Represents either the number of requested characters, the number of requested
       * message stanzas, or the number seconds, depending on the value of @c type.
       * @param type
       * @note If this function is not used to request a specific amount of room history, it is up
       * to the MUC service to decide how much history to send.
       */
      void setRequestHistory( int value, HistoryRequestType type );

      /**
       * Use this function to request room history since specific datetime.
       * @param since A string representing a datetime conforming to the DateTime profile specified
       * in Jabber Date and Time Profiles (XEP-0082).
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

      enum TrackEnum
      {
        RequestUniqueName,
        GetRoomInfo,
        GetRoomItems
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
      bool m_configChanged;
      bool m_publishNick;
      bool m_publish;
      bool m_joined;
      bool m_unique;

  };

}

#endif // MUCROOM_H__

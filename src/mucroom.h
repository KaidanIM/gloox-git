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
#include "presencehandler.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "mucroomlistener.h"
#include "jid.h"

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
  class MUCRoom : private DiscoHandler, private PresenceHandler, private IqHandler, private MessageHandler
  {
    public:
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
       *
       */
      void setPassword( const std::string& password ) { m_password = password; };

      /**
       * Join this room.
       */
      void join();

      /**
       * Leave this room.
       */
      void leave( const std::string& msg = "" );

      /**
       *
       */
      void send( const std::string& message );

      /**
       * Sets the subject of the room to the given string.
       * @param
       */
      void setSubject( const std::string& subject );

      /**
       *
       */
      MUCRoomAffiliation affiliation() const { return m_affiliation; };

      /**
       *
       */
      MUCRoomRole role() const { return m_role; };

      /**
       *
       */
      MUCRoomType type() const { return m_type; };

      /**
       *
       */
      bool publicLogging() const { return m_flags & FlagPublicLogging; };

      /**
       *
       */
      bool passwordProtected() const { return m_flags & FlagPasswordProtected; };

      /**
       *
       */
//       void setUnique( bool unique );

      /**
       *
       */
      void registerMUCRoomListener( MUCRoomListener *mrl ) { m_roomListener = mrl; };

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

    private:
      bool handleIqGet( Stanza *stanza, int context );
      bool handleIqSet( Stanza *stanza, int context );
      bool handleIqResult( Stanza *stanza, int context );
      bool handleIqError( Stanza *stanza, int context );

      enum TrackEnum
      {
        RequestUniqueName
      };

      ClientBase *m_parent;
      JID m_nick;
      MUCRoomListener *m_roomListener;
      MUCMessageSession *m_session;

      typedef std::list<MUCRoomParticipant> ParticipantList;
      ParticipantList m_participants;

      std::string m_password;

      MUCRoomAffiliation m_affiliation;
      MUCRoomRole m_role;
      MUCRoomType m_type;

      int m_flags;
      bool m_configChanged;
      bool m_joined;
      bool m_unique;

  };

}

#endif // MUCROOM_H__

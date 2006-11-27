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
#include "mucroomlistener.h"
#include "jid.h"

namespace gloox
{

  class ClientBase;
  class Stanza;

  /**
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class MUCRoom : public DiscoHandler, PresenceHandler
  {
    public:
      /**
       * Creates a new abstraction of a Multi-User Chat room. The room is not joined automatically.
       * Use join() to join the room, use leave() to leave it.
       * @param parent The ClientBase object to use for the communication.
       * @param nick The room's name and service plus the desired nickname in the form
       * room@service/nick.
       */
      MUCRoom( ClientBase *parent, const JID& nick, MUCRoomListener *mrl );

      /**
       * Virtual Destructor.
       */
      virtual ~MUCRoom();

      /**
       * Join this room.
       */
      void join();

      /**
       * Leave this room.
       */
      void leave();

      // reimplemented from DiscoHandler
      virtual void handleDiscoInfoResult( Stanza *stanza, int context );

      // reimplemented from DiscoHandler
      virtual void handleDiscoItemsResult( Stanza *stanza, int context );

      // reimplemented from DiscoHandler
      virtual void handleDiscoError( Stanza *stanza, int context );

      // reimplemented from PresenceHandler
      virtual void handlePresence( Stanza *stanza );

    private:
      ClientBase *m_parent;
      JID m_nick;
      MUCRoomListener *m_roomListener;

      typedef std::list<MUCRoomParticipant> ParticipantList;
      ParticipantList m_participants;

      bool m_joined;

  };

}

#endif // MUCROOM_H__

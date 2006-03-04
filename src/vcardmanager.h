/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef VCARDMANAGER_H__
#define VCARDMANAGER_H__

#include "gloox.h"
#include "iqhandler.h"

namespace gloox
{

  class ClientBase;
  class Disco;
  class VCard;
  class VCardHandler;

  /**
   * @brief A VCardManager can be used to fetch an entities VCard as well as for setting
   * one's own VCard.
   *
   * You need only one VCardManager per Client/ClientBase.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API VCardManager : public IqHandler
  {
    public:
      /**
       * Constructor.
       */
      VCardManager( ClientBase *parent, Disco *disco );

      /**
       * Virtual destructor.
       */
      virtual ~VCardManager();

      /**
       * Use this function to fetch the VCard of a remote entity or yourself.
       * The result will be announced by calling handleVCard() the VCardHandler.
       * @param jid The entity's JID.
       * @param vch The VCardHandler that will receive the result of the VCard fetch.
       */
      void fetchVCard( const JID& jid, VCardHandler *vch );

      /**
       * Use this function to store or update your own VCard on the server. Remember to
       * always send a full VCard, not a delta of changes.
       * If you, for any reason, pass a foreign VCard to this function, your own will be
       * overwritten.
       * @param vcard Your VCard to store.
       * @param vch The VCardHandler that will receive the result of the VCard store.
       */
      void storeVCard( const VCard *vcard, VCardHandler *vch );

      // reimplemented from IqHandler
      virtual bool handleIq( Stanza *stanza );

      // reimplemented from IqHandler
      virtual bool handleIqID( Stanza *stanza, int context );

    private:
      typedef std::map<std::string, VCardHandler*> TrackMap;
      ClientBase *m_parent;
      TrackMap m_trackMap;

  };

}

#endif // VCARDMANAGER_H__

/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef SIPROFILEFT_H__
#define SIPROFILEFT_H__

#include "siprofilehandler.h"
#include "sihandler.h"
#include "simanager.h"

#include <string>

namespace gloox
{

  class JID;
  class SIProfileFTHandler;

  /**
   * @brief An implementation of the file transfer SI profile (XEP-0096).
   *
   * An SIProfileFT object acts as a 'plugin' to the SIManager.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class SIProfileFT : public SIProfileHandler, public SIHandler
  {
    public:
      /**
       * Supported stream types.
       */
      enum StreamType
      {
        FTTypeS5B,                  /**< SOCKS5 Bytestreams. */
        FTTypeIBB,                  /**< In-Band Bytestreams. */
        FTTypeOOB                   /**< Out-of-Band Data. */
      };

      /**
       * Constructor.
       */
      SIProfileFT( SIManager* manager );

      /**
       * Virtual destructor.
       */
      virtual ~SIProfileFT();

      /**
       * Starts negotiating a file transfer with a remote entity.
       * @param to The entity to talk to.
       * @param name The file's name. Mandatory.
       * @param size The file's size. Mandatory.
       * @param hash The file's content's MD5 hash.
       * @param desc A description.
       * @param date The file's last modification date/time. See XEP-0082 for details.
       * @param mimetype The file's mime-type. Defaults to 'binary/octet-stream' if empty.
       */
      void requestFT( const JID& to, const std::string& name, int size, const std::string& hash,
                      const std::string& desc, const std::string& date, const std::string& mimetype );

      /**
       * Call this function to accept a file transfer request previously announced by means of
       * SIProfileFTHandler::handleFTRequest().
       * @param to The requestor.
       * @param id The request's id, as passed to SIProfileHandler::handleFTRequest().
       * @param type The desired stream type to use for this file transfer. Defaults to
       * SOCKS5 Bytestream.
       */
      void acceptFT( const JID& to, const std::string& id, StreamType type = FTTypeS5B );

      /**
       * Call this function to decline a FT request previously announced by means of
       * SIProfileFTHandler::handleFTRequest().
       * @param to The requestor.
       * @param id The request's id, as passed to SIProfileFTHandler::handleFTRequest().
       * @param reason The reason for the reject.
       * @param text An optional human-readable text explaining the decline.
       */
      void declineFT( const JID& to, const std::string& id, SIManager::SIError reason,
                      const std::string& text = "" );

      /**
       * Registers a handler that will be informed about incoming file transfer requests,
       * i.e. when a remote entity wishes to send a file.
       * @param sipfth A SIProfileFTHandler to register. Only one handler can be registered
       * at any one time.
       */
      void registerSIProfileFTHandler( SIProfileFTHandler* sipfth ) { m_handler = sipfth; }

      /**
       * Removes the previously registered file transfer request handler.
       */
      void removeSIProfileFTHandler() { m_handler = 0; }

      // re-implemented from SIProfileHandler
      virtual void handleSIRequest( const JID& from, const std::string& id, const std::string& profile,
                                    Tag* si, Tag* ptag, Tag* fneg );

      // re-implemented from SIHandler
      virtual void handleSIRequestResult( const JID& from, const std::string& sid,
                                          Tag* si, Tag* ptag, Tag* fneg );

      // re-implemented from SIHandler
      virtual void handleSIRequestError( Stanza* stanza );

    private:
      SIManager* m_parent;
      SIProfileFTHandler* m_handler;

  };

}

#endif // SIPROFILEFT_H__

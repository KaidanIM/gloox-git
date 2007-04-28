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
#include "socks5bytestreamhandler.h"

#include <string>

namespace gloox
{

  class ClientBase;
  class JID;
  class SIProfileFTHandler;
  class SOCKS5Bytestream;
  class SOCKS5BytestreamManager;

  /**
   * @brief An implementation of the file transfer SI profile (XEP-0096).
   *
   * An SIProfileFT object acts as a 'plugin' to the SIManager. SIProfileFT
   * manages most of the file transfer functionality.
   *
   * Usage:
   *
   * Create a new SIProfileFT object.It needs a ClientBase -derived object as well as a
   * SIProfileFTHandler -derived object that will receive file transfer-related events.
   * The naming comes from the fact that File Transfer (FT) is a profile of Stream Initiation (SI).
   * If you already use SI and the SIManager somewhere else, you should pass a pointer to that
   * SIManager object as third parameter to SIProfileFT's constructor.
   * @code
   * class MyFileTransferHandler : public SIProfileFTHandler
   * {
   *   // ...
   * };
   *
   * Client* client = new Client( ... );
   * // ...
   * MyFileTransferHandler* mh = new MyFileTransferHandler( ... );
   *
   * SIProfileFT* ft = new SIProfileFT( client, mh );
   * @endcode
   *
   * You are now, basically, ready to send and receive files.
   *
   * A couple of notes:
   * @li To be able to send files, you will need access to a SOCKS5 bytestream proxy (called
   * StreamHost)(not an ordinary SOCKS5 proxy). You should use Disco to query it for its host
   * and port and feed that information into SIProfileFT:
   *
   * @code
   * ft->addStreamHost( JID( "proxy.jabber.org" ), "101.102.103.104", 666 );
   * @endcode
   *
   * You should @b not hard-code this information (esp. host/IP and port) into your app since
   * the proxy may go down occasionally or vanish completely.
   *
   * @li When you finally receive a SOCKS5Bytestream via the SIProfileFTHandler, you will need
   * to integrate this bytestream with your mainloop, or put it into a separate thread (if
   * occasional blocking is not acceptable). You will need to call
   * @link gloox::SOCKS5Bytestream::connect() SOCKS5Bytestream::connect() @endlink.
   * @link gloox::SOCKS5Bytestream::connect() connect() @endlink will try to connect to each of the
   * given StreamHosts and block until it has established a connection with one of the them (or
   * until all attempts failed). Further, if you want to receive a file via the bytestream, you will
   * have to call recv() on the object from time to time.
   *
   * @li Do @b not delete a SOCKS5Bytestream manually. Use dispose() for this.
   *
   * When cleaning up, delete the objectes you created above in the opposite order of
   * creation:
   *
   * @code
   * delete ft;
   * delete client;
   * @endcode
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class SIProfileFT : public SIProfileHandler, public SIHandler, public SOCKS5BytestreamHandler
  {
    public:
      /**
       * Supported stream types.
       */
      enum StreamType
      {
        FTTypeS5B     = 1/*,*/      /**< SOCKS5 Bytestreams. */
//        FTTypeIBB   = 2,          /**< In-Band Bytestreams. */
//        FTTypeOOB   = 4           /**< Out-of-Band Data. */
      };

      /**
       * Constructor.
       * @param parent The ClientBase to use for signaling.
       * @param sipfth The SIProfileFTHandler to receive events.
       * @param manager An optional SIManager to register with. If this is zero, SIProfileFT
       * will create its own SIManager. You should pass a valid SIManager here if you are
       * already using one with the @c parent ClientBase above.
       * @param s5Manager An optional SOCKS5BytestreamManager to use. If this is zero, SIProfileFT
       * will create its own SOCKS5BytestreamManager. You should pass a valid SOCKS5BytestreamManager
       * here if you are already using one with the @c parent ClientBase above.
       * @note If you passed a SIManager and/or SOCKS5BytestreamManager to SIProfileFT's constructor,
       * these objects will @b not be deleted on desctruction of SIProfileFT.
       */
      SIProfileFT( ClientBase* parent, SIProfileFTHandler* sipfth, SIManager* manager = 0,
                   SOCKS5BytestreamManager* s5Manager = 0 );

      /**
       * Virtual destructor.
       */
      virtual ~SIProfileFT();

      /**
       * Starts negotiating a file transfer with a remote entity.
       * @param to The entity to talk to.
       * @param name The file's name. Mandatory.
       * @param size The file's size. Mandatory and must be > 0.
       * @param hash The file content's MD5 hash.
       * @param desc A description.
       * @param date The file's last modification date/time. See XEP-0082 for details.
       * @param mimetype The file's mime-type. Defaults to 'binary/octet-stream' if empty.
       */
      void requestFT( const JID& to, const std::string& name, int size, const std::string& hash = "",
                      const std::string& desc = "", const std::string& date = "",
                      const std::string& mimetype = "" );

      /**
       * Call this function to accept a file transfer request previously announced by means of
       * @link gloox::SIProfileFTHandler::handleFTRequest() SIProfileFTHandler::handleFTRequest() @endlink.
       * @param to The requestor.
       * @param id The request's id, as passed to SIProfileHandler::handleFTRequest().
       * @param type The desired stream type to use for this file transfer. Defaults to
       * SOCKS5 Bytestream.
       */
      void acceptFT( const JID& to, const std::string& id, StreamType type = FTTypeS5B );

      /**
       * Call this function to decline a FT request previously announced by means of
       * @link gloox::SIProfileFTHandler::handleFTRequest() SIProfileFTHandler::handleFTRequest() @endlink.
       * @param to The requestor.
       * @param id The request's id, as passed to SIProfileFTHandler::handleFTRequest().
       * @param reason The reason for the reject.
       * @param text An optional human-readable text explaining the decline.
       */
      void declineFT( const JID& to, const std::string& id, SIManager::SIError reason,
                      const std::string& text = "" );

      /**
       * To get rid of a bytestream (i.e., close and delete it), call this function. You
       * should not use the bytestream any more.
       * The remote entity will be notified about the closing of the stream.
       * @param s5b The bytestream to dispose. It will be deleted here.
       */
      void dispose( SOCKS5Bytestream *s5b );

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

      /**
       * Sets a list of StreamHosts that will be used for subsequent SOCKS5 bytestream requests.
       * @note At least one StreamHost is required.
       * @param hosts A list of StreamHosts.
       */
      void setStreamHosts( StreamHostList hosts );

      /**
       * Adds one StreamHost to the list of SOCKS5 StreamHosts.
       * @param jid The StreamHost's JID.
       * @param host The StreamHost's hostname.
       * @param port The StreamHost's port.
       */
      void addStreamHost( const JID& jid, const std::string& host, int port );

      // re-implemented from SIProfileHandler
      virtual void handleSIRequest( const JID& from, const std::string& id, const std::string& profile,
                                    Tag* si, Tag* ptag, Tag* fneg );

      // re-implemented from SIHandler
      virtual void handleSIRequestResult( const JID& from, const std::string& sid,
                                          Tag* si, Tag* ptag, Tag* fneg );

      // re-implemented from SIHandler
      virtual void handleSIRequestError( Stanza* stanza );

      // re-implemented from SOCKS5BytestreamHandler
      virtual void handleIncomingSOCKS5BytestreamRequest( const std::string& sid, const JID& from );

      // re-implemented from SOCKS5BytestreamHandler
      virtual void handleIncomingSOCKS5Bytestream( const std::string& sid, SOCKS5Bytestream* s5b );

      // re-implemented from SOCKS5BytestreamHandler
      virtual void handleOutgoingSOCKS5Bytestream( const JID& to, SOCKS5Bytestream *s5b );

      // re-implemented from SOCKS5BytestreamHandler
      virtual void handleSOCKS5BytestreamError( const JID& remote, StanzaError se );

    private:
      ClientBase* m_parent;
      SIManager* m_manager;
      SIProfileFTHandler* m_handler;
      SOCKS5BytestreamManager* m_socks5Manager;
      StreamHostList m_hosts;
      bool m_delManager;
      bool m_delS5Manager;

  };

}

#endif // SIPROFILEFT_H__

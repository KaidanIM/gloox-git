/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef BYTESTREAM_H__
#define BYTESTREAM_H__

#include "jid.h"
#include "logsink.h"

#include <string>

namespace gloox
{

  class BytestreamDataHandler;

  /**
   * @brief An abstraction of a single bytestream.
   *
   * Used as a base class for InBand Bytestreams as well as SOCKS5 Bytestreams.
   * You should not need to use this class directly.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class GLOOX_API Bytestream
  {
    public:
      /**
       *
       */
      enum StreamType
      {
        S5B,                      /**< SOCKS5 Bytestream */
        IBB                       /**< In-Band Bytestream */
      };

      /**
       *
       */
      Bytestream( StreamType type, LogSink& logInstance, const JID& initiator, const JID& target,
                  const std::string& sid )
      : m_handler( 0 ), m_logInstance( logInstance ), m_initiator( initiator ), m_target( target ),
        m_type( type ), m_sid( sid ), m_open( false )
        {}

      /**
       * Virtual destructor.
       */
      virtual ~Bytestream() {}

      /**
       * Returns whether the bytestream is open, that is, accepted by both parties.
       * @return Whether the bytestream is open or not.
       */
      bool isOpen() const { return m_open; }

      /**
       * This function starts the connection process. That is, it attempts to connect
       * to each of the available StreamHosts. Once a working StreamHosts is found, the
       * SOCKS5BytestreamManager is notified and the function returns.
       * @return @b True if a connection to a StreamHost could be established, @b false
       * otherwise.
       * @note If @b false is returned you should hand this SOCKS5Bytestream object
       * to SOCKS5BytestreamManager::dispose() for deletion.
       * @note Make sure you have a SOCKS5BytestreamDataHandler registered (using
       * registerSOCKS5BytestreamDataHandler()) before calling this function.
       */
      virtual bool connect() = 0;

      /**
       * Closes the bytestream.
       */
      virtual void close() = 0;

      /**
       * Use this function to send a chunk of data over an open bytestream.
       * If the stream is not open or has been closed again
       * (by the remote entity or locally), nothing is sent and @b false is returned.
       * This function does any base64 encoding for you, if necessary.
       * @param data The block of data to send.
       * @return @b True if the data has been sent (no guarantee of receipt), @b false
       * in case of an error.
       */
      virtual bool send( const std::string& data ) = 0;

      /**
       * Call this function repeatedly to receive data from the socket. You should even do this
       * if you use the bytestream to merely @b send data.
       * @param timeout The timeout to use for select in microseconds. Default of -1 means blocking.
       * @return The state of the connection.
       */
      virtual ConnectionError recv( int timeout = -1 ) = 0;

      /**
       * Lets you retrieve the stream's ID.
       * @return The stream's ID.
       */
      const std::string& sid() const { return m_sid; }

      /**
       * Returns the stream's type.
       * @return The stream's type.
       */
      StreamType type() const { return m_type; }

      /**
       * Returns the target entity's JID. If this bytestream is remote-initiated, this is
       * the local JID. If it is local-initiated, this is the remote entity's JID.
       * @return The target's JID.
       */
      const JID& target() const { return m_target; }

      /**
       * Returns the initiating entity's JID. If this bytestream is remote-initiated, this is
       * the remote entity's JID. If it is local-initiated, this is the local JID.
       * @return The initiator's JID.
       */
      const JID& initiator() const { return m_initiator; }

      /**
       * Use this function to register an object that will receive any notifications from
       * the Bytestream instance. Only one BytestreamDataHandler can be registered
       * at any one time.
       * @param bbdh The BytestreamDataHandler-derived object to receive notifications.
       */
      void registerBytestreamDataHandler( BytestreamDataHandler* bdh )
        { m_handler = bdh; }

      /**
       * Removes the registered BytestreamDataHandler.
       */
      void removeBytestreamDataHandler()
        { m_handler = 0; }

    protected:
      void setSid( const std::string& sid ) { m_sid = sid; }

      BytestreamDataHandler *m_handler;
      const LogSink& m_logInstance;
      const JID m_initiator;
      const JID m_target;
      StreamType m_type;
      std::string m_sid;
      bool m_open;

  };

}

#endif // BYTESTREAM_H__

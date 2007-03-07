/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef CONNECTIONBASE_H__
#define CONNECTIONBASE_H__

#ifdef WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#include "gloox.h"
#include "connectiondatahandler.h"

namespace gloox
{

  /**
   * @brief An abstract base class for a connection.
   *
   * You should not need to use this class directly.
   *
   * @author Jakob Schröter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API ConnectionBase
  {
    public:
      /**
       * Constructor.
       * @param cdh An object derived from @ref ConnectionDataHandler that will receive
       * received data.
       */
      ConnectionBase( ConnectionDataHandler *cdh )
        : m_handler( cdh ), m_state( StateDisconnected )
      {};

      /**
       * Virtual destructor.
       */
      virtual ~ConnectionBase() { cleanup(); };

      /**
       * Used to initiate the connection.
       * @return Returns the connection state.
       */
      virtual ConnectionError connect() = 0;

      /**
       * Use this periodically to receive data from the socket and to feed the parser.
       * @param timeout The timeout to use for select in microseconds. Default of -1 means blocking.
       * @return The state of the connection.
       */
      virtual ConnectionError recv( int timeout = -1 ) = 0;

      /**
       * Use this function to send a string of data over the wire. The function returns only after
       * all data has been sent.
       * @param data The data to send.
       */
      virtual bool send( const std::string& data ) = 0;

      /**
       * Use this function to put the connection into 'receive mode', i.e. this function returns only
       * when the connection is terminated.
       * @return Returns a value indicating the disconnection reason.
       */
      virtual ConnectionError receive() = 0;

      /**
       * Disconnects an established connection. NOOP if no active connection exists.
       */
      virtual void disconnect() = 0;

      /**
       * This function is called after a disconnect to clean up internal state. It is also called by
       * ConnectionBase's destructor.
       */
      virtual void cleanup() {};

      /**
       * Returns the current connection state.
       * @return The state of the connection.
       */
      ConnectionState state() const { return m_state; };

    protected:
      ConnectionDataHandler *m_handler;
      ConnectionState m_state;

  };

}

#endif // CONNECTIONBASE_H__

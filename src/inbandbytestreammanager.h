/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef INBANDBYTESTREAMMANAGER_H__
#define INBANDBYTESTREAMMANAGER_H__

#include "iqhandler.h"
#include "disco.h"
#include "jid.h"

namespace gloox
{

  class InBandBytestreamHandler;

  /**
   * @brief An InBandBytestreamManager dispatches In-Band Bytestreams.
   *
   * @section init_ibb Initiating a bytestream
   *
   * To initiate a new bytestream, you need an InBandBytestreamManager object. You will
   * also need an InBandBytestreamHandler-derived object which will receive incoming and
   * outgoing bytestreams (not the data but the InBandBytestream objects).
   * @code
   * class MyClass : public InBandBytestreamHandler
   * {
   *   ...
   *   private:
   *     InBandBytestreamManager *m_ibbManager;
   *   ...
   * };
   * @endcode
   *
   * Create a new InBandBytestreamManager, register an InBandBytestreamHandler (MyClass in this
   * example) and request a new bytestream:
   * @code
   * MyClass::MyClass()
   * {
   *   m_ibbManager = new InBandBytestreamManager( m_client, m_client->disco() );
   *   m_ibbManager->registerInBandBytestreamHandler( this );
   * }
   *
   * void MyClass::myFunc()
   * {
   *   m_ibbManager->requestInBandBytestream( "entity@server/resource" );
   * }
   * @endcode
   *
   * After the bytestream has been negotiated with the peer,
   * InBandBytestreamHandler::handleOutgoingInBandBytestream() is called. Here you should
   * attach the bytestream to a MessageSession associated with the remote entity.
   * In this example, there is a map of JID/MessageSession pairs and a map of
   * JID/InBandBytestreams.
   * @code
   * void MyClass::handleOutgoingInBandBytestream( const JID& to, InBandBytestream *ibb )
   * {
   *   MessageSessionList::iterator it = m_messageSessions.find( to.full() );
   *   if( it != m_messageSessions.end() )
   *   {
   *     ibb->attachTo( (*it).second );
   *   }
   *   else
   *   {
   *     MessageSession *session = new MessageSession( m_client, to );
   *     ibb->attachTo( session );
   *     m_messageSessions[to.full()] = session;
   *   }
   *
   *   m_ibbs[to.full()] = ibb;
   * }
   * @endcode
   *
   * When sending data you should make sure you never try to send a block larger than the
   * negotiated blocksize (which defaults to 4096 bytes). If a block is larger it will not
   * be sent.
   *
   * @section recv_ibb Receiving a bytestream
   *
   * To receive a bytestream you need a InBandBytestreamManager, too, and you have to
   * register an InBandBytestreamHandler similar to the example above.
   *
   * Upon an incoming request the InBandBytestreamManager notifies the registered
   * InBandBytestreamHandler by calling
   * @link InBandBytestreamHandler::handleIncomingInBandBytestream() handleIncomingInBandBytestream() @endlink.
   * The return value of the handler determines whether the stream shall be accepted or not.
   * @code
   * void MyClass::handleIncomingInBandBytestream( const JID& from, InBandBytestream *ibb )
   * {
   *   // Check whether you want to accept the bytestream
   *
   *   // The rest of this function probaly looks similar to the implementation of
   *   // handleOutgoingInBandBytestream() above.
   *   // You should *not* start to send blocks of data from within this
   *   // function, though.
   *
   *   // return true to accept the bytestream, false to reject it
   *   return true;
   * }
   * @endcode
   *
   * To actually send data, you should utilise some kind of mainloop integration that allows
   * to call a function periodically. It is important to remember that:
   * @li chunks of data passed to InBandBytestream::sendBlock() may not exceed the negotiated block-size
   * @li neither InBandBytestreamManager nor InBandBytestream will ask for data to send.
   *
   * The following is an example for a primitive mainloop integration:
   * @code
   * void MyClass::mainloop()
   * {
   *   if( m_client->connect(false) )
   *   {
   *     ConnectionError ce = ConnNoError;
   *     while( ce == ConnNoError )
   *     {
   *       ce = m_client->recv();
   *       sendIBBData();
   *     }
   *     printf( "disconnected. reason: %d\n", ce );
   *   }
   * }
   * @endcode
   *
   * In sendIBBData() you would then iterate over your bytestreams and send a block of data
   * where appropriate.
   * @code
   * void MyClass::sendIBBData()
   * {
   *   IBBList::iterator it = m_ibbs.begin();
   *   for( ; it != m_ibbs.end(); ++it )
   *   {
   *     (*it).second->sendBlock( "some data" );
   *   }
   * }
   * @endcode
   *
   * @note You should have only one InBandBytestreamManager per Client/ClientBase lying around.
   * One is enough for receiving and initiating bytestreams.
   *
   * @note In the excerpts above, only one bytestream per remote entity is possible (without leaking).
   * However, gloox in general does not impose such a limitation, nor does the In-Band Bytestreams
   * specification.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_API InBandBytestreamManager : public IqHandler
  {
    public:
      /**
       * Constructs a new InBandBytestreamManager.
       * @param parent The ClientBase to use for sending data.
       * @param disco The Disco object to announce the IBB feature with.
       */
      InBandBytestreamManager( ClientBase *parent, Disco *disco );

      /**
       * Virtual destructor.
       */
      virtual ~InBandBytestreamManager();

      /**
       * This function initiates opening of a bytestream with the MessageSession's remote entity.
       * Data can only be sent over an open stream. Use open() to find out what the stream's
       * current state is. However, successful opening/initiation will be announced by means of the
       * InBandBytestreamHandler interface. Only one byte stream per JID can be initiated at once,
       * however, once a byte stream has been accepted (or declined), another one can be opened for
       * that JID immediately. There is no such restriction for multiple bytestreams to different
       * JIDs.
       * @param to The recipient of the requested bytestream.
       * @return @b False in case of an error, @b true otherwise. A return value of @b true does
       * @b not indicate that the bytestream has been opened. This is announced by means of the
       * InBandBytestreamHandler.
       */
      bool requestInBandBytestream( const JID& to );

      /**
       * Sets the default block-size. Default: 4096
       * @param blockSize The default block-size in byte.
       */
      void setBlockSize( int blockSize ) { m_blockSize = blockSize; };

      /**
       * Returns the currently set block-size.
       * @return The currently set block-size.
       */
      int blockSize() const { return m_blockSize; };

      /**
       * Use this function to register an object that will receive new incoming bytestream requests
       * as well as requested outgoing bytestreams from the InBandBytestreamManager. Only one
       * InBandBytestreamHandler can be registered at any one time.
       * @param ibbh The InBandBytestreamHandler derived object to receive notifications.
       */
      void registerInBandBytestreamHandler( InBandBytestreamHandler *ibbh );

      /**
       * Removes the registered InBandBytestreamHandler.
       */
      void removeInBandBytestreamHandler();

      // reimplemented from IqHandler
      virtual bool handleIq( Stanza *stanza );

      // reimplemented from IqHandler
      virtual bool handleIqID( Stanza *stanza, int context );

    private:
      enum IBBActionType
      {
        IBB_OPEN_STREAM,
        IBB_CLOSE_STREAM
      };

      ClientBase *m_parent;
      InBandBytestreamHandler *m_inbandBytestreamHandler;
      int m_blockSize;
      StringMap m_trackMap;

  };

}

#endif // INBANDBYTESTREAMMANAGER_H__

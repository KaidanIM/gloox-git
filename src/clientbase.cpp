/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/



#include "clientbase.h"

#include "jthread.h"

namespace gloox
{

  ClientBase::ClientBase( const std::string& ns )
    : Stream( ns ),
      m_port( -1 ), m_thread( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
    init();
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password, int port )
    : Stream( ns ),
      m_password( password ), m_port( port ), m_thread( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
    init();
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password,
                          const std::string& server, int port )
    : Stream( ns ),
      m_server( server ), m_port( port ), m_thread( 0 ), m_password( password ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
    init();
  }

  ClientBase::~ClientBase()
  {
  }

  void ClientBase::init()
  {
    setupFilter();
  }

  void ClientBase::connect( bool blocking )
  {
    if( server().empty() )
      return;

    m_blockingConnect = blocking;

    m_state = STATE_CONNECTING;

    m_thread = new JThread( this );
    m_thread->start();

    if( !username().empty() && !password().empty() )
    {
      while( m_state >= STATE_CONNECTED &&
            m_state != STATE_AUTHENTICATED &&
            m_state != STATE_AUTHENTICATION_FAILED )
      {
        JThread::sleep( 1000 );
      }

      if ( m_state == STATE_AUTHENTICATION_FAILED )
      {
        notifyOnDisconnect();
      }
    }

    if( m_blockingConnect )
    {
      m_thread->join();
      cleanUp();
    }
  }

  void ClientBase::disconnect()
  {
    if( m_state != STATE_DISCONNECTED )
    {
      m_state = STATE_DISCONNECTED;
      m_thread->cancel();
      Stream::disconnect();

      if( !m_blockingConnect )
      {
        m_thread->join();
        cleanUp();
      }
    }
  }

  std::string ClientBase::getID()
  {
    char tmp[10];
    sprintf( tmp, "uid%d", ++m_idCount );
    return ( tmp );
  }

  void ClientBase::send( iks* x )
  {
    Stream::send( x );
    iks_delete( x );
  }

  iksparser* ClientBase::parser()
  {
    return this->P;
  }

  void ClientBase::cleanUp()
  {
    iks_filter_delete( m_filter );
    delete m_thread;
  }


  void ClientBase::setupFilter()
  {
    m_filter = iks_filter_new();

    iks_filter_add_rule( m_filter, (iksFilterHook*) msgHook, this,
                        IKS_RULE_TYPE, IKS_PAK_MESSAGE,
                        IKS_RULE_DONE );
    iks_filter_add_rule( m_filter, (iksFilterHook*) presenceHook, this,
                        IKS_RULE_TYPE, IKS_PAK_PRESENCE,
                        IKS_RULE_DONE );
    iks_filter_add_rule( m_filter, (iksFilterHook*) subscriptionHook, this,
                        IKS_RULE_TYPE, IKS_PAK_S10N,
                        IKS_RULE_DONE );
    iks_filter_add_rule( m_filter, (iksFilterHook*) iqHook, this,
                        IKS_RULE_TYPE, IKS_PAK_IQ,
                        IKS_RULE_DONE );
  }

  void ClientBase::on_log( const char* data, size_t size, int is_incoming )
  {
    if( m_debug )
    {
      if ( is_secure() )
        printf( "Sec" );

      if (is_incoming)
        printf( "RECV " );
      else
        printf( "SEND " );

      printf( "[%s]", data );
      if( strncmp( &data[size-1], "\n", 1 ) != 0 )
        printf( "\n" );
    }
  }

  void ClientBase::registerPresenceHandler( PresenceHandler* ph )
  {
    m_presenceHandlers.push_back( ph );
  }

  void ClientBase::removePresenceHandler( PresenceHandler* ph )
  {
    m_presenceHandlers.remove( ph );
  }

  void ClientBase::registerIqHandler( IqHandler* ih, const char* xmlns )
  {
    m_iqNSHandlers[xmlns] = ih;
  }

  void ClientBase::trackID( IqHandler* ih, const std::string& id, int context )
  {
    TrackStruct *track;
    track->ih = ih;
    track->context = context;
    m_iqIDHandlers[id] = track;
  }

  void ClientBase::removeIqHandler( const char* xmlns )
  {
    m_iqNSHandlers.erase( xmlns );
  }

  void ClientBase::registerMessageHandler( MessageHandler* mh )
  {
    m_messageHandlers.push_back( mh );
  }

  void ClientBase::removeMessageHandler( MessageHandler* mh )
  {
    m_messageHandlers.remove( mh );
  }

  void ClientBase::registerSubscriptionHandler( SubscriptionHandler* sh )
  {
    m_subscriptionHandlers.push_back( sh );
  }

  void ClientBase::removeSubscriptionHandler( SubscriptionHandler* sh )
  {
    m_subscriptionHandlers.remove( sh );
  }

  void ClientBase::registerConnectionListener( ConnectionListener* cl )
  {
    m_connectionListeners.push_back( cl );
  }

  void ClientBase::removeConnectionListener( ConnectionListener* cl )
  {
    m_connectionListeners.remove( cl );
  }

  void ClientBase::notifyOnConnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onConnect();
    }
  }

  void ClientBase::notifyOnDisconnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onDisconnect();
    }
  }

  void ClientBase::notifyPresenceHandlers( iksid* from, iksubtype type, ikshowtype show, const char* msg )
  {
    PresenceHandlerList::const_iterator it = m_presenceHandlers.begin();
    for( it; it != m_presenceHandlers.end(); it++ )
    {
      (*it)->handlePresence( from, type, show, msg );
    }
  }

  void ClientBase::notifySubscriptionHandlers( iksid* from, iksubtype type, const char* msg )
  {
    SubscriptionHandlerList::const_iterator it = m_subscriptionHandlers.begin();
    for( it; it != m_subscriptionHandlers.end(); it++ )
    {
      (*it)->handleSubscription( from, type, msg );
    }
  }

  void ClientBase::notifyIqHandlers( const char* xmlns, ikspak* pak )
  {
    IqHandlerMap::const_iterator it_ns = m_iqNSHandlers.begin();
    for( it_ns; it_ns != m_iqNSHandlers.end(); ++it_ns )
    {
      if( iks_strncmp( (*it_ns).first.c_str(), xmlns, (*it_ns).first.length() ) == 0 )
      {
        char* tag = iks_name( iks_first_tag( pak->x ) );
        (*it_ns).second->handleIq( tag, xmlns, pak );
      }
    }

    IqTrackMap::const_iterator it_id = m_iqIDHandlers.begin();
    for( it_id; it_id != m_iqIDHandlers.end(); ++it_id )
    {
      if( iks_strncmp( (*it_id).first.c_str(), pak->id, (*it_id).first.length() ) == 0 )
      {
        (*it_id).second->ih->handleIqID( pak->id, pak, (*it_id).second->context );
        m_iqIDHandlers.erase( pak->id );
      }
    }
  }

  void ClientBase::notifyMessageHandlers( iksid* from, iksubtype type, const char* msg )
  {
    MessageHandlerList::const_iterator it = m_messageHandlers.begin();
    for( it; it != m_messageHandlers.end(); it++ )
    {
      (*it)->handleMessage( from, type, msg );
    }
  }

  int msgHook( ClientBase* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("msgHook\n");
    stream->notifyMessageHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "body" ) );
    return IKS_FILTER_EAT;
  }

  int iqHook( ClientBase* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("iqHook\n");
    stream->notifyIqHandlers( pak->ns, pak );
    return IKS_FILTER_EAT;
  }

  int presenceHook( ClientBase* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("presenceHook\n");
    stream->notifyPresenceHandlers( pak->from, pak->subtype, pak->show, iks_find_cdata( pak->x, "status" ) );
    return IKS_FILTER_EAT;
  }

  int subscriptionHook( ClientBase* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("subscriptionHook\n");
    stream->notifySubscriptionHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "status" ) );
    return IKS_FILTER_EAT;
  }

  int errorHook( ClientBase* stream, ikspak* pak )
  {
    if( stream->debug() ) printf("errorHook\n");
    return IKS_FILTER_EAT;
  }

};

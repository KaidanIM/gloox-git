/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "flexoff.h"
#include "dataform.h"
#include "disco.h"
#include "error.h"

#include <cstdlib>

namespace gloox
{

  FlexibleOffline::FlexibleOffline( ClientBase* parent )
    : m_parent( parent ), m_flexibleOfflineHandler( 0 )
  {
  }

  FlexibleOffline::~FlexibleOffline()
  {
  }

  void FlexibleOffline::checkSupport()
  {
    m_parent->disco()->getDiscoInfo( m_parent->jid().server(), EmptyString, this, FOCheckSupport );
  }

  void FlexibleOffline::getMsgCount()
  {
    m_parent->disco()->getDiscoInfo( m_parent->jid().server(), XMLNS_OFFLINE, this, FORequestNum );
  }

  void FlexibleOffline::fetchHeaders()
  {
    m_parent->disco()->getDiscoItems( m_parent->jid().server(), XMLNS_OFFLINE, this, FORequestHeaders );
  }

  void FlexibleOffline::messageOperation( int context, const StringList& msgs )
  {
    const std::string& id = m_parent->getID();
    IQ::IqType iqType = context == FORequestMsgs ? IQ::Get : IQ::Set;
    IQ* iq = new IQ( iqType, JID(), id, XMLNS_OFFLINE, "offline" );
    Tag* o = iq->query();

    if( msgs.empty() )
      new Tag( o, context == FORequestMsgs ? "fetch" : "purge" );
    else
    {
      const std::string action = context == FORequestMsgs ? "view" : "remove";
      StringList::const_iterator it = msgs.begin();
      for( ; it != msgs.end(); ++it )
      {
        Tag* i = new Tag( o, "item", "action", action );
        i->addAttribute( "node", (*it) );
      }
    }

    m_parent->trackID( this, id, context );
    m_parent->send( iq );
  }

  void FlexibleOffline::registerFlexibleOfflineHandler( FlexibleOfflineHandler* foh )
  {
    m_flexibleOfflineHandler = foh;
  }

  void FlexibleOffline::removeFlexibleOfflineHandler()
  {
    m_flexibleOfflineHandler = 0;
  }

  void FlexibleOffline::handleDiscoInfoResult( IQ* iq, int context )
  {
    if( !m_flexibleOfflineHandler )
      return;

    switch( context )
    {
      case FOCheckSupport:
        m_flexibleOfflineHandler->handleFlexibleOfflineSupport(
            iq->query()->hasChild( "feature", "var", XMLNS_OFFLINE ) );
        break;

      case FORequestNum:
        int num = -1;
        DataForm f( iq->query()->findChild( "x" ) );
        if( f.hasField( "number_of_messages" ) )
          num = atoi( f.field( "number_of_messages" )->value().c_str() );

        m_flexibleOfflineHandler->handleFlexibleOfflineMsgNum( num );
        break;
    }
  }

  void FlexibleOffline::handleDiscoItemsResult( IQ* iq, int context )
  {
    if( context == FORequestHeaders && m_flexibleOfflineHandler )
    {
      Tag* q = iq->query();
      if( q && q->hasAttribute( XMLNS, XMLNS_DISCO_ITEMS ) && q->hasAttribute( "node", XMLNS_OFFLINE ) )
      {
        StringMap m;
        const TagList& l = q->children();
        TagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
        {
          m[(*it)->findAttribute( "node" )] = (*it)->findAttribute( "name" );
        }
        m_flexibleOfflineHandler->handleFlexibleOfflineMessageHeaders( m );
      }
    }
  }

  void FlexibleOffline::handleDiscoError( IQ * /*iq*/, int /*context*/ )
  {
  }

  void FlexibleOffline::handleIqID( IQ* iq, int context )
  {
    if( !m_flexibleOfflineHandler )
      return;

    switch( context )
    {
      case FORequestMsgs:
        switch( iq->subtype() )
        {
          case IQ::Result:
            m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrRequestSuccess );
            break;
          case IQ::Error:
            switch( iq->error()->error() )
            {
              case StanzaErrorForbidden:
                m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrForbidden );
                break;
              case StanzaErrorItemNotFound:
                m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrItemNotFound );
                break;
              default:
                m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrUnknownError );
                break;
            }
            break;
          default:
            break;
        }
        break;
      case FORemoveMsgs:
        switch( iq->subtype() )
        {
          case IQ::Result:
            m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrRemoveSuccess );
            break;
          case IQ::Error:
            switch( iq->error()->error() )
            {
              case StanzaErrorForbidden:
                m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrForbidden );
                break;
              case StanzaErrorItemNotFound:
                m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrItemNotFound );
                break;
              default:
                m_flexibleOfflineHandler->handleFlexibleOfflineResult( FomrUnknownError );
                break;
            }
            break;
          default:
            break;
        }
        break;
    }
  }

  bool FlexibleOffline::handleIq( IQ* /*iq*/ )
  {
    return false;
  }

}

/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "privacymanager.h"
#include "clientbase.h"

#ifndef _WIN32_WCE
# include <sstream>
#endif

namespace gloox
{

  PrivacyManager::PrivacyManager( ClientBase *parent )
    : m_parent( parent ), m_privacyListHandler( 0 )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_PRIVACY );
  }

  PrivacyManager::~PrivacyManager()
  {
    if( m_parent )
      m_parent->removeIqHandler( this, XMLNS_PRIVACY );
  }

  std::string PrivacyManager::operation( int context, const std::string& name )
  {
    const std::string& id = m_parent->getID();
    IQ::IqType iqType = IQ::Set;
    if( context == PLRequestNames || context == PLRequestList )
      iqType = IQ::Get;
    IQ* iq = new IQ( iqType, JID(), id, XMLNS_PRIVACY );
    if( context != PLRequestNames )
    {
      const char * child = 0;
      switch( context )
      {
        case PLRequestList:
        case PLRemove:
          child = "list";
          break;
        case PLDefault:
        case PLUnsetDefault:
          child = "default";
          break;
        case PLActivate:
        case PLUnsetActivate:
          child = "active";
          break;
      }
      Tag* l = new Tag( iq->query(), child );
      if( !name.empty() )
        l->addAttribute( "name", name );
    }
    m_parent->trackID( this, id, context );
    m_parent->send( iq );
    return id;
  }

  std::string PrivacyManager::store( const std::string& name, PrivacyListHandler::PrivacyList& list )
  {
    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, JID(), id, XMLNS_PRIVACY );
    Tag *l = new Tag( iq->query(), "list" );
    l->addAttribute( "name", name );

    int count = 0;
    PrivacyListHandler::PrivacyList::iterator it = list.begin();
    for( ; it != list.end(); ++it )
    {
      Tag *i = new Tag( l, "item" );

      switch( (*it).type() )
      {
        case PrivacyItem::TypeJid:
          i->addAttribute( "type", "jid" );
          break;
        case PrivacyItem::TypeGroup:
          i->addAttribute( "type", "group" );
          break;
        case PrivacyItem::TypeSubscription:
          i->addAttribute( "type", "subscription" );
          break;
        default:
          break;
      }

      switch( (*it).action() )
      {
        case PrivacyItem::ActionAllow:
          i->addAttribute( "action", "allow" );
          break;
        case PrivacyItem::ActionDeny:
          i->addAttribute( "action", "deny" );
          break;
      }

      int pType = (*it).packetType();
      if( pType != 15 )
      {
        if( pType & PrivacyItem::PacketMessage )
          new Tag( i, "message" );
        if( pType & PrivacyItem::PacketPresenceIn )
          new Tag( i, "presence-in" );
        if( pType & PrivacyItem::PacketPresenceOut )
          new Tag( i, "presence-out" );
        if( pType & PrivacyItem::PacketIq )
          new Tag( i, "iq" );
      }

      i->addAttribute( "value", (*it).value() );
      i->addAttribute( "order", ++count );
    }

    m_parent->trackID( this, id, PLStore );
    m_parent->send( iq );
    return id;
  }

  bool PrivacyManager::handleIq( IQ* iq )
  {
    if( iq->subtype() != IQ::Set || !m_privacyListHandler )
      return false;

    Tag *l = iq->query()->findChild( "list" );
    if( l->hasAttribute( "name" ) )
    {
      const std::string& name = l->findAttribute( "name" );
      m_privacyListHandler->handlePrivacyListChanged( name );

      IQ* re = new IQ( IQ::Result, JID(), iq->id() );
      m_parent->send( re );
      return true;
    }

    return false;
  }

  void PrivacyManager::handleIqID( IQ* iq, int context )
  {
    if( iq->subtype() != IQ::Result || !m_privacyListHandler )
      return;

    switch( iq->subtype() )
    {
      case IQ::Result:
        switch( context )
        {
          case PLStore:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultStoreSuccess );
            break;
          case PLActivate:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultActivateSuccess );
            break;
          case PLDefault:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultDefaultSuccess );
            break;
          case PLRemove:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultRemoveSuccess );
            break;
          case PLRequestNames:
          {
            StringList lists;
            std::string def;
            std::string active;
            Tag *q = iq->query();
            const Tag::TagList& l = q->children();
            Tag::TagList::const_iterator it = l.begin();
            for( ; it != l.end(); ++it )
            {
              if( (*it)->name() == "default" )
                def = (*it)->findAttribute( "name" );
              if( (*it)->name() == "active" )
                def = (*it)->findAttribute( "name" );
              if( (*it)->name() == "list" )
              {
                const std::string& name = (*it)->findAttribute( "name" );
                lists.push_back( name );
              }
            }

            m_privacyListHandler->handlePrivacyListNames( def, active, lists );
            break;
          }
          case PLRequestList:
          {
            PrivacyListHandler::PrivacyList items;

            Tag *list = iq->query()->findChild( "list" );
            const std::string& name = list->findAttribute( "name" );
            const Tag::TagList& l = list->children();
            Tag::TagList::const_iterator it = l.begin();
            for( ; it != l.end(); ++it )
            {
              PrivacyItem::ItemType type;
              PrivacyItem::ItemAction action;
              int packetType = 0;

              const std::string& t = (*it)->findAttribute( "type" );
              if( t == "jid" )
                type = PrivacyItem::TypeJid;
              else if( t == "group" )
                type = PrivacyItem::TypeGroup;
              else if( t == "subscription" )
                type = PrivacyItem::TypeSubscription;
              else
                type = PrivacyItem::TypeUndefined;

              const std::string& a = (*it)->findAttribute( "action" );
              if( a == "allow" )
                action = PrivacyItem::ActionAllow;
              else if( a == "deny" )
                action = PrivacyItem::ActionDeny;
              else
                action = PrivacyItem::ActionAllow;

              const std::string& value = (*it)->findAttribute( "value" );

              const Tag::TagList& c = (*it)->children();
              Tag::TagList::const_iterator it_c = c.begin();
              for( ; it_c != c.end(); ++it_c )
              {
                if( (*it_c)->name() == "iq" )
                  packetType |= PrivacyItem::PacketIq;
                else if( (*it_c)->name() == "presence-out" )
                  packetType |= PrivacyItem::PacketPresenceOut;
                else if( (*it_c)->name() == "presence-in" )
                  packetType |= PrivacyItem::PacketPresenceIn;
                else if( (*it_c)->name() == "message" )
                  packetType |= PrivacyItem::PacketMessage;
              }

              PrivacyItem item( type, action, packetType, value );
              items.push_back( item );
            }
            m_privacyListHandler->handlePrivacyList( name, items );
            break;
          }
        }
        break;

      case IQ::Error:
      {
        switch( iq->error() )
        {
          case StanzaErrorConflict:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultConflict );
            break;
          case StanzaErrorItemNotFound:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultItemNotFound );
            break;
          case StanzaErrorBadRequest:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultBadRequest );
            break;
          default:
            m_privacyListHandler->handlePrivacyListResult( iq->id(), ResultUnknownError );
            break;
        }
        break;
      }

      default:
        break;
    }
  }

  void PrivacyManager::registerPrivacyListHandler( PrivacyListHandler *plh )
  {
    m_privacyListHandler = plh;
  }

  void PrivacyManager::removePrivacyListHandler()
  {
    m_privacyListHandler = 0;
  }

}

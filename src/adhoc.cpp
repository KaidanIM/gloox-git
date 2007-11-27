/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "adhoc.h"
#include "adhochandler.h"
#include "adhoccommandprovider.h"
#include "disco.h"
#include "error.h"
#include "discohandler.h"
#include "client.h"
#include "dataform.h"


namespace gloox
{

  Adhoc::Adhoc( ClientBase* parent )
    : m_parent( parent )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->addFeature( XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->registerNodeHandler( this, XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->registerNodeHandler( this, EmptyString );
    }
  }

  Adhoc::~Adhoc()
  {
    if( m_parent )
    {
      m_parent->disco()->removeFeature( XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->removeNodeHandler( this, XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->removeNodeHandler( this, EmptyString );
      m_parent->removeIqHandler( this, XMLNS_ADHOC_COMMANDS );
    }
  }

  StringList Adhoc::handleDiscoNodeFeatures( const JID& /*from*/, const std::string& /*node*/ )
  {
    StringList features;
    features.push_back( XMLNS_ADHOC_COMMANDS );
    return features;
//    return StringList( 1, XMLNS_ADHOC_COMMANDS );
  }

  Disco::ItemList Adhoc::handleDiscoNodeItems( const JID& /*from*/, const std::string& node )
  {
    Disco::ItemList l;
    if( node.empty() )
    {
      l.push_back( new Disco::Item( m_parent->jid(), XMLNS_ADHOC_COMMANDS, "Ad-Hoc Commands" ) );
    }
    else if( node == XMLNS_ADHOC_COMMANDS )
    {
      StringMap::const_iterator it = m_items.begin();
      for( ; it != m_items.end(); ++it )
      {
        l.push_back( new Disco::Item( m_parent->jid(), (*it).first, (*it).second ) );
      }
    }
    return l;
  }

  Disco::IdentityList Adhoc::handleDiscoNodeIdentities( const JID& /*from*/, const std::string& node )
  {
    Disco::IdentityList l;
    StringMap::const_iterator it = m_items.find( node );
    l.push_back( new Disco::Identity( "automation",
                               node == XMLNS_ADHOC_COMMANDS ? "command-list" : "command-node",
                               it == m_items.end() ? "Ad-Hoc Commands" : (*it).second ) );
    return l;
  }

  bool Adhoc::handleIq( IQ* iq )
  {
    if( iq->subtype() != IQ::Set )
      return false;

    Tag* c = iq->query();
    if( c && c->name() == "command" )
    {
      const std::string& node = c->findAttribute( "node" );
      AdhocCommandProviderMap::const_iterator it = m_adhocCommandProviders.find( node );
      if( !node.empty() && ( it != m_adhocCommandProviders.end() ) )
      {
        (*it).second->handleAdhocCommand( node, c, iq->from(), iq->id() );
        return true;
      }
    }

    return false;
  }

  void Adhoc::handleIqID( IQ* iq, int context )
  {
    if( context != ExecuteAdhocCommand || iq->subtype() != IQ::Result )
      return;

    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end() && (*it).second.context != context
                                       && (*it).second.remote  != iq->from(); ++it )
    if( it == m_adhocTrackMap.end() )
      return;

    Tag* c = iq->query();
    if( c && c->name() == "command" && c->xmlns() == XMLNS_ADHOC_COMMANDS )
    {
      const std::string& command = c->findAttribute( "node" );
      const std::string& id = c->findAttribute( "sessionid" );
      const Tag* a = c->findChild( "actions" );
      int actions = ActionCancel;
      Adhoc::AdhocExecuteActions def = ActionCancel;
      if( a )
      {
        if( a->hasChild( "prev" ) )
          actions |= ActionPrevious;
        if( a->hasChild( "next" ) )
          actions |= ActionNext;
        if( a->hasChild( "complete" ) )
          actions |= ActionComplete;

        const std::string& d = a->findAttribute( "execute" );
        if( d == "next" )
          def = ActionNext;
        else if( d == "prev" )
          def = ActionPrevious;
        else if( d == "complete" )
          def = ActionComplete;
      }

      const Tag* n = c->findChild( "note" );
      std::string note;
      AdhocNoteType type = AdhocNoteInfo;
      if( n )
      {
        note = n->cdata();
        if( n->hasAttribute( TYPE, "warn" ) )
          type = AdhocNoteWarn;
        else if( n->hasAttribute( TYPE, "error" ) )
          type = AdhocNoteError;
      }

      const std::string& s = c->findAttribute( "status" );
      AdhocCommandStatus status = AdhocCommandStatusUnknown;
      if( s == "executing" )
        status = AdhocCommandExecuting;
      else if( s == "completed" )
        status = AdhocCommandCompleted;
      else if( s == "canceled" )
        status = AdhocCommandCanceled;

      const DataForm form( c->findChild( "x", XMLNS, XMLNS_X_DATA ) );

      (*it).second.ah->handleAdhocExecutionResult( iq->from(), command, status, id, form,
                                                   actions, def, note, type );
    }

    m_adhocTrackMap.erase( it );
  }

  void Adhoc::registerAdhocCommandProvider( AdhocCommandProvider* acp, const std::string& command,
                                            const std::string& name )
  {
    m_parent->disco()->registerNodeHandler( this, command );
    m_adhocCommandProviders[command] = acp;
    m_items[command] = name;
  }

  void Adhoc::handleDiscoInfo( const JID& from, const Disco::Info& info, int context )
  {
    if( context != CheckAdhocSupport )
      return;

    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end() && (*it).second.context != context
                                       && (*it).second.remote  != from; ++it )
      ;
    if( it == m_adhocTrackMap.end() )
      return;

    (*it).second.ah->handleAdhocSupport( (*it).second.remote, info.hasFeature( XMLNS_ADHOC_COMMANDS ) );
    m_adhocTrackMap.erase( it );
  }

  void Adhoc::handleDiscoItems( const JID& from, const Disco::Items& items, int context )
  {
    if( context != FetchAdhocCommands )
      return;

    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end(); ++it )
    {
      if( (*it).second.context == context && (*it).second.remote == from )
      {
        StringMap commands;
        const Disco::ItemList& l = items.items();
        Disco::ItemList::const_iterator it2 = l.begin();
        for( ; it2 != l.end(); ++it2 )
        {
          commands[(*it2)->node()] = (*it2)->name();
        }
        (*it).second.ah->handleAdhocCommands( from, commands );

        m_adhocTrackMap.erase( it );
        break;
      }
    }
  }

  void Adhoc::handleDiscoError( const JID& from, const Error& error, int context )
  {
    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end(); ++it )
    {
      if( (*it).second.context == context && (*it).second.remote == from )
      {
        (*it).second.ah->handleAdhocError( from, error );

        m_adhocTrackMap.erase( it );
      }
    }
  }

  void Adhoc::checkSupport( const JID& remote, AdhocHandler* ah )
  {
    if( !remote || !ah )
      return;

    TrackStruct track;
    track.remote = remote;
    track.context = CheckAdhocSupport;
    track.ah = ah;
    const std::string& id = m_parent->getID();
    m_adhocTrackMap[id] = track;
    m_parent->disco()->getDiscoInfo( remote, EmptyString, this, CheckAdhocSupport, id );
  }

  void Adhoc::getCommands( const JID& remote, AdhocHandler* ah )
  {
    if( !remote || !ah )
      return;

    TrackStruct track;
    track.remote = remote;
    track.context = FetchAdhocCommands;
    track.ah = ah;
    const std::string& id = m_parent->getID();
    m_adhocTrackMap[id] = track;
    m_parent->disco()->getDiscoItems( remote, XMLNS_ADHOC_COMMANDS, this, FetchAdhocCommands, id );
  }

  void Adhoc::execute( const JID& remote, const std::string& command, AdhocHandler* ah,
                       const std::string& sessionid, DataForm* form,
                       AdhocExecuteActions action )
  {
    if( !remote || command.empty() || !ah )
      return;

    const std::string& id = m_parent->getID();
    IQ iq( IQ::Set, remote, id, XMLNS_ADHOC_COMMANDS, "command" );
    Tag* c = iq.query();
    c->addAttribute( "node", command );
    c->addAttribute( "action", "execute" );
    if( !sessionid.empty() )
      c->addAttribute( "sessionid", sessionid );
    if( action != ActionDefault )
    {
      switch( action )
      {
        case ActionPrevious:
          c->addAttribute( "action", "prev" );
          break;
        case ActionNext:
          c->addAttribute( "action", "next" );
          break;
        case ActionCancel:
          c->addAttribute( "action", "cancel" );
          break;
        case ActionComplete:
          c->addAttribute( "action", "complete" );
          break;
        default:
          break;
      }
    }
    if( form )
      c->addChild( form->tag() );

    TrackStruct track;
    track.remote = remote;
    track.context = ExecuteAdhocCommand;
    track.ah = ah;
    m_adhocTrackMap[id] = track;

    m_parent->send( iq, this, ExecuteAdhocCommand );
  }

  void Adhoc::removeAdhocCommandProvider( const std::string& command )
  {
    m_parent->disco()->removeNodeHandler( this, command );
    m_adhocCommandProviders.erase( command );
    m_items.erase( command );
  }

}

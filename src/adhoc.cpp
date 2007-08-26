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
      m_parent->disco()->registerNodeHandler( this, std::string() );
    }
  }

  Adhoc::~Adhoc()
  {
    if( m_parent )
    {
      m_parent->disco()->removeFeature( XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->removeNodeHandler( this, XMLNS_ADHOC_COMMANDS );
      m_parent->disco()->removeNodeHandler( this, std::string() );
      m_parent->removeIqHandler( this, XMLNS_ADHOC_COMMANDS );
    }
  }

  StringList Adhoc::handleDiscoNodeFeatures( const std::string& /*node*/ )
  {
    StringList features;
    features.push_back( XMLNS_ADHOC_COMMANDS );
    return features;
  }

  DiscoNodeItemList Adhoc::handleDiscoNodeItems( const std::string& node )
  {
    DiscoNodeItemList l;
    if( node.empty() )
    {
      DiscoNodeItem item;
      item.node = XMLNS_ADHOC_COMMANDS;
      item.jid = m_parent->jid().full();
      item.name = "Ad-Hoc Commands";
      l.push_back( item );
    }
    else if( node == XMLNS_ADHOC_COMMANDS )
    {
      StringMap::const_iterator it = m_items.begin();
      for( ; it != m_items.end(); ++it )
      {
        DiscoNodeItem item;
        item.node = (*it).first;
        item.jid = m_parent->jid().full();
        item.name = (*it).second;
        l.push_back( item );
      }
    }
    return l;
  }

  StringMap Adhoc::handleDiscoNodeIdentities( const std::string& node, std::string& name )
  {
    StringMap::const_iterator it = m_items.find( node );
    if( it != m_items.end() )
      name = (*it).second;
    else
      name = "Ad-Hoc Commands";

    StringMap ident;
    if( node == XMLNS_ADHOC_COMMANDS )
      ident["automation"] = "command-list";
    else
      ident["automation"] = "command-node";
    return ident;
  }

  bool Adhoc::handleIq( IQ* iq )
  {
    if( iq->subtype() != IQ::Set )
      return false;

    if( iq->hasChild( "command" ) )
    {
      Tag* c = iq->findChild( "command" );
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
    for( ; it != m_adhocTrackMap.end(); ++it )
    {
      if( (*it).second.context == context && (*it).second.remote == iq->from() )
      {
        Tag* c = iq->findChild( "command", XMLNS, XMLNS_ADHOC_COMMANDS );
        if( c )
        {
          const std::string& command = c->findAttribute( "node" );
          const std::string& id = c->findAttribute( "sessionid" );
          Tag* a = c->findChild( "actions" );
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
          Tag* n = c->findChild( "note" );
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
          DataForm form;
          Tag* x = c->findChild( "x", XMLNS, XMLNS_X_DATA );
          if( x )
            form.parse( x );

          (*it).second.ah->handleAdhocExecutionResult( iq->from(), command, status, id, form,
                                                       actions, def, note, type );
        }

        m_adhocTrackMap.erase( it );
        return;
      }
    }

  }

  void Adhoc::registerAdhocCommandProvider( AdhocCommandProvider* acp, const std::string& command,
                                            const std::string& name )
  {
    m_parent->disco()->registerNodeHandler( this, command );
    m_adhocCommandProviders[command] = acp;
    m_items[command] = name;
  }

  void Adhoc::handleDiscoInfoResult( IQ* iq, int context )
  {
    if( context != CheckAdhocSupport )
      return;

    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end(); ++it )
    {
      if( (*it).second.context == context && (*it).second.remote == iq->from() )
      {
        Tag* q = iq->findChild( "query", XMLNS, XMLNS_DISCO_INFO );
        if( q )
          (*it).second.ah->handleAdhocSupport( (*it).second.remote,
                  q->hasChild( "feature", "var", XMLNS_ADHOC_COMMANDS ) );
        m_adhocTrackMap.erase( it );
        break;
      }
    }
  }

  void Adhoc::handleDiscoItemsResult( IQ* iq, int context )
  {
    if( context != FetchAdhocCommands )
      return;

    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end(); ++it )
    {
      if( (*it).second.context == context && (*it).second.remote == iq->from() )
      {
        Tag* q = iq->findChild( "query", XMLNS, XMLNS_DISCO_ITEMS );
        if( q )
        {
          StringMap commands;
          const TagList& l = q->children();
          TagList::const_iterator itt = l.begin();
          for( ; itt != l.end(); ++itt )
          {
            const std::string& name = (*itt)->findAttribute( "name" );
            const std::string& node = (*itt)->findAttribute( "node" );
            if( (*itt)->name() == "item" && !name.empty() && !node.empty() )
            {
              commands[node] = name;
            }
          }
          (*it).second.ah->handleAdhocCommands( (*it).second.remote, commands );
        }

        m_adhocTrackMap.erase( it );
        break;
      }
    }
  }

  void Adhoc::handleDiscoError( IQ* iq, int context )
  {
    AdhocTrackMap::iterator it = m_adhocTrackMap.begin();
    for( ; it != m_adhocTrackMap.end(); ++it )
    {
      if( (*it).second.context == context && (*it).second.remote == iq->from() )
      {
        (*it).second.ah->handleAdhocError( (*it).second.remote, iq->error()->error() );

        m_adhocTrackMap.erase( it );
      }
    }
  }

  void Adhoc::checkSupport( const JID& remote, AdhocHandler* ah )
  {
    if( remote.empty() || !ah )
      return;

    TrackStruct track;
    track.remote = remote;
    track.context = CheckAdhocSupport;
    track.ah = ah;
    m_adhocTrackMap[m_parent->getID()] = track;
    m_parent->disco()->getDiscoInfo( remote, "", this, CheckAdhocSupport );
  }

  void Adhoc::getCommands( const JID& remote, AdhocHandler* ah )
  {
    if( remote.empty() || !ah )
      return;

    TrackStruct track;
    track.remote = remote;
    track.context = FetchAdhocCommands;
    track.ah = ah;
    m_adhocTrackMap[m_parent->getID()] = track;
    m_parent->disco()->getDiscoItems( remote, XMLNS_ADHOC_COMMANDS, this, FetchAdhocCommands );
  }

  void Adhoc::execute( const JID& remote, const std::string& command, AdhocHandler* ah,
                       const std::string& sessionid, DataForm* form,
                       AdhocExecuteActions action )
  {
    if( remote.empty() || command.empty() || !ah )
      return;

    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Set, remote, id, XMLNS_ADHOC_COMMANDS, "command" );
    Tag* c = iq->query();
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

    m_parent->trackID( this, id, ExecuteAdhocCommand );
    m_parent->send( iq );
  }

  void Adhoc::removeAdhocCommandProvider( const std::string& command )
  {
    m_parent->disco()->removeNodeHandler( this, command );
    m_adhocCommandProviders.erase( command );
    m_items.erase( command );
  }

}
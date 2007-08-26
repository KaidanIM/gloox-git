/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "search.h"

#include "clientbase.h"
#include "iq.h"

namespace gloox
{

  Search::Search( ClientBase* parent )
    : m_parent( parent )
  {
  }

  Search::~Search()
  {
  }

  void Search::fetchSearchFields( const JID& directory, SearchHandler* sh )
  {
    if( !m_parent || directory.empty() || !sh )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Get, directory, id, XMLNS_SEARCH );

    m_track[id] = sh;
    m_parent->trackID( this, id, FetchSearchFields );
    m_parent->send( iq );
  }

  void Search::search( const JID& directory, const DataForm& form, SearchHandler* sh )
  {
    if( !m_parent || directory.empty() || !sh )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, directory, id, XMLNS_SEARCH );
    iq->query()->addChild( form.tag() );

    m_track[id] = sh;
    m_parent->trackID( this, id, DoSearch );
    m_parent->send( iq );
  }

  void Search::search( const JID& directory, int fields, const SearchFieldStruct& values, SearchHandler* sh )
  {
    if( !m_parent || directory.empty() || !sh )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, directory, id, XMLNS_SEARCH );
    Tag* q = iq->query();

    if( fields & SearchFieldFirst )
      new Tag( q, "first", values.first );
    if( fields & SearchFieldLast )
      new Tag( q, "last", values.last );
    if( fields & SearchFieldNick )
      new Tag( q, "nick", values.nick );
    if( fields & SearchFieldEmail )
      new Tag( q, "email", values.email );

    m_track[id] = sh;
    m_parent->trackID( this, id, DoSearch );
    m_parent->send( iq );
  }

  void Search::handleIqID( IQ* iq, int context )
  {
    TrackMap::iterator it = m_track.find( iq->id() );
    if( it != m_track.end() )
    {
      switch( iq->subtype() )
      {
        case IQ::Result:
          switch( context )
          {
            case FetchSearchFields:
            {
              Tag* q = iq->query();
              if( q && q->hasAttribute( XMLNS, XMLNS_SEARCH ) )
              {
                Tag* x = q->findChild( "x", XMLNS, XMLNS_X_DATA );
                if( x )
                {
                  DataForm* df = new DataForm( x );
                  (*it).second->handleSearchFields( iq->from(), df );
                }
                else
                {
                  int fields = 0;
                  std::string instructions;

                  if( q->hasChild( "first" ) )
                    fields |= SearchFieldFirst;
                  if( q->hasChild( "last" ) )
                    fields |= SearchFieldLast;
                  if( q->hasChild( "nick" ) )
                    fields |= SearchFieldNick;
                  if( q->hasChild( "email" ) )
                    fields |= SearchFieldEmail;
                  if( q->hasChild( "instructions" ) )
                    instructions = q->findChild( "instructions" )->cdata();

                  (*it).second->handleSearchFields( iq->from(), fields, instructions );
                }
              }
              break;
            }
            case DoSearch:
            {
              Tag* q = iq->query();
              if( q && q->hasAttribute( XMLNS, XMLNS_SEARCH ) )
              {
                Tag* x = q->findChild( "x", XMLNS, XMLNS_X_DATA );
                if( x )
                {
                  DataForm* df = new DataForm( x );
                  (*it).second->handleSearchResult( iq->from(), df );
                }
                else
                {
                  SearchResultList e;
                  SearchFieldStruct s;
                  const TagList &l = q->children();
                  TagList::const_iterator itl = l.begin();
                  for( ; itl != l.end(); ++itl )
                  {
                    if( (*itl)->name() == "item" )
                    {
                      s.jid.setJID( (*itl)->findAttribute( "jid" ) );
                      Tag* t = 0;
                      if( ( t = (*itl)->findChild( "first" ) ) != 0 )
                        s.first = t->cdata();
                      if( ( t = (*itl)->findChild( "last" ) ) != 0 )
                        s.last = t->cdata();
                      if( ( t = (*itl)->findChild( "nick" ) ) != 0 )
                        s.nick = t->cdata();
                      if( ( t = (*itl)->findChild( "email" ) ) != 0 )
                        s.email = t->cdata();
                      e.push_back( s );
                    }
                  }

                  (*it).second->handleSearchResult( iq->from(), e );
                }
              }
              break;
            }
          }
          break;
        case IQ::Error:
          (*it).second->handleSearchError( iq->from(), iq );
          break;

        default:
          break;
      }

      m_track.erase( it );
    }

    return;
  }

}
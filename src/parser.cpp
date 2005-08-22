/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
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



// #include "config.h"

#include "gloox.h"

#include "parser.h"
#include "clientbase.h"
#include "tag.h"

using namespace std;


namespace gloox
{

  Parser::Parser( ClientBase *parent, const std::string& ns )
    : m_parent( parent )
  {
    m_parser = iks_stream_new( (char*)ns.c_str(), this, (iksStreamHook*)streamHook );
  }

  Parser::~Parser()
  {
    iks_parser_delete( m_parser );
  }

  Parser::ParserState Parser::feed( const std::string& data )
  {
    int res = iks_parse( m_parser, data.c_str(), data.length(), 0 );
    switch( res )
    {
      case IKS_OK:
        return PARSER_OK;
        break;
      case IKS_NOMEM:
        return PARSER_NOMEM;
        break;
      case IKS_BADXML:
        return PARSER_BADXML;
        break;
      case IKS_HOOK:
        return PARSER_HOOK;
        break;
    }
  }

  void Parser::streamEvent( int type, iks* node )
  {
    if( m_parent && node )
    {
      Tag tag = convertFromIks( node );
      m_parent->filter( type, tag );
    }

    iks_delete( node );
  }

  const Tag Parser::convertFromIks( iks *x )
  {
    Tag tag( iks_name( x ) );

    iks *y = iks_attrib( x );
    while( y )
    {
      tag.addAttrib( iks_name( y ), iks_cdata( y ) );
      y = iks_next( y );
    }

    y = iks_child( x );
    while( y )
    {
      switch( iks_type( y ) )
      {
        case IKS_TAG:
          tag.addChild( convertFromIks( y ) );
          break;
        case IKS_CDATA:
          tag.setCData( iks_cdata( y ) );
          break;
      }
      y = iks_next( y );
    }

    return tag;
  }

//   int msgHook( ClientBase *cb, ikspak* pak )
//   {
//     printf( "in msgHook\n" );
//     if( cb )
//       cb->notifyMessageHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "body" ) );
//     return IKS_FILTER_EAT;
//   }
//
//   int iqHook( ClientBase *cb, ikspak* pak )
//   {
//     printf( "in iqHook\n" );
//     if( cb )
//       cb->notifyIqHandlers( pak->ns, pak );
//     return IKS_FILTER_EAT;
//   }
//
//   int presenceHook( ClientBase *cb, ikspak* pak )
//   {
//     printf( "in presenceHook\n" );
//     if( cb )
//       cb->notifyPresenceHandlers( pak->from, pak->subtype, pak->show,
//                                       iks_find_cdata( pak->x, "status" ) );
//     return IKS_FILTER_EAT;
//   }
//
//   int subscriptionHook( ClientBase *cb, ikspak* pak )
//   {
//     printf( "in subscriptionHook\n" );
//     if( cb )
//       cb->notifySubscriptionHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "status" ) );
//     return IKS_FILTER_EAT;
//   }
//
//   int bindHook( ClientBase *cb, ikspak* pak )
//   {
//     printf( "in Parser::bindHook\n" );
// //    if( cb )
// //      cb->notifySubscriptionHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "status" ) );
//     return IKS_FILTER_EAT;
//   }
//
//   int sessionHook( ClientBase *cb, ikspak* pak )
//   {
//     printf( "in Parser::sessionHook\n" );//    if( cb )
// //      cb->notifySubscriptionHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "status" ) );
//     return IKS_FILTER_EAT;
//   }

  int streamHook( Parser *parser, int type, iks *node )
  {
    printf( "in streamHook\n" );
    parser->streamEvent( type, node );
    return IKS_OK;
  }

//   int logHook( ClientBase *cb, const char *data, size_t size, int is_incoming )
//   {
//     printf( "in logHook\n" );
//     if( cb )
//       cb->logEvent( data, size, is_incoming );
//     return IKS_OK;
//   }

};

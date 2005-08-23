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



#include "gloox.h"

#include "parser.h"
#include "clientbase.h"
#include "tag.h"

namespace gloox
{

  Parser::Parser( ClientBase *parent, const std::string& ns )
    : m_parent( parent ), m_root( 0 ), m_current( 0 )
  {
    m_parser = iks_sax_new( this, (iksTagHook*)tagHook, (iksCDataHook*)cdataHook );
  }

  Parser::~Parser()
  {
    iks_parser_delete( m_parser );
    delete( m_root );
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
    }
  }

  void Parser::streamEvent( Tag *tag )
  {
    if( m_parent && tag )
    {
      ClientBase::NodeType type = ClientBase::NODE_STREAM_CHILD;
      if( m_root->name() == "stream:stream" )
        type = ClientBase::NODE_STREAM_START;
      else if( m_root->name() == "stream:error" )
        type = ClientBase::NODE_STREAM_ERROR;

      m_parent->filter( type, tag );
    }
  }

  int tagHook( Parser *parser, char *name, char **atts, int type )
  {
    switch( type )
    {
      case IKS_OPEN:
      case IKS_SINGLE:
        Tag *tag = new Tag( name );
        for(int i=0; atts && atts[i]; )
        {
          tag->addAttrib( atts[i], atts[i+1] );
          i+=2;
        }
        if( !parser->m_root )
        {
          parser->m_root = tag;
          parser->m_current = parser->m_root;
        }
        else
        {
          parser->m_current->addChild( tag );
          parser->m_current = tag;
        }
        if( tag->name() == "stream:stream" )
        {
          parser->streamEvent( parser->m_root );
          delete( parser->m_root );
          parser->m_root = 0;
          parser->m_current = 0;
        }
        if( type == IKS_OPEN )
        break;
      case IKS_CLOSE:
        parser->m_current = parser->m_current->parent();
        if( !parser->m_current )
        {
          parser->streamEvent( parser->m_root );
          delete( parser->m_root );
          parser->m_root = 0;
          parser->m_current = 0;
        }
        break;
    }
    return IKS_OK;
  }

  int cdataHook( Parser *parser, char *data, size_t len )
  {
    if( parser->m_current && data )
    {
      std::string tmp = data;
      parser->m_current->setCData( tmp.substr( 0, len ) );
    }

    return IKS_OK;
  }
};

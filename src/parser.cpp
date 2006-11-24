/*
  Copyright (c) 2004-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "gloox.h"

#include "parser.h"
#include "clientbase.h"
#include "stanza.h"

namespace gloox
{

  Parser::Parser( ClientBase *parent )
    : m_parent( parent ), m_current( 0 ), m_root( 0 ), m_state( INITIAL ),
      m_preamble( 0 )
  {
  }

  Parser::~Parser()
  {
    delete m_root;
  }

  Parser::ParserState Parser::feed( const std::string& data )
  {
    std::string::const_iterator it = data.begin();
    for( ; it != data.end(); ++it )
    {
      unsigned char c = (*it);
//       printf( "found char:   %c, ", c );

      if( !isValid( c ) )
      {
        cleanup();
        return PARSER_BADXML;
      }

      switch( m_state )
      {
        case INITIAL:
          m_tag = "";
          if( isWhitespace( c ) )
            break;

          switch( c )
          {
            case '<':
              m_state = TAG_OPENING;
              break;
            case '>':
            default:
              cleanup();
              return PARSER_BADXML;
              break;
          }
          break;
        case TAG_OPENING:               // opening '<' has been found before
          if( isWhitespace( c ) )
            break;

          switch( c )
          {
            case '<':
            case '>':
              cleanup();
              return PARSER_BADXML;
              break;
            case '/':
              m_state = TAG_CLOSING_SLASH;
              break;
            case '?':
              m_state = TAG_NAME_COLLECT;
              m_preamble = 1;
              break;
            default:
              m_tag += c;
              m_state = TAG_NAME_COLLECT;
              break;
          }
          break;
        case TAG_NAME_COLLECT:          // we're collecting the tag's name, we have at least one octet already
          if( isWhitespace( c ) )
          {
            m_state = TAG_NAME_COMPLETE;
            break;
          }

          switch( c )
          {
            case '<':
            case '?':
              cleanup();
              return PARSER_BADXML;
              break;
            case '/':
              m_state = TAG_OPENING_SLASH;
              break;
            case '>':
              addTag();
              m_state = TAG_INSIDE;
              break;
            default:
              m_tag += c;
              break;
          }
          break;
        case TAG_INSIDE:                // we're inside a tag, expecting a child tag or cdata
          m_tag = "";
          switch( c )
          {
            case '<':
              addCData();
              m_state = TAG_OPENING;
              break;
            case '>':
              cleanup();
              return PARSER_BADXML;
              break;
            default:
              m_cdata += c;
              break;
          }
          break;
        case TAG_OPENING_SLASH:         // a slash in an opening tag has been found, initing close of the tag
          if( isWhitespace( c ) )
            break;

          if( c == '>' )
          {
            addTag();
            if( !closeTag() )
            {
              cleanup();
              return PARSER_BADXML;
            }

            m_state = INITIAL;
          }
          else
          {
            cleanup();
            return PARSER_BADXML;
          }
          break;
        case TAG_CLOSING_SLASH:         // we have found the '/' of a closing tag
          if( isWhitespace( c ) )
            break;

          switch( c )
          {
            case '>':
            case '<':
            case '/':
              cleanup();
              return PARSER_BADXML;
              break;
            default:
              m_tag += c;
              m_state = TAG_CLOSING;
              break;
          }
          break;
        case TAG_CLOSING:               // we're collecting the name of a closing tag
          switch( c )
          {
            case '<':
            case '/':
              cleanup();
              return PARSER_BADXML;
              break;
            case '>':
              if( !closeTag() )
              {
                cleanup();
                return PARSER_BADXML;
              }

              m_state = INITIAL;
              break;
            default:
              m_tag += c;
              break;
          }
          break;
        case TAG_NAME_COMPLETE:        // a tag name is complete, expect tag close or attribs
          if( isWhitespace( c ) )
            break;

          switch( c )
          {
            case '<':
              cleanup();
              return PARSER_BADXML;
              break;
            case '/':
              m_state = TAG_OPENING_SLASH;
              break;
            case '>':
              if( m_preamble == 1 )
              {
                cleanup();
                return PARSER_BADXML;
              }
              m_state = TAG_INSIDE;
              addTag();
              break;
            case '?':
              if( m_preamble == 1 )
                m_preamble = 2;
              else
              {
                cleanup();
                return PARSER_BADXML;
              }
              break;
            default:
              m_attrib += c;
              m_state = TAG_ATTR;
              break;
          }
          break;
        case TAG_ATTR:                  // we're collecting the name of an attribute, we have at least 1 octet
          if( isWhitespace( c ) )
          {
            m_state = TAG_ATTR_COMPLETE;
            break;
          }

          switch( c )
          {
            case '<':
            case '/':
            case '>':
              cleanup();
              return PARSER_BADXML;
              break;
            case '=':
              m_state = TAG_ATTR_EQUAL;
              break;
            default:
              m_attrib += c;
          }
          break;
        case TAG_ATTR_COMPLETE:         // we're expecting an equals sign or ws or the attrib value
          if( isWhitespace( c ) )
            break;

          switch( c )
          {
            case '=':
              m_state = TAG_ATTR_EQUAL;
              break;
            case '<':
            case '/':
            case '>':
            default:
              cleanup();
              return PARSER_BADXML;
              break;
          }
          break;
        case TAG_ATTR_EQUAL:            // we have found a equals sign
          if( isWhitespace( c ) )
            break;

          switch( c )
          {
            case '\'':
            case '"':
              m_state = TAG_VALUE;
              break;
            case '=':
            case '<':
            case '>':
            default:
              cleanup();
              return PARSER_BADXML;
              break;
          }
          break;
        case TAG_VALUE:                 // we're expecting value data
          switch( c )
          {
            case '<':
            case '>':
              cleanup();
              return PARSER_BADXML;
              break;
            case '\'':
            case '"':
              addAttribute();
              m_state = TAG_NAME_COMPLETE;
              break;
            default:
              m_value += c;
          }
          break;
        default:
//           printf( "default action!?\n" );
          break;
      }
//       printf( "parser state: %d\n", m_state );
    }

    return PARSER_OK;
  }

  void Parser::addTag()
  {
    if( !m_root )
    {
//       printf( "created Stanza named %s, ", m_tag.c_str() );
      m_root = new Stanza( m_tag, "", "default", true );
      m_current = (Tag*)m_root;
    }
    else
    {
//       printf( "created Tag named %s, ", m_tag.c_str() );
      m_current = new Tag( m_current, m_tag, "", true );
    }

    if( m_attribs.size() )
    {
      m_current->setAttributes( m_attribs );
//       printf( "added %d attributes, ", m_attribs.size() );
      m_attribs.clear();
    }

    if( m_tag == "stream:stream" )
    {
      streamEvent( m_root );
      cleanup();
    }
//     else
//       printf( "%s, ", m_root->xml().c_str() );

    if( m_tag == "xml" && m_preamble == 2 )
      cleanup();
  }

  void Parser::addAttribute()
  {
//     printf( "adding attribute: %s='%s', ", m_attrib.c_str(), m_value.c_str() );
    m_attribs[m_attrib] = m_value;
    m_attrib = "";
    m_value = "";
//     printf( "added, " );
  }

  void Parser::addCData()
  {
    if( m_current )
    {
      m_current->setCData( m_cdata );
//       printf( "added cdata %s, ", m_cdata.c_str() );
      m_cdata = "";
    }
  }

  bool Parser::closeTag()
  {
//     printf( "about to close, " );

    if( !m_current )
      return false;
//     else
//       printf( "m_current: %s, ", m_current->name().c_str() );

    if( m_current->name() != m_tag )
    {
//       printf( "m_tag: %s, ", m_tag.c_str() );
      return false;
    }

    if( m_current->parent() )
      m_current = m_current->parent();
    else
    {
//       printf( "pushing upstream, " );
      m_root->finalize();
      streamEvent( m_root );
      cleanup();
    }

    return true;
  }

  void Parser::cleanup()
  {
    delete m_root;
    m_root = 0;
    m_current = 0;
    m_cdata = "";
    m_tag = "";
    m_attrib = "";
    m_value = "";
    m_attribs.clear();
    m_state = INITIAL;
    m_preamble = 0;
  }

  bool Parser::isValid( unsigned char& c )
  {
    if( c != 0xc0 || c != 0xc1 || c < 0xf5 )
      return true;

    return false;
  }

  bool Parser::isWhitespace( unsigned char& c )
  {
    if( c == 0x09 || c == 0x0a || c == 0x0d || c == 0x20 )
      return true;

    return false;
  }

  void Parser::streamEvent( Stanza *stanza )
  {
    if( m_parent && stanza )
    {
      ClientBase::NodeType type = ClientBase::NODE_STREAM_CHILD;
      if( stanza->name() == "stream:stream" )
        type = ClientBase::NODE_STREAM_START;
      else if( stanza->name() == "stream:error" )
        type = ClientBase::NODE_STREAM_ERROR;

      m_parent->filter( type, stanza );
    }
    else if( !stanza )
      m_parent->filter( ClientBase::NODE_STREAM_CLOSE, 0 );
  }

}

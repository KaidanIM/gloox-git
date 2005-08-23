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



#ifndef PARSER_H__
#define PARSER_H__

#include "gloox.h"

#include <iksemel.h>

#include <string>

namespace gloox
{

  class ClientBase;
  class Tag;

  /**
   * This class is an abstraction of libiksemel's XML parser.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class Parser
  {
    public:
      /**
       * Describes the return values of the parser.
       */
      enum ParserState
      {
        PARSER_OK,                     /**< Everything's alright. */
        PARSER_NOMEM,                  /**< Memory allcation error. */
        PARSER_BADXML,                 /**< XML parse error. */
        PARSER_HOOK                    /**< Unknown. */
      };

      /**
       * Describes the possibel node types
       */
      enum NodeType
      {
        NODE_STREAM_START,             /**< The &lt;stream:stream&gt; tag. */
        NODE_STREAM_ERROR,             /**< The &lt;stream:error&gt; tag. */
        NODE_STREAM_CLOSE,             /**< The &lt;/stream:stream&gt; tag. */
        NODE_STREAM_CHILD,             /**< Everything else. */
      };

      /**
       * Constructs a new Parser object.
       * @param parent The object to send incoming Tags to.
       * @param ns The parser's namespace. Legacy, libiksemel-related.
       */
      Parser( ClientBase *parent, const std::string& ns );

      /**
       * Virtual destructor.
       */
      virtual ~Parser();

      /**
       * Use this function to feed the parser with more XML.
       * @param data Raw xml to parse.
       */
      ParserState feed( const std::string& data );

    private:
      void streamEvent( NodeType type, Tag *tag );

      iksparser *m_parser;
      ClientBase *m_parent;
      Tag *m_current;
      Tag *m_root;

      friend int cdataHook( Parser *parser, char *data, size_t len );
      friend int tagHook( Parser *parser, char *name, char **atts, int type );
  };

};

#endif // PARSER_H__

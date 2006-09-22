/*
  Copyright (c) 2004-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef PARSER_H__
#define PARSER_H__

#include "gloox.h"

#include <string>

namespace gloox
{

  class ClientBase;
  class Stanza;
  class Tag;

  /**
   * @brief This class is an abstraction of libiksemel's XML parser.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class GLOOX_API Parser
  {
    public:
      /**
       * Describes the return values of the parser.
       */
      enum ParserState
      {
        PARSER_OK,                     /**< Everything's alright. */
        PARSER_NOMEM,                  /**< Memory allcation error. */
        PARSER_BADXML                  /**< XML parse error. */
      };

      /**
       * Constructs a new Parser object.
       * @param parent The object to send incoming Tags to.
       */
      Parser( ClientBase *parent );

      /**
       * Virtual destructor.
       */
      virtual ~Parser();

      /**
       * Use this function to feed the parser with more XML.
       * @param data Raw xml to parse.
       * @return The return value indicates success or failure of the parsing.
       */
      ParserState feed( const std::string& data );

    private:
      void addTag();
      void addAttribute();
      void addCData();
      bool closeTag();
      void cleanup();
      bool isWhitespace( unsigned char& c );
      bool isValid( unsigned char& c );
      void streamEvent( Stanza *stanza );

      enum ParserInternalState
      {
        INITIAL,
        TAG_OPENING,
        TAG_OPENING_SLASH,
        TAG_OPENING_LT,
        TAG_INSIDE,
        TAG_NAME_COLLECT,
        TAG_NAME_COMPLETE,
        TAG_ATTR,
        TAG_ATTR_COMPLETE,
        TAG_ATTR_EQUAL,
        TAG_CLOSING,
        TAG_CLOSING_SLASH,
        TAG_VALUE_APOS,
        TAG_VALUE,
        TAG_PREAMBLE
      };

      ClientBase *m_parent;
      Tag *m_current;
      Stanza *m_root;

      ParserInternalState m_state;
      StringMap m_attribs;
      std::string m_tag;
      std::string m_cdata;
      std::string m_attrib;
      std::string m_value;
      int m_preamble;

  };

}

#endif // PARSER_H__

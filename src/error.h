/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef __ERROR_H__
#define __ERROR_H__

#include "gloox.h"
#include "stanzaextension.h"

#include <string>
#include <map>

namespace gloox
{

  class Tag;

  /**
   * 
   */
  class Error : public StanzaExtension
  {
    public:

      Error()
        : StanzaExtension( ExtError ), m_type( StanzaErrorTypeUndefined ),
          m_error( StanzaErrorUndefined ), m_appError( 0 )
      {}

      Error( const Tag * );

      Error( const Error& error );

      Error( StanzaErrorType type, StanzaError error, Tag * appError = 0 )
        : StanzaExtension( ExtError ), m_type( type ),
          m_error( error ), m_appError( appError )
      {}

      ~Error();

      /**
       * 
       */
      StanzaErrorType type() const { return m_type; }

      /**
       *
       */
      StanzaError error() const { return m_error; }

      /**
       * Returns the specific application 
       */
      const Tag * appError() const { return m_appError; }

      /**
       * Returns The text associated with a specific lang.
       * @param lang The lang for which the text has to be retrieved (empty for default lang)
       * @return The text associated with a specific lang.
       */
      std::string text( const std::string& lang = "default" ) const;

      /**
       * The 
       */
      Tag * tag() const;

    private:

      typedef std::map< std::string, std::string > StringMap;

      void setValues( const Tag * tag );
      StanzaErrorType m_type;
      StanzaError m_error;
      Tag * m_appError;
      StringMap m_text;
  };

}

#endif /* __ERROR_H__ */

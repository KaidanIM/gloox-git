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
  class GLOOX_API Error : public StanzaExtension
  {
    public:

      Error()
        : StanzaExtension( ExtError ), m_type( StanzaErrorTypeUndefined ),
          m_error( StanzaErrorUndefined ), m_appError( 0 )
      {}

      Error( const Tag* tag );

      Error( StanzaErrorType type, StanzaError error, Tag * appError = 0 )
        : StanzaExtension( ExtError ), m_type( type ),
          m_error( error ), m_appError( appError )
      {}

      virtual ~Error();

      /**
       *
       */
      StanzaErrorType type() const { return m_type; }

      /**
       *
       */
      StanzaError error() const { return m_error; }

      /**
       * This function can be used to retrieve the application-specific error
       * condition of a stanza error.
       * @return The application-specific error element of a stanza error.
       * 0 if no respective element was found or no error occured.
       */
      const Tag* appError() const { return m_appError; }

      /**
       * Returns the text of a error stanza for the given language if available.
       * If the requested language is not available, the default text (without
       * a xml:lang attribute) will be returned.
       * @param lang The language identifier for the desired language. It must
       * conform to section 2.12 of the XML specification and RFC 3066. If
       * empty, the default subject will be returned, if any.
       * @return The text of an error stanza. Empty for non-error stanzas.
       */
      const std::string text( const std::string& lang = "default" ) const;

      // reimplemented from StanzaExtension
      virtual const std::string filterString() const
      {
        return "/iq/error"
               "|/message/error"
               "|/presence/error"
               "|/subscription/error";
      }

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new Error( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

    private:

      Error( const Error& error );

      typedef std::map< std::string, std::string > StringMap;

      void setValues( const Tag* tag );
      StanzaErrorType m_type;
      StanzaError m_error;
      Tag* m_appError;
      StringMap m_text;
  };

}

#endif /* __ERROR_H__ */

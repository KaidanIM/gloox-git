/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "stanza.h"
#include "error.h"
#include "jid.h"
#include "util.h"
#include "stanzaextension.h"
#include "stanzaextensionfactory.h"

#include <cstdlib>

namespace gloox
{

  Stanza::Stanza( const std::string& name, const JID& to, const JID& from )
    : Tag( name ), m_xmllang( "default" ), m_from( from ), m_to( to )
  {
    if( !m_to.empty() )
      addAttribute( "to", m_to.full() );
    if( !m_from.empty() )
      addAttribute( "from", m_from.full() );
  }

  Stanza::Stanza( Tag* tag, bool rip )
    : Tag( tag->name(), tag->cdata() ), m_xmllang( "default" )
  {
    if( rip )
    {
      ripoff( tag );
    }
    else
    {
      const Tag::AttributeList& al = tag->attributes();
      Tag::AttributeList::const_iterator ia = al.begin();
      for( ; ia != al.end(); ++ia )
        m_attribs->push_back( new Tag::Attribute( *(*ia) ) );

      const TagList& l = tag->children();
      TagList::const_iterator it = l.begin();
      for( ; it != l.end(); ++it )
        m_children->push_back( (*it)->clone() );
    }

    m_from.setJID( findAttribute( "from" ) );
    m_to.setJID( findAttribute( "to" ) );
    m_id = findAttribute( "id" );

  }

  Stanza::~Stanza()
  {
    util::clear( m_extensionList );
  }

//   void Stanza::init()
//   {
//     m_xmllang = findAttribute( "xml:lang" );
//
//     if( hasAttribute( TYPE, "error" ) && hasChild( "error" ) )
//     {
//       Tag* e = findChild( "error" );
//
//       if( e->hasAttribute( TYPE, "cancel" ) )
//         m_stanzaErrorType = StanzaErrorTypeCancel;
//       else if( e->hasAttribute( TYPE, "continue" ) )
//         m_stanzaErrorType = StanzaErrorTypeContinue;
//       else if( e->hasAttribute( TYPE, "modify" ) )
//         m_stanzaErrorType = StanzaErrorTypeModify;
//       else if( e->hasAttribute( TYPE, "auth" ) )
//         m_stanzaErrorType = StanzaErrorTypeAuth;
//       else if( e->hasAttribute( TYPE, "wait" ) )
//         m_stanzaErrorType = StanzaErrorTypeWait;
//
//       const TagList& c = e->children();
//       TagList::const_iterator it = c.begin();
//       StanzaError err = StanzaErrorUndefined;
//       for( ; it != c.end(); ++it )
//       {
//         if( (*it)->name() == "bad-request" )
//           err = StanzaErrorBadRequest;
//         else if( (*it)->name() == "conflict" )
//           err = StanzaErrorConflict;
//         else if( (*it)->name() == "feature-not-implemented" )
//           err = StanzaErrorFeatureNotImplemented;
//         else if( (*it)->name() == "forbidden" )
//           err = StanzaErrorForbidden;
//         else if( (*it)->name() == "gone" )
//           err = StanzaErrorGone;
//         else if( (*it)->name() == "internal-server-error" )
//           err = StanzaErrorInternalServerError;
//         else if( (*it)->name() == "item-not-found" )
//           err = StanzaErrorItemNotFound;
//         else if( (*it)->name() == "jid-malformed" )
//           err = StanzaErrorJidMalformed;
//         else if( (*it)->name() == "not-acceptable" )
//           err = StanzaErrorNotAcceptable;
//         else if( (*it)->name() == "not-allowed" )
//           err = StanzaErrorNotAllowed;
//         else if( (*it)->name() == "not-authorized" )
//           err = StanzaErrorNotAuthorized;
//         else if( (*it)->name() == "recipient-unavailable" )
//           err = StanzaErrorRecipientUnavailable;
//         else if( (*it)->name() == "redirect" )
//           err = StanzaErrorRedirect;
//         else if( (*it)->name() == "registration-required" )
//           err = StanzaErrorRegistrationRequired;
//         else if( (*it)->name() == "remote-server-not-found" )
//           err = StanzaErrorRemoteServerNotFound;
//         else if( (*it)->name() == "remote-server-timeout" )
//           err = StanzaErrorRemoteServerTimeout;
//         else if( (*it)->name() == "resource-constraint" )
//           err = StanzaErrorResourceConstraint;
//         else if( (*it)->name() == "service-unavailable" )
//           err = StanzaErrorServiceUnavailable;
//         else if( (*it)->name() == "subscription-required" )
//           err = StanzaErrorSubscribtionRequired;
//         else if( (*it)->name() == "undefined-condition" )
//           err = StanzaErrorUndefinedCondition;
//         else if( (*it)->name() == "unexpected-request" )
//           err = StanzaErrorUnexpectedRequest;
//         else if( (*it)->name() == "text" )
//         {
//           setLang( m_errorText, (*it) );
//         }
//         else {
//           m_stanzaErrorAppCondition = (*it);
//         }
//
//         if( err != StanzaErrorUndefined && (*it)->hasAttribute( XMLNS, XMLNS_XMPP_STANZAS ) )
//         {
//           m_stanzaError = err;
//         }
//       }
//     }
//   }

  const Error* Stanza::error() const
  {
    return static_cast< const Error* >( findExtension( ExtError ) );
  }

  void Stanza::addExtension( StanzaExtension* se )
  {
    m_extensionList.push_back( se );
    addChild( se->tag() );
  }

  const StanzaExtension* Stanza::findExtension( StanzaExtensionType type ) const
  {
    StanzaExtensionList::const_iterator it = m_extensionList.begin();
    for( ; it != m_extensionList.end() && (*it)->type() != type; ++it ) ;
    return it != m_extensionList.end() ? (*it) : 0;
  }

  void Stanza::setLang( StringMap& map, const Tag* tag )
  {
    const std::string& lang = tag->findAttribute( "xml:lang" );
    map[ lang.empty() ? "default" : lang ] = tag->cdata();
  }

  const std::string Stanza::findLang( const StringMap& map, const std::string& lang )
  {
    StringMap::const_iterator it = map.find( lang );
    return ( it != map.end() ) ? (*it).second : std::string();
  }

}
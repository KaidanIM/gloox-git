/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "stanzaextensionfactory.h"

#include "gloox.h"
#include "util.h"
#include "stanza.h"
#include "stanzaextension.h"
#include "tag.h"

namespace gloox
{

  StanzaExtensionFactory::StanzaExtensionFactory()
  {
  }

  StanzaExtensionFactory::~StanzaExtensionFactory()
  {
    util::clear( m_extensions );
  }

  void StanzaExtensionFactory::registerExtension( StanzaExtension* ext )
  {
    SEList::iterator it = m_extensions.begin();
    for( ; it != m_extensions.end(); ++it )
    {
      if( ext->extensionType() == (*it)->extensionType() )
      {
        delete (*it);
        m_extensions.erase( it );
      }
    }
    m_extensions.push_back( ext );
  }

  void StanzaExtensionFactory::addExtensions( Stanza& stanza, Tag* tag )
  {
    TagList match;
    TagList::const_iterator it;
    SEList::const_iterator ite = m_extensions.begin();
    for( ; ite != m_extensions.end(); ++ite )
    {
      match = tag->findTagList( (*ite)->filterString() );
      it = match.begin();
      for( ; it != match.end(); ++it )
        stanza.addExtension( (*ite)->newInstance( (*it) ) );
    }
  }

}

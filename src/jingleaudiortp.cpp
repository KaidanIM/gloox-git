/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "jingleaudiortp.h"
#include "tag.h"
#include "util.h"

namespace gloox
{

  namespace Jingle
  {

    // ---- Jingle::AudioRTP::Payload ----
    Tag* AudioRTP::Payload::tag() const
    {
      if( m_attribs.empty() )
        return 0;

      Tag* t = new Tag( "payload-type" );
      StringMap::const_iterator it = m_attribs.begin();
      for( ; it != m_attribs.end(); ++it )
        t->addAttribute( (*it).first, (*it).second );

      it = m_parameters.begin();
      for( ; it != m_parameters.end(); ++it )
      {
        Tag* p = new Tag( t, "parameter" );
        p->addAttribute( "name", (*it).first );
        p->addAttribute( "value", (*it).second );
      }

      return t;
    }
    // ---- ~Jingle::AudioRTP::Payload ----

    // ---- Jingle::AudioRTP ----
    AudioRTP::AudioRTP( const PayloadList& payload )
    {
    }

    AudioRTP::~AudioRTP()
    {
    }

    const std::string& AudioRTP::filterString() const
    {
      static const std::string filter = "description[@xmlns='" + XMLNS_JINGLE_RTP +"']";
      return filter;
    }

    Tag* AudioRTP::tag() const
    {
      return 0;
    }

  }

}

/*
  Copyright (c) 2015 by Jakob Schröter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#if !defined( GLOOX_MINIMAL ) || defined( WANT_ROSTER_ITEM_EXCHANGE )

#include "rosterx.h"

#include "tag.h"
#include "util.h"

namespace gloox
{

  RosterX::RosterX( const Tag* tag )
  : StanzaExtension( ExtRosterX )
  {
    if( !tag || tag->name() != "x" || tag->xmlns() != XMLNS_ROSTER_X )
      return;

    TagList::const_iterator it = tag->children().begin();
    for ( ; it != tag->children().end(); ++it )
      m_list.push_back( new RosterXItemData( (*it) ) );

//     if( tag->parent() )
//       m_from = JID( tag->parent()->findAttribute( "from" ) );
  }

  RosterX::~RosterX()
  {
    util::clearList( m_list );
  }

  const std::string& RosterX::filterString() const
  {
    static const std::string filter = "/message/x"
                                      "|/iq/x";
    return filter;
  }

  Tag* RosterX::tag() const
  {
    Tag* x = new Tag( "x" );
    x->setXmlns( XMLNS_ROSTER_X );

    RosterXItemList::const_iterator it = m_list.begin();
    for( ; it != m_list.end(); ++it )
    {
      x->addChild( (*it)->tag() );
    }

    return x;
  }

  StanzaExtension* RosterX::clone() const
  {
    return 0;
  }

}

#endif // GLOOX_MINIMAL

/*
  Copyright (c) 2004-2015 by Jakob Schröter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#if !defined( GLOOX_MINIMAL ) || defined( WANT_ROSTER_ITEM_EXCHANGE )

#include "rosterxitemdata.h"
#include "tag.h"
#include "jid.h"
#include "util.h"

namespace gloox
{

  /* roster item exchange action values */
  static const char* actionValues [] = {
    "add",
    "delete",
    "modify"
  };

  RosterXItemData::RosterXItemData( RosterXItemData::XActions action, const JID& jid, const std::string& name,
                                    const StringList& groups )
    : RosterItemBase( jid, name, groups ),
      m_action( action )
  {
  }

  RosterXItemData::RosterXItemData( const Tag* tag )
    : RosterItemBase( tag )
  {
    if( !tag || tag->name() != "item" || !tag->hasAttribute( "action" ) )
      return;

    m_action = (RosterXItemData::XActions)util::lookup( tag->findAttribute( "action" ), actionValues );
  }

  Tag* RosterXItemData::tag() const
  {
    if( m_action == XInvalid )
      return 0;

    Tag* i = RosterItemBase::tag();
    i->addAttribute( "action", util::lookup( m_action, actionValues ) );

    return i;
  }

}

#endif // GLOOX_MINIMAL

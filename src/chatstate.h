/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef __CHATSTATE_H__
#define __CHATSTATE_H__

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
  class GLOOX_API ChatState : public StanzaExtension
  {
    public:

      ChatState( const Tag* tag );

      ChatState( ChatStateType type )
        : StanzaExtension( ExtChatState ), m_type( type )
      {}

      virtual ~ChatState() {}

      ChatStateType type() const { return m_type; }

      // reimplemented from StanzaExtension
      Tag * tag() const;

    private:

      ChatStateType m_type;

  };

}

#endif /* __CHATSTATE_H__ */

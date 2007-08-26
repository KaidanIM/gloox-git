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
#include "tag.h"
#include "oob.h"
#include "amp.h"
#include "vcardupdate.h"
#include "delayeddelivery.h"
#include "xdelayeddelivery.h"
#include "gpgsigned.h"
#include "gpgencrypted.h"
#include "capabilities.h"
#include "error.h"

namespace gloox
{

  StanzaExtension* StanzaExtensionFactory::create( const Tag* tag )
  {
    const std::string& name = tag->name();
    const std::string& xmlns = tag->findAttribute( XMLNS );
    if( name == "x" )
    {
      if( xmlns == XMLNS_X_DELAY )
        return new XDelayedDelivery( tag );
      else if( xmlns == XMLNS_X_OOB )
        return new OOB( tag );
      else if( xmlns == XMLNS_X_VCARD_UPDATE )
        return new VCardUpdate( tag );
      else if( xmlns == XMLNS_X_GPGSIGNED )
        return new GPGSigned( tag );
      else if( xmlns == XMLNS_X_GPGENCRYPTED )
        return new GPGEncrypted( tag );
      else if( xmlns == XMLNS_IQ_OOB )
        return new OOB( tag );
    }
    else if( name == "delay" && xmlns == XMLNS_DELAY )
    {
      return new DelayedDelivery( tag );
    }
    else if( name == "amp" && xmlns == XMLNS_AMP )
    {
      return new AMP( tag );
    }
    else if( name == "c" && xmlns == XMLNS_CAPS )
    {
      return new Capabilities( tag );
    }
    else if( name == "error" )
    {
      return new Error( tag );
    }


    return 0;
  }

}
/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBDISCOHANDLER_H_
#define PUBSUBDISCOHANDLER_H_

//#include "jid.h"
//#include "dataform.h"
#include <string>

namespace gloox
{

  class JID;
  class DataForm;

  namespace PubSub
  {

    class DiscoHandler
    {

      public:

        /**
	 *
	 */
        virtual void handleServiceInfoResult( const JID& service, int features ) = 0;

	
        /**
	 *
	 */
	virtual void handleNodeInfoResult( const JID& service,
	                                   const std::string& node,
					   const NodeType nodeType,
					   const DataForm& df ) = 0;

	
        /**
	 *
	 */
	virtual void handleNodeItemDiscovery( const JID& service,
	                                      const std::string& parent,
					      const DiscoNodeItemList& children ) = 0;
    };

  }

}

#endif /* PUBSUBDISCOHANDLER_H_ */

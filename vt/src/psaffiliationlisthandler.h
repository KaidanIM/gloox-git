/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PSAFFILIATIONLISTHANDLER_H__
#define PSAFFILIATIONLISTHANDLER_H__

#include "jid.h"
#include <string>
#include <map>
#include "pubsub.h"

namespace gloox
{

  namespace PubSub
  {

    /**
     * @brief A virtual interface for receiving (un)subscription result.
     *
     * Derive from this interface and register with the Manager.
     *
     * @author Jakob Schroeter <js@camaya.net>
     */
    class AffiliationListHandler
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~AffiliationListHandler() {}

        /**
         * Receives the Affiliation map for a specific service.
         * @param service The queried service.
         * @param subMap The map of each node's affiliation subscription.
         */
        virtual void handleAffiliationListResult( const JID& service, const AffiliationMap& subMap ) = 0;

        /**
         * Receives the affiliation listing error from a service (ie unsupported).
         * @param service The queried service.
         */
        virtual void handleAffiliationListError( const JID& service ) = 0;

    };

  }

}

#endif /* PSAFFILIATIONLISTHANDLER_H__ */

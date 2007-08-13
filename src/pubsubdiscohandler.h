/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBDISCOHANDLER_H__
#define PUBSUBDISCOHANDLER_H__

#include <string>

namespace gloox
{

  class JID;
  class DataForm;

  namespace PubSub
  {

    /**
     * Handler used for receiving results of PubSub services Disco queries.
     *
     * @author Vincent Thomasset
     */
    class DiscoHandler
    {

      public:

        /**
         * Default virtual destructor.
         */
        virtual ~DiscoHandler() {}

        /**
         * Receives results for a PubSub service info query.
         * @param service JID of the queried service.
         * @param features PubSub::PubSubFeature supported by the service.
         * @param error Describe the error if the request failed.
         * @see Manager::discoverServiceInfos
         */
        virtual void handleServiceInfos( const JID& service,
                                         int features,
                                         const Error* error = 0 ) = 0;

        /**
         * Receives results for a PubSub node info query. Either one of df or
         * error will be null, depending on whether the request succeeded or
         * not.
         * @param service JID of the queried service.
         * @param node Node ID of the queried node.
         * @param nodeType PubSub::NodeType of the queried node.
         * @param df DataForm of the node meta-data (may be null).
         * @param error Describe the error if the request failed.
         * @see Manager::discoverNodeInfos
         */
        virtual void handleNodeInfos( const JID& service,
                                      const std::string& node,
                                      const NodeType nodeType,
                                      const DataForm* df,
                                      const Error* error = 0 ) = 0;

        /**
         * Receives results for a PubSub item query. Either one of the children
         * list or the error will be null, depending on whether the request
         * succeeded or not.
         * @param service JID of the queried service.
         * @param parent Node ID of the node to query (empty if the root node
         *               has been queried).
         * @param error Describe the error if the request failed.
         * @see Manager::discoverNodeItems
         */
        virtual void handleNodeItems( const JID& service,
                                      const std::string& parent,
                                      const DiscoNodeItemList* children,
                                      const Error* error = 0 ) = 0;

    };

  }

}

#endif /* PUBSUBDISCOHANDLER_H__ */

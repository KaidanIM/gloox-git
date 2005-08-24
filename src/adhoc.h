/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#ifndef ADHOC_H__
#define ADHOC_H__

#include "adhoccommandprovider.h"
#include "disconodehandler.h"
#include "iqhandler.h"

#include <string>
#include <list>
#include <map>

namespace gloox
{

  class ClientBase;
  class Disco;
  class Stanza;

  /**
   * This class implements a provider for JEP-0050 (Ad-hoc Commands).
   * @author Jakob Schroeter <js@camaya.net>
   */
  class Adhoc : public DiscoNodeHandler, IqHandler
  {
    public:
      /**
       * Constructor.
       * You should access the Adhoc object through the @c Client object.
       * Creates a new Adhoc client that registers as IqHandler with @c Client.
       * @param parent The ClientBase used for XMPP communication.
       * @param disco The Disco object used to announce available commands.
       */
      Adhoc( ClientBase *parent, Disco *disco );

      /**
       * Virtual destructor.
       */
      virtual ~Adhoc();

      // reimplemented from DiscoNodeHandler
      virtual FeatureList handleDiscoNodeFeatures( const std::string& node );

      // reimplemented from DiscoNodeHandler
      virtual IdentityMap handleDiscoNodeIdentities( const std::string& node, std::string& name );

      // reimplemented from DiscoNodeHandler
      virtual ItemMap handleDiscoNodeItems( const std::string& node );

      // reimplemented from IqHandler
      virtual bool handleIq( Stanza *stanza );

      // reimplemented from IqHandler
      virtual bool handleIqID( Stanza *stanza, int context ) {};

      /**
       * Using this function, you can register a @ref AdhocCommandProvider -derived object as
       * handler for a specific Ad-hoc Command as defined in JEP-0050.
       * @param acp The obejct to register for the specified command.
       * @param command The name of the command. Will be announced in disco#items.
       * @param name The natural-language name of the command. Will be announced in disco#items.
       */
      void registerAdhocCommandProvider( AdhocCommandProvider *acp, const std::string& command,
                                         const std::string& name );

    private:
      typedef std::map<const std::string, AdhocCommandProvider*> AdhocCommandProviderMap;

      ClientBase *m_parent;
      Disco *m_disco;

      AdhocCommandProviderMap m_adhocCommandProviders;
      DiscoNodeHandler::ItemMap m_items;

  };

};

#endif // ADHOC_H__

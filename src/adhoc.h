/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
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
       * You should access the Adhoc object through the @c JClient object.
       * Creates a new Adhoc client that registers as IqHandler with @c JClient.
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
      virtual IdentityMap handleDiscoNodeIdentities( const std::string& node );

      // reimplemented from DiscoNodeHandler
      virtual ItemMap handleDiscoNodeItems( const std::string& node );

      // reimplemented from IqHandler
      virtual bool handleIq( const Stanza& stanza );

      // reimplemented from IqHandler
      virtual bool handleIqID( const Stanza& stanza, int context ) {};

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

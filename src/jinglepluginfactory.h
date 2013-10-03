/*
  Copyright (c) 2008-2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEPLUGINFACTORY_H__
#define JINGLEPLUGINFACTORY_H__

#include "jingleplugin.h"

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0.7
     */
    class PluginFactory
    {
      public:
        /**
         *
         */
        PluginFactory();

        /**
         *
         */
        virtual ~PluginFactory();

        /**
         *
         */
        void registerPlugin( Plugin* plugin );

        /**
         *
         */
        void addPlugins( Plugin& plugin, const Tag* tag );

      private:
        PluginList m_plugins;

    };

  }
}

#endif // JINGLEPLUGINFACTORY_H__

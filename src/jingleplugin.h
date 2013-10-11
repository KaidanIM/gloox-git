/*
  Copyright (c) 2008-2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEPLUGIN_H__
#define JINGLEPLUGIN_H__

#include "macros.h"
#include "util.h"

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    class Plugin;
    class PluginFactory;

    /**
     * A list of Jingle plugins.
     */
    typedef std::list<const Plugin*> PluginList;

    /**
     * @brief An abstraction of a Jingle plugin. This is part of Jingle (@xep{0166}) et al.
     *
     * This is the base class for Content and all other pluggable Jingle-related containers, e.g.
     * session information, such as the 'ringing' info in Jingle Audio, or Jingle DTMF, etc.
     *
     * A Plugin abstracts the XML that gets sent and received as part of a Jingle session negotiation.
     *
     * XEP Version: 1.1
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0.5
     */
    class GLOOX_API Plugin
    {

      friend class PluginFactory;

      public:
        /**
         * Simple initializer.
         */
        Plugin() : m_factory( 0 ) {}

        /**
         * Virtual destructor.
         */
        virtual ~Plugin() { util::clearList( m_plugins ) ; }

        /**
         * Adds another Plugin as child.
         * @param plugin A plugin to be embedded. Will be owned by this instance and deleted in the destructor.
         */
        void addPlugin( const Plugin* plugin ) { if( plugin ) m_plugins.push_back( plugin ); }

        /**
         * Returns a reference to a list of embedded plugins.
         * @return A reference to a list of embedded plugins.
         */
        const PluginList& plugins() const { return m_plugins; }

        /**
         * Reimplement this function if your plugin wants to add anything to the list of
         * features announced via Disco.
         * @return A list of additional feature strings.
         */
        virtual const StringList features() const { return StringList(); }

        /**
         * Returns an XPath expression that describes a path to child elements of a
         * jingle element that the plugin handles.
         * The result should be a single Tag.
         *
         * @return The plugin's filter string.
         */
        virtual const std::string& filterString() const = 0;

        /**
         * Returns a Tag representation of the plugin.
         * @return A Tag representation of the plugin.
         */
        virtual Tag* tag() const = 0;

        /**
         * Returns a new instance of the same plugin type,
         * based on the Tag provided.
         * @param tag The Tag to parse and create a new instance from.
         * @return The new plugin instance.
         */
        virtual Plugin* newInstance( const Tag* tag ) const = 0;

        /**
         * Creates an identical deep copy of the current instance.
         * @return An identical deep copy of the current instance.
         */
        virtual Plugin* clone() const = 0;

      protected:
        PluginList m_plugins;
        PluginFactory* m_factory;

      private:
        void setFactory( PluginFactory* factory ) { m_factory = factory; }

    };

  }

}

#endif // JINGLEPLUGIN_H__

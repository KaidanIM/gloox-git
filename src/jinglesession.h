/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLESESSION_H__
#define JINGLESESSION_H__

#include "stanzaextension.h"
#include "tag.h"
#include "iqhandler.h"

#include <string>

namespace gloox
{

  class ClientBase;

  /**
   * @brief The namespace containing Jingle-related (XEP-0166 et. al.) classes.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  namespace Jingle
  {

    class Description;
    class Transport;
    class SessionHandler;
    class Content;

    /**
     * A list of Jingle Content Types.
     */
    typedef std::list<const Content*> ContentList;

    /**
     * @brief This is an implementation of a Jingle Session (XEP-0166).
     *
     * XEP Version: 0.25
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class GLOOX_API Session : public IqHandler
    {

      public:
        /**
         * Session state.
         */
        enum State
        {
          Ended,                    /**< */
          Pending,                  /**< */
          Active                    /**< */
        };

        /**
         */
        Session( ClientBase* parent, const JID& callee, SessionHandler* jsh );

        /**
         * Virtual Destructor.
         */
        virtual ~Session();

        /**
         *
         */
        bool initiate();

        /**
         *
         */
        bool terminate();

        /**
         *
         */
        State state() const { return m_state; }

        /**
         * Use this function to add a new Jingle Content Type to
         * this Session.
         * A Content Type consists of a Description (Application Format), e.g. AudioRTP,
         * and a Transport, e.g. ICEUDP.
         * This enables the Session to offer that Content Type to the peer.
         * @param content A ContentType to add.
         */
        void addContent( Content* content )
        {
          if( content )
            m_contents.push_back( content );
        }

        // reimplemented from IqHandler
        virtual bool handleIq( const IQ& iq );

        // reimplemented from IqHandler
        virtual void handleIqID( const IQ& iq, int context );

#ifdef JINGLESESSION_TEST
      public:
#else
      private:
#endif
        enum Action
        {
          ContentAccept,
          ContentAdd,
          ContentModify,
          ContentRemove,
          ContentReplace,
          SessionAccept,
          SessionInfo,
          SessionInitiate,
          SessionTerminate,
          TransportInfo,
          InvalidAction
        };

        /**
         * @brief This is an implementation of XEP-0166 (Jingle) as a StanzaExtension.
         *
         * XEP Version: 0.25
         * @author Jakob Schroeter <js@camaya.net>
         * @since 1.1
         */
        class Jingle : public StanzaExtension
        {

          public:
            /**
             * Constructs a new object and fills it according to the parameters.
             * @param action The Action to carry out.
             */
            Jingle( const ContentList& contents, Action action );

            /**
             *
             */
            Jingle( const Content* content, Action action );

            /**
             * Constructs a new object from the given Tag.
             * @param tag The Tag to parse.
             */
            Jingle( const Tag* tag = 0 );

            /**
             * Virtual Destructor.
             */
            virtual ~Jingle();

            /**
             *
             */
            void addContent( const Content* content, Action action );

            /**
             *
             */
            const std::string& sid() const { return m_sid; }

            /**
             *
             */
            Action action() const { return m_action; }

            // reimplemented from StanzaExtension
            virtual const std::string& filterString() const;

            // reimplemented from StanzaExtension
            virtual StanzaExtension* newInstance( const Tag* tag ) const
            {
              return new Jingle( tag );
            }

            // reimplemented from StanzaExtension
            virtual Tag* tag() const;

          private:
            Action m_action;
            std::string m_sid;
            ContentList m_contents;

        };

        ClientBase* m_parent;
        State m_state;
        JID m_callee;
        SessionHandler* m_handler;
        ContentList m_contents;
        std::string m_sid;
        bool m_valid;

    };

  }

}

#endif // JINGLESESSION_H__

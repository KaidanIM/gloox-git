/*
  Copyright (c) 2007-2013 by Jakob Schroeter <js@camaya.net>
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
#include "jingleplugin.h"

#include <string>

namespace gloox
{

  class ClientBase;

  /**
   * @brief The namespace containing Jingle-related (@xep{0166} et. al.) classes.
   *
   * See @link gloox::Jingle::SessionManager SessionManager @endlink for more information
   * about Jingle in gloox.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0.5
   */
  namespace Jingle
  {

    class Description;
    class Transport;
    class SessionHandler;
    class Content;

    /**
     * Jingle Session actions.
     */
    enum Action
    {
      ContentAccept,                /**< Accept a content-add action received from another party. */
      ContentAdd,                   /**< Add one or more new content definitions to the session. */
      ContentModify,                /**< Change the directionality of media sending. */
      ContentReject,                /**< Reject a content-add action received from another party. */
      ContentRemove,                /**< Remove one or more content definitions from the session. */
      DescriptionInfo,              /**< Exchange information about parameters for an application type. */
      SecurityInfo,                 /**< Send information related to establishment or maintenance of security preconditions. */
      SessionAccept,                /**< Definitively accept a session negotiation. */
      SessionInfo,                  /**< Send session-level information, such as a ping or a ringing message. */
      SessionInitiate,              /**< Request negotiation of a new Jingle session. */
      SessionTerminate,             /**< End an existing session. */
      TransportAccept,              /**< Accept a transport-replace action received from another party. */
      TransportInfo,                /**< Exchange transport candidates. */
      TransportReject,              /**< Reject a transport-replace action received from another party. */
      TransportReplace,             /**< Redefine a transport method or replace it with a different method. */
      InvalidAction                 /**< Invalid action. */
    };

    /**
     * @brief This is an implementation of a Jingle Session (@xep{0166}).
     *
     * See @link gloox::Jingle::SessionManager Jingle::SessionManager @endlink for info on how to use
     * Jingle in gloox.
     *
     * XEP Version: 1.1
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0.5
     */
    class GLOOX_API Session : public IqHandler
    {

      friend class SessionManager;

      public:
        /**
         * Session state.
         */
        enum State
        {
          Ended,                    /**< The session has ended or was not active yet. */
          Pending,                  /**< The session has been initiated but has not yet been accepted by the remote party. */
          Active                    /**< The session is active. */
        };

        /**
         * @brief An abstraction of a Jingle (@xep{0166}) session terminate reason.
         *
         * XEP Version: 1.1
         *
         * @author Jakob Schroeter <js@camaya.net>
         * @since 1.0.5
         */
        class GLOOX_API Reason : public Plugin
        {
          public:
            /**
             * Defined reasons for terminating a Jingle Session.
             */
            enum Reasons
            {
              AlternativeSession,           /**< An alternative session exists that should be used. */
              Busy,                         /**< The terminating party is busy. */
              Cancel,                       /**< The session has been canceled. */
              ConnectivityError,            /**< Connectivity error. */
              Decline,                      /**< The terminating party formally declines the request. */
              Expired,                      /**< The session has expired. */
              FailedApplication,            /**< Application type setup failed. */
              FailedTransport,              /**< Transport setup has failed. */
              GeneralError,                 /**< General error. */
              Gone,                         /**< Participant went away. */
              IncompatibleParameters,       /**< Offered or negotiated application type parameters not supported. */
              MediaError,                   /**< Media error. */
              SecurityError,                /**< Security error. */
              Success,                      /**< Session terminated after successful call. */
              Timeout,                      /**< A timeout occured. */
              UnsupportedApplications,      /**< The terminating party does not support any of the offered application formats. */
              UnsupportedTransports,        /**< The terminating party does not support any of the offered transport methods. */
              InvalidReason                 /**< Invalid reason. */
            };

            /**
             * Constructor.
             * @param reason The reason for the termination of the session.
             * @param sid An optional session ID (only used if reason is AlternativeSession).
             * @param text An optional human-readable text explaining the reason for the session termination.
             */
            Reason( Reasons reason, const std::string& sid = EmptyString,
                    const std::string& text = EmptyString );

            /**
             * Constructs a new element by parsing the given Tag.
             * @param tag A tag to parse.
             */
            Reason( const Tag* tag = 0 );

            /**
             * Virtual destructor.
             */
            virtual ~Reason() {}

            /**
             * Returns the reason for the session termination.
             * @return The reason for the session termination.
             */
            Reasons reason() const { return m_reason; }

            /**
             * Returns the session ID of the alternate session, if given (only applicable
             * if reason() returns AlternativeSession).
             * @return The session ID of the alternative session, or an empty string.
             */
            const std::string& sid() const { return m_sid; }

            /**
             * Returns the content of an optional, human-readable
             * &lt;text&gt; element.
             * @return An optional text describing the reason for the terminate action.
             */
            const std::string& text() const { return m_text; }

            // reimplemented from Plugin
            virtual const std::string& filterString() const;

            // reimplemented from Plugin
            virtual Tag* tag() const;

            // reimplemented from Plugin
            virtual Plugin* newInstance( const Tag* tag ) const { return new Reason( tag ); }

            // reimplemented from Plugin
            virtual Plugin* clone() const;

          private:
            Reasons m_reason;
            std::string m_sid;
            std::string m_text;

        };

        /**
         * @brief This is an abstraction of Jingle's (@xep{0166}) &lt;jingle&gt; element as a StanzaExtension.
         *
         * XEP Version: 1.1
         * @author Jakob Schroeter <js@camaya.net>
         * @since 1.0.5
         */
        class Jingle : public StanzaExtension
        {

          friend class Session;

          public:
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
             * Returns the session ID.
             * @return The session ID.
             */
            const std::string& sid() const { return m_sid; }

            /**
             * Set's the session's initiator.
             * @param jid The session initiator's JID.
             */
            void setInitiator( const JID& jid ) { m_initiator = jid; }

            /**
             * Returns the initiator.
             * @return The initiator.
             */
            const JID& initiator() const { return m_initiator; }

            /**
             * Set's the session's responder.
             * @param jid The session responder's JID.
             */
            void setResponder( const JID& jid ) { m_responder = jid; }

            /**
             * Returns this Jingle's action.
             * @return The action.
             */
            Action action() const { return m_action; }

            /**
             * Adds a Plugin as child.
             * @param plugin A plugin to be embedded. Will be owned by this instance and deleted in the destructor.
             */
            void addPlugin( const Plugin* plugin ) { if( plugin ) m_plugins.push_back( plugin ); }

            /**
             * Returns a reference to a list of embedded plugins.
             * @return A reference to a list of embedded plugins.
             */
            const PluginList& plugins() const { return m_plugins; }

            /**
             * Returns the tag to build plugins from.
             * @return The tag to build plugins from.
             */
            Tag* embeddedTag() const { return m_tag; }

            // reimplemented from StanzaExtension
            virtual const std::string& filterString() const;

            // reimplemented from StanzaExtension
            virtual StanzaExtension* newInstance( const Tag* tag ) const
            {
              return new Jingle( tag );
            }

            // reimplemented from StanzaExtension
            virtual Tag* tag() const;

            // reimplemented from StanzaExtension
            virtual StanzaExtension* clone() const;

#ifdef JINGLE_TEST
          public:
#else
          private:
#endif
            /**
             * Constructs a new object and fills it according to the parameters.
             * @param action The Action to carry out.
             * @param initiator The full JID initiator of the session flow.
             * @param plugins A list of contents (plugins) for the &lt;jingle&gt;
             * element. Usually, this will be Content objects.
             * @param sid The session ID:
             */
            Jingle( Action action, const JID& initiator,
                    const PluginList& plugins, const std::string& sid );

            /**
             * Constructs a new object and fills it according to the parameters.
             * @param action The Action to carry out.
             * @param initiator The full JID initiator of the session flow.
             * @param plugin A single content (plugin) for the &lt;jingle&gt;
             * element. This may be Content object, but can be any Plugin-derived object.
             * @param sid The session ID:
             */
            Jingle( Action action, const JID& initiator,
                    const Plugin* plugin, const std::string& sid );

//             /**
//              * Copy constructor.
//              * @param right The instance to copy.
//              */
//             Jingle( const Jingle& right );

            Action m_action;
            std::string m_sid;
            JID m_initiator;
            JID m_responder;
            PluginList m_plugins;
            Tag* m_tag;

        };

        /**
         * Virtual Destructor.
         */
        virtual ~Session();

        /**
         * Sends a 'content-accept' notification.
         * @param content The accepted content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool contentAccept( const Content* content );

        /**
         * Sends a 'content-add' request.
         * @param content The proposed content to be added.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool contentAdd( const Content* content );

        /**
         * Sends a 'content-add' request.
         * @param contents A list of proposed content to be added.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool contentAdd( const PluginList& contents );

        /**
         * Sends a 'content-modify' request.
         * @param content The proposed content type to be modified.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool contentModify( const Content* content );

        /**
         * Sends a 'content-reject' reply.
         * @param content The rejected content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool contentReject( const Content* content );

        /**
         * Sends a 'content-remove' request.
         * @param content The content type to be removed.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool contentRemove( const Content* content );

        /**
         * Sends a 'description-info' notice.
         * @param info The payload.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool descriptionInfo( const Plugin* info );

        /**
         * Sends a 'security-info' notice.
         * @param info A security pre-condition.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool securityInfo( const Plugin* info );

        /**
         * Accepts an incoming session with the given content.
         * @param content A pair of application description and transport method wrapped in a Content that describes
         * the accepted session parameters.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool sessionAccept( const Content* content );

        /**
         * Sends a 'session-info' notice.
         * @param info The payload.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool sessionInfo( const Plugin* info );

        /**
         * Initiates a session with a remote entity.
         * @param content A Content object. You may use initiate( const PluginList& contents ) for more than one Content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool sessionInitiate( const Content* content );

        /**
         * Initiates a session with a remote entity.
         * @param plugins A list of Content objects. It is important to pass a (list of) Content objects here.
         * Even though e.g. Jingle::ICEUDP are Plugin-derived, too, using anything other than Content here will result
         * in erroneous behaviour at best. You may use sessionInitiate( const Content* content ) for just one Content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool sessionInitiate( const PluginList& plugins );

        /**
         * Terminates the current session, if it is at least in Pending state, with the given reason. The sid parameter
         * is ignored unless the reason is AlternativeSession.
         * @param reason The reason for terminating the session.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool sessionTerminate( Session::Reason* reason );

        /**
         * Sends a 'transport-accept' reply.
         * @param content The accepted transport wrapped in a Content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool transportAccept( const Content* content );

        /**
         * Sends a 'transport-info' notice.
         * @param info The payload.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool transportInfo( const Plugin* info );

        /**
         * Sends a 'transport-reject' reply.
         * @param content The rejected transport wrapped in a Content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool transportReject( const Content* content );

        /**
         * Sends a 'transport-replace' request.
         * @param content The proposed transport to be replaced wrapped in a Content.
         * @return @b False if a prerequisite is not met, @b true otherwise.
         */
        bool transportReplace( const Content* content );

        /**
         * Returns the session's state.
         * @return The session's state.
         */
        State state() const { return m_state; }

        /**
         * Sets the session's ID.
         * @param sid  The session's id.
         */
        void setSID( const std::string& sid ) { m_sid = sid; }

        /**
         * Returns the session's ID.
         * @return The session's ID.
         */
        const std::string& sid() const { return m_sid; }

        // reimplemented from IqHandler
        virtual bool handleIq( const IQ& iq );

        // reimplemented from IqHandler
        virtual void handleIqID( const IQ& iq, int context );

#ifdef JINGLE_TEST
      public:
#else
      private:
#endif
        /**
         * Creates a new Jingle Session.
         * @param parent The ClientBase to use for communication.
         * @param callee The remote end of the session.
         * @param jsh The handler to receive events and results.
         */
        Session( ClientBase* parent, const JID& callee, SessionHandler* jsh );

        /**
         * Creates a new Session from the incoming Jingle object.
         * This is a NOOP for Jingles that have an action() different from SessionInitiate.
         * @param parent The ClientBase to use for communication.
         * @param jingle The Jingle object to init the Session from.
         * @param jsh The handler to receive events and results.
         */
        Session( ClientBase* parent, const Session::Jingle* jingle,
                 SessionHandler* jsh );

        bool doAction( Action action, const Plugin* plugin );
        bool doAction( Action action, const PluginList& plugin );

        ClientBase* m_parent;
        State m_state;
        JID m_callee;
        JID m_initiator;
        SessionHandler* m_handler;
        std::string m_sid;
        bool m_valid;

    };

  }

}

#endif // JINGLESESSION_H__

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


#ifndef GLOOX_H__
#define GLOOX_H__

#define XMLNS_CLIENT            "jabber:client"
#define XMLNS_COMPONENT_ACCEPT  "jabber:component:accept"
#define XMLNS_COMPONENT_CONNECT "jabber:component:connect"

#define XMLNS_DISCO_INFO        "http://jabber.org/protocol/disco#info"
#define XMLNS_DISCO_ITEMS       "http://jabber.org/protocol/disco#items"
#define XMLNS_ADHOC_COMMANDS    "http://jabber.org/protocol/commands"
#define XMLNS_ROSTER            "jabber:iq:roster"
#define XMLNS_VERSION           "jabber:iq:version"
#define XMLNS_REGISTER          "jabber:iq:register"
#define XMLNS_PRIVACY           "jabber:iq:privacy"
#define XMLNS_AUTH              "jabber:iq:auth"
#define XMLNS_PRIVATE_XML       "jabber:iq:private"
#define XMLNS_ROSTER            "jabber:iq:roster"
#define XMLNS_BOOKMARKS         "storage:bookmarks"
#define XMLNS_ANNOTATIONS       "storage:rosternotes"

#define XMLNS_XMPP_STANZAS      "urn:ietf:params:xml:ns:xmpp-stanzas"
#define XMLNS_STREAM_TLS        "urn:ietf:params:xml:ns:xmpp-tls"
#define XMLNS_STREAM_SASL       "urn:ietf:params:xml:ns:xmpp-sasl"
#define XMLNS_STREAM_BIND       "urn:ietf:params:xml:ns:xmpp-bind"
#define XMLNS_STREAM_SESSION    "urn:ietf:params:xml:ns:xmpp-session"
#define XMLNS_STREAM_IQAUTH     "http://jabber.org/features/iq-auth"
#define XMLNS_STREAM_IQREGISTER "http://jabber.org/features/iq-register"

#define GLOOX_VERSION "0.4"

/**
 * The namespace for the gloox library.
 * @author Jakob Schroeter <js@camaya.net>
 * @since 0.3
 */
namespace gloox
{

  /**
   * This describes the possible states of a stream.
   */
  enum ConnectionState
  {
    STATE_ERROR,                    /**< An error occured. The stream has been closed. */
    STATE_IO_ERROR,                 /**< An I/O error occured. */
    STATE_OUT_OF_MEMORY,            /**< Out of memory. Uhoh. */
    STATE_TLS_FAILED,               /**< TLS negotiation failed. */
    STATE_PARSE_ERROR,              /**< XML parse error. */
    STATE_NO_SUPPORTED_AUTH,        /**< The auth mechanisms the server offers are not supported. */
    STATE_AUTHENTICATION_FAILED,    /**< Authentication failed. Username/password wrong or account does
                                     * not exist. */
    STATE_DISCONNECTED,             /**< The client is in disconnected state. */
    STATE_CONNECTING,               /**< The client is currently trying to establish a connection. */
    STATE_CONNECTED,                /**< The client is connected to the server but authentication is not
                                     * (yet) done. */
    STATE_AUTHENTICATED             /**< The client has successfully authenticated itself to the server. */
  };

  /**
   *
   */
  enum SaslMechanisms
  {
    GLOOX_SASL_PLAIN,               /**< */
    GLOOX_SASL_DIGEST_MD5           /**< */
  };

  /**
   *
   */
  enum StanzaType
  {
    STANZA_UNDEFINED,               /**< Undefined. */
    STANZA_IQ,                      /**< */
    STANZA_MESSAGE,                 /**< */
    STANZA_S10N,                    /**< */
    STANZA_PRESENCE,                /**< */
  };

  /**
   *
   */
  enum StanzaSubType
  {
    STANZA_SUB_UNDEFINED,               /**< Undefined. */
    STANZA_IQ_GET,                  /**< The stanza is a request for information or requirements. */
    STANZA_IQ_SET,                  /**<
                                     * The stanza provides required data, sets new values, or
                                     * replaces existing values.
                                     */
    STANZA_IQ_RESULT,               /**< The stanza is a response to a successful get or set request. */
    STANZA_IQ_ERROR,                /**<
                                     * An error has occurred regarding processing or
                                     * delivery of a previously-sent get or set (see Stanza Errors
                                     * (Section 9.3)).
                                     */
    STANZA_PRES_UNAVAILABLE,        /**<
                                     * Signals that the entity is no longer available for
                                     * communication.
                                     */
    STANZA_PRES_AVAILABLE,          /**<
                                     * Signals to the server that the sender is online and available
                                     * for communication.
                                     */
    STANZA_PRES_PROBE,              /**<
                                     * A request for an entity's current presence; SHOULD be
                                     * generated only by a server on behalf of a user.
                                     */
    STANZA_PRES_ERROR,              /**<
                                     * An error has occurred regarding processing or delivery of
                                     * a previously-sent presence stanza.
                                     */
    STANZA_S10N_SUBSCRIBE,          /**<
                                     * The sender wishes to subscribe to the recipient's
                                     * presence.
                                     */
    STANZA_S10N_SUBSCRIBED,         /**<
                                     * The sender has allowed the recipient to receive
                                     * their presence.
                                     */
    STANZA_S10N_UNSUBSCRIBE,        /**<
                                     * The sender is unsubscribing from another entity's
                                     * presence.
                                     */
    STANZA_S10N_UNSUBSCRIBED,       /**<
                                     * The subscription request has been denied or a
                                     * previously-granted subscription has been cancelled.
                                     */
    STANZA_MESSAGE_CHAT,            /**<
                                     * The message is sent in the context of a one-to-one chat
                                     * conversation.  A compliant client SHOULD present the message in an
                                     * interface enabling one-to-one chat between the two parties,
                                     * including an appropriate conversation history.
                                     */
    STANZA_MESSAGE_ERROR,           /**<
                                     * An error has occurred related to a previous message sent
                                     * by the sender (for details regarding stanza error syntax, refer to
                                     * [XMPP-CORE]).  A compliant client SHOULD present an appropriate
                                     * interface informing the sender of the nature of the error.
                                     */
    STANZA_MESSAGE_GROUPCHAT,       /**<
                                     * The message is sent in the context of a multi-user
                                     * chat environment (similar to that of [IRC]).  A compliant client
                                     * SHOULD present the message in an interface enabling many-to-many
                                     * chat between the parties, including a roster of parties in the
                                     * chatroom and an appropriate conversation history.  Full definition
                                     * of XMPP-based groupchat protocols is out of scope for this memo.
                                     */
    STANZA_MESSAGE_HEADLINE,        /**<
                                     * The message is probably generated by an automated
                                     * service that delivers or broadcasts content (news, sports, market
                                     * information, RSS feeds, etc.).  No reply to the message is
                                     * expected, and a compliant client SHOULD present the message in an
                                     * interface that appropriately differentiates the message from
                                     * standalone messages, chat sessions, or groupchat sessions (e.g.,
                                     * by not providing the recipient with the ability to reply).
                                     */
    STANZA_MESSAGE_NORMAL           /**<
                                     * The message is a single message that is sent outside the
                                     * context of a one-to-one conversation or groupchat, and to which it
                                     * is expected that the recipient will reply.  A compliant client
                                     * SHOULD present the message in an interface enabling the recipient
                                     * to reply, but without a conversation history.
                                     */
  };

  /**
   * Describes the possible 'available presence' types.
   */
  enum PresenceStatus
  {
    PRESENCE_UNKNOWN,               /**< Unknown status. */
    PRESENCE_AVAILABLE,             /**< The entity or resource is online and available. */
    PRESENCE_CHAT,                  /**< The entity or resource is actively interested in chatting. */
    PRESENCE_AWAY,                  /**< The entity or resource is temporarily away. */
    PRESENCE_DND,                   /**< The entity or resource is busy (dnd = "Do Not Disturb"). */
    PRESENCE_XA,                    /**< The entity or resource is away for an extended period (xa =
                                     * "eXtended Away"). */
    PRESENCE_UNAVAILABLE,           /**< The entity or resource if offline. */
  };

};

#endif // GLOOX_H__

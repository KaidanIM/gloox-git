/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef GLOOX_H__
#define GLOOX_H__

#include <string>
#include <list>
#include <map>

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

#define XMPP_STREAM_VERSION_MAJOR  "1"
#define XMPP_STREAM_VERSION_MINOR  "0"
#define GLOOX_VERSION              "0.5"

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
    STATE_DISCONNECTED,             /**< The client is in disconnected state. */
    STATE_CONNECTING,               /**< The client is currently trying to establish a connection. */
    STATE_CONNECTED,                /**< The client is connected to the server but authentication is not
                                     * (yet) done. */
  };

  /**
   * This describes connection error conditions.
   */
  enum ConnectionError
  {
    CONN_OK,                       /**< Not really an error. Everything went just fine. */
    CONN_STREAM_ERROR,             /**< An stream error occured. The stream has been closed. */
    CONN_STREAM_CLOSED,            /**< The stream has been closed graciously. */
    CONN_IO_ERROR,                 /**< An I/O error occured. */
    CONN_OUT_OF_MEMORY,            /**< Out of memory. Uhoh. */
    CONN_NO_SUPPORTED_AUTH,        /**< The auth mechanisms the server offers are not supported. */
    CONN_TLS_FAILED,               /**< The server's certificate could not be verified. */
    CONN_AUTHENTICATION_FAILED,    /**< Authentication failed. Username/password wrong or account does
                                     * not exist. */
    CONN_USER_DISCONNECTED,        /**< The user (or higher-level protocol) requested a disconnecct. */
  };

  /**
   * This decribes stream error conditions as defined in RFC 3920 Sec. 4.7.3.
   */
  enum StreamError
  {
    ERROR_UNDEFINED,                /**< An undefined/unknown error occured. Also used if a diconnect was
                                     * user-initiated. Also set before and during a established connection
                                    (where obviously no error occured). */
    ERROR_BAD_FORMAT,               /**< The entity has sent XML that cannot be processed;
                                     * this error MAY be used instead of the more specific XML-related
                                     * errors, such as &lt;bad-namespace-prefix/&gt;, &lt;invalid-xml/&gt;,
                                     * &lt;restricted-xml/&gt;, &lt;unsupported-encoding/&gt;, and
                                     * &lt;xml-not-well-formed/&gt;, although the more specific errors are
                                     * preferred. */
    ERROR_BAD_NAMESPACE_PREFIX,     /**< The entity has sent a namespace prefix that is unsupported, or has
                                     * sent no namespace prefix on an element that requires such a prefix
                                     * (see XML Namespace Names and Prefixes (Section 11.2)). */
    ERROR_CONFLICT,                 /**< The server is closing the active stream for this entity because a
                                     * new stream has been initiated that conflicts with the existing
                                     * stream. */
    ERROR_CONNECTION_TIMEOUT,       /**< The entity has not generated any traffic over the stream for some
                                     * period of time (configurable according to a local service policy).*/
    ERROR_HOST_GONE,                /**< the value of the 'to' attribute provided by the initiating entity
                                     * in the stream header corresponds to a hostname that is no longer
                                     * hosted by the server.*/
    ERROR_HOST_UNKNOWN,             /**< The value of the 'to' attribute provided by the initiating entity
                                     * in the stream header does not correspond to a hostname that is hosted
                                     * by the server. */
    ERROR_IMPROPER_ADDRESSING,      /**< A stanza sent between two servers lacks a 'to' or 'from' attribute
                                     * (or the attribute has no value). */
    ERROR_INTERNAL_SERVER_ERROR,    /**< the server has experienced a misconfiguration or an
                                     * otherwise-undefined internal error that prevents it from servicing the
                                     * stream. */
    ERROR_INVALID_FROM,             /**< The JID or hostname provided in a 'from' address does not match an
                                     * authorized JID or validated domain negotiated between servers via SASL
                                     * or dialback, or between a client and a server via authentication and
                                     * resource binding.*/
    ERROR_INVALID_ID,               /**< The stream ID or dialback ID is invalid or does not match an ID
                                     * previously provided. */
    ERROR_INVALID_NAMESPACE,        /**< The streams namespace name is something other than
                                     * "http://etherx.jabber.org/streams" or the dialback namespace name is
                                     * something other than "jabber:server:dialback" (see XML Namespace Names
                                     * and Prefixes (Section 11.2)). */
    ERROR_INVALID_XML,              /**< The entity has sent invalid XML over the stream to a server that
                                     * performs validation (see Validation (Section 11.3)). */
    ERROR_NOT_AUTHORIZED,           /**< The entity has attempted to send data before the stream has been
                                     * authenticated, or otherwise is not authorized to perform an action
                                     * related to stream negotiation; the receiving entity MUST NOT process
                                     * the offending stanza before sending the stream error. */
    ERROR_POLICY_VIOLATION,         /**< The entity has violated some local service policy; the server MAY
                                     * choose to specify the policy in the &lt;text/&gt;  element or an
                                     * application-specific condition element. */
    ERROR_REMOTE_CONNECTION_FAILED, /**< The server is unable to properly connect to a remote entity that is
                                     * required for authentication or authorization. */
    ERROR_RESOURCE_CONSTRAINT,      /**< the server lacks the system resources necessary to service the
                                     * stream. */
    ERROR_RESTRICTED_XML,           /**< The entity has attempted to send restricted XML features such as a
                                     * comment, processing instruction, DTD, entity reference, or unescaped
                                     * character (see Restrictions (Section 11.1)). */
    ERROR_SEE_OTHER_HOST,           /**< The server will not provide service to the initiating entity but is
                                     * redirecting traffic to another host; the server SHOULD specify the
                                     * alternate hostname or IP address (which MUST be a valid domain
                                     * identifier) as the XML character data of the &lt;see-other-host/&gt;
                                     * element. */
    ERROR_SYSTEM_SHUTDOWN,          /**< The server is being shut down and all active streams are being
                                     * closed. */
    ERROR_UNDEFINED_CONDITION,      /**< The error condition is not one of those defined by the other
                                     * conditions in this list; this error condition SHOULD be used only in
                                     * conjunction with an application-specific condition. */
    ERROR_UNSUPPORTED_ENCODING,     /**< The initiating entity has encoded the stream in an encoding that is
                                     * not supported by the server (see Character Encoding (Section 11.5)). */
    ERROR_UNSUPPORTED_STANZA_TYPE,  /**< The initiating entity has sent a first-level child of the stream
                                     * that is not supported by the server. */
    ERROR_UNSUPPORTED_VERSION,      /**< The value of the 'version' attribute provided by the initiating
                                     * entity in the stream header specifies a version of XMPP that is not
                                     * supported by the server; the server MAY specify the version(s) it
                                     * supports in the &lt;text/&gt; element. */
    ERROR_XML_NOT_WELL_FORMED,      /**< The initiating entity has sent XML that is not well-formed as
                                     * defined by [XML]. */
  };

  /**
   * Describes the possible stanza types.
   */
  enum StanzaType
  {
    STANZA_UNDEFINED,               /**< Undefined. */
    STANZA_IQ,                      /**< An Info/Query stanza. */
    STANZA_MESSAGE,                 /**< A message stanza. */
    STANZA_S10N,                    /**< A presence/subscription stanza. */
    STANZA_PRESENCE,                /**< A presence stanza. */
  };

  /**
   * Describes the possible stanza-sub-types.
   */
  enum StanzaSubType
  {
    STANZA_SUB_UNDEFINED,           /**< Undefined. */
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

  /**
   * Describes the verification results of a certificate.
   */
  enum CertStatus
  {
    CERT_OK              =  0,      /**< The certificate is valid and trusted. */
    CERT_INVALID         =  1,      /**< The certificate is not trusted. */
    CERT_SIGNER_UNKNOWN  =  2,      /**< The certificate hasn't got a known issuer. */
    CERT_REVOKED         =  4,      /**< The certificate has been revoked. */
    CERT_EXPIRED         =  8,      /**< The certificate has expired. */
    CERT_NOT_ACTIVE      = 16,      /**< The certifiacte is not yet active. */
    CERT_WRONG_PEER      = 32,      /**< The certificate has not been issued for the
                                     * peer we're connected to. */
    CERT_SIGNER_NOT_CA   = 64,      /**< The signer is not a CA. */
  };

  /**
   * Describes the certificate presented by the peer.
   */
  struct CertInfo
  {
    int status;                     /**< Bitwise or'ed CertStatus. */
    bool chain;                     /**< determines whether the cert chain verified ok. */
    std::string issuer;             /**< The name of the issuing entity.*/
    std::string server;             /**< The server the certificate has been issued for. */
    int date_from;                  /**< The date from which onwards the certificate is valid. */
    int date_to;                    /**< The date up to which the certificate is valid. */
    std::string protocol;           /**< The encryption protocol used for the connection. */
    std::string cipher;             /**< The cipher used for the connection. */
    std::string mac;                /**< The MAC used for the connection. */
    std::string compression;        /**< The compression used for the connection. */
  };

  /**
   * A list of strings.
   */
  typedef std::list<std::string> StringList;

  /**
   * A map of strings.
   */
  typedef std::map<std::string, std::string> StringMap;

};

#endif // GLOOX_H__

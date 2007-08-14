/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "gloox.h"
#include "connectiontls.h"
#include "tlsdefault.h"

namespace gloox
{

ConnectionTLS::ConnectionTLS(ConnectionBase* conn, ConnectionDataHandler* cdh, const LogSink& log) : ConnectionBase ( this ), m_connection ( conn ), m_handler( cdh ), m_log ( log ), m_handshaked ( false )
{
	if(m_handler)
		conn->registerConnectionDataHandler(m_handler);
	m_tls = new TLSDefault(this, m_connection->server());
}

ConnectionTLS::~ConnectionTLS()
{
	if(m_tls)
		delete m_tls;
}

/**************** ConnectionBase methods ****************/

ConnectionError ConnectionTLS::connect()
{
	m_state = StateConnecting;
	ConnectionError e = m_connection->connect();
	if(e == ConnNoError)
	{
		m_state = StateConnecting;
		m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Beginning TLS handshake....");
		m_tls->handshake(); // Initiate handshake
	}
	return e;
}

ConnectionError ConnectionTLS::recv( int timeout )
{
	return m_connection->recv(timeout);
}

bool ConnectionTLS::send( const std::string& data )
{
	m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Encrypting data...");
	m_tls->encrypt(data);
}

ConnectionError ConnectionTLS::receive()
{
	ConnectionError e;
	while((e = recv(500)) == ConnNoError);
	return e;
}

void ConnectionTLS::disconnect()
{
	m_connection->disconnect();
}

void ConnectionTLS::cleanup()
{
	m_connection->cleanup();
	delete m_connection;
}

void ConnectionTLS::getStatistics(int& totalIn, int& totalOut)
{
	getStatistics(totalIn, totalOut);
}

/************ ConnectionDataHandler methods *************/

void ConnectionTLS::handleReceivedData( const ConnectionBase* connection, const std::string& data )
{
	m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Decrypting received data...");
	m_tls->decrypt(data);
}

void ConnectionTLS::handleConnect( const ConnectionBase* connection )
{
	if(m_handler)
		m_handler->handleConnect(connection);
}

void ConnectionTLS::handleDisconnect( const ConnectionBase* connection, ConnectionError reason )
{
	if(m_handler)
		m_handler->handleDisconnect(connection, reason);
}

ConnectionBase* ConnectionTLS::newInstance() const
{
	return new ConnectionTLS(m_connection->newInstance(), m_handler, m_log);
}

/***************** TLSHandler methods *******************/

void ConnectionTLS::handleEncryptedData(const TLSBase* tls, const std::string& data)
{
	m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Sending encrypted data...");
	m_connection->send(data);
}

void ConnectionTLS::handleDecryptedData(const TLSBase* tls, const std::string& data)
{
	if(m_handler)
	{
		m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Handling decrypted data...");
		m_handler->handleReceivedData(m_connection, data);
	}
	else
	{
		m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Data received and decrypted but no handler");
	}
}

void ConnectionTLS::handleHandshakeResult (const TLSBase *base, bool success, CertInfo &certinfo)
{
	if(m_handshaked = success)
	{
		m_state = StateConnected;
		m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "TLS handshake succeeded");
	}
	else
	{
		m_state = StateDisconnected;
		m_log.log(LogLevelWarning, LogAreaClassConnectionTLS, "TLS handshake failed");
	}
}

} // namespace gloox

/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef CONNECTIONTLS_H__
#define CONNECTIONTLS_H__

#include <string>
#include "gloox.h"
#include "logsink.h"
#include "connectionbase.h"
#include "tlsdefault.h"
#include "connectiondatahandler.h"

namespace gloox
{

class GLOOX_API ConnectionTLS : public TLSHandler, ConnectionBase, ConnectionDataHandler
{
public:
	ConnectionTLS(ConnectionBase* conn, ConnectionDataHandler* cdh, const LogSink& log);
	~ConnectionTLS();
	
	// reimplemented from ConnectionBase
	virtual ConnectionError connect();
	
	// reimplemented from ConnectionBase
	virtual ConnectionError recv( int timeout = -1 );
	
	// reimplemented from ConnectionBase
	virtual bool send( const std::string& data );
	
	// reimplemented from ConnectionBase
	virtual ConnectionError receive();
	
	// reimplemented from ConnectionBase
	virtual void disconnect();
	
	// reimplemented from ConnectionBase
	virtual void cleanup();
	
	// reimplemented from ConnectionBase
	virtual void getStatistics( int &totalIn, int &totalOut );
	
	// reimplemented from ConnectionDataHandler
	virtual void handleReceivedData( const ConnectionBase* connection, const std::string& data );
	
	// reimplemented from ConnectionDataHandler
	virtual void handleConnect( const ConnectionBase* connection );
	
	// reimplemented from ConnectionDataHandler
	virtual void handleDisconnect( const ConnectionBase* connection, ConnectionError reason );
	
	// reimplemented from ConnectionDataHandler
	virtual ConnectionBase* newInstance() const;
		
	// TLSHandler callbacks
	virtual void handleEncryptedData(const TLSBase*, const std::string& data);
	virtual void handleDecryptedData(const TLSBase*, const std::string& data);
	virtual void handleHandshakeResult (const TLSBase *base, bool success, CertInfo &certinfo);
	
private:
	// Properties
	ConnectionBase* m_connection;
	ConnectionDataHandler* m_handler;
	TLSDefault* m_tls;
	const LogSink& m_log;
	bool m_handshaked;
	
	ConnectionError m_lastError;
};

}

#endif

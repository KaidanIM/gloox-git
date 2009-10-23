#include "../../gloox.h"
#include "../../tlshandler.h"
#include "../../tlsschannelclient.h"
#include "../../tlsschannelserver.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>
#include <cstdio> // [s]print[f]

#include "../../config.h"

#ifdef HAVE_WINTLS

class SChannelTest : TLSHandler
{
  public:
    SChannelTest();
    ~SChannelTest();
    virtual void handleEncryptedData( const TLSBase* base, const std::string& data );
    virtual void handleDecryptedData( const TLSBase* base, const std::string& data );
    virtual void handleHandshakeResult( const TLSBase* base, bool success, CertInfo &certinfo );

    bool handshake();
    std::string send( const std::string& txt );
  private:
    void printfCert( CertInfo &certinfo );
    void loop();
    SChannelClient *m_client;
    SChannelServer *m_server;
    std::string m_clientToServer;
    std::string m_serverToClient;
    std::string m_clientDecrypted;
    std::string m_serverDecrypted;
    bool m_clientHandshake;
    bool m_clientHandshakeResult;
    bool m_serverHandshake;
    bool m_serverHandshakeResult;
};

SChannelTest::SChannelTest()
 : m_clientHandshake( false ), m_clientHandshakeResult( false ),
   m_serverHandshake( false ), m_serverHandshakeResult( false )
{
  m_client = new SChannelClient( this, "foo" );
  m_client->init();
  m_server = new SChannelServer( this );
  m_server->init();
}

SChannelTest::~SChannelTest()
{
  delete m_client;
  delete m_server;
}

bool SChannelTest::handshake()
{
  m_client->handshake();
  while( !m_clientHandshakeResult && !m_serverHandshakeResult )
    loop();
  return m_clientHandshake && m_serverHandshake;
}

void SChannelTest::loop()
{
  while( m_clientToServer.length() )
  {
//     printf( "we have %d bytes for the server\n", m_clientToServer.length() );
    m_server->decrypt( m_clientToServer );
    m_clientToServer = "";
//     printf( "we have %d bytes left for the server\n", m_clientToServer.length() );
  }
  while( m_serverToClient.length() )
  {
//     printf( "we have %d bytes for the client\n", m_serverToClient.length() );
    m_client->decrypt( m_serverToClient );
    m_serverToClient = "";
//     printf( "we have %d bytes left for the client\n", m_serverToClient.length() );
  }
  while( m_serverDecrypted.length() )
  {
//     printf( "we have %d bytes for the server to encrypt\n", m_serverDecrypted.length() );
    m_server->encrypt( m_serverDecrypted );
    m_serverDecrypted = "";
//     printf( "we have %d bytes left for the server to encrypt\n", m_serverDecrypted.length() );
  }
}

void SChannelTest::handleEncryptedData( const TLSBase* base, const std::string& data )
{
  const SChannelClient *c = dynamic_cast<const SChannelClient*>( base );
  if( c )
  {
//     printf( "recv encrypted data from client: %d\n", data.length() );
    m_clientToServer += data;
    return;
  }

  const SChannelServer *s = dynamic_cast<const SChannelServer*>( base );
  if( s )
  {
//     printf( "recv encrypted data from server: %d\n", data.length() );
    m_serverToClient += data;
  }
}

void SChannelTest::handleDecryptedData( const TLSBase* base, const std::string& data )
{
  const SChannelClient *c = dynamic_cast<const SChannelClient*>( base );
  if( c )
  {
//     printf( "recv decrypted data from client: %d\n", data.length() );
    m_clientDecrypted += data;
    return;
  }

  const SChannelServer *s = dynamic_cast<const SChannelServer*>( base );
  if( s )
  {
//     printf( "recv decrypted data from server: %d\n", data.length() );
    m_serverDecrypted += data;
  }
}

void SChannelTest::handleHandshakeResult( const TLSBase* base, bool success, CertInfo& /*certinfo*/ )
{
//   printfCert( certinfo );
  const SChannelClient *c = dynamic_cast<const SChannelClient*>( base );
  if( c )
  {
//     printf( "recv handshake result from client: %d\n", success );
    m_clientHandshakeResult = true;
    m_clientHandshake = success;
    return;
  }

  const SChannelServer *s = dynamic_cast<const SChannelServer*>( base );
  if( s )
  {
//     printf( "recv handshake result from server: %d\n", success );
    m_serverHandshakeResult = true;
    m_serverHandshake = success;
  }
}

void SChannelTest::printfCert( CertInfo &certinfo )
{
  printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
          certinfo.status, certinfo.issuer.c_str(), certinfo.server.c_str(),
          certinfo.protocol.c_str(), certinfo.mac.c_str(), certinfo.cipher.c_str(),
          certinfo.compression.c_str() );
}

std::string SChannelTest::send( const std::string& txt )
{
//   printf( "sending %s\n", txt.c_str() );

  m_client->encrypt( txt );
  while( m_clientDecrypted.empty() )
    loop();

//   printf( "recv'ed %s\n", m_clientDecrypted.c_str() );
  const std::string t = m_clientDecrypted;
  m_clientDecrypted = "";
  return t;
}

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;
  std::string name;

  // -------
  name = "anon client/server handshake test";
  SChannelTest *t = new SChannelTest();
  if( !t->handshake() )
  {
    ++fail;
    printf( "test '%s' failed\n", name.c_str() );
  }

  if( !fail )
  {

    // -------
    name = "simple send";
    std::string text( "text" );
    if( t->send( text ) != text )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }

    // -------
    name = "subseququent send";
    text = std::string( "txt"/*17000, 'x'*/ );
    if( t->send( text ) != text )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }

    // -------
    name = "large send";
    text = std::string( 17000, 'x' );
    if( t->send( text ) != text )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }

    // -------
    name = "larger send";
    text = std::string( 170000, 'x' );
    if( t->send( text ) != text )
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }

  }



  delete t;













  if( fail == 0 )
  {
    printf( "TLSSChannel: OK\n" );
    return 0;
  }
  else
  {
    printf( "TLSSChannel: %d test(s) failed\n", fail );
    return 1;
  }
}
#else
int main( int /*argc*/, char** /*argv*/ )
{
  printf( "SChannel not enabled. Skipped tests.\n" );
}
#endif // HAVE_WINTLS


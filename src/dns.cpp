/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifdef WIN32
#include "../config.h.win"
#else
#include "config.h"
#endif

#include "dns.h"
#include "logsink.h"

#include <sys/types.h>

#ifndef WIN32
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif

#include <sstream>

#define SRV_COST    (RRFIXEDSZ+0)
#define SRV_WEIGHT  (RRFIXEDSZ+2)
#define SRV_PORT    (RRFIXEDSZ+4)
#define SRV_SERVER  (RRFIXEDSZ+6)
#define SRV_FIXEDSZ (RRFIXEDSZ+6)

#ifndef T_SRV
#define T_SRV 33
#endif

#ifndef C_IN
#define C_IN 1
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#define XMPP_PORT 5222

namespace gloox
{

#if defined( SKYOS ) || defined( WIN32 ) || ( defined( __NetBSD__ ) && ( __NetBSD_Version__ < 300000000 ) ) || defined( __UCLIBC__ )
  int DNS::connect( const std::string& domain )
  {
    return DNS::connect( domain, XMPP_PORT );
  }
#else
  DNS::HostMap DNS::resolve( const std::string& domain )
  {
    std::string service = "xmpp-client";
    std::string proto = "tcp";

    return resolve( service, proto, domain );
  }

  DNS::HostMap DNS::resolve( const std::string& service, const std::string& proto,
                             const std::string& domain )
  {
    buffer srvbuf;
    bool error = false;

    const std::string dname = "_" +  service + "._" + proto;

    if( !domain.empty() )
      srvbuf.len = res_querydomain( dname.c_str(), const_cast<char*>( domain.c_str() ),
                                    C_IN, T_SRV, srvbuf.buf, NS_PACKETSZ );
    else
      srvbuf.len = res_query( dname.c_str(), C_IN, T_SRV, srvbuf.buf, NS_PACKETSZ );

    if( srvbuf.len < 0 )
      error = true;

    HEADER* hdr = (HEADER*)srvbuf.buf;
    unsigned char* here = srvbuf.buf + NS_HFIXEDSZ;

    if( ( hdr->tc ) || ( srvbuf.len < NS_HFIXEDSZ ) )
      error = true;

    if( hdr->rcode >= 1 && hdr->rcode <= 5 )
      error = true;

    if( ntohs( hdr->ancount ) == 0 )
      error = true;

    if( ntohs( hdr->ancount ) > NS_PACKETSZ )
      error = true;

    int cnt;
    for( cnt = ntohs( hdr->qdcount ); cnt>0; cnt-- )
    {
      int strlen = dn_skipname( here, srvbuf.buf + srvbuf.len );
      here += strlen + NS_QFIXEDSZ;
    }

    unsigned char *srv[NS_PACKETSZ];
    int srvnum = 0;
    for( cnt = ntohs( hdr->ancount ); cnt>0; cnt-- )
    {
      int strlen = dn_skipname( here, srvbuf.buf + srvbuf.len );
      here += strlen;
      srv[srvnum++] = here;
      here += SRV_FIXEDSZ;
      here += dn_skipname( here, srvbuf.buf + srvbuf.len );
    }

    if( error )
    {
      HostMap server;
      struct servent *servent;

      if( ( servent = getservbyname( service.c_str(), proto.c_str() ) ) == 0 )
      {
        server[domain] = 0;
        return server;
      }

      if( !domain.empty() )
        server[domain] = ntohs( servent->s_port );

      return server;
    }

    // (q)sort here

    HostMap servers;
    for( cnt=0; cnt<srvnum; ++cnt )
    {
      name srvname;

      if( ns_name_ntop( srv[cnt] + SRV_SERVER, (char*)srvname, NS_MAXDNAME ) < 0 )
        printf( "handle this error!\n" );

      servers[(char*)srvname] = ns_get16( srv[cnt] + SRV_PORT );
    }

    return servers;
  }

  int DNS::connect( const std::string& domain )
  {
    HostMap hosts = resolve( domain );
    if( hosts.size() == 0 )
      return -DNS_NO_HOSTS_FOUND;

    struct protoent* prot;
    if( ( prot = getprotobyname( "tcp" ) ) == 0)
      return -DNS_COULD_NOT_RESOLVE;

    int fd;
    if( ( fd = socket( PF_INET, SOCK_STREAM, prot->p_proto ) ) == -1 )
      return -DNS_COULD_NOT_RESOLVE;

    struct hostent *h;
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    int ret = 0;
    HostMap::const_iterator it = hosts.begin();
    for( ; it != hosts.end(); ++it )
    {
      int port;
      if( (*it).second == 0 )
        port = XMPP_PORT;
      else
        port = (*it).second;

      target.sin_port = htons( port );
      if( ( h = gethostbyname( (*it).first.c_str() ) ) == 0 )
      {
        ret = -DNS_COULD_NOT_RESOLVE;
        continue;
      }

      char *tmp = inet_ntoa( *((struct in_addr *)h->h_addr) );
      std::ostringstream oss;
      oss << "resolved " << (*it).first.c_str() <<  " to: " << tmp << ":" << port;
      LogSink::instance().log( LOG_DEBUG, LOG_CLASS_DNS, oss.str() );

      if( inet_aton( inet_ntoa(*((struct in_addr *)h->h_addr)), &(target.sin_addr) ) == 0 )
        continue;

      memset( &(target.sin_zero), '\0', 8 );
      if( ::connect( fd, (struct sockaddr *)&target, sizeof( struct sockaddr ) ) == 0 )
        return fd;

      close( fd );
    }
    if( ret )
      return ret;

    return -DNS_COULD_NOT_CONNECT;
  }
#endif

  int DNS::connect( const std::string& domain, int port )
  {
#ifdef WIN32
    WSADATA wsaData;
    if( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0 )
      return -DNS_COULD_NOT_RESOLVE;
#endif

    struct protoent* prot;
    if( ( prot = getprotobyname( "tcp" ) ) == 0 )
    {
      cleanup();
      return -DNS_COULD_NOT_RESOLVE;
    }

    int fd;
    if( ( fd = socket( PF_INET, SOCK_STREAM, prot->p_proto ) ) == -1 )
    {
      cleanup();
      return -DNS_COULD_NOT_CONNECT;
    }

    struct hostent *h;
    if( ( h = gethostbyname( domain.c_str() ) ) == 0 )
    {
      cleanup();
      return -DNS_COULD_NOT_RESOLVE;
    }

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons( port );

    if( h->h_length != sizeof( struct in_addr ) )
    {
      cleanup();
      return -DNS_COULD_NOT_RESOLVE;
    }
    else
    {
      memcpy( &target.sin_addr, h->h_addr, sizeof( struct in_addr ) );
    }

    std::ostringstream oss;
    oss << "resolved " << domain.c_str() << " to: " << (char*)&target.sin_addr;
    LogSink::instance().log( LOG_DEBUG, LOG_CLASS_DNS, oss.str() );

    memset( &(target.sin_zero), '\0', 8 );
    if( ::connect( fd, (struct sockaddr *)&target, sizeof( struct sockaddr ) ) == 0 )
      return fd;

#ifndef WIN32
    close( fd );
#else
    closesocket( fd );
    cleanup();
#endif
    return -DNS_COULD_NOT_CONNECT;
  }

  void DNS::cleanup()
  {
#ifdef WIN32
    WSACleanup();
#endif
  }
}

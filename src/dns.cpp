/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



//#include "config.h"

#include "dns.h"

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

#define SRV_COST    (RRFIXEDSZ+0)
#define SRV_WEIGHT  (RRFIXEDSZ+2)
#define SRV_PORT    (RRFIXEDSZ+4)
#define SRV_SERVER  (RRFIXEDSZ+6)
#define SRV_FIXEDSZ (RRFIXEDSZ+6)

#define XMPP_PORT 5222

namespace gloox
{

#ifndef WIN32
  DNS::HostMap DNS::resolve( const std::string& domain )
  {
    std::string service = "xmpp-client";
    std::string proto = "tcp";

    return resolve( service, proto, domain );
  }

  DNS::HostMap DNS::resolve( const std::string& service, const std::string& proto,
                               const std::string& domain )
  {
    buf srvbuf;
    bool error = false;

    std::string dname = "_" +  service + "._" + proto;

    if( !domain.empty() )
      srvbuf.len = res_querydomain( dname.c_str(), (char*)domain.c_str(), C_IN, T_SRV, srvbuf.buf, PACKETSZ );
    else
      srvbuf.len = res_query( dname.c_str(), C_IN, T_SRV, srvbuf.buf, PACKETSZ );

    if( srvbuf.len < 0 )
      error = true;

    HEADER* hdr = (HEADER*)srvbuf.buf;
    unsigned char* here = srvbuf.buf + HFIXEDSZ;

    if( ( hdr->tc ) || ( srvbuf.len < HFIXEDSZ ) )
      error = true;

    if( hdr->rcode >= 1 && hdr->rcode <= 5 )
      error = true;

    if( ntohs( hdr->ancount ) == 0 )
      error = true;

    if( ntohs( hdr->ancount ) > PACKETSZ )
      error = true;

    int cnt;
    for( cnt = ntohs( hdr->qdcount ); cnt>0; cnt-- )
    {
      int strlen = dn_skipname( here, srvbuf.buf + srvbuf.len );
      here += strlen + QFIXEDSZ;
    }

    unsigned char *srv[PACKETSZ];
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
    for( cnt=0; cnt<srvnum; cnt++ )
    {
      name srvname;

      if( ns_name_ntop( srv[cnt] + SRV_SERVER, (char*)srvname, MAXDNAME ) < 0 )
        printf( "handle this error!\n" );

      servers[(char*)srvname] = ns_get16( srv[cnt] + SRV_PORT );
    }

    return servers;
  }

  int DNS::connect( const std::string& domain )
  {
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    struct hostent *h;

    HostMap hosts = resolve( domain );
    if( hosts.size() == 0 )
      return -DNS_NO_HOSTS_FOUND;

    struct protoent* prot = getprotobyname( "tcp" );
    int fd = socket( PF_INET, SOCK_STREAM, prot->p_proto );

    int ret = 0;

    HostMap::const_iterator it = hosts.begin();
    for( it; it != hosts.end(); it++ )
    {
      int port;
      if( (*it).second == 0 )
        port = XMPP_PORT;
      else
        port = (*it).second;

      dest_addr.sin_port = htons( port );
      if( ( h = gethostbyname( (*it).first.c_str() ) ) == 0 )
      {
        ret = -DNS_COULD_NOT_RESOLVE;
        continue;
      }

#ifdef DEBUG
      char *tmp = inet_ntoa( *((struct in_addr *)h->h_addr) );
      printf( "resolved %s to: %s:%d\n", (*it).first.c_str(), tmp, port );
#endif

      if( inet_aton( inet_ntoa(*((struct in_addr *)h->h_addr)), &(dest_addr.sin_addr) ) == 0 )
        continue;

      memset( &(dest_addr.sin_zero), '\0', 8 );
      if( ::connect( fd, (struct sockaddr *)&dest_addr, sizeof( struct sockaddr ) ) == 0 )
        return fd;

      close( fd );
    }
    if( ret )
      return ret;

    return -DNS_COULD_NOT_CONNECT;
  }
#else
  int DNS::connect( const std::string& domain )
  {
    DNS::connect( domain, XMPP_PORT );
  }
#endif

  int DNS::connect( const std::string& domain, int port )
  {
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    struct hostent *h;

    struct protoent* prot = getprotobyname( "tcp" );
    int fd = socket( PF_INET, SOCK_STREAM, prot->p_proto );

    dest_addr.sin_port = htons( port );
    if( ( h = gethostbyname( domain.c_str() ) ) == 0 )
      return -DNS_COULD_NOT_RESOLVE;

#ifdef DEBUG
    printf( "resolved %s to: %s\n", domain.c_str(), inet_ntoa( *((struct in_addr *)h->h_addr) ) );
#endif

#ifdef WIN32
		dest_addr.sin_addr.s_addr = inet_addr( inet_ntoa(*((struct in_addr *)h->h_addr)) );
#else
    if( inet_aton( inet_ntoa(*((struct in_addr *)h->h_addr)), &(dest_addr.sin_addr) ) == 0 )
      return -DNS_COULD_NOT_RESOLVE;
#endif

    memset( &(dest_addr.sin_zero), '\0', 8 );
    if( ::connect( fd, (struct sockaddr *)&dest_addr, sizeof( struct sockaddr ) ) == 0 )
      return fd;

#ifndef WIN32
    close( fd );
#endif
    return -DNS_COULD_NOT_CONNECT;
  }

};

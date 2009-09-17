/*
  Copyright (c) 2005-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifdef _WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#include "gloox.h"
#include "dns.h"

#ifndef _WIN32_WCE
# include <sys/types.h>
# include <sstream>
#endif

#include <stdio.h>

#if !defined( _WIN32 ) && !defined( _WIN32_WCE )
# include <netinet/in.h>
# include <arpa/nameser.h>
# include <resolv.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <unistd.h>
#endif

#ifdef _WIN32
# include <winsock.h>
#elif defined( _WIN32_WCE )
# include <winsock2.h>
#endif

#ifdef HAVE_WINDNS_H
# include <windns.h>
#endif

#define SRV_COST    (RRFIXEDSZ+0)
#define SRV_WEIGHT  (RRFIXEDSZ+2)
#define SRV_PORT    (RRFIXEDSZ+4)
#define SRV_SERVER  (RRFIXEDSZ+6)
#define SRV_FIXEDSZ (RRFIXEDSZ+6)

#ifndef T_SRV
# define T_SRV 33
#endif

// mingw
#ifndef DNS_TYPE_SRV
# define DNS_TYPE_SRV 33
#endif

#ifndef NS_CMPRSFLGS
# define NS_CMPRSFLGS 0xc0
#endif

#ifndef C_IN
# define C_IN 1
#endif

#ifndef INVALID_SOCKET
# define INVALID_SOCKET -1
#endif

#define XMPP_PORT 5222

namespace gloox
{

  /*
   * The following copyright notices apply ONLY to the functions
   * printable(), special(), and x__ns_name_ntop() below.
   * They are included here as temporary copies.
   */

  /*
   * Copyright (c) 1985, 1993
   * The Regents of the University of California. All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions
   * are met:
   * 1. Redistributions of source code must retain the above copyright
   * notice, this list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright
   * notice, this list of conditions and the following disclaimer in the
   * documentation and/or other materials provided with the distribution.
   * 4. Neither the name of the University nor the names of its contributors
   * may be used to endorse or promote products derived from this software
   * without specific prior written permission.
   *
   * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
   * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   * SUCH DAMAGE.
   */

  /*
   * Portions Copyright (c) 1993 by Digital Equipment Corporation.
   *
   * Permission to use, copy, modify, and distribute this software for any
   * purpose with or without fee is hereby granted, provided that the above
   * copyright notice and this permission notice appear in all copies, and that
   * the name of Digital Equipment Corporation not be used in advertising or
   * publicity pertaining to distribution of the document or software without
   * specific, written prior permission.
   *
   * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
   * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
   * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL DIGITAL EQUIPMENT
   * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
   * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
   * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
   * SOFTWARE.
   */

  /*
   * Portions Copyright (c) 1996-1999 by Internet Software Consortium.
   *
   * Permission to use, copy, modify, and distribute this software for any
   * purpose with or without fee is hereby granted, provided that the above
   * copyright notice and this permission notice appear in all copies.
   *
   * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
   * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
      WARRANTIES
   * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
   * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
   * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
   * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
   * SOFTWARE.
   */

  /*
   * Copyright (c) 1996,1999 by Internet Software Consortium.
   *
   * Permission to use, copy, modify, and distribute this software for any
   * purpose with or without fee is hereby granted, provided that the above
   * copyright notice and this permission notice appear in all copies.
   *
   * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
   * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
      WARRANTIES
   * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
   * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
   * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
   * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
   * SOFTWARE.
   */
  static const char digits[] = "0123456789";

  /*
   * printable(ch)
   * Thinking in noninternationalized USASCII (per the DNS spec),
   * is this character visible and not a space when printed ?
   * return:
   * boolean.
   */
  static int printable( int ch )
  {
    return ( ch > 0x20 && ch < 0x7f );
  }

  /*
   * special(ch)
   * Thinking in noninternationalized USASCII (per the DNS spec),
   * is this characted special ("in need of quoting") ?
   * return:
   * boolean.
   */
  static int special( int ch )
  {
    switch( ch )
    {
      case 0x22: /* '"' */
      case 0x2E: /* '.' */
      case 0x3B: /* ';' */
      case 0x5C: /* '\\' */
            /* Special modifiers in zone files. */
      case 0x40: /* '@' */
      case 0x24: /* '$' */
        return 1;
      default:
        return 0;
    }
  }

  /*
   * ns_name_ntop(src, dst, dstsiz)
   * Convert an encoded domain name to printable ascii as per RFC1035.
   * return:
   * Number of bytes written to buffer, or -1 (with errno set)
   * notes:
   * The root is returned as "."
   * All other domains are returned in non absolute form
   */
  int x__ns_name_ntop( const u_char *src, char *dst, size_t dstsiz )
  {
    const u_char *cp;
    char *dn, *eom;
    u_char c;
    u_int n;

    cp = src;
    dn = dst;
    eom = dst + dstsiz;

    while( (n = *cp++) != 0 )
    {
      if( (n & NS_CMPRSFLGS) != 0 )
      {
        /* Some kind of compression pointer. */
        return -1;
      }
      if( dn != dst )
      {
        if( dn >= eom )
        {
          return -1;
        }
        *dn++ = '.';
      }
      if( dn + n >= eom )
      {
        return -1;
      }
      for( ; n > 0; n-- )
      {
        c = *cp++;
        if( special( c ) )
        {
          if( dn + 1 >= eom )
          {
            return -1;
          }
          *dn++ = '\\';
          *dn++ = (char)c;
        }
        else if( !printable( c ) )
        {
          if( dn + 3 >= eom )
          {
            return -1;
          }
          *dn++ = '\\';
          *dn++ = digits[c / 100];
          *dn++ = digits[( c % 100) / 10];
          *dn++ = digits[c % 10];
        }
        else
        {
          if( dn >= eom )
          {
            return -1;
          }
          *dn++ = (char)c;
        }
      }
    }
    if(dn == dst )
    {
      if( dn >= eom )
      {
        return -1;
      }
      *dn++ = '.';
    }
    if( dn >= eom )
    {
      return -1;
    }
    *dn++ = '\0';
    return (dn - dst);
  }

#if defined( HAVE_RES_QUERYDOMAIN ) && defined( HAVE_DN_SKIPNAME ) && defined( HAVE_RES_QUERY )
  DNS::HostMap DNS::resolve( const std::string& service, const std::string& proto,
                             const std::string& domain, const LogSink& logInstance )
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
      return defaultHostMap( domain, logInstance );

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
    for( cnt = ntohs( hdr->qdcount ); cnt>0; --cnt )
    {
      int strlen = dn_skipname( here, srvbuf.buf + srvbuf.len );
      here += strlen + NS_QFIXEDSZ;
    }

    unsigned char* srv[NS_PACKETSZ];
    int srvnum = 0;
    for( cnt = ntohs( hdr->ancount ); cnt>0; --cnt )
    {
      int strlen = dn_skipname( here, srvbuf.buf + srvbuf.len );
      here += strlen;
      srv[srvnum++] = here;
      here += SRV_FIXEDSZ;
      here += dn_skipname( here, srvbuf.buf + srvbuf.len );
    }

    if( error )
    {
      return defaultHostMap( domain, logInstance );
    }

    // (q)sort here

    HostMap servers;
    for( cnt=0; cnt<srvnum; ++cnt )
    {
      name srvname;

      if( x__ns_name_ntop( srv[cnt] + SRV_SERVER, (char*)srvname, NS_MAXDNAME ) < 0 )
      {
        //FIXME do we need to handle this? How? Can it actually happen at all?
//         printf( "handle this error!\n" );
      }

      unsigned char* c = srv[cnt] + SRV_PORT;

      servers.insert( std::make_pair( (char*)srvname, ntohs( c[1] << 8 | c[0] ) ) );
    }

    return servers;
  }

#elif defined( _WIN32 ) && defined( HAVE_WINDNS_H )
  DNS::HostMap DNS::resolve( const std::string& service, const std::string& proto,
                             const std::string& domain, const LogSink& logInstance )
  {
    const std::string dname = "_" +  service + "._" + proto + "." + domain;
    bool error = false;

    DNS::HostMap servers;
    DNS_RECORD* pRecord = NULL;
    DNS_STATUS status = DnsQuery_UTF8( dname.c_str(), DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &pRecord, NULL );
    if( status == ERROR_SUCCESS )
    {
      // NOTE: DnsQuery_UTF8 and DnsQuery_A really should have been defined with
      // PDNS_RECORDA instead of PDNS_RECORD, since that's what it is (even with _UNICODE defined).
      // We'll correct for that mistake with a cast.
//       DNS_RECORD* pRec = (DNS_RECORD*)pRecord;
      do
      {
        if( pRecord->wType == DNS_TYPE_SRV )
        {
          servers[pRecord->Data.SRV.pNameTarget] = pRecord->Data.SRV.wPort;
        }
        pRecord = pRecord->pNext;
      }
      while( pRecord != NULL );
      DnsRecordListFree( pRecord, DnsFreeRecordList );
    }
    else
      error = true;

    if( error || !servers.size() )
    {
      servers = defaultHostMap( domain, logInstance );
    }

    return servers;
  }

#else
  DNS::HostMap DNS::resolve( const std::string& service, const std::string& proto,
                             const std::string& domain, const LogSink& logInstance )
  {
    logInstance.warn( LogAreaClassDns, "notice: gloox does not support SRV "
                        "records on this platform. Using A records instead." );
    return defaultHostMap( domain, logInstance );
  }
#endif

  DNS::HostMap DNS::defaultHostMap( const std::string& domain, const LogSink& logInstance )
  {
    HostMap server;

    logInstance.warn( LogAreaClassDns, "notice: no SRV record found for "
                                          + domain + ", using default port." );

    if( !domain.empty() )
      server[domain] = XMPP_PORT;

    return server;
  }

#ifdef HAVE_GETADDRINFO
  void DNS::resolve( struct addrinfo** res, const std::string& service, const std::string& proto,
                     const std::string& domain, const LogSink& logInstance )
  {
    logInstance.dbg( LogAreaClassDns, "Resolving: _" +  service + "._" + proto + "." + domain );
    struct addrinfo hints;
    if( proto == "tcp" )
      hints.ai_socktype = SOCK_STREAM;
    else if( proto == "udp" )
      hints.ai_socktype = SOCK_DGRAM;
    else
    {
      logInstance.err( LogAreaClassDns, "Unknown/Invalid protocol: " + proto );
    }
    memset( &hints, '\0', sizeof( hints ) );
    hints.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
    hints.ai_socktype = SOCK_STREAM;
    int e = getaddrinfo( domain.c_str(), service.c_str(), &hints, res );
    if( e )
      logInstance.err( LogAreaClassDns, "getaddrinfo() failed" );
  }

  int DNS::connect( const std::string& host, const LogSink& logInstance )
  {
    struct addrinfo* results = 0;

    resolve( &results, host, logInstance );
    if( !results )
    {
      logInstance.err( LogAreaClassDns, "host not found: " + host );
      return -ConnDnsError;
    }

    struct addrinfo* runp = results;
    while( runp )
    {
      int fd = DNS::connect( runp, logInstance );
      if( fd >= 0 )
        return fd;

      runp = runp->ai_next;
    }

    freeaddrinfo( results );

    return -ConnConnectionRefused;
  }

  int DNS::connect( struct addrinfo* res, const LogSink& logInstance )
  {
    if( !res )
      return -1;

    int fd = getSocket( res->ai_family, res->ai_socktype, res->ai_protocol );
    if( fd < 0 )
      return fd;

    if( ::connect( fd, res->ai_addr, res->ai_addrlen ) == 0 )
    {
      char ip[NI_MAXHOST];
      char port[NI_MAXSERV];

      if( getnameinfo( res->ai_addr, sizeof( sockaddr ),
                       ip, sizeof( ip ),
                       port, sizeof( port ),
                       NI_NUMERICHOST | NI_NUMERICSERV ) )
      {
        //FIXME do we need to handle this? How? Can it actually happen at all?
//         printf( "could not get numeric hostname");
      }

      std::ostringstream oss;
      oss << "Connecting to ";
      if( res->ai_canonname )
      {
        oss << res->ai_canonname;
        oss << " (" << ip << "), port " << port;
      }
      else
      {
        oss << ip << ":" << port;
      }
      logInstance.dbg( LogAreaClassDns, oss.str() );
      return fd;
    }

    closeSocket( fd );
    return -ConnConnectionRefused;
  }

#else

  int DNS::connect( const std::string& host, const LogSink& logInstance )
  {
    HostMap hosts = resolve( host, logInstance );
    if( hosts.size() == 0 )
      return -ConnDnsError;

    HostMap::const_iterator it = hosts.begin();
    for( ; it != hosts.end(); ++it )
    {
      int fd = DNS::connect( (*it).first, (*it).second, logInstance );
      if( fd >= 0 )
        return fd;
    }

    return -ConnConnectionRefused;
  }
#endif

  int DNS::getSocket()
  {
#ifdef _WIN32
    WSADATA wsaData;
    if( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0 )
      return -ConnDnsError;
#endif

    struct protoent* prot;
    if( ( prot = getprotobyname( "tcp" ) ) == 0 )
    {
      cleanup();
      return -ConnDnsError;
    }
    return getSocket( PF_INET, SOCK_STREAM, prot->p_proto );
  }

  int DNS::getSocket( int af, int socktype, int proto )
  {
#ifdef _WIN32
    SOCKET fd;
#else
    int fd;
#endif
    if( ( fd = socket( af, socktype, proto ) ) == -1 )
    {
      cleanup();
      return -ConnSocketError;
    }

#ifdef HAVE_SETSOCKOPT
    int timeout = 5000;
    setsockopt( fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof( timeout ) );
    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char*)&timeout, sizeof( timeout ) );
#endif

    return (int)fd;
  }

  int DNS::connect( const std::string& host, int port, const LogSink& logInstance )
  {
    int fd = getSocket();
    if( fd < 0 )
      return fd;

    struct hostent* h;
    if( ( h = gethostbyname( host.c_str() ) ) == 0 )
    {
      cleanup();
      return -ConnDnsError;
    }

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons( (u_short)port );

    if( h->h_length != sizeof( struct in_addr ) )
    {
      cleanup();
      return -ConnDnsError;
    }
    else
    {
      memcpy( &target.sin_addr, h->h_addr, sizeof( struct in_addr ) );
    }

#ifndef _WIN32_WCE
    std::ostringstream oss;
#endif

    memset( target.sin_zero, '\0', 8 );
    if( ::connect( fd, (struct sockaddr *)&target, sizeof( struct sockaddr ) ) == 0 )
    {
#ifndef _WIN32_WCE
      oss << "connecting to " << host.c_str()
          << " (" << inet_ntoa( target.sin_addr ) << ":" << port << ")";
      logInstance.dbg( LogAreaClassDns, oss.str() );
#endif
      return fd;
    }

#ifndef _WIN32_WCE
    oss << "connection to " << host.c_str()
         << " (" << inet_ntoa( target.sin_addr ) << ":" << port << ") failed";
    logInstance.dbg( LogAreaClassDns, oss.str() );
#endif

    closeSocket( fd );
    return -ConnConnectionRefused;
  }

  void DNS::closeSocket( int fd )
  {
#ifndef _WIN32
    close( fd );
#else
    closesocket( fd );
#endif
  }

  void DNS::cleanup()
  {
#ifdef _WIN32
    WSACleanup();
#endif
  }

}

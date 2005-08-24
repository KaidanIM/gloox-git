/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef DNS_H__
#define DNS_H__

#include <arpa/nameser.h>

#include <string>
#include <map>

namespace gloox
{

  /**
   * This class holds a number of static functions used for DNS related stuff.
   * You should not need to use these functions directly.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.3
   */
  class DNS
  {
    public:

      /**
       * Possible errors occuring while resolving and connecting.
       */
      enum DNSError
      {
        DNS_COULD_NOT_CONNECT = 1,      /**< None of the resolved hosts could be contacted. */
        DNS_NO_HOSTS_FOUND,             /**< It was not possible to resolve SRV record. */
        DNS_COULD_NOT_RESOLVE           /**< The given domain name could not be resolved. */
      };

      /**
       * A list of strings (used for server addresses) and ints (used for port numbers).
       */
      typedef std::map<std::string, int> HostMap;

      /**
       * This funtion resolves a service/protocol/domain tuple.
       * @param service The SRV service type.
       * @param proto The SRV protocol.
       * @param domain The domain to search for SRV records.
       * @return A list of weighted hostname/port pairs from SRV records, or A records if no SRV
       * records where found.
       */
      static HostMap resolve( const std::string& service, const std::string& proto,
                                const std::string& domain );

      /**
       * This is a convenience funtion which uses @ref resolve() to resolve SRV records
       * for a given domain, using a service of @b xmpp and a proto of @b tcp.
       * @param domain The domain to resolve SRV records for.
       * @return A list of weighted hostname/port pairs from SRV records, or A records if no SRV
       * records where found.
       */
      static HostMap resolve( const std::string& domain );

      /**
       * This is a convenience function which uses @ref resolve() to get a list of hosts
       * and connects to one of them.
       * @param domain The domain to resolve SRV records for.
       * @return A file descriptor for the established connection.
       */
      static int connect( const std::string& domain );

      /**
       * This is a convenience function which uses connects to the given host and port. No SRV
       * records are resolved. Use this function for special setups.
       * @param domain The domain to connect to.
       * @param port A custom port to connect to.
       * @return A file descriptor for the established connection.
       */
      static int connect( const std::string& domain, int port );

    private:
      typedef struct buf {
        unsigned char buf[PACKETSZ];
        int len;
      };
      typedef unsigned char name [MAXDNAME];
  };

};

#endif // DNS_H__

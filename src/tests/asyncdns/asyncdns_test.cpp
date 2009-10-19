#include "../../asyncdns.h"
#include "../../asyncdnshandler.h"
#include "../../logsink.h"

#include <stdio.h>
#include <locale.h>
#include <string>
#include <cstdio> // [s]print[f]

std::string subjects[5] = { "camaya.net", "jabber.org", "glooxd.im", "öööö.öö", "jabber.jsmart.id" };
class ADNSTest : public gloox::AsyncDNSHandler
{

  // reimplemented from AsyncDNSHandler
  virtual void handleAsyncResolveResult( const gloox::DNS::HostMap& hosts, void* context );

  // reimplemented from AsyncDNSHandler
  virtual void handleAsyncConnectResult( int fd, void* context );

};

void ADNSTest::handleAsyncResolveResult( const gloox::DNS::HostMap& hosts, void* context )
{
  if( hosts.size() )
  {
    printf( "results for %s\n", subjects[reinterpret_cast<int>( context )].c_str() );
    gloox::DNS::HostMap::const_iterator it = hosts.begin();
    for( ; it != hosts.end(); ++it )
    {
      printf( "found: %s:%d\n", (*it).first.c_str(), (*it).second );
    }
  }
  else
    printf( "no hosts found for %s\n", subjects[reinterpret_cast<int>( context )].c_str() );
}

void ADNSTest::handleAsyncConnectResult( int fd, void* context )
{
}

int main( int, char** )
{
  int fail = 0;
  std::string name;
  gloox::AsyncDNS adns;
  ADNSTest adt;
  gloox::LogSink ls;
  // -------
  {
    name = "resolve test";
    for( int i = 0; i < 5; ++i )
      adns.resolve( &adt, subjects[i], ls, reinterpret_cast<void*>( i ) );
    if( 0 ) // should fail, as there's no handler for foonode anymore
    {
      ++fail;
      printf( "test '%s' failed\n", name.c_str() );
    }
  }





  printf( "AsyncDNS: " );
  if( fail == 0 )
  {
    printf( "OK\n" );
    return 0;
  }
  else
  {
    printf( "%d test(s) failed\n", fail );
    printf( "Note: these tests are bound to fail if no network connection is available\n" );
    return 1;
  }

}

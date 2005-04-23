/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#include "workertest.h"
#include "../worker/worker.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

WorkerTest f;

int main(int argc, char *argv[])
{
  signal(SIGINT, sigHandler);

  if( f.setCmdLineArgs( argc, argv ) )
    f.start();
  else
    return 1;

  return 0;
}

void sigHandler( int /*signal*/ )
{
  f.c->disconnect();
}

WorkerTest::~WorkerTest()
{
}

void WorkerTest::start()
{
  if( m_feeder.empty() )
    m_feeder = "remon@camaya.net/feeder";

  if( m_passwd.empty() )
    m_passwd = "jline";

  if( m_self.empty() )
    m_self = "jline@camaya.net/worker";

  c = new Worker( m_self, m_passwd, m_debug );
  c->registerInfoHandler( this );
  c->registerDataHandler( this );
  c->setFeeder( m_feeder );
  c->connect();
  delete c;
}

void WorkerTest::connected()
{
  printf( "connected\n" );
}

void WorkerTest::disconnected()
{
  printf( "disconnected\n" );
}

void WorkerTest::data( const char* data)
{
  int num = atoi( data );
  printf( "prime factors for %d: ", num );
  c->result( RESULT_SUCCESS, factorise( num ) );
}

char* WorkerTest::factorise( int number )
{
  int num = number;
  char* result = 0;
  IntList::const_iterator it = m_primes.begin();
  int rem = -1/*, sq = sqrt( number )*/;
  while( ( rem != 0 ) && ( num != 1 ) && ( (*it) < number ) )
  {
//     printf("using prime number: %d\n", (*it));
    int rem = num % (*it);
    if( rem == 0 )
    {
      if( !result )
      {
        result = (char*)realloc( result, sizeof( int ) );
        sprintf( result, "%d", (*it) );
      }
      else
      {
        result = (char*)realloc( result, strlen( result ) + sizeof( char ) + sizeof( int ) );
        sprintf( result, "%s*%d", result, (*it) );
      }
      num = num / (*it);
      it = m_primes.begin();
    }
    else
    {
      ++it;
      if( it == m_primes.end() )
        break;
    }
  }
  if( result )
    printf( "%s\n", result );
  else
  {
    m_primes.push_back( number );
    printf( "none. %d is prime\n", number );
  }
  return result;
}

bool WorkerTest::setCmdLineArgs( int argc, char *argv[] )
{
  for (int i=0;i<argc;++i )
  {
    if ( argv[i][0] == '-' )
      switch ( argv[i][1] )
    {
      case 'f':
        if ( argv[++i] && argv[i][0] != '-' )
        {
          m_feeder = argv[i];
        }
        else
        {
          printf( "parameter -f requires a Jabber ID.\n" );
          return false;
        }
        break;

      case 'd':
        m_debug = true;
        break;

      case 's':
        if ( argv[++i] && argv[i][0] != '-' )
        {
          m_self = argv[i];
        }
        else
        {
          printf( "parameter -s requires a Jabber ID.\n" );
          return false;
        }
        break;

      case 'p':
        if ( argv[++i] && argv[i][0] != '-' )
        {
          m_passwd =  argv[i];
        }
        else
        {
          printf( "parameter -p requires a password.\n" );
          return false;
        }
        break;
    }
  }

  return true;
}

WorkerTest::WorkerTest()
  : m_debug( false )
{
  m_primes.push_back(2);
  m_primes.push_back(3);
  m_primes.push_back(5);
  m_primes.push_back(7);
  m_primes.push_back(11);
  m_primes.push_back(13);
  m_primes.push_back(17);
  m_primes.push_back(19);
  m_primes.push_back(23);
  m_primes.push_back(29);
  m_primes.push_back(31);
  m_primes.push_back(37);
  m_primes.push_back(41);
  m_primes.push_back(43);
  m_primes.push_back(47);
  m_primes.push_back(53);
  m_primes.push_back(59);
  m_primes.push_back(61);
  m_primes.push_back(67);
  m_primes.push_back(71);
  m_primes.push_back(73);
  m_primes.push_back(79);
  m_primes.push_back(83);
  m_primes.push_back(89);
  m_primes.push_back(97);
  m_primes.push_back(101);
  m_primes.push_back(103);
  m_primes.push_back(107);
  m_primes.push_back(109);
  m_primes.push_back(113);
  m_primes.push_back(127);
  m_primes.push_back(131);
  m_primes.push_back(137);
  m_primes.push_back(139);
  m_primes.push_back(149);
  m_primes.push_back(151);
  m_primes.push_back(157);
  m_primes.push_back(163);
  m_primes.push_back(167);
  m_primes.push_back(173);
  m_primes.push_back(179);
  m_primes.push_back(181);
  m_primes.push_back(191);
  m_primes.push_back(193);
  m_primes.push_back(197);
  m_primes.push_back(199);
  m_primes.push_back(211);
  m_primes.push_back(223);
  m_primes.push_back(227);
  m_primes.push_back(229);
  m_primes.push_back(233);
  m_primes.push_back(239);
  m_primes.push_back(241);
  m_primes.push_back(251);
  m_primes.push_back(257);
  m_primes.push_back(263);
  m_primes.push_back(269);
  m_primes.push_back(271);
  m_primes.push_back(277);
  m_primes.push_back(281);
  m_primes.push_back(283);
  m_primes.push_back(293);
  m_primes.push_back(307);
  m_primes.push_back(311);
  m_primes.push_back(313);
  m_primes.push_back(317);
  m_primes.push_back(331);
  m_primes.push_back(337);
  m_primes.push_back(347);
  m_primes.push_back(349);
  m_primes.push_back(353);
  m_primes.push_back(359);
  m_primes.push_back(367);
  m_primes.push_back(373);
  m_primes.push_back(379);
  m_primes.push_back(383);
  m_primes.push_back(389);
  m_primes.push_back(397);
  m_primes.push_back(401);
  m_primes.push_back(409);
  m_primes.push_back(419);
  m_primes.push_back(421);
  m_primes.push_back(431);
  m_primes.push_back(433);
  m_primes.push_back(439);
  m_primes.push_back(443);
  m_primes.push_back(449);
  m_primes.push_back(457);
  m_primes.push_back(461);
  m_primes.push_back(463);
  m_primes.push_back(467);
  m_primes.push_back(479);
  m_primes.push_back(487);
  m_primes.push_back(491);
  m_primes.push_back(499);
  m_primes.push_back(503);
  m_primes.push_back(509);
  m_primes.push_back(521);
  m_primes.push_back(523);
  m_primes.push_back(541);
  m_primes.push_back(547);
  m_primes.push_back(557);
  m_primes.push_back(563);
  m_primes.push_back(569);
  m_primes.push_back(571);
  m_primes.push_back(577);
  m_primes.push_back(587);
  m_primes.push_back(593);
  m_primes.push_back(599);
  m_primes.push_back(601);
  m_primes.push_back(607);
  m_primes.push_back(613);
  m_primes.push_back(617);
  m_primes.push_back(619);
  m_primes.push_back(631);
  m_primes.push_back(641);
  m_primes.push_back(643);
  m_primes.push_back(647);
  m_primes.push_back(653);
  m_primes.push_back(659);
  m_primes.push_back(661);
  m_primes.push_back(673);
  m_primes.push_back(677);
  m_primes.push_back(683);
  m_primes.push_back(691);
  m_primes.push_back(701);
  m_primes.push_back(709);
  m_primes.push_back(719);
  m_primes.push_back(727);
  m_primes.push_back(733);
  m_primes.push_back(739);
  m_primes.push_back(743);
  m_primes.push_back(751);
  m_primes.push_back(757);
  m_primes.push_back(761);
  m_primes.push_back(769);
  m_primes.push_back(773);
  m_primes.push_back(787);
  m_primes.push_back(797);
  m_primes.push_back(809);
  m_primes.push_back(811);
  m_primes.push_back(821);
  m_primes.push_back(823);
  m_primes.push_back(827);
  m_primes.push_back(829);
  m_primes.push_back(839);
  m_primes.push_back(853);
  m_primes.push_back(857);
  m_primes.push_back(859);
  m_primes.push_back(863);
  m_primes.push_back(877);
  m_primes.push_back(881);
  m_primes.push_back(883);
  m_primes.push_back(887);
  m_primes.push_back(907);
  m_primes.push_back(911);
  m_primes.push_back(919);
  m_primes.push_back(929);
  m_primes.push_back(937);
  m_primes.push_back(941);
  m_primes.push_back(947);
  m_primes.push_back(953);
  m_primes.push_back(967);
  m_primes.push_back(971);
  m_primes.push_back(977);
  m_primes.push_back(983);
  m_primes.push_back(991);
  m_primes.push_back(997);
  m_primes.push_back(1009);
  m_primes.push_back(1013);
  m_primes.push_back(1019);
  m_primes.push_back(1021);
  m_primes.push_back(1031);
  m_primes.push_back(1033);
  m_primes.push_back(1039);
  m_primes.push_back(1049);
  m_primes.push_back(1051);
  m_primes.push_back(1061);
  m_primes.push_back(1063);
  m_primes.push_back(1069);
  m_primes.push_back(1087);
  m_primes.push_back(1091);
  m_primes.push_back(1093);
  m_primes.push_back(1097);
  m_primes.push_back(1103);
  m_primes.push_back(1109);
  m_primes.push_back(1117);
  m_primes.push_back(1123);
  m_primes.push_back(1129);
  m_primes.push_back(1151);
  m_primes.push_back(1153);
  m_primes.push_back(1163);
  m_primes.push_back(1171);
  m_primes.push_back(1181);
  m_primes.push_back(1187);
  m_primes.push_back(1193);
  m_primes.push_back(1201);
  m_primes.push_back(1213);
  m_primes.push_back(1217);
  m_primes.push_back(1223);
  m_primes.push_back(1229);
  m_primes.push_back(1231);
  m_primes.push_back(1237);
  m_primes.push_back(1249);
  m_primes.push_back(1259);
  m_primes.push_back(1277);
  m_primes.push_back(1279);
  m_primes.push_back(1283);
  m_primes.push_back(1289);
  m_primes.push_back(1291);
  m_primes.push_back(1297);
  m_primes.push_back(1301);
  m_primes.push_back(1303);
  m_primes.push_back(1307);
  m_primes.push_back(1319);
  m_primes.push_back(1321);
  m_primes.push_back(1327);
  m_primes.push_back(1361);
  m_primes.push_back(1367);
  m_primes.push_back(1373);
  m_primes.push_back(1381);
  m_primes.push_back(1399);
  m_primes.push_back(1409);
  m_primes.push_back(1423);
  m_primes.push_back(1427);
  m_primes.push_back(1429);
  m_primes.push_back(1433);
  m_primes.push_back(1439);
  m_primes.push_back(1447);
  m_primes.push_back(1451);
  m_primes.push_back(1453);
  m_primes.push_back(1459);
  m_primes.push_back(1471);
  m_primes.push_back(1481);
  m_primes.push_back(1483);
  m_primes.push_back(1487);
  m_primes.push_back(1489);
  m_primes.push_back(1493);
  m_primes.push_back(1499);
  m_primes.push_back(1511);
  m_primes.push_back(1523);
  m_primes.push_back(1531);
  m_primes.push_back(1543);
  m_primes.push_back(1549);
  m_primes.push_back(1553);
  m_primes.push_back(1559);
  m_primes.push_back(1567);
  m_primes.push_back(1571);
  m_primes.push_back(1579);
  m_primes.push_back(1583);
  m_primes.push_back(1597);
  m_primes.push_back(1601);
  m_primes.push_back(1607);
  m_primes.push_back(1609);
  m_primes.push_back(1613);
  m_primes.push_back(1619);
  m_primes.push_back(1621);
  m_primes.push_back(1627);
  m_primes.push_back(1637);
  m_primes.push_back(1657);
  m_primes.push_back(1663);
  m_primes.push_back(1667);
  m_primes.push_back(1669);
  m_primes.push_back(1693);
  m_primes.push_back(1697);
  m_primes.push_back(1699);
  m_primes.push_back(1709);
  m_primes.push_back(1721);
  m_primes.push_back(1723);
  m_primes.push_back(1733);
  m_primes.push_back(1741);
  m_primes.push_back(1747);
  m_primes.push_back(1753);
  m_primes.push_back(1759);
  m_primes.push_back(1777);
  m_primes.push_back(1783);
  m_primes.push_back(1787);
  m_primes.push_back(1789);
  m_primes.push_back(1801);
  m_primes.push_back(1811);
  m_primes.push_back(1823);
  m_primes.push_back(1831);
  m_primes.push_back(1847);
  m_primes.push_back(1861);
  m_primes.push_back(1867);
  m_primes.push_back(1871);
  m_primes.push_back(1873);
  m_primes.push_back(1877);
  m_primes.push_back(1879);
  m_primes.push_back(1889);
  m_primes.push_back(1901);
  m_primes.push_back(1907);
  m_primes.push_back(1913);
  m_primes.push_back(1931);
  m_primes.push_back(1933);
  m_primes.push_back(1949);
  m_primes.push_back(1951);
  m_primes.push_back(1973);
  m_primes.push_back(1979);
  m_primes.push_back(1987);
  m_primes.push_back(1993);
  m_primes.push_back(1997);
  m_primes.push_back(1999);
  m_primes.push_back(2003);
  m_primes.push_back(2011);
  m_primes.push_back(2017);
  m_primes.push_back(2027);
  m_primes.push_back(2029);
  m_primes.push_back(2039);
  m_primes.push_back(2053);
  m_primes.push_back(2063);
  m_primes.push_back(2069);
  m_primes.push_back(2081);
  m_primes.push_back(2083);
  m_primes.push_back(2087);
  m_primes.push_back(2089);
  m_primes.push_back(2099);
  m_primes.push_back(2111);
  m_primes.push_back(2113);
  m_primes.push_back(2129);
  m_primes.push_back(2131);
  m_primes.push_back(2137);
  m_primes.push_back(2141);
  m_primes.push_back(2143);
  m_primes.push_back(2153);
  m_primes.push_back(2161);
  m_primes.push_back(2179);
  m_primes.push_back(2203);
  m_primes.push_back(2207);
  m_primes.push_back(2213);
  m_primes.push_back(2221);
  m_primes.push_back(2237);
  m_primes.push_back(2239);
  m_primes.push_back(2243);
  m_primes.push_back(2251);
  m_primes.push_back(2267);
  m_primes.push_back(2269);
  m_primes.push_back(2273);
  m_primes.push_back(2281);
  m_primes.push_back(2287);
  m_primes.push_back(2293);
  m_primes.push_back(2297);
  m_primes.push_back(2309);
  m_primes.push_back(2311);
  m_primes.push_back(2333);
  m_primes.push_back(2339);
  m_primes.push_back(2341);
  m_primes.push_back(2347);
  m_primes.push_back(2351);
  m_primes.push_back(2357);
  m_primes.push_back(2371);
  m_primes.push_back(2377);
  m_primes.push_back(2381);
  m_primes.push_back(2383);
  m_primes.push_back(2389);
  m_primes.push_back(2393);
  m_primes.push_back(2399);
  m_primes.push_back(2411);
  m_primes.push_back(2417);
  m_primes.push_back(2423);
  m_primes.push_back(2437);
  m_primes.push_back(2441);
  m_primes.push_back(2447);
  m_primes.push_back(2459);
  m_primes.push_back(2467);
  m_primes.push_back(2473);
  m_primes.push_back(2477);
  m_primes.push_back(2503);
  m_primes.push_back(2521);
  m_primes.push_back(2531);
  m_primes.push_back(2539);
  m_primes.push_back(2543);
  m_primes.push_back(2549);
  m_primes.push_back(2551);
  m_primes.push_back(2557);
  m_primes.push_back(2579);
  m_primes.push_back(2591);
  m_primes.push_back(2593);
  m_primes.push_back(2609);
  m_primes.push_back(2617);
  m_primes.push_back(2621);
  m_primes.push_back(2633);
  m_primes.push_back(2647);
  m_primes.push_back(2657);
  m_primes.push_back(2659);
  m_primes.push_back(2663);
  m_primes.push_back(2671);
  m_primes.push_back(2677);
  m_primes.push_back(2683);
  m_primes.push_back(2687);
  m_primes.push_back(2689);
  m_primes.push_back(2693);
  m_primes.push_back(2699);
  m_primes.push_back(2707);
  m_primes.push_back(2711);
  m_primes.push_back(2713);
  m_primes.push_back(2719);
  m_primes.push_back(2729);
  m_primes.push_back(2731);
  m_primes.push_back(2741);
  m_primes.push_back(2749);
  m_primes.push_back(2753);
  m_primes.push_back(2767);
  m_primes.push_back(2777);
  m_primes.push_back(2789);
  m_primes.push_back(2791);
  m_primes.push_back(2797);
  m_primes.push_back(2801);
  m_primes.push_back(2803);
  m_primes.push_back(2819);
  m_primes.push_back(2833);
  m_primes.push_back(2837);
  m_primes.push_back(2843);
  m_primes.push_back(2851);
  m_primes.push_back(2857);
  m_primes.push_back(2861);
  m_primes.push_back(2879);
  m_primes.push_back(2887);
  m_primes.push_back(2897);
  m_primes.push_back(2903);
  m_primes.push_back(2909);
  m_primes.push_back(2917);
  m_primes.push_back(2927);
  m_primes.push_back(2939);
  m_primes.push_back(2953);
  m_primes.push_back(2957);
  m_primes.push_back(2963);
  m_primes.push_back(2969);
  m_primes.push_back(2971);
  m_primes.push_back(2999);
  m_primes.push_back(3001);
  m_primes.push_back(3011);
  m_primes.push_back(3019);
  m_primes.push_back(3023);
  m_primes.push_back(3037);
  m_primes.push_back(3041);
  m_primes.push_back(3049);
  m_primes.push_back(3061);
  m_primes.push_back(3067);
  m_primes.push_back(3079);
  m_primes.push_back(3083);
  m_primes.push_back(3089);
  m_primes.push_back(3109);
  m_primes.push_back(3119);
  m_primes.push_back(3121);
  m_primes.push_back(3137);
  m_primes.push_back(3163);
  m_primes.push_back(3167);
  m_primes.push_back(3169);
  m_primes.push_back(3181);
  m_primes.push_back(3187);
  m_primes.push_back(3191);
  m_primes.push_back(3203);
  m_primes.push_back(3209);
  m_primes.push_back(3217);
  m_primes.push_back(3221);
  m_primes.push_back(3229);
  m_primes.push_back(3251);
  m_primes.push_back(3253);
  m_primes.push_back(3257);
  m_primes.push_back(3259);
  m_primes.push_back(3271);
  m_primes.push_back(3299);
  m_primes.push_back(3301);
  m_primes.push_back(3307);
  m_primes.push_back(3313);
  m_primes.push_back(3319);
  m_primes.push_back(3323);
  m_primes.push_back(3329);
  m_primes.push_back(3331);
  m_primes.push_back(3343);
  m_primes.push_back(3347);
  m_primes.push_back(3359);
  m_primes.push_back(3361);
  m_primes.push_back(3371);
  m_primes.push_back(3373);
  m_primes.push_back(3389);
  m_primes.push_back(3391);
  m_primes.push_back(3407);
  m_primes.push_back(3413);
  m_primes.push_back(3433);
  m_primes.push_back(3449);
  m_primes.push_back(3457);
  m_primes.push_back(3461);
  m_primes.push_back(3463);
  m_primes.push_back(3467);
  m_primes.push_back(3469);
  m_primes.push_back(3491);
  m_primes.push_back(3499);
  m_primes.push_back(3511);
  m_primes.push_back(3517);
  m_primes.push_back(3527);
  m_primes.push_back(3529);
  m_primes.push_back(3533);
  m_primes.push_back(3539);
  m_primes.push_back(3541);
  m_primes.push_back(3547);
  m_primes.push_back(3557);
  m_primes.push_back(3559);
  m_primes.push_back(3571);
}

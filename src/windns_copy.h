#ifndef _WINDNS_COPY_H
#define _WINDNS_COPY_H
#if __GNUC__ >= 3
#pragma GCC system_header
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DNS_QUERY_STANDARD 0x0
#define DNS_QUERY_ACCEPT_TRUNCATED_RESPONSE 0x1
#define DNS_QUERY_USE_TCP_ONLY 0x2
#define DNS_QUERY_NO_RECURSION 0x4
#define DNS_QUERY_BYPASS_CACHE 0x8
#define DNS_QUERY_NO_WIRE_QUERY	0x10
#define DNS_QUERY_NO_LOCAL_NAME 0x20
#define DNS_QUERY_NO_HOSTS_FILE 0x40
#define DNS_QUERY_NO_NETBT 0x80
#define DNS_QUERY_TREAT_AS_FQDN 0x1000
#define DNS_QUERY_WIRE_ONLY 0x100
#define DNS_QUERY_RETURN_MESSAGE 0x200
#define DNS_QUERY_DONT_RESET_TTL_VALUES 0x100000
#define DNS_QUERY_RESERVED 0xff000000

#define DNS_UPDATE_SECURITY_USE_DEFAULT	0x0
#define DNS_UPDATE_SECURITY_OFF 0x10
#define DNS_UPDATE_SECURITY_ON 0x20
#define DNS_UPDATE_SECURITY_ONLY 0x100
#define DNS_UPDATE_CACHE_SECURITY_CONTEXT 0x200
#define DNS_UPDATE_TEST_USE_LOCAL_SYS_ACCT 0x400
#define DNS_UPDATE_FORCE_SECURITY_NEGO 0x800
#define DNS_UPDATE_RESERVED 0xffff0000

#ifndef RC_INVOKE
typedef DWORD IP4_ADDRESS;
typedef DWORD DNS_STATUS;
typedef struct _DnsRecordA {
	struct _DnsRecordA* pNext;
	LPSTR pName;
	WORD wType;
	WORD wDataLength;
	union {
		DWORD DW;
		DNS_RECORD_FLAGS S;
	} Flags;
	DWORD dwTtl;
	DWORD dwReserved;
	union {
		DNS_A_DATA A;
		DNS_SOA_DATAA SOA, Soa;
		DNS_PTR_DATAA PTR, Ptr, NS, Ns, CNAME, Cname, MB, Mb,
			      MD, Md, MF, Mf, MG, Mg, MR, Mr;
		DNS_MINFO_DATAA MINFO, Minfo, RP, Rp;
		DNS_MX_DATAA MX, Mx, AFSDB, Afsdb, RT, Rt;
		DNS_TXT_DATAA HINFO, Hinfo, ISDN, Isdn, TXT, Txt, X25;
		DNS_NULL_DATA Null;
		DNS_WKS_DATA WKS, Wks;
		DNS_AAAA_DATA AAAA;
		DNS_KEY_DATA KEY, Key;
		DNS_SIG_DATAA SIG, Sig;
		DNS_ATMA_DATA ATMA, Atma;
		DNS_NXT_DATAA NXT, Nxt;
		DNS_SRV_DATAA SRV, Srv;
		DNS_TKEY_DATAA TKEY, Tkey;
		DNS_TSIG_DATAA TSIG, Tsig;
		DNS_WINS_DATA WINS, Wins;
		DNS_WINSR_DATAA WINSR, WinsR, NBSTAT, Nbstat;
	} Data;
} DNS_RECORDA, *PDNS_RECORDA;
typedef struct _DnsRecordW {
	struct _DnsRecordW* pNext;
	LPWSTR pName;
	WORD wType;
	WORD wDataLength;
	union {
		DWORD DW;
		DNS_RECORD_FLAGS S;
	} Flags;
	DWORD dwTtl;
	DWORD dwReserved;
	union {
		DNS_A_DATA A;
		DNS_SOA_DATAW SOA, Soa;
		DNS_PTR_DATAW PTR, Ptr, NS, Ns, CNAME, Cname, MB, Mb,
			      MD, Md, MF, Mf, MG, Mg, MR, Mr;
		DNS_MINFO_DATAW MINFO, Minfo, RP, Rp;
		DNS_MX_DATAW MX, Mx, AFSDB, Afsdb, RT, Rt;
		DNS_TXT_DATAW HINFO, Hinfo, ISDN, Isdn, TXT, Txt, X25;
		DNS_NULL_DATA Null;
		DNS_WKS_DATA WKS, Wks;
		DNS_AAAA_DATA AAAA;
		DNS_KEY_DATA KEY, Key;
		DNS_SIG_DATAW SIG, Sig;
		DNS_ATMA_DATA ATMA, Atma;
		DNS_NXT_DATAW NXT, Nxt;
		DNS_SRV_DATAW SRV, Srv;
		DNS_TKEY_DATAW TKEY, Tkey;
		DNS_TSIG_DATAW TSIG, Tsig;
		DNS_WINS_DATA WINS, Wins;
		DNS_WINSR_DATAW WINSR, WinsR, NBSTAT, Nbstat;
	} Data;
} DNS_RECORDW, *PDNS_RECORDW;

#ifdef UNICODE
#define DNS_RECORD DNS_RECORDW
#define PDNS_RECORD PDNS_RECORDW
#else
#define DNS_RECORD DNS_RECORDA
#define PDNS_RECORD PDNS_RECORDA
#endif

typedef struct _DnsRRSet {
	PDNS_RECORD pFirstRR;
	PDNS_RECORD pLastRR;
} DNS_RRSET, *PDNS_RRSET;

#endif /* RC_INVOKED */

#ifdef __cplusplus
}
#endif

#endif /* _WINDNS_COPY_H */

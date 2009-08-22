#ifndef _WINDNS_H
#define _WINDNS_H
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
enum {
	DNS_TYPE_A=1,
	DNS_TYPE_NS,
	DNS_TYPE_MD,
	DNS_TYPE_MF,
	DNS_TYPE_CNAME,
	DNS_TYPE_SOA,
	DNS_TYPE_MB,
	DNS_TYPE_MG,
	DNS_TYPE_MR,
	DNS_TYPE_NULL,
	DNS_TYPE_WKS,
	DNS_TYPE_PTR,
	DNS_TYPE_HINFO,
	DNS_TYPE_MINFO,
	DNS_TYPE_MX,
	DNS_TYPE_TEXT
};
typedef enum _DNS_CHARSET
{
	DnsCharSetUnknown,
	DnsCharSetUnicode,
	DnsCharSetUtf8,
	DnsCharSetAnsi
} DNS_CHARSET;
typedef enum
{
	DnsConfigPrimaryDomainName_W,
	DnsConfigPrimaryDomainName_A,
	DnsConfigPrimaryDomainName_UTF8,
	DnsConfigAdapterDomainName_W,
	DnsConfigAdapterDomainName_A,
	DnsConfigAdapterDomainName_UTF8,
	DnsConfigDnsServerList,
	DnsConfigSearchList,
	DnsConfigAdapterInfo,
	DnsConfigPrimaryHostNameRegistrationEnabled,
	DnsConfigAdapterHostNameRegistrationEnabled,
	DnsConfigAddressRegistrationMaxCount,
	DnsConfigHostName_W,
	DnsConfigHostName_A,
	DnsConfigHostName_UTF8,
	DnsConfigFullHostName_W,
	DnsConfigFullHostName_A,
	DnsConfigFullHostName_UTF8
} DNS_CONFIG_TYPE;
typedef enum
{
	DnsFreeFlat = 0,
	DnsFreeRecordList,
	DnsFreeParsedMessageFields
} DNS_FREE_TYPE;
typedef enum _DNS_NAME_FORMAT
{
	DnsNameDomain,
	DnsNameDomainLabel,
	DnsNameHostnameFull,
	DnsNameHostnameLabel,
	DnsNameWildcard,
	DnsNameSrvRecord
} DNS_NAME_FORMAT;
typedef enum
{
	DnsSectionQuestion,
	DnsSectionAnswer,
	DnsSectionAuthority,
	DnsSectionAdditional
} DNS_SECTION;
typedef struct _IP4_ARRAY {
	DWORD AddrCount;
	IP4_ADDRESS AddrArray[1];
} IP4_ARRAY, *PIP4_ARRAY;
typedef struct {
	DWORD IP6Dword[4];
} IP6_ADDRESS, *PIP6_ADDRESS, DNS_IP6_ADDRESS, *PDNS_IP6_ADDRESS;
typedef struct _DNS_HEADER {
	WORD Xid;
	BYTE RecursionDesired;
	BYTE Truncation;
	BYTE Authoritative;
	BYTE Opcode;
	BYTE IsResponse;
	BYTE ResponseCode;
	BYTE Reserved;
	BYTE RecursionAvailable;
	WORD QuestionCount;
	WORD AnswerCount;
	WORD NameServerCount;
	WORD AdditionalCount;
} DNS_HEADER, *PDNS_HEADER;
typedef struct _DNS_MESSAGE_BUFFER {
	DNS_HEADER MessageHead;
	CHAR MessageBody[1];
} DNS_MESSAGE_BUFFER, *PDNS_MESSAGE_BUFFER;
typedef struct _DnsRecordFlags {
	DWORD Section	:2;
	DWORD Delete	:1;
	DWORD CharSet	:2;
	DWORD Unused	:3;
	DWORD Reserved	:24;
} DNS_RECORD_FLAGS;
typedef struct {
	IP4_ADDRESS IpAddress;
} DNS_A_DATA, *PDNS_A_DATA;
typedef struct {
	DNS_IP6_ADDRESS Ip6Address;
} DNS_AAAA_DATA, *PDNS_AAAA_DATA;
#define DNS_ATMA_MAX_ADDR_LENGTH 20
typedef struct {
	BYTE AddressType;
	BYTE Address[DNS_ATMA_MAX_ADDR_LENGTH];
} DNS_ATMA_DATA, *PDNS_ATMA_DATA;
typedef struct {
	WORD wFlags;
	BYTE chProtocol;
	BYTE chAlgorithm;
	BYTE Key[1];
} DNS_KEY_DATA, *PDNS_KEY_DATA;
typedef struct {
	WORD wVersion;
	WORD wSize;
	WORD wHorPrec;
	WORD wVerPrec;
	DWORD dwLatitude;
	DWORD dwLongitude;
	DWORD dwAltitude;
} DNS_LOC_DATA, *PDNS_LOC_DATA;
typedef struct {
	LPSTR pNameMailbox;
	LPSTR pNameErrorsMailbox;
} DNS_MINFO_DATAA, *PDNS_MINFO_DATAA;
typedef struct {
	LPWSTR pNameMailbox;
	LPWSTR pNameErrorsMailbox;
} DNS_MINFO_DATAW, *PDNS_MINFO_DATAW;
typedef struct {
	LPSTR pNameExchange;
	WORD wPreference;
	WORD Pad;
} DNS_MX_DATAA, *PDNS_MX_DATAA;
typedef struct {
	LPWSTR pNameExchange;
	WORD wPreference;
	WORD Pad;
} DNS_MX_DATAW, *PDNS_MX_DATAW;
typedef struct {
	DWORD dwByteCount;
	BYTE Data[1];
} DNS_NULL_DATA, *PDNS_NULL_DATA;
typedef struct {
	LPSTR pNameNext;
	WORD wNumTypes;
	WORD wTypes[1];
} DNS_NXT_DATAA, *PDNS_NXT_DATAA;
typedef struct {
	LPWSTR pNameNext;
	WORD wNumTypes;
	WORD wTypes[1];
} DNS_NXT_DATAW, *PDNS_NXT_DATAW;
typedef struct {
	LPSTR pNameHost;
} DNS_PTR_DATAA, *PDNS_PTR_DATAA;
typedef struct {
	LPWSTR pNameHost;
} DNS_PTR_DATAW, *PDNS_PTR_DATAW;
typedef struct {
	LPSTR pNameSigner;
	WORD wTypeCovered;
	BYTE chAlgorithm;
	BYTE chLabelCount;
	DWORD dwOriginalTtl;
	DWORD dwExpiration;
	DWORD dwTimeSigned;
	WORD wKeyTag;
	WORD Pad;
	BYTE Signature[1];
} DNS_SIG_DATAA, *PDNS_SIG_DATAA;
typedef struct {
	LPWSTR pNameSigner;
	WORD wTypeCovered;
	BYTE chAlgorithm;
	BYTE chLabelCount;
	DWORD dwOriginalTtl;
	DWORD dwExpiration;
	DWORD dwTimeSigned;
	WORD wKeyTag;
	WORD Pad;
	BYTE Signature[1];
} DNS_SIG_DATAW, *PDNS_SIG_DATAW;
typedef struct {
	LPSTR pNamePrimaryServer;
	LPSTR pNameAdministrator;
	DWORD dwSerialNo;
	DWORD dwRefresh;
	DWORD dwRetry;
	DWORD dwExpire;
	DWORD dwDefaultTtl;
} DNS_SOA_DATAA, *PDNS_SOA_DATAA;
typedef struct {
	LPWSTR pNamePrimaryServer;
	LPWSTR pNameAdministrator;
	DWORD dwSerialNo;
	DWORD dwRefresh;
	DWORD dwRetry;
	DWORD dwExpire;
	DWORD dwDefaultTtl;
} DNS_SOA_DATAW, *PDNS_SOA_DATAW;
typedef struct {
	LPSTR pNameTarget;
	WORD wPriority;
	WORD wWeight;
	WORD wPort;
	WORD Pad;
} DNS_SRV_DATAA, *PDNS_SRV_DATAA;
typedef struct {
	LPWSTR pNameTarget;
	WORD wPriority;
	WORD wWeight;
	WORD wPort;
	WORD Pad;
} DNS_SRV_DATAW, *PDNS_SRV_DATAW;
typedef struct {
	DWORD dwStringCount;
	LPSTR pStringArray[1];
} DNS_TXT_DATAA, *PDNS_TXT_DATAA;
typedef struct {
	DWORD dwStringCount;
	LPWSTR pStringArray[1];
} DNS_TXT_DATAW, *PDNS_TXT_DATAW;
typedef struct {
	LPSTR pNameAlgorithm;
	PBYTE pAlgorithmPacket;
	PBYTE pKey;
	PBYTE pOtherData;
	DWORD dwCreateTime;
	DWORD dwExpireTime;
	WORD wMode;
	WORD wError;
	WORD wKeyLength;
	WORD wOtherLength;
	UCHAR cAlgNameLength;
	BOOL bPacketPointers;
} DNS_TKEY_DATAA, *PDNS_TKEY_DATAA;
typedef struct {
	LPWSTR pNameAlgorithm;
	PBYTE pAlgorithmPacket;
	PBYTE pKey;
	PBYTE pOtherData;
	DWORD dwCreateTime;
	DWORD dwExpireTime;
	WORD wMode;
	WORD wError;
	WORD wKeyLength;
	WORD wOtherLength;
	UCHAR cAlgNameLength;
	BOOL bPacketPointers;
} DNS_TKEY_DATAW, *PDNS_TKEY_DATAW;
typedef struct {
	LPSTR pNameAlgorithm;
	PBYTE pAlgorithmPacket;
	PBYTE pSignature;
	PBYTE pOtherData;
	LONGLONG i64CreateTime;
	WORD wFudgeTime;
	WORD wOriginalXid;
	WORD wError;
	WORD wSigLength;
	WORD wOtherLength;
	UCHAR cAlgNameLength;
	BOOL bPacketPointers;
} DNS_TSIG_DATAA, *PDNS_TSIG_DATAA;
typedef struct {
	LPWSTR pNameAlgorithm;
	PBYTE pAlgorithmPacket;
	PBYTE pSignature;
	PBYTE pOtherData;
	LONGLONG i64CreateTime;
	WORD wFudgeTime;
	WORD wOriginalXid;
	WORD wError;
	WORD wSigLength;
	WORD wOtherLength;
	UCHAR cAlgNameLength;
	BOOL bPacketPointers;
} DNS_TSIG_DATAW, *PDNS_TSIG_DATAW;
typedef struct {
	DWORD dwMappingFlag;
	DWORD dwLookupTimeout;
	DWORD dwCacheTimeout;
	DWORD cWinsServerCount;
	IP4_ADDRESS WinsServers[1];
} DNS_WINS_DATA, *PDNS_WINS_DATA;
typedef struct {
	DWORD dwMappingFlag;
	DWORD dwLookupTimeout;
	DWORD dwCacheTimeout;
	LPSTR pNameResultDomain;
} DNS_WINSR_DATAA, *PDNS_WINSR_DATAA;
typedef struct {
	DWORD dwMappingFlag;
	DWORD dwLookupTimeout;
	DWORD dwCacheTimeout;
	LPWSTR pNameResultDomain;
} DNS_WINSR_DATAW, *PDNS_WINSR_DATAW;
typedef struct _DNS_WIRE_QUESTION {
	WORD QuestionType;
	WORD QuestionClass;
} DNS_WIRE_QUESTION, *PDNS_WIRE_QUESTION;
typedef struct _DNS_WIRE_RECORD {
	WORD RecordType;
	WORD RecordClass;
	DWORD TimeToLive;
	WORD DataLength;
} DNS_WIRE_RECORD, *PDNS_WIRE_RECORD;
typedef struct {
	IP4_ADDRESS IpAddress;
	UCHAR chProtocol;
	BYTE BitMask[1];
} DNS_WKS_DATA, *PDNS_WKS_DATA;
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

#endif /* _WINDNS_H */

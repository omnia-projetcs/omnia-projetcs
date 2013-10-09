//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/aa390422%28v=vs.85%29.aspx
#include <stdio.h>
#define _WIN32_DCOM

#include "WbemCli.h"
#include "wbemprov.h"
#include "wbemtran.h"
#include <objbase.h>

#include "crypt/sha2.h"
#include "crypt/md5.h"
#include "resources.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "wbemuuid.lib")

#undef BYTE_ORDER	/* 1 = big-endian, -1 = little-endian, 0 = unknown */
#ifdef ARCH_IS_BIG_ENDIAN
#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
#else
#  define BYTE_ORDER 0
#endif

#define T_MASK ((md5_word_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)
typedef struct md5_state_s {
    md5_word_t count[2];
    md5_word_t abcd[4];
    md5_byte_t buf[64];
} md5_state_t;

void md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
    md5_word_t
	a = pms->abcd[0], b = pms->abcd[1],
	c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;
#if BYTE_ORDER > 0
    /* Define storage only for big-endian CPUs. */
    md5_word_t X[16];
#else
    /* Define storage for little-endian or both types of CPUs. */
    md5_word_t xbuf[16];
    const md5_word_t *X;
#endif

    {
#if BYTE_ORDER == 0
	/*
	 * Determine dynamically whether this is a big-endian or
	 * little-endian machine, since we can use a more efficient
	 * algorithm on the latter.
	 */
	static const int w = 1;

	if (*((const md5_byte_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0		/* little-endian */
	{
	    /*
	     * On little-endian machines, we can process properly aligned
	     * data without copying it.
	     */
	    if (!((data - (const md5_byte_t *)0) & 3)) {
		/* data are properly aligned */
		X = (const md5_word_t *)data;
	    } else {
		/* not aligned */
		memcpy(xbuf, data, 64);
		X = xbuf;
	    }
	}
#endif
#if BYTE_ORDER == 0
	else			/* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0		/* big-endian */
	{
	    /*
	     * On big-endian machines, we must arrange the bytes in the
	     * right order.
	     */
	    const md5_byte_t *xp = data;
	    int i;

#  if BYTE_ORDER == 0
	    X = xbuf;		/* (dynamic only) */
#  else
#    define xbuf X		/* (static only) */
#  endif
	    for (i = 0; i < 16; ++i, xp += 4)
		xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
	}
#endif
    }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s i] denote the operation
       a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + F(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  7,  T1);
    SET(d, a, b, c,  1, 12,  T2);
    SET(c, d, a, b,  2, 17,  T3);
    SET(b, c, d, a,  3, 22,  T4);
    SET(a, b, c, d,  4,  7,  T5);
    SET(d, a, b, c,  5, 12,  T6);
    SET(c, d, a, b,  6, 17,  T7);
    SET(b, c, d, a,  7, 22,  T8);
    SET(a, b, c, d,  8,  7,  T9);
    SET(d, a, b, c,  9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12,  7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET

     /* Round 2. */
     /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + G(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  1,  5, T17);
    SET(d, a, b, c,  6,  9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a,  0, 20, T20);
    SET(a, b, c, d,  5,  5, T21);
    SET(d, a, b, c, 10,  9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a,  4, 20, T24);
    SET(a, b, c, d,  9,  5, T25);
    SET(d, a, b, c, 14,  9, T26);
    SET(c, d, a, b,  3, 14, T27);
    SET(b, c, d, a,  8, 20, T28);
    SET(a, b, c, d, 13,  5, T29);
    SET(d, a, b, c,  2,  9, T30);
    SET(c, d, a, b,  7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET

     /* Round 3. */
     /* Let [abcd k s t] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + H(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  5,  4, T33);
    SET(d, a, b, c,  8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d,  1,  4, T37);
    SET(d, a, b, c,  4, 11, T38);
    SET(c, d, a, b,  7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13,  4, T41);
    SET(d, a, b, c,  0, 11, T42);
    SET(c, d, a, b,  3, 16, T43);
    SET(b, c, d, a,  6, 23, T44);
    SET(a, b, c, d,  9,  4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a,  2, 23, T48);
#undef SET

     /* Round 4. */
     /* Let [abcd k s t] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + I(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  6, T49);
    SET(d, a, b, c,  7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a,  5, 21, T52);
    SET(a, b, c, d, 12,  6, T53);
    SET(d, a, b, c,  3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a,  1, 21, T56);
    SET(a, b, c, d,  8,  6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b,  6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d,  4,  6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b,  2, 15, T63);
    SET(b, c, d, a,  9, 21, T64);
#undef SET

     /* Then perform the following additions. (That is increment each
        of the four registers by the value it had before this block
        was started.) */
    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
    pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
    pms->abcd[3] = 0x10325476;
}

void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0)
	return;

    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
	pms->count[1]++;

    /* Process an initial partial block. */
    if (offset) {
	int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

	memcpy(pms->buf + offset, p, copy);
	if (offset + copy < 64)
	    return;
	p += copy;
	left -= copy;
	md5_process(pms, pms->buf);
    }

    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64)
	md5_process(pms, p);

    /* Process a final partial block. */
    if (left)
	memcpy(pms->buf, p, left);
}

void md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    md5_byte_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
	data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64. */
    md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    /* Append the length. */
    md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
	digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

//******************************************************************************
//exemple WMI
//http://msdn.microsoft.com/en-us/library/aa394558(VS.85).aspx
//http://msdn.microsoft.com/en-us/library/aa394554(VS.85).aspx

//classes WMI
//http://msdn.microsoft.com/en-us/library/aa394084(VS.85).aspx
//http://msdn.microsoft.com/en-us/library/aa394585(VS.85).aspx

//http://msdn.microsoft.com/en-us/library/aa390423.aspx
//http://msdn.microsoft.com/en-us/library/aa286547.aspx
//******************************************************************************
#include "resources.h"
//----------------------------------------------------------------
void init(HWND hwnd)
{
  h_main            = hwnd;
  scan_start        = FALSE;
  tri_order         = FALSE;

  SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));

  CheckDlgButton(hwnd,CHK_NULL_SESSION,BST_CHECKED);
  EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),FALSE);
  EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),FALSE);
  EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),FALSE);

  //LV
  HWND hlv = GetDlgItem(hwnd,LV_results);
  LVCOLUMN lvc;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
  lvc.fmt  = LVCFMT_LEFT;

  //IP + DNS
  lvc.cx = 110;
  lvc.pszText = (LPSTR)"IP";
  ListView_InsertColumn(hlv, COL_IP, &lvc);
  lvc.cx = 110;
  lvc.pszText = (LPSTR)"DNS";
  ListView_InsertColumn(hlv, COL_DNS, &lvc);
  lvc.cx = 150;
  lvc.pszText = (LPSTR)"TTL/OS";
  ListView_InsertColumn(hlv, COL_TTL, &lvc);

  lvc.cx = 150;
  lvc.pszText = (LPSTR)"Config";
  ListView_InsertColumn(hlv, COL_CONFIG, &lvc);

  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Files";
  ListView_InsertColumn(hlv, COL_FILES, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Registry";
  ListView_InsertColumn(hlv, COL_REG, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Services";
  ListView_InsertColumn(hlv, COL_SERVICE, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Software";
  ListView_InsertColumn(hlv, COL_SOFTWARE, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"USB";
  ListView_InsertColumn(hlv, COL_USB, &lvc);
  //State
  lvc.cx = 50;
  lvc.pszText = (LPSTR)"State";
  ListView_InsertColumn(hlv, COL_STATE, &lvc);
  SendMessage(hlv,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

  //log file
  h_log = CreateFile("NS_log.txt", GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  SetFilePointer(h_log,0,0,FILE_END);
  DWORD copiee = 0;
  WriteFile(h_log,"----------------\r\n",18,&copiee,0);

  //insert test
  //SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:ARP");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:PING");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:DNS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:NetBIOS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");
/*
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Services");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Users");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Software");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:USB");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Start");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Revers SID");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:RPC");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");*/

  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Files");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Registry");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Services");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Software");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:USB");

  //critical section
  InitializeCriticalSection(&Sync);

  //load ICMP functions
  IcmpOk = FALSE;
  if ((hndlIcmp = LoadLibrary("ICMP.DLL"))!=0)
  {
    pIcmpCreateFile  = (HANDLE (WINAPI *)(void))GetProcAddress((HMODULE)hndlIcmp,"IcmpCreateFile");
    pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))GetProcAddress((HMODULE)hndlIcmp,"IcmpCloseHandle");
    pIcmpSendEcho = (DWORD (WINAPI *)(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD))	GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho");
    pIcmpSendEcho2 = (DWORD (WINAPI *)(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,IPAddr,LPVOID,WORD,PIP_OPTION_INFORMATION,LPVOID,DWORD,DWORD))	GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho2");

    if (pIcmpCreateFile!=0 && pIcmpCloseHandle!=0 && pIcmpSendEcho!=0 && pIcmpSendEcho2)
      IcmpOk = TRUE;
  }
}
//------------------------------------------------------------------------------
unsigned long int Contient(char*data,char*chaine)
{
  unsigned long int i=0;
  char *d = data;
  char *c = chaine;

  if (!*c || !*d) return 0;

  while (*d)
  {
    c = chaine;
    while (*d == *c && *c && *d){d++;c++;i++;}

    if (*c == 0) return i;
    d++;i++;
  }
  return FALSE;
}
//----------------------------------------------------------------
char *charToLowChar(char *src)
{
  unsigned int i;
  for (i=0;i<strlen(src);i++)
  {
    if (src[i]>64 && src[i]<91)src[i] = src[i]+32;
  }
  return src;
}
//----------------------------------------------------------------
int CALLBACK CompareStringTri(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
  SORT_ST *st = (SORT_ST *)lParam3;

  static char buf1[MAX_LINE_SIZE], buf2[MAX_LINE_SIZE];

  ListView_GetItemText(st->hlv, lParam1, st->col, buf1, MAX_LINE_SIZE);
  ListView_GetItemText(st->hlv, lParam2, st->col, buf2, MAX_LINE_SIZE);

  if (st->sort) return (strcmp(charToLowChar(buf1), charToLowChar(buf2)));
  else return (strcmp(charToLowChar(buf1), charToLowChar(buf2))*-1);
}
//----------------------------------------------------------------
void c_Tri(HWND hlv, unsigned short colonne_ref, BOOL sort)
{
  static SORT_ST st;
  st.hlv  = hlv;
  st.sort = sort;
  st.col  = colonne_ref;

  ListView_SortItemsEx(st.hlv,CompareStringTri, (LPARAM)&st);
}
//----------------------------------------------------------------
void AddMsg(HWND hwnd, char *type, char *txt, char *info)
{
  char msg[MAX_PATH],date[DATE_SIZE];
  time_t dateEtHMs;
  time(&dateEtHMs);
  DWORD copiee = 0;

  snprintf(date,20,"%s",(char *)ctime(&dateEtHMs));
  if (info != NULL) snprintf(msg,MAX_PATH,"[%s] %s - %s %s",date+11,type,txt,info);
  else snprintf(msg,MAX_PATH,"[%s] %s - %s",date+11,type,txt);
  SendDlgItemMessage(hwnd,CB_infos,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)msg);
  if (h_log != INVALID_HANDLE_VALUE)
  {
    strncat(msg,"\r\n\0",MAX_PATH);
    WriteFile(h_log,msg,strlen(msg),&copiee,0);
  }
}
//------------------------------------------------------------------------------
void AddLSTVUpdateItem(char *add, DWORD column, DWORD iitem)
{
  HWND hlstv  = GetDlgItem(h_main,LV_results);
  char buffer[MAX_LINE_SIZE] = "";
  ListView_GetItemText(hlstv,iitem,column,buffer,MAX_LINE_SIZE);
  if (buffer[0] != 0)
  {
    strncat(buffer,"\r\n",MAX_LINE_SIZE);
    strncat(buffer,add,MAX_LINE_SIZE);
    strncat(buffer,"\0",MAX_LINE_SIZE);
  }else
  {
    strncpy(buffer,add,MAX_LINE_SIZE);
  }
  ListView_SetItemText(hlstv,iitem,column,buffer);
}
//----------------------------------------------------------------
DWORD AddLSTVItem(char *ip, char *dns, char *ttl, char *config, char *files, char *registry, char *Services, char *software, char *USB, char *state)
{
  LVITEM lvi;
  HWND hlstv  = GetDlgItem(h_main,LV_results);
  lvi.mask      = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem  = 0;
  lvi.lParam    = LVM_SORTITEMS;
  lvi.pszText   = (LPSTR)"";
  EnterCriticalSection(&Sync);
  lvi.iItem     = ListView_GetItemCount(hlstv);
  DWORD itemPos = ListView_InsertItem(hlstv, &lvi);
  LeaveCriticalSection(&Sync);

  if(ip!=NULL)      ListView_SetItemText(hlstv,itemPos,COL_IP,ip);
  if(dns!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_DNS,dns);
  if(ttl!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_TTL,ttl);
  if(config!=NULL)  ListView_SetItemText(hlstv,itemPos,COL_CONFIG,config);
  if(files!=NULL)   ListView_SetItemText(hlstv,itemPos,COL_FILES,files);
  if(registry!=NULL)ListView_SetItemText(hlstv,itemPos,COL_REG,registry);
  if(Services!=NULL)ListView_SetItemText(hlstv,itemPos,COL_SERVICE,Services);
  if(software!=NULL)ListView_SetItemText(hlstv,itemPos,COL_SOFTWARE,software);
  if(USB!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_USB,USB);
  if(state!=NULL)   ListView_SetItemText(hlstv,itemPos,COL_STATE,state);

  return itemPos;
}
//------------------------------------------------------------------------------
void replace_one_char(char *buffer, unsigned long int taille, char chtoreplace, char chreplace)
{
  char *c = buffer;

  while (c != buffer+taille)
  {
    if (*c == chtoreplace) *c = chreplace;
    c++;
  }
}
//------------------------------------------------------------------------------
BOOL SaveLSTV(HWND hlv, char *file, unsigned int type, unsigned int nb_column)
{
  //get item count
  unsigned long int nb_items = ListView_GetItemCount(hlv);
  if (nb_items > 0 && nb_column > 0)
  {
    //open file
    HANDLE hfile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
      return FALSE;
    }

    char lines[MAX_LINE_SIZE]="", buffer[MAX_LINE_SIZE]="";
    DWORD copiee;
    unsigned long int i=0,j=0;

    LVCOLUMN lvc;
    lvc.mask        = LVCF_TEXT;
    lvc.cchTextMax  = MAX_LINE_SIZE;
    lvc.pszText     = buffer;

    switch(type)
    {
      case SAVE_TYPE_CSV:
        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }
        strncat(lines,"\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          lines[0]=0;
          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"\";");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
      break;
      case SAVE_TYPE_XML:
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        LINE_ITEM lv_line[nb_column+1];

        //title line
        for (i=0;i<nb_column;i++)
        {
          lv_line[i].c[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[i].c;
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          replace_one_char(lv_line[i].c, strlen(lv_line[i].c), ' ', '_');
        }

        //save all line
        for (j=0;j<nb_items;j++)
        {
          WriteFile(hfile," <Data>\r\n",9,&copiee,0);
          for (i=0;i<nb_column;i++)
          {
            copiee = 0;
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines,MAX_LINE_SIZE,"  <%s><![CDATA[%s]]></%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(hfile,lines,strlen(lines),&copiee,0);
            }
          }
          WriteFile(hfile," </Data>\r\n",10,&copiee,0);
        }
        WriteFile(hfile,"</RtCA>",7,&copiee,0);
      }
      break;
      case SAVE_TYPE_HTML:
      {
        char head[]="<html>\r\n <head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n <table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n  <tr bgcolor=\"#CCCCCC\">\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"  <th>%s</th>",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }

        strncat(lines,"\r\n  </tr>\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          if (j%2==1)strcpy(lines,"  <tr bgcolor=\"#ddddff\">");
          else strcpy(lines,"  <tr>");

          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td>%s</td>",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td></td>");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"</tr>\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
        WriteFile(hfile," </table>\r\n</html>",17,&copiee,0);
      }
      break;
    }
    CloseHandle(hfile);
    return TRUE;
  }else return FALSE;
}
//------------------------------------------------------------------------------
void addIPInterval(char *ip_src, char *ip_dst)
{
  //load IP interval
  char IpAdd[IP_SIZE];
  BYTE L11,L12,L13,L14,L21,L22,L23,L24;

  //get ip1
  char tmp[5]="\0\0\0\0";
  char *d = tmp;
  char *s = ip_src;

  while (*s && *s!='.')*d++ = *s++;
  if (*s == '.')
  {
    *d = 0;
    L11 = atoi(tmp);
    s++;

    d = tmp;
    while (*s && *s!='.')*d++ = *s++;
    if (*s == '.')
    {
      *d = 0;
      L12 = atoi(tmp);
      s++;

      d = tmp;
      while (*s && *s!='.')*d++ = *s++;
      if (*s == '.')
      {
        *d = 0;
        L13 = atoi(tmp);
        s++;

        d = tmp;
        while (*s && *s!='.')*d++ = *s++;
        if (*s == 0)
        {
          *d = 0;
          L14 = atoi(tmp);
        }else return;
      }else return;
    }else return;
  }else return;

  //get ip2
  d = tmp;
  s = ip_dst;

  while (*s && *s!='.')*d++ = *s++;
  if (*s == '.')
  {
    *d = 0;
    L21 = atoi(tmp);
    s++;

    d = tmp;
    while (*s && *s!='.')*d++ = *s++;
    if (*s == '.')
    {
      *d = 0;
      L22 = atoi(tmp);
      s++;

      d = tmp;
      while (*s && *s!='.')*d++ = *s++;
      if (*s == '.')
      {
        *d = 0;
        L23 = atoi(tmp);
        s++;

        d = tmp;
        while (*s && *s!='.')*d++ = *s++;
        if (*s == 0)
        {
          *d = 0;
          L24 = atoi(tmp);
        }else return;
      }else return;
    }else return;
  }else return;

  if ((L11+L12+L13+L14)!=0 && L14 >0 && L14 <= 255 && L13 <=255 && L12 <=255 && L11<255 || (L21+L22+L23+L24)!=0 && L24 >0 && L24 <= 255 && L23 <=255 && L22 <=255 && L21<255)
  {
    if (((L11+L12+L13+L14) == 0 && L21>0) || !strcmp(ip_src,ip_dst))SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_src);
    else if ((L21+L22+L23+L24) == 0 && L14 >0 && L14 <=255 && L13 <=255 && L12 <=255 && L11<255)SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_dst);
    else if (L21 >= L11 && (L22 >= L12 || L21 > L11) && (L23 >= L13 || (L22 > L12 || L21 > L11)) && (L24 >= L14 || (L23 > L13 || (L22 > L12 || L21 > L11))))
    {
      int a,b,c,d, initb,initc,initd,finb,finc,find;
      if (L14 == 0)L14 = 1;

      for (a = L11; a<=L21;a++)
      {
        if (a == L11 && a == L21)
        {
          initb = L12;
          finb = L22;
        }else if (a == L11)
        {
          initb = L12;
          finb = 255;
        }else if (a == L21)
        {
          initb = 0;
          finb = L22;
        }else
        {
          initb = 0;
          finb = 255;
        }
        for (b = initb; b<=finb;b++ )
        {
          if (b == L12 && b == L22)
          {
            initc = L13;
            finc = L23;
          }else if (b == L12)
          {
            initc = L13;
            finc = 255;
          }else if (b == L22)
          {
            initc = 0;
            finc = L23;
          }else
          {
            initc = 0;
            finc = 255;
          }

          for (c = initc; c<=finc;c++)
          {
            if (c == L13 && c == L23)
            {
              initd = L14;
              find = L24;
            }else if (c == L13)
            {
              initd = L14;
              find = 255;
            }else if (c == L23)
            {
              initd = 0;
              find = L24;
            }else
            {
              initd = 0;
              find = 255;
            }

            for (d = initd; d<=find;d++)
            {
              snprintf(IpAdd,IP_SIZE,"%d.%d.%d.%d",a,b,c,d);
              SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)IpAdd);
            }
          }
        }
      }
    }else
    {
      char msg[MAX_PATH];
      snprintf(msg,MAX_PATH,"%s->%s",ip_src,ip_dst);
      AddMsg(h_main,(char*)"ERROR",(char*)"Invalid interval",msg);
    }
  }else
  {
    char msg[MAX_PATH];
    snprintf(msg,MAX_PATH,"%s->%s",ip_src,ip_dst);
    AddMsg(h_main,(char*)"ERROR",(char*)"Invalid interval",msg);
  }
}
//------------------------------------------------------------------------------
void addIPTest(char *ip_format)
{
  if (ip_format == NULL) return;
  unsigned int size = strlen(ip_format);

  //search if '-'
  char *c = ip_format;
  while (*c && (*c !='-') && (*c != '/'))c++;
  if (*c == 0)//ip
  {
    SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_format);
  }else if (*c == '-')//ip-ip
  {
    char ip1[MAX_PATH];
    char *i1 = ip1;
    char *s = ip_format;
    while (*s && (*s!='-'))*i1++ = *s++;
    if (*s == '-')
    {
      s++;
      *i1 = 0;
      addIPInterval(ip1, s);
    }
  }else if (*c == '/')//ip/24
  {
    char ip1[MAX_PATH],ip2[MAX_PATH];
    char *i1 = ip1;
    char *s = ip_format;
    while (*s && (*s!='/'))*i1++ = *s++;
    if (*s == '/')
    {
      s++;
      *i1 = 0;

      //get interval type : /24 -> /0
      DWORD ip_tmp, interval  = pow(2, 32-atoi(s))-2;
      BYTE L11,L12,L13,L14;
      char tmp[5]="\0\0\0\0";

      char *d = tmp;
      char *s = ip1;
      while (*s && *s!='.')*d++ = *s++;
      if (*s == '.')
      {
        *d = 0;
        L11 = atoi(tmp);
        s++;

        d = tmp;
        while (*s && *s!='.')*d++ = *s++;
        if (*s == '.')
        {
          *d = 0;
          L12 = atoi(tmp);
          s++;

          d = tmp;
          while (*s && *s!='.')*d++ = *s++;
          if (*s == '.')
          {
            *d = 0;
            L13 = atoi(tmp);
            s++;

            d = tmp;
            while (*s && *s!='.')*d++ = *s++;
            if (*s == 0)
            {
              *d = 0;
              L14 = atoi(tmp);
            }else return;
          }else return;
        }else return;
      }else return;

      ip_tmp = interval + (L11 << 24) + (L12 << 16) + (L13 << 8) + (L14);
      L11 = ip_tmp >> 24;
      L12 = (ip_tmp >> 16) & 0xFF;
      L13 = (ip_tmp >> 8) & 0xFF;
      L14 = ip_tmp & 0xFF;

      snprintf(ip2,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      addIPInterval(ip1, ip2);
    }
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI load_file_ip(LPVOID lParam)
{
  //disable GUI
  EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),FALSE);
  EnableWindow(GetDlgItem(h_main,IP1),FALSE);
  EnableWindow(GetDlgItem(h_main,BT_IP_CP),FALSE);
  EnableWindow(GetDlgItem(h_main,IP2),FALSE);
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),FALSE);

  //init IP list
  SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

  //load file
  char file[LINE_SIZE]="";
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize   = sizeof(OPENFILENAME);
  ofn.hwndOwner     = h_main;
  ofn.lpstrFile     = file;
  ofn.nMaxFile      = LINE_SIZE;
  ofn.lpstrFilter   = "*.txt \0*.txt\0*.* \0*.*\0";
  ofn.nFilterIndex  = 1;
  ofn.Flags         = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_SHOWHELP;
  ofn.lpstrDefExt   = "*.*";
  if (GetOpenFileName(&ofn)==TRUE)
  {
    //load file
    HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
    if (hfile != INVALID_HANDLE_VALUE)
    {
      DWORD size      = GetFileSize(hfile,NULL);
      char *buffer    = (char *) malloc(size+1);
      if (buffer != NULL)
      {
        DWORD copiee =0;
        ReadFile(hfile, buffer, size,&copiee,0);
        if (size != copiee)AddMsg(h_main, (char*)"ERROR",(char*)"In loading file",file);

        //line by line
        char tmp[MAX_PATH];
        char *s = buffer, *d = tmp;
        while (*s)
        {
          tmp[0] = 0;
          d      = tmp;
          while(*s /*&& (d-tmp < MAX_PATH)*/ && (*s != '\r') && (*s != '\n'))*d++ = *s++;
          while(*s && ((*s == '\n') || (*s == '\r')))s++;
          *d = 0;

          if (tmp[0] != 0)
          {
            addIPTest(tmp);
          }
        }

        snprintf(tmp,LINE_SIZE,"Loaded file with %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
        AddMsg(h_main,(char*)"INFORMATION",tmp,file);
        free(buffer);
      }
    }
    CloseHandle(hfile);
  }
  //reinit GUI
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  return 0;
}

//------------------------------------------------------------------------------
void load_file_list(DWORD lsb, char *file)
{
  //init IP list
  SendDlgItemMessage(h_main,lsb,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

  //load file with on item by line to lstv
  HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    DWORD size      = GetFileSize(hfile,NULL);
    char *buffer    = (char *) malloc(size+1);
    if (buffer != NULL)
    {
      DWORD copiee =0;
      ReadFile(hfile, buffer, size,&copiee,0);
      if (size != copiee)AddMsg(h_main, (char*)"ERROR",(char*)"In loading file",file);

      //line by line
      char tmp[MAX_PATH];
      char *s = buffer, *d = tmp;
      while (*s)
      {
        tmp[0] = 0;
        d      = tmp;
        while(*s && (*s != '\r') && (*s != '\n'))*d++ = *s++;
        while(*s && ((*s == '\n') || (*s == '\r')))s++;
        *d = 0;

        if (tmp[0] != 0 && tmp[0] != '#')
        {
          SendDlgItemMessage(h_main,lsb,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)tmp);
        }
      }

      //message
      snprintf(tmp,LINE_SIZE,"Loaded file with %lu item(s)",SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
      AddMsg(h_main,(char*)"INFORMATION",tmp,file);
      free(buffer);
    }
    CloseHandle(hfile);
  }
}
//----------------------------------------------------------------
BOOL mLSBExist(DWORD lsb, char *st)
{
  char buffer[LINE_SIZE];
  if (st == NULL) return FALSE;
  if (st[0] == 0) return FALSE;
  DWORD i, nb_i = SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  for (i=0;i<nb_i;i++)
  {
    if (SendDlgItemMessage(h_main,lsb,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      if (!strcmp(buffer,st))return TRUE;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
void mAddLSTVUpdateItem(char *add, DWORD column, DWORD iitem)
{
  HWND hlstv  = GetDlgItem(h_main,LV_results);
  char buffer[MAX_LINE_SIZE] = "";
  ListView_GetItemText(hlstv,iitem,column,buffer,MAX_LINE_SIZE);
  if (buffer[0] != 0)
  {
    strncat(buffer,"\r\n",MAX_LINE_SIZE);
    strncat(buffer,add,MAX_LINE_SIZE);
    strncat(buffer,"\0",MAX_LINE_SIZE);
  }else
  {
    strncpy(buffer,add,MAX_LINE_SIZE);
  }
  ListView_SetItemText(hlstv,iitem,column,buffer);
}
//----------------------------------------------------------------
void mAddMsg(HWND hwnd, char *type, char *txt, char *info)
{
  char msg[MAX_PATH],date[DATE_SIZE];
  time_t dateEtHMs;
  time(&dateEtHMs);
  DWORD copiee = 0;

  snprintf(date,20,"%s",(char *)ctime(&dateEtHMs));
  if (info != NULL) snprintf(msg,MAX_PATH,"[%s] %s - %s %s",date+11,type,txt,info);
  else snprintf(msg,MAX_PATH,"[%s] %s - %s",date+11,type,txt);
  SendDlgItemMessage(hwnd,CB_infos,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)msg);
  if (h_log != INVALID_HANDLE_VALUE)
  {
    strncat(msg,"\r\n\0",MAX_PATH);
    WriteFile(h_log,msg,strlen(msg),&copiee,0);
  }
}
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/aa394558%28v=vs.85%29.aspx
BOOL GetWMITests(DWORD iitem, char *ip, SCANNE_ST config)
{
  //init
  //objet de connexion WMI
  IWbemLocator *pLoc = NULL;
  HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres))return FALSE;

  if(FAILED(CoInitializeSecurity( NULL,-1,NULL,NULL,
     RPC_C_AUTHN_LEVEL_DEFAULT,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE,NULL)))
  {
    CoUninitialize();
    return FALSE;
  };

  printf("[%s] -001- CoInitializeSecurity\n",ip);

  //init objet
  if(FAILED(CoCreateInstance(CLSID_WbemLocator,NULL,CLSCTX_INPROC_SERVER,
     IID_IWbemLocator, (LPVOID *) &pLoc)))
  {
    pLoc->Release();
    CoUninitialize();
    return FALSE;
  };

  printf("[%s] -002- CoCreateInstance\n",ip);

  //connexion
  IWbemServices *pSvc = NULL;
  char tmp[MAX_LINE_SIZE];

  //login
  WCHAR user[MAX_LINE_SIZE];
  if (config.domain[0] != 0)
  {
    snprintf(tmp,MAX_LINE_SIZE,"%s\\%s",config.domain,config.login);
    MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)tmp,(int)-1,(LPWSTR)user,(int)MAX_LINE_SIZE);
  }else MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.login,(int)-1,(LPWSTR)user,(int)MAX_LINE_SIZE);

  //password
  WCHAR wpassword[MAX_LINE_SIZE];
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.mdp,(int)-1,(LPWSTR)wpassword,(int)MAX_LINE_SIZE);

  //connexion
  WCHAR connexion[MAX_LINE_SIZE];
  snprintf(tmp,MAX_LINE_SIZE,"\\\\%s\\ROOT\\CIMV2",ip);
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)tmp,(int)-1,(LPWSTR)connexion,(int)MAX_LINE_SIZE);

  //connexion à la base
  if(FAILED(pLoc->ConnectServer(
          BSTR(connexion),      //emplacement           : \\myserver\root\default
          BSTR(user),           //login actuel          : domain\user
          BSTR(wpassword),       //mots de passe actuel  : mdp
          NULL,                 //langue
          0,                    //securityFlags (par défaut)
          0,                    //authorité (NTLM par défaiut)
          0,                    //context
          &pSvc)))
  {
    if(pSvc!=NULL)pSvc->Release();
    if(pLoc!=NULL)pLoc->Release();
    CoUninitialize();
    return FALSE;
  }

  printf("[%s] -003- ConnectServer\n",ip);

  //authenticiation + impersonate lvl
  typedef struct _COAUTHIDENTITY
  {
    USHORT *User;
    ULONG  UserLength;
    USHORT *Domain;
    ULONG  DomainLength;
    USHORT *Password;
    ULONG  PasswordLength;
    ULONG  Flags;
  } COAUTHIDENTITY;

  COAUTHIDENTITY *userAcct  =  NULL ;
  COAUTHIDENTITY authIdent;

  memset(&authIdent, 0, sizeof(COAUTHIDENTITY));
  authIdent.PasswordLength  = wcslen(wpassword);
  authIdent.Password        = (USHORT*)wpassword;

  WCHAR wuser[MAX_LINE_SIZE], wdomain[MAX_LINE_SIZE];
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.login,(int)-1,(LPWSTR)wuser,(int)MAX_LINE_SIZE);
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.domain,(int)-1,(LPWSTR)wdomain,(int)MAX_LINE_SIZE);
  authIdent.UserLength      = wcslen(wuser);
  authIdent.User            = (USHORT*)wuser;
  authIdent.DomainLength    = wcslen(wdomain);
  authIdent.Domain          = (USHORT*)wdomain;
  authIdent.Flags           = SEC_WINNT_AUTH_IDENTITY_UNICODE;
  userAcct                  = &authIdent;

  if(FAILED(CoSetProxyBlanket(
     pSvc,                         // the proxy to set
     RPC_C_AUTHN_WINNT,            // authentication service
     RPC_C_AUTHZ_NONE,             // authorization service
     COLE_DEFAULT_PRINCIPAL,       // Server principal name
     RPC_C_AUTHN_LEVEL_PKT_PRIVACY,// authentication level
     RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
     userAcct,                         // client identity
     EOAC_NONE                     // proxy capabilities
  )))
  {
    if(pSvc!=NULL)pSvc->Release();
    if(pLoc!=NULL)pLoc->Release();
    CoUninitialize();
    return FALSE;
  }

  printf("[%s] -004- CoSetProxyBlanket\n",ip);

  //get datas !!!
  IEnumWbemClassObject* pEnumerator;

  //services
  if (config.check_services)
  {
    pEnumerator = NULL;
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_Service"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      printf("[%s] -005- ExecQuery services\n",ip);

      IWbemClassObject *pclsObj = NULL;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        printf("[%s] -005- ExecQuery services -\n",ip);

        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        printf("[%s] -005- ExecQuery services -pEnumerator\n",ip);

        if(uReturn == 0)break;

        printf("[%s] -005- ExecQuery services -pEnumerator-ret\n",ip);

        pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        printf("[%s] -005- ExecQuery services -Get\n",ip);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);

        printf("[%s] -005- ExecQuery services : %S\n",ip,vtProp.bstrVal);

        if (mLSBExist(CB_T_SERVICES, ctmp))
        {
          pclsObj->Get(L"PathName", 0, &vtProp2, 0, 0);
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\PathName=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (Service)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
          VariantClear(&vtProp2);
        }else
        {
          pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
          snprintf(ctmp2,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
          if (mLSBExist(CB_T_SERVICES, ctmp2))
          {
            pclsObj->Get(L"PathName", 0, &vtProp2, 0, 0);
            snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\PathName=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
            mAddMsg(h_main,(char*)"FOUND (Service)",msg,ctmp2);
            mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
            VariantClear(&vtProp2);
          }
        }
        VariantClear(&vtProp);
      }
      pEnumerator->Release();
    }
  }

  //software
  if (config.check_software)
  {
    pEnumerator = NULL;
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_Product"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;

        pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_SOFTWARE, ctmp))
        {
          pclsObj->Get(L"InstallLocation", 0, &vtProp2, 0, 0);
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\InstallLocation=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (Software)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
          VariantClear(&vtProp2);
        }else
        {
          pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
          snprintf(ctmp2,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
          if (mLSBExist(CB_T_SOFTWARE, ctmp2))
          {
            pclsObj->Get(L"InstallLocation", 0, &vtProp2, 0, 0);
            snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\InstallLocation=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
            mAddMsg(h_main,(char*)"FOUND (Software)",msg,ctmp2);
            mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
            VariantClear(&vtProp2);
          }
        }
        VariantClear(&vtProp);
      }
      pEnumerator->Release();
    }

    //updates
    pEnumerator = NULL;
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_QuickFixEngineering"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;

        pclsObj->Get(L"Description", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_SOFTWARE, ctmp))
        {
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S",ip,vtProp.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (Software)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
        }
        VariantClear(&vtProp);
      }
      pEnumerator->Release();
    }
  }

  //USB
  if (config.check_USB)
  {
    pEnumerator = NULL;
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_USBController"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;

        pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_USB, ctmp))
        {
          pclsObj->Get(L"DeviceID", 0, &vtProp2, 0, 0);
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\DeviceID=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (USB)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
          VariantClear(&vtProp2);
        }else
        {
          pclsObj->Get(L"DeviceID", 0, &vtProp2, 0, 0);
          snprintf(ctmp2,MAX_LINE_SIZE,"%S",vtProp2.bstrVal);
          if (mLSBExist(CB_T_USB, ctmp2))
          {
            snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\DeviceID=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
            mAddMsg(h_main,(char*)"FOUND (USB)",msg,ctmp2);
            mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
            VariantClear(&vtProp2);
          }
        }
        VariantClear(&vtProp);
      }
      pEnumerator->Release();
    }
  }

  //clean
  if (pSvc != NULL)pSvc->Release();
  if (pLoc != NULL)pLoc->Release();
  CoUninitialize();
  return TRUE;
}
//----------------------------------------------------------------
int Ping(char *ip)
{
  int Ttl = -1;
  if (IcmpOk)
  {
    HANDLE hndlFile = pIcmpCreateFile();
    if (hndlFile != INVALID_HANDLE_VALUE)
    {
      LPHOSTENT pHost     = gethostbyname(ip);
      DWORD *dwAddress    = (DWORD *)(*pHost->h_addr_list);
      ICMPECHO icmpEcho;

      if (pIcmpSendEcho(hndlFile,*dwAddress,0,0,0,&icmpEcho,sizeof(icmpEcho),ICMP_TIMEOUT)!=0)
      {
        //existe
        if ((icmpEcho.Status==0)&&(icmpEcho.Options.Ttl>0))
        {
          Ttl = icmpEcho.Options.Ttl;
        }
      }

      pIcmpCloseHandle(hndlFile);
      return  Ttl;
    }
  }
  return Ttl;
}
//------------------------------------------------------------------------------
BOOL ResDNS(char *ip, char *name, unsigned int sz_max)
{
 name[0]=0;
 struct hostent* remoteHost;
 struct in_addr in;
 in.s_addr = inet_addr(ip);
 if ((remoteHost=gethostbyaddr((char *)&in, 4, AF_INET))!=0)
 {
   snprintf(name,sz_max,"%s",remoteHost->h_name);
   return TRUE;
 }
 return FALSE;
}
//----------------------------------------------------------------
//source : http://msdn.microsoft.com/en-us/library/windows/desktop/ms724832%28v=vs.85%29.aspx
BOOL Netbios_OS(char *ip, char*txtOS, char *name, char *domain, unsigned int sz_max)
{
  wchar_t serveur[MAX_PATH];
  char tmp[MAX_PATH];
  BOOL ret = FALSE;
  _snprintf(tmp,MAX_PATH,"\\\\%s",ip);
  //init de la chaine (pour connexion à la machine)
  mbstowcs( serveur,tmp,MAX_PATH);

  //lecture des informations NETBIOS
  WKSTA_INFO_100 *mybuff;
  NET_API_STATUS res = NetWkstaGetInfo(serveur, 100,(BYTE**)&mybuff);

  //OS
  if((res == ERROR_SUCCESS || res == ERROR_MORE_DATA) && mybuff)
  {
    ret = TRUE;
    if (name!=NULL)   snprintf(name,sz_max,"%S",serveur);
    if (domain!=NULL) snprintf(domain,sz_max,"%S",mybuff->wki100_langroup);

    //on test le type d'os et on met a jour
    switch(mybuff->wki100_ver_major)
    {
      case 4:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Windows 95/NT4",sz_max);break;
            case 10:strncpy(txtOS,"Windows 98",sz_max);break;//
            case 90:strncpy(txtOS,"Windows ME",sz_max);break;
        }
      break;
      case 5:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Windows 2K",sz_max);break;
            case 1:strncpy(txtOS,"Windows XP",sz_max);break;//
            case 2:strncpy(txtOS,"Windows 2003/XP-64b",sz_max);break;
        }
      break;
      case 6:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Windows Vista/2008\n",sz_max);break;
            case 1:strncpy(txtOS,"Windows 7/2008 R2-64b\n",sz_max);break;
            case 2:strncpy(txtOS,"Windows 8/2012\n",sz_max);break;
        }
      break;
      case 7:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Windows 7\n",sz_max);break;
        }
      break;
      default:
            _snprintf(txtOS,sz_max,"Windows [major:%d;minor:%d]",mybuff->wki100_ver_major,mybuff->wki100_ver_minor);
      break;
    }
  }
  NetApiBufferFree(mybuff);
  return ret;
}
//----------------------------------------------------------------
BOOL Netbios_NULLSession(char *ip)
{
  char tmp[MAX_PATH];
  _snprintf(tmp,MAX_PATH,"\\\\%s\\ipc$",ip);

  BOOL ret            = FALSE;
  NETRESOURCE NetRes;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = (LPSTR)"";
  NetRes.lpRemoteName	= tmp;
  NetRes.lpProvider   = (LPSTR)"";
  if (WNetAddConnection2(&NetRes,"","",0) == NO_ERROR)ret = TRUE;

  WNetCancelConnection2(tmp,CONNECT_UPDATE_PROFILE,1);
  return ret;
}
//----------------------------------------------------------------
BOOL Netbios_Time(wchar_t *server, char *time, unsigned int sz_max)
{
  TIME_OF_DAY_INFO *timep;
  BOOL ret = FALSE;

  if (NetRemoteTOD(server,(BYTE**)&timep) == NERR_Success)
  {
    if (timep)
    {
      snprintf(time,sz_max, "%d/%02d/%02d %d:%02d:%02d",timep->tod_year,timep->tod_month,timep->tod_day,timep->tod_hours, timep->tod_mins, timep->tod_secs);
      ret = TRUE;
    }
    NetApiBufferFree(timep);
  }
  return ret;
}
//----------------------------------------------------------------
BOOL Netbios_Share(wchar_t *server, char *share, unsigned int sz_max)
{
  BOOL ret = FALSE;
  NET_API_STATUS res;
  PSHARE_INFO_1 BufPtr,p;
  DWORD i, er=0,tr=0,resume=0;
  char tmp[MAX_PATH];
  share[0] = 0;

  do
  {
    res = NetShareEnum (server, 1, (LPBYTE *) &BufPtr, -1, &er, &tr, &resume);
    if((res == ERROR_SUCCESS || res == ERROR_MORE_DATA) && BufPtr!=NULL)
    {
      ret = TRUE;
      p   = BufPtr;

      for(i=1;i<=er;i++)
      {
        snprintf(tmp,MAX_PATH,"%S (%S)\r\n",p->shi1_netname,p->shi1_remark);
        strncat(share,tmp,sz_max);
        p++;
      }
     NetApiBufferFree(BufPtr);
    }
  }while(res==ERROR_MORE_DATA);
  strncat(share,"\0",sz_max);

  return ret;
}
//----------------------------------------------------------------
BOOL TestReversSID(char *ip, char* user)
{
  UCHAR domain[MAX_PATH];
  UCHAR BSid[MAX_PATH];
  SID_NAME_USE peUse;

  PSID Sid        = (PSID) BSid;
  DWORD sz_Sid    = MAX_PATH;
  DWORD sz_domain = MAX_PATH;

  if (LookupAccountName((LPSTR)ip,(LPSTR)user, (PSID)Sid, &sz_Sid,(LPSTR)domain, &sz_domain,&peUse))return TRUE;
  else return FALSE;
}
//------------------------------------------------------------------------------
DWORD ReadValue(HKEY hk,char *path,char *value,void *data, DWORD data_size)
{
  DWORD data_size_read = 0;
  HKEY CleTmp=0;

  //open key
  if (RegOpenKey(hk,path,&CleTmp)!=ERROR_SUCCESS)
     return FALSE;

  //size of data
  if (RegQueryValueEx(CleTmp, value, 0, 0, 0, &data_size_read)!=ERROR_SUCCESS)
     return FALSE;

  //alloc
  char *c = (char *)malloc(data_size_read+1);
  if (c == NULL)return FALSE;

  //read value
  if (RegQueryValueEx(CleTmp, value, 0, 0, (LPBYTE)c, &data_size_read)!=ERROR_SUCCESS)
    return FALSE;

  if (data_size_read<data_size) memcpy(data,c,data_size_read);
  else memcpy(data,c,data_size);

  //free + close
  free(c);
  RegCloseKey(CleTmp);
  return data_size_read;
}
//----------------------------------------------------------------
BOOL parseLineToReg(char *line, REG_LINE_ST *reg_st)
{
  //line format :"SYSTEM\\CurrentControlSet\\Services\\";"value";"data";"format";"check";
  //get path
  strncpy(reg_st->path,line+1,LINE_SIZE);
  char *s = reg_st->path;
  while (*s && *s != '\"')s++;
  if (*s != '\"')return FALSE;

  strncpy(reg_st->value,s+3,LINE_SIZE);
  *s = 0;
  if (strlen(reg_st->path) == 0) return FALSE;

  //get value
  s = reg_st->value;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  strncpy(reg_st->data,s+3,LINE_SIZE);
  *s = 0;

  //get data
  s = reg_st->data;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  char tmp_format[LINE_SIZE];
  strncpy(tmp_format,s+3,LINE_SIZE);
  *s = 0;

  //get format
  s = tmp_format;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  char tmp_check[LINE_SIZE];
  strncpy(tmp_check,s+3,LINE_SIZE);
  *s = 0;

  //word or dword
  if (tmp_format[0] == 'D' || tmp_format[0] == 'd' || tmp_format[0] == 'W' || tmp_format[0] == 'w')
  {
    reg_st->data_dword  = TRUE;
    reg_st->data_string = FALSE;
  }else
  {
    reg_st->data_dword  = FALSE;
    reg_st->data_string = TRUE;
  }

  //check
  reg_st->check_equal     = FALSE;
  reg_st->check_inf       = FALSE;
  reg_st->check_sup       = FALSE;
  reg_st->check_diff      = FALSE;
  reg_st->check_content   = FALSE;
  reg_st->check_no_data   = FALSE;
  reg_st->check_no_value  = FALSE;

  if (reg_st->data_dword)
  {
    switch(tmp_check[0])
    {
      case '=':reg_st->check_equal    = TRUE;break;
      case '<':reg_st->check_inf      = TRUE;break;
      case '>':reg_st->check_sup      = TRUE;break;
      case '!':reg_st->check_diff     = TRUE;break;
      case '*':reg_st->check_no_data  = TRUE;break;
      case '\"':
      case ' ':
      default:reg_st->check_no_value  = TRUE;break;
    }
  }else
  {
    switch(tmp_check[0])
    {
      case '=':reg_st->check_equal    = TRUE;break;
      case '!':reg_st->check_diff     = TRUE;break;
      case '?':reg_st->check_content  = TRUE;break;
      case '*':reg_st->check_no_data  = TRUE;break;
      case '\"':
      case ' ':
      default:reg_st->check_no_value  = TRUE;break;
    }
  }

  return TRUE;
}
//----------------------------------------------------------------
void RegistryScan(DWORD iitem,char *ip, HKEY hkey)
{
  //get datas to check
  char buffer[LINE_SIZE];
  DWORD i, nb_i = SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  char msg[LINE_SIZE];
  DWORD key_data_dw;
  HKEY CleTmp;
  REG_LINE_ST reg_st;
  DWORD dw_datas, ok;
  char ch_datas[LINE_SIZE];
  unsigned int key_data_type, key_src_data_type;

  for (i=0;i<nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      //for title line add # after the "
      //line format :"SYSTEM\\CurrentControlSet\\Services\\";"value";"data";"format";"check";
      //data format : string/dword
      //check format: =<>!*
      if (parseLineToReg(buffer,&reg_st))
      {
        if (RegOpenKey(hkey,reg_st.path,&CleTmp)==ERROR_SUCCESS)
        {
          if (reg_st.check_no_value)
          {
            snprintf(msg,LINE_SIZE,"%s\\%s",ip,reg_st.path);
            AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Path Exist");
            AddLSTVUpdateItem(msg, COL_REG, iitem);
          }else
          {
            ok          = 0;
            dw_datas    = 0;
            ch_datas[0] = 0;

            if (reg_st.data_dword)ok = ReadValue(hkey,reg_st.path,reg_st.value,&dw_datas, sizeof(DWORD));
            else ok = ReadValue(hkey,reg_st.path,reg_st.value,ch_datas, LINE_SIZE);
            if (!ok)continue;

            //check
            if (reg_st.check_no_data)
            {
              if (reg_st.data_dword)snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
              else snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);

              AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Exist");
              AddLSTVUpdateItem(msg, COL_REG, iitem);
            }else if (reg_st.check_equal)
            {
              if (reg_st.data_dword)
              {
                if (atol(reg_st.data) == dw_datas)
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                  AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }else if (reg_st.data_string)
              {
                if (!strcmp(reg_st.data,ch_datas))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);
                  AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }
            }else if (reg_st.check_diff)
            {
              if (reg_st.data_dword)
              {
                if (atol(reg_st.data) != dw_datas)
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                  AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }else if (reg_st.data_string)
              {
                if (strcmp(reg_st.data,ch_datas))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);
                  AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }
            }else if (reg_st.check_inf)
            {
              if (atol(reg_st.data) > dw_datas)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }else if (reg_st.check_sup)
            {
              if (atol(reg_st.data) < dw_datas)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }else if (reg_st.check_content)
            {
              if (Contient(ch_datas,reg_st.data))
              {
                snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Value Data Exist");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }
          }
          RegCloseKey(CleTmp);
        }
      }
    }
  }
}
//----------------------------------------------------------------
BOOL LSBExist(DWORD lsb, char *st)
{
  char buffer[LINE_SIZE];
  if (st == NULL) return FALSE;
  if (st[0] == 0) return FALSE;
  DWORD i, nb_i = SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  for (i=0;i<nb_i;i++)
  {
    if (SendDlgItemMessage(h_main,lsb,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      if (!strcmp(buffer,st))return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
void RegistryServiceScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Services");
  HKEY CleTmp;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,nbSubKey = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key_path[LINE_SIZE],name[LINE_SIZE],msg[LINE_SIZE],ImagePath[LINE_SIZE];
      DWORD key_size;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistryServiceScan:RegEnumKeyEx=OK",ip);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,(char*)"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                AddMsg(h_main,(char*)"FOUND (Service)",key_path,key);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }else if (LSBExist(CB_T_SERVICES, name))
            {
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,name);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                AddMsg(h_main,(char*)"FOUND (Service)",key_path,name);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                AddMsg(h_main,(char*)"FOUND (Service)",key_path,key);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
void RegistrySoftwareScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Softwares");
  HKEY CleTmp;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,nbSubKey = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key_path[LINE_SIZE],name[LINE_SIZE],msg[LINE_SIZE],ImagePath[LINE_SIZE];
      DWORD key_size;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistrySoftwareScan:RegEnumKeyEx2=OK",ip);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,(char*)"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sinstalllocation=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sInstallSource=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sUninstallString=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                AddMsg(h_main,(char*)"FOUND (Software)",key_path,key);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }else if (LSBExist(CB_T_SOFTWARE, name))
            {
              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sinstalllocation=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sInstallSource=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sUninstallString=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                AddMsg(h_main,(char*)"FOUND (Software)",key_path,name);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sinstalllocation=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                AddMsg(h_main,(char*)"FOUND (Software)",key_path,key);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
void RegistryUSBScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"USB");
  HKEY CleTmp,CleTmp2;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,j,nbSubKey = 0, nbSubKey2;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key2[LINE_SIZE],key_path[LINE_SIZE],key_path2[LINE_SIZE],msg[LINE_SIZE];
      DWORD key_size, key_size2;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistryUSBScan:RegEnumKeyEx=OK",ip);
          #endif
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (RegOpenKey(hkey,key_path,&CleTmp2)!=ERROR_SUCCESS)continue;

          nbSubKey2 = 0;
          if (RegQueryInfoKey (CleTmp2,0,0,0,&nbSubKey2,0,0,0,0,0,0,0)==ERROR_SUCCESS)
          {
            for (j=0;j<nbSubKey2 && scan_start;j++)
            {
              key_size2 = LINE_SIZE;
              key2[0]   = 0;
              if (RegEnumKeyEx (CleTmp2,j,key2,&key_size2,0,0,0,0)==ERROR_SUCCESS)
              {
                snprintf(key_path2,LINE_SIZE,"%s%s",key_path,key2);
                if (LSBExist(CB_T_USB, key))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s",ip,key_path,key);
                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }else if (LSBExist(CB_T_USB, key2))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s",ip,key_path,key2);
                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key2);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }
              }
            }
          }
          RegCloseKey(CleTmp2);
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
BOOL NetConnexionAuthenticate(char *ip, char*remote_name, SCANNE_ST config)
{
  char user_netbios[LINE_SIZE] = "";
  if (config.domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config.domain,config.login);
  else snprintf(user_netbios,LINE_SIZE,"%s",config.login);

  char tmp_connect[LINE_SIZE];
  remote_name[0] = 0;
  snprintf(tmp_connect,LINE_SIZE,"\\\\%s\\ipc$",ip);

  NETRESOURCE NetRes;
  NetRes.dwScope      = RESOURCE_GLOBALNET;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = (LPSTR)"";
  NetRes.lpProvider   = (LPSTR)"";
  NetRes.lpRemoteName	= remote_name;
  if (WNetAddConnection2(&NetRes,config.mdp,user_netbios,CONNECT_PROMPT)==NO_ERROR)
  {
    char msg[LINE_SIZE];
    snprintf(msg,LINE_SIZE,"Login in %s IP with %s account.",ip,user_netbios);
    AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");

    return TRUE;//CONNECT_UPDATE_PROFILE
  }else
  {
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
    #endif
    return FALSE;
  }

  //for close : WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
  return FALSE;
}
//----------------------------------------------------------------
HANDLE UserConnect(char *ip,SCANNE_ST config)
{
  HANDLE htoken = NULL;
  if (LogonUser(config.login, config.domain, config.mdp, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &htoken))
  {
    ImpersonateLoggedOnUser(htoken);
    #ifdef DEBUG_MODE
    if (htoken == 0)AddMsg(h_main,"DEBUG","registry:Authent:UserConnect=FAIL",ip);
    #endif
    return htoken;
  }else return FALSE;
}
//----------------------------------------------------------------
void UserDisConnect(HANDLE htoken)
{
  CloseHandle(htoken);
}
//----------------------------------------------------------------
BOOL RemoteRegistryNetConnexion(DWORD iitem,char *name, char *ip, SCANNE_ST config)
{
  BOOL ret            = FALSE;
  char tmp[MAX_PATH]  = "", remote_name[MAX_PATH]  = "", msg[LINE_SIZE];
  BOOL connect = NetConnexionAuthenticate(ip, remote_name,config);

  //if(NetConnexionAuthenticate(ip, remote_name))
  //{
    //net
    HKEY hkey;
    snprintf(tmp,MAX_PATH,"\\\\%s",name);
    if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
    {
      if (connect)
      {
        snprintf(msg,LINE_SIZE,"Login (NET) in %s IP with %s account.",ip,config.login);
        AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
      }else
      {
        snprintf(msg,LINE_SIZE,"Login (NET) in %s IP with NULL session account.",ip);
        AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
      }

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:RemoteRegistryNetConnexion:RegConnectRegistry=OK",ip);
      #endif
      //work
      if (config.check_registry && scan_start)RegistryScan(iitem,ip,hkey);
      if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey);
      if (config.check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

      RegCloseKey(hkey);
      ret = TRUE;
    }
    if(connect)WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
  //}
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteRegistryImpersonateConnexion(DWORD iitem, char *name, char *ip, SCANNE_ST config)
{
  BOOL ret            = FALSE;
  HANDLE htoken = UserConnect(ip,config);
  if (htoken != NULL)
  {
    char msg[LINE_SIZE];
    snprintf(msg,LINE_SIZE,"Login (LogonUser) in %s IP with %s\\%s account.",ip,config.domain,config.login);
    AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");

    HKEY hkey;
    char tmp[MAX_PATH] = "";
    snprintf(tmp,MAX_PATH,"\\\\%s",name);
    if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
    {
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:RemoteRegistryImpersonateConnexion:RegConnectRegistry=OK",ip);
      #endif
      //work
      if (config.check_registry && scan_start)RegistryScan(iitem,ip,hkey);
      if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey);
      if (config.check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

      RegCloseKey(hkey);
      ret = TRUE;
    }
    UserDisConnect(htoken);
  }
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteRegistrySimpleConnexion(DWORD iitem, char *name, char *ip, SCANNE_ST config)
{
  BOOL ret            = FALSE;
  HKEY hkey;
  char tmp[MAX_PATH]  = "";
  snprintf(tmp,MAX_PATH,"\\\\%s",name);

  if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
  {
    char msg[LINE_SIZE];
    snprintf(msg,LINE_SIZE,"Login (direct User) in %s IP with NULL session account.",ip);
    AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");

    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","registry:RemoteRegistrySimpleConnexion:RegConnectRegistry=OK",ip);
    #endif
    //work
    if (config.check_registry && scan_start)RegistryScan(iitem,ip,hkey);
    if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey);
    if (config.check_software && scan_start)
    {
      RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
    }
    if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

    RegCloseKey(hkey);
    ret = TRUE;
  }
  return ret;
}
//----------------------------------------------------------------
//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/iphlp/iphlp/sendarp.asp
BOOL __cdecl ARP(char * ip,char *mac_dst)
{
    //variables
    IPAddr  ipAddr;
    ULONG   pulMac[2];
    ULONG   ulLen;

    //transformation pour traitement de l'adresse ip
    ipAddr = inet_addr(ip);

    //init
    memset (pulMac, 0xff, sizeof (pulMac));
    ulLen = 6;

    //on récupère l'adresse mac ici si possible (si après un routage type internet = impossible)
    if (SendARP (ipAddr, 0, pulMac, &ulLen)==NO_ERROR)
    {
      PBYTE pbHexMac = (PBYTE) pulMac;
      snprintf (mac_dst,18,"%02X:%02X:%02X:%02X:%02X:%02X",pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5],pbHexMac[6]);
      return 1;
    }
    return 0;
}
//----------------------------------------------------------------
BOOL RemoteConnexionScan(DWORD iitem, char *name, char *ip, SCANNE_ST config)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","registry:RemoteConnexionScan",ip);
  #endif
  if (RemoteRegistryNetConnexion(iitem, name, ip, config)) return TRUE;
  else if (RemoteRegistryImpersonateConnexion(iitem, name, ip, config)) return TRUE;
  else
  {
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_REG,(LPSTR)"CONNEXION FAIL!");
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_SERVICE,(LPSTR)"CONNEXION FAIL!");
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_SOFTWARE,(LPSTR)"CONNEXION FAIL!");
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_USB,(LPSTR)"CONNEXION FAIL!");
  }
  //else if (RemoteRegistrySimpleConnexion(iitem, name, ip, config)) return TRUE;
  return FALSE;
}

//----------------------------------------------------------------
void FileToMd5(HANDLE Hfic, char *md5)
{
  //ouverture du fichier en lecture partagé
  md5[0]=0;
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
  {
    unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
    if (buffer == NULL)return;

    //lecture du fichier
    DWORD copiee, position = 0, increm = 0;
    if (taille_fic > DIXM)increm = DIXM;
    else increm = taille_fic;
    BOOL read = TRUE;
    while (position<taille_fic && increm!=0 && read)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
    {
      copiee = 0;
      read =ReadFile(Hfic, buffer+position, increm,&copiee,0);
      position +=copiee;
      if (taille_fic-position < increm)increm = taille_fic-position ;
    }

    //traitement en MD5
    md5_state_t     state;
    md5_byte_t      digest[16];
    md5_init(&state);
    md5_append(&state,(const md5_byte_t  *)buffer,taille_fic);
    md5_finish(&state,digest);

    //génération du md5 en chaine
    unsigned short i;
    for(i=0;i<16;i++)snprintf(md5+i*2,3,"%02X",digest[i]);
    md5[32]=0;
    HeapFree(GetProcessHeap(), 0,buffer);
  }
}
//----------------------------------------------------------------
void FileToSHA256(HANDLE Hfic, char *csha256)
{
  //ouverture du fichier en lecture partagé
  csha256[0]=0;
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
  {
    unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
    if (buffer == NULL)return;

    //lecture du fichier
    DWORD copiee, position = 0, increm = 0;
    if (taille_fic > DIXM)increm = DIXM;
    else increm = taille_fic;

    BOOL read = TRUE;
    while (position<taille_fic && increm!=0 && read)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
    {
      copiee = 0;
      read = ReadFile(Hfic, buffer+position, increm,&copiee,0);
      position +=copiee;
      if (taille_fic-position < increm)increm = taille_fic-position ;
    }

    //traitement en SHA256
    sha256_ctx    m_sha256;
    unsigned char digest[32];

    sha256_begin(&m_sha256);
    sha256_hash(buffer, taille_fic, &m_sha256);
    sha256_end(digest, &m_sha256);

    //génération du SHA256 en chaine
    unsigned short i;
    for(i=0;i<32;i++)snprintf(csha256+i*2,3,"%02x",digest[i]&0xFF);
    csha256[64]=0;
    HeapFree(GetProcessHeap(), 0,buffer);
  }
}

//----------------------------------------------------------------            //MD5
BOOL RemoteAuthenticationFilesScan(DWORD iitem, char *ip, char *remote_share, SCANNE_ST config)
{
  //check file
  char remote_name[LINE_SIZE], msg[LINE_SIZE];
  snprintf(remote_name,LINE_SIZE,"\\\\%s\\%s",ip,remote_share);

  NETRESOURCE NetRes  = {0};
  NetRes.dwScope      = RESOURCE_GLOBALNET;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = (LPSTR)"";
  NetRes.lpProvider   = (LPSTR)"";
  NetRes.lpRemoteName	= remote_name;

  if (WNetAddConnection2(&NetRes,config.mdp,config.mdp,CONNECT_PROMPT)==NO_ERROR)
  //if (WNetAddConnection2(&NetRes,config.mdp,config.mdp,CONNECT_TEMPORARY)==NO_ERROR)
  {
    snprintf(msg,LINE_SIZE,"Login (NET) in %s IP with %s account.",ip,config.login);
    AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");

    //check file
    char tmp_path[LINE_SIZE], file[LINE_SIZE], s_sha[SHA256_SIZE]="",s_md5[MAX_PATH];
    DWORD i, nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
    for (i=0;i<nb_i && scan_start;i++)
    {
      if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)i,(LPARAM)file))
      {
        snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","File:check",tmp_path);
        #endif
        HANDLE hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
        if (hfile != INVALID_HANDLE_VALUE)
        {
            //MD5
            AddMsg(h_main,(char*)"FOUND (File)",tmp_path,(char*)"CHECK MD5");
            FileToMd5(hfile, s_md5);
            CloseHandle(hfile);

            //SHA256
            hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              AddMsg(h_main,(char*)"FOUND (File)",tmp_path,(char*)"CHECK SHA256");
              FileToSHA256(hfile, s_sha);
              CloseHandle(hfile);
            }

            if (s_sha[0] != 0 && s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;SHA256;%s",remote_name,file,s_md5,s_sha);
            }else if (s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;;",remote_name,file,s_md5);
            }else if (s_sha[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;;;SHA256;%s",remote_name,file,s_sha);
            }

            AddMsg(h_main,(char*)"FOUND (File)",tmp_path,(char*)"");
            AddLSTVUpdateItem(tmp_path, COL_FILES, iitem);
        }
      }
    }
    WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    return TRUE;
  }else
  {
    HANDLE htoken = (HANDLE)UserConnect(ip,config);
    if (htoken != NULL)
    {
      snprintf(msg,LINE_SIZE,"Login (LogonUser) in %s IP with %s\\%s account.",ip,config.domain,config.login);
      AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");

      //check file
      char tmp_path[LINE_SIZE], file[LINE_SIZE], s_sha[SHA256_SIZE]="",s_md5[MAX_PATH];
      DWORD i, nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
      for (i=0;i<nb_i && scan_start;i++)
      {
        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)i,(LPARAM)file))
        {
          snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","File:check",tmp_path);
          #endif
          HANDLE hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
          if (hfile != INVALID_HANDLE_VALUE)
          {
            //MD5
            AddMsg(h_main,(char*)"FOUND (File)",tmp_path,(char*)"CHECK MD5");
            FileToMd5(hfile, s_md5);
            CloseHandle(hfile);

            //SHA256
            hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              AddMsg(h_main,(char*)"FOUND (File)",tmp_path,(char*)"CHECK SHA256");
              FileToSHA256(hfile, s_sha);
              CloseHandle(hfile);
            }

            if (s_sha[0] != 0 && s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;SHA256;%s",remote_name,file,s_md5,s_sha);
            }else if (s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;;",remote_name,file,s_md5);
            }else if (s_sha[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;;;SHA256;%s",remote_name,file,s_sha);
            }

            AddMsg(h_main,(char*)"FOUND (File)",tmp_path,(char*)"");
            AddLSTVUpdateItem(tmp_path, COL_FILES, iitem);
          }
        }
      }
      UserDisConnect(htoken);
      return TRUE;
    }else
    {
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","files:RemoteAuthenticationFilesScan:UserConnect=FAIL",ip);
      #endif
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
BOOL RemoteConnexionFilesScan(DWORD iitem,char *name, char *ip, SCANNE_ST config)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","files:RemoteConnexionFilesScan",ip);
  #endif
  BOOL ret = FALSE;

  if(RemoteAuthenticationFilesScan(iitem, ip, (char*)"C$", config))
  {
    RemoteAuthenticationFilesScan(iitem, ip, (char*)"D$", config);
    return TRUE;
  }else ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_FILES,(LPSTR)"CONNEXION FAIL!");

  return FALSE;
}
//----------------------------------------------------------------
DWORD WINAPI ScanIp(LPVOID lParam)
{
  DWORD index = (DWORD)lParam;
  char ip[MAX_PATH]="", ip_mac[MAX_PATH]="", dns[MAX_PATH]="", ttl_os[MAX_PATH]="", cfg[MAX_PATH]="";
  DWORD iitem = 0;
  BOOL exist  = FALSE, dnsok = FALSE, netBIOS = FALSE;

  SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)index,(LPARAM)ip);

  if (ip[0]!=0)
  {
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","SCAN:BEGIN",ip);
    #endif
    //check if exist + NetBIOS
    if (config.disco_arp||config.disco_icmp||config.disco_dns||config.disco_netbios)
    {
      WaitForSingleObject(hs_disco,0);

      //ICMP
      int ttl = -1;
      if (config.disco_icmp && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ICMP:BEGIN",ip);
        #endif
        ttl = Ping(ip);
        if (ttl > -1)
        {
          exist = TRUE;

          if (ttl <= MACH_LINUX)snprintf(ttl_os,MAX_PATH,"TTL:%d (Linux?)",ttl);
          else if (ttl <= MACH_WINDOWS)snprintf(ttl_os,MAX_PATH,"TTL:%d (Windows?)",ttl);
          else if (ttl <= MACH_WINDOWS)snprintf(ttl_os,MAX_PATH,"TTL:%d (Router?)",ttl);

          iitem = AddLSTVItem(ip, NULL, ttl_os, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ICMP:END",ip);
        #endif
      }

      //ARP
      /*if (config.disco_arp)
      {
        if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"ARP");
        char mac[18]="";
        if (ARP(ip,mac))
        {
          snprintf(ip_mac,MAX_PATH,"%s [%s]",ip,mac);
          if (!exist)
          {
            iitem = AddLSTVItem(ip_mac, NULL, "Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_IP,ip_mac);
            exist = TRUE;
          }
        }
      }*/

      //DNS
      if (config.disco_dns && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:BEGIN",ip);
        #endif
        if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"DNS");
        if(ResDNS(ip, dns, MAX_PATH))
        {
          if (!exist)
          {
            iitem = AddLSTVItem(ip, dns, (char*)"Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            dnsok = TRUE;
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
            exist = TRUE;
            dnsok = TRUE;
          }
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:END",ip);
        #endif
      }

      //NetBIOS
      if ((exist || dnsok || !config.disco_dns) && config.disco_netbios && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","NetBIOS:BEGIN",ip);
        #endif
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"NetBIOS");
        WaitForSingleObject(hs_netbios,0);
        char domain[MAX_PATH] = "";
        char os[MAX_PATH]     = "";

        if (scan_start)
        {
          if (dns[0] == 0) Netbios_OS(ip, os, dns, domain, MAX_PATH);
          else Netbios_OS(ip, os, NULL, domain, MAX_PATH);
        }

        if (os[0] != 0)
        {
          if (ttl > -1)snprintf(ttl_os,MAX_PATH,"TTL:%d (%s)",ttl,os);
          else snprintf(ttl_os,MAX_PATH,"Firewall (%s)",os);

          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_TTL,ttl_os);
          netBIOS = TRUE;
        }

        char tmp[MAX_PATH] = "";
        if (domain[0] != 0)
        {
          snprintf(tmp,MAX_PATH,"Domain: %s\r\n\0",domain);
          strncat(cfg,tmp,MAX_PATH);
          netBIOS = TRUE;
        }

        //NULL session
        if (scan_start)
        {
          if(Netbios_NULLSession(ip))
          {
            strncat(cfg,"NULL Session: Enable\r\n\0",MAX_PATH);
            //ReversSID (only administrator + guest + defaults account test)
            if(TestReversSID(ip,(char*)"invité"))            strncat(cfg,(char*)"Revers SID: Enable (OK with \"invité\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,(char*)"guest"))        strncat(cfg,(char*)"Revers SID: Enable (OK with \"guest\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,(char*)"HelpAssistant"))strncat(cfg,(char*)"Revers SID: Enable (OK with \"HelpAssistant\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,(char*)"ASPNET"))       strncat(cfg,(char*)"Revers SID: Enable (OK with \"ASPNET\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,(char*)"administrateur"))strncat(cfg,(char*)"Revers SID: Enable (OK with \"administrateur\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,(char*)"administrator"))strncat(cfg,(char*)"Revers SID: Enable (OK with \"administrator\" account)\r\n\0",MAX_PATH);
            netBIOS = TRUE;
          }
        }

        if (scan_start)
        {
          wchar_t server[MAX_PATH];
          char c_time[MAX_PATH]="";
          snprintf(tmp,MAX_PATH,"\\\\%s",ip);
          mbstowcs(server, tmp, MAX_PATH);
          Netbios_Time(server, c_time, MAX_PATH);
          if (c_time[0] != 0)
          {
            snprintf(tmp,MAX_PATH,"Time: %s\r\n\0",c_time);
            strncat(cfg,tmp,MAX_PATH);
            netBIOS = TRUE;
          }

          //Share
          if (scan_start)
          {
            char shares[MAX_PATH]="";
            Netbios_Share(server, shares, MAX_PATH);
            if (shares[0] != 0)
            {
              snprintf(tmp,MAX_PATH,"Share:\r\n%s\0",shares);
              strncat(cfg,tmp,MAX_PATH);
              netBIOS = TRUE;
            }
          }
        }

        if (cfg[0] != 0)
        {
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_CONFIG,cfg);
        }

        ReleaseSemaphore(hs_netbios,1,NULL);
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","NetBIOS:END",ip);
        #endif
      }
      ReleaseSemaphore(hs_disco,1,NULL);
    }

    //if (exist&& scan_start) GetWMITests(iitem, ip, config);

    if((exist || netBIOS) && scan_start)
    {
      //registry
      BOOL remote_con = FALSE;
      if (config.check_registry || config.check_services || config.check_software || config.check_USB)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","registry:BEGIN",ip);
        #endif
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Registry");
        WaitForSingleObject(hs_registry,0);
        remote_con = RemoteConnexionScan(iitem, dns, ip, config);
        ReleaseSemaphore(hs_registry,1,NULL);
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","registry:END",ip);
        #endif
      }

      //files
      if (config.check_files && scan_start)
      {

        //if ((remote_con && (config.check_registry || config.check_services || config.check_software || config.check_USB)) ||
        //    (!(config.check_registry || config.check_services || config.check_software || config.check_USB) && !remote_con))
        //{
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","files:BEGIN",ip);
          #endif

          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files");
          WaitForSingleObject(hs_file,0);
          if (!RemoteConnexionFilesScan(iitem, dns, ip, config))ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_FILES,(LPSTR)"CONNEXION FAIL!");
          ReleaseSemaphore(hs_file,1,NULL);

          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","files:END",ip);
          #endif
        //}else ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_FILES,"CONNEXION DENY!");
      }

      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"OK");
    }
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","SCAN:END",ip);
    #endif
  }
  ReleaseSemaphore(hs_threads,1,NULL);
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI scan(LPVOID lParam)
{
  //load IP
  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    //load IP interval
    SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

    DWORD LIp1,LIp2;
    BYTE L11,L12,L13,L14,L21,L22,L23,L24;
    SendDlgItemMessage(h_main,IP1, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp1);
    SendDlgItemMessage(h_main,IP2, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp2);

    L11 = LIp1 >> 24;
    L12 = (LIp1 >> 16) & 0xFF;
    L13 = (LIp1 >> 8) & 0xFF;
    L14 = LIp1 & 0xFF;

    L21 = LIp2 >> 24;
    L22 = (LIp2 >> 16) & 0xFF;
    L23 = (LIp2 >> 8) & 0xFF;
    L24 = LIp2 & 0xFF;

    char sip1[IP_SIZE]="",sip2[IP_SIZE]="";
    snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
    snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);

    addIPInterval(sip1, sip2);
  }

  char tmp[MAX_PATH];
  snprintf(tmp,LINE_SIZE,"Loaded %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
  AddMsg(h_main,(char*)"INFORMATION",tmp,(char*)"");

  //load config
  unsigned int ref = 0;
  config.disco_arp          = 0;//SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)0,(LPARAM)NULL);
  config.disco_icmp         = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_dns          = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_netbios      = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  ref++;
  /*config.config_service     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_user        = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_software    = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_USB         = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_start       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_revers_SID  = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_RPC         = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  ref++;*/
  config.check_files        = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_registry     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_services     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_software     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_USB          = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);

  //load files
  if (config.check_files)     load_file_list(CB_T_FILES,     (char*)DEFAULT_LIST_FILES);
  if (config.check_registry)  load_file_list(CB_T_REGISTRY,  (char*)DEFAULT_LIST_REGISTRY);
  if (config.check_services)  load_file_list(CB_T_SERVICES,  (char*)DEFAULT_LIST_SERVICES);
  if (config.check_software)  load_file_list(CB_T_SOFTWARE,  (char*)DEFAULT_LIST_SOFTWARE);
  if (config.check_USB)       load_file_list(CB_T_USB,       (char*)DEFAULT_LIST_USB);

  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    config.local_account   = FALSE;
    GetWindowText(GetDlgItem(h_main,ED_NET_LOGIN),config.login,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_PASSWORD),config.mdp,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_DOMAIN),config.domain,MAX_PATH);

    /*AddMsg(h_main,"AUTHENT","get config.login",config.login);
    AddMsg(h_main,"AUTHENT","get config.mdp",config.mdp);
    AddMsg(h_main,"AUTHENT","get config.domain",config.domain);*/
  }else
  {
    config.local_account = TRUE;
    config.login[0]      = 0;
    config.domain[0]     = 0;
    config.mdp[0]        = 0;
  }

  //---------------------------------------------
  //scan_start
  DWORD i, nb_i = SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  char ip_test[MAX_PATH], test_title[MAX_PATH];

  //threads
  hs_threads  = CreateSemaphore(NULL,NB_MAX_THREAD,NB_MAX_THREAD,NULL);
  hs_disco    = CreateSemaphore(NULL,NB_MAX_DISCO_THREADS,NB_MAX_DISCO_THREADS,NULL);
  hs_netbios  = CreateSemaphore(NULL,NB_MAX_NETBIOS_THREADS,NB_MAX_NETBIOS_THREADS,NULL);
  hs_file     = CreateSemaphore(NULL,NB_MAX_FILE_THREADS,NB_MAX_FILE_THREADS,NULL);
  hs_registry = CreateSemaphore(NULL,NB_MAX_REGISTRY_THREADS,NB_MAX_REGISTRY_THREADS,NULL);

  //wsa init
  WSADATA WSAData;
  WSAStartup(0x02, &WSAData );

  for (i=0;(i<nb_i) && scan_start;i++)
  {
    WaitForSingleObject(hs_threads,0);
    //ScanIp(i);
    CreateThread(NULL,0,ScanIp,(PVOID)i,0,0);

    //tracking
    snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,i+1,nb_i);
    SetWindowText(h_main,test_title);
  }

  //wait
  AddMsg(h_main,(char*)"INFORMATION",(char*)"Start waiting threads.",(char*)"");
  for(i=0;i<NB_MAX_THREAD;i++)WaitForSingleObject(hs_threads,INFINITE);
  /*for(i=0;i<NB_MAX_DISCO_THREADS;i++)WaitForSingleObject(hs_disco,INFINITE);
  WaitForSingleObject(hs_netbios,INFINITE);
  WaitForSingleObject(hs_file,INFINITE);
  WaitForSingleObject(hs_registry,INFINITE);*/

  WSACleanup();
  AddMsg(h_main,(char*)"INFORMATION",(char*)"End of scan!",(char*)"");

  //close
  CloseHandle(hs_threads);
  CloseHandle(hs_disco);
  CloseHandle(hs_netbios);
  CloseHandle(hs_file);
  CloseHandle(hs_registry);

  //---------------------------------------------
  //init
  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),TRUE);
  }

  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),TRUE);
    EnableWindow(GetDlgItem(h_main,IP1),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_IP_CP),TRUE);
    EnableWindow(GetDlgItem(h_main,IP2),TRUE);
  }

  EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  EnableWindow(GetDlgItem(h_main,CB_tests),TRUE);
  scan_start = FALSE;

  SetWindowText(GetDlgItem(h_main,BT_START),"Start");
  SetWindowText(h_main,TITLE);
}
//----------------------------------------------------------------
BOOL CALLBACK DlgMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    //--------------------------------------
    case WM_INITDIALOG:init(hwnd);break;
    //--------------------------------------
    case WM_CLOSE:
    {
      scan_start = FALSE;
      DeleteCriticalSection(&Sync);
      CloseHandle(h_log);
      FreeLibrary((HMODULE)hndlIcmp);
      EndDialog(hwnd,0);
    }
    break;
    //--------------------------------------
    case WM_COMMAND:
    {
      switch (HIWORD(wParam))
      {
        case BN_CLICKED:
          switch(LOWORD(wParam))
          {
            //------------------------------
            case CHK_NULL_SESSION:
              if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)
              {
                EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),FALSE);
              }else
              {
                EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),TRUE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),TRUE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),TRUE);
              }
            break;
            //------------------------------
            case CHK_LOAD_IP_FILE:
              if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)CreateThread(NULL,0,load_file_ip,0,0,0);
              else
              {
                EnableWindow(GetDlgItem(hwnd,GRP_PERIMETER),TRUE);
                EnableWindow(GetDlgItem(hwnd,IP1),TRUE);
                EnableWindow(GetDlgItem(hwnd,BT_IP_CP),TRUE);
                EnableWindow(GetDlgItem(hwnd,IP2),TRUE);
              }
            break;
            //------------------------------
            case BT_SAVE:
            {
              char file[MAX_PATH]= "";
              OPENFILENAME ofn;
              ZeroMemory(&ofn, sizeof(OPENFILENAME));
              ofn.lStructSize    = sizeof(OPENFILENAME);
              ofn.hwndOwner      = hwnd;
              ofn.lpstrFile      = file;
              ofn.nMaxFile       = MAX_PATH;
              ofn.nFilterIndex   = 1;
              ofn.Flags          = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
              ofn.lpstrFilter    = "*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
              ofn.lpstrDefExt    = ".csv\0";
              if (GetSaveFileName(&ofn)==TRUE)
              {
                if(SaveLSTV(GetDlgItem(hwnd,LV_results), file, ofn.nFilterIndex, NB_COLUMN)) AddMsg(hwnd, (char*)"INFORMATION",(char*)"Recorded data",file);
                else AddMsg(hwnd, (char*)"ERROR",(char*)"No data saved!",(char*)"");
              }
            }
            break;
            //------------------------------
            case BT_START:
              scan_start = !scan_start;
              if (scan_start)
              {
                EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),FALSE);
                EnableWindow(GetDlgItem(hwnd,CHK_NULL_SESSION),FALSE);
                EnableWindow(GetDlgItem(hwnd,GRP_PERIMETER),FALSE);
                EnableWindow(GetDlgItem(hwnd,IP1),FALSE);
                EnableWindow(GetDlgItem(hwnd,BT_IP_CP),FALSE);
                EnableWindow(GetDlgItem(hwnd,IP2),FALSE);
                EnableWindow(GetDlgItem(hwnd,CHK_LOAD_IP_FILE),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),FALSE);
                EnableWindow(GetDlgItem(hwnd,CB_tests),FALSE);
                ListView_DeleteAllItems(GetDlgItem(h_main,LV_results));

                SetWindowText(GetDlgItem(hwnd,BT_START),"Stop");
                AddMsg(hwnd, (char*)"INFORMATION",(char*)"Start scan",(char*)"");
                h_thread_scan = CreateThread(NULL,0,scan,0,0,0);
              }else EnableWindow(GetDlgItem(hwnd,BT_START),FALSE);
            break;
            //------------------------------
            case CHK_ALL_TEST:
            {
              //check all
              SendDlgItemMessage(hwnd,CB_tests,LB_SELITEMRANGEEX,(WPARAM)0,(LPARAM)NB_COLUMN);

              //uncheck only separator
              SendDlgItemMessage(hwnd,CB_tests,LB_SETSEL,(WPARAM)FALSE,(LPARAM)3);
            }
            break;
              //------------------------------
            case BT_IP_CP:
            {
              DWORD LIp1=0;
              SendMessage(GetDlgItem(hwnd,IP1),IPM_GETADDRESS, 0 ,(LPARAM) &LIp1);
              SendMessage(GetDlgItem(hwnd,IP2),IPM_SETADDRESS, 0 ,(LPARAM) MAKEIPADDRESS(LIp1 >> 24,(LIp1 >> 16) & 0xFF,(LIp1 >> 8) & 0xFF,254));
            }
            break;
          }
        break;
      }
    }
    break;
    //--------------------------------------
    case WM_NOTIFY:
      switch(((LPNMHDR)lParam)->code)
      {
        case LVN_COLUMNCLICK:
          if (!scan_start)
          {
            tri_order = !tri_order;
            c_Tri(GetDlgItem(hwnd,LV_results),((LPNMLISTVIEW)lParam)->iSubItem,tri_order);
          }
        break;
        case NM_DBLCLK:
        {
          char msg[MAX_LINE_SIZE]="",tmp[MAX_LINE_SIZE];
          long int index = SendDlgItemMessage(hwnd,LV_results,LVM_GETNEXTITEM,(WPARAM)-1,(LPARAM)LVNI_FOCUSED);
          if (index != -1)
          {
            tmp[0] = 0;
            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_IP,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,tmp,MAX_LINE_SIZE);

              tmp[0] = 0;
              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_DNS,tmp,MAX_LINE_SIZE);
              if (tmp[0] != 0)
              {
                strncat(msg," ",MAX_LINE_SIZE);
                strncat(msg,tmp,MAX_LINE_SIZE);
                tmp[0] = 0;
              }

              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_TTL,tmp,MAX_LINE_SIZE);
              if (tmp[0] != 0)
              {
                strncat(msg," ",MAX_LINE_SIZE);
                strncat(msg,tmp,MAX_LINE_SIZE);
                tmp[0] = 0;
              }
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_CONFIG,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Config]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_FILES,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[File]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_REG,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Registry]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SERVICE,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Services]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SOFTWARE,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Softwares]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_USB,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[USB]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }
            strncat(msg,"\0",MAX_LINE_SIZE);
            if (strlen(msg))MessageBox(h_main,msg,"Global View",MB_OK|MB_TOPMOST);
          }
        }
        break;
      }
    break;
    //--------------------------------------
    case WM_SIZE:
    {
      unsigned int mWidth  = LOWORD(lParam);
      unsigned int mHeight = HIWORD(lParam);

      if (mWidth<800 ||mHeight<500)
      {
        RECT Rect;
        GetWindowRect(hwnd, &Rect);
        MoveWindow(hwnd,Rect.left,Rect.top,800+20,500+44,TRUE);
      }else
      {
        MoveWindow(GetDlgItem(hwnd,CB_tests)  ,0  ,75         ,200        ,mHeight-80 ,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_results),202,57         ,mWidth-204 ,mHeight-199,TRUE);
        MoveWindow(GetDlgItem(hwnd,CB_infos)  ,202,mHeight-140,mWidth-204 ,135        ,TRUE);
      }
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hinst = hInstance;
    InitCommonControls();
    return DialogBox(hInstance, MAKEINTRESOURCE(DLG_NS), NULL, (DLGPROC)DlgMain);
}

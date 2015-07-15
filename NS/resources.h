//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------

//----------------------------------------------------------------
#define _WIN32_IE                               0x0501  // IE5 min

//for used of wowo64 local check
/*#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#define NTDDI_VERSION 0x05010000*/

#define UF_PASSWORD_EXPIRED                     0x800000// bad record for netbios account policy
//----------------------------------------------------------------
//#define DEBUG_MODE                                  1
//#define DEBUG_MODE_SSH                              1
//#define DEBUG_MODE_REGISTRY                         1
//#define DEBUG_MODE_FILES                            1
//#define DEBUG_THREADS                               1
//#define DEBUG_NOERROR                               1
//----------------------------------------------------------------
#include <Winsock2.h>
#include <windows.h>
#include <Windowsx.h> //for LSB macro
#include <commctrl.h>
#include <stdio.h>
#include <time.h>
#include <lm.h>
#include <Winnetwk.h>
#include <iphlpapi.h>
#include <math.h>
#include <Shlobj.h>  //for GetPathToSAve
#include <richedit.h>
#include "crypt/sha2.h"
#include "crypt/sha1.h"
#include "crypt/md5.h"

#include <gpg-error.h>
#include <gcrypt.h>
#include <libssh2.h>
/*
//for visual studio compatibilities
#pragma comment(lib, "gcrypt.lib")
#pragma comment(lib, "gpg-error.lib")
#pragma comment(lib, "ssh2.dll.lib")

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
*/
//#pragma comment(lib, "Shell32.lib") //for GetPathToSAve

#ifndef RESOURCES
#define RESOURCES
//----------------------------------------------------------------
#define TITLE                                       "NS v0.5.46 15/07/2015"
#define ICON_APP                                    100
//----------------------------------------------------------------
#define DEFAULT_LIST_FILES                          "\\conf_files.txt"
#define DEFAULT_LIST_MULFILES                       "\\conf_mulfiles.txt"
#define DEFAULT_LIST_REGISTRY                       "\\conf_registry.csv"
#define DEFAULT_LIST_SERVICES                       "\\conf_services.txt"
#define DEFAULT_LIST_SOFTWARE                       "\\conf_softwares.txt"
#define DEFAULT_LIST_USB                            "\\conf_USB.txt"
#define DEFAULT_LIST_REGISTRY_W                     "\\conf_registry_write.csv"
#define DEFAULT_LIST_SSH                            "\\conf_ssh.txt"

#define AUTO_SCAN_FILE_INI                          "\\NS.ini"
//----------------------------------------------------------------
#define LINE_SIZE                                   2048
#define FILE_BUFFER_SIZE                            0xA000
#define MAX_LINE_SIZE                               LINE_SIZE*4
#define MAX_MSG_SIZE                                0xA000
#define MAX_MSG_SIZE_LINE                           0x4100
#define MAX_COUNT_MSG                               0X10
#define IP_SIZE                                     16
#define SHA256_SIZE                                 65
#define SHA1_SIZE                                   41
#define DATE_SIZE                                   26
#define HK_SIZE_MAX                                 20

#define ICMP_TIMEOUT                                6000            //6 seconds
#define THREAD_MAX_TIMEOUT                          100000          //100 secondes
#define THE_END_THREAD_WAIT                         100//*100ms
#define DIXM                                        10*1024*1024    //10mo
//----------------------------------------------------------------
#define ID_ERROR                                    -1
//----------------------------------------------------------------
//couleur
#define ROUGE RGB(255, 0, 0)
#define NOIR  RGB(0  ,  0, 0)
#define VERT  RGB(51 ,153, 0)
#define BLEU  RGB(0  ,  0,255)
#define GRIS  RGB(153,153,153)
//----------------------------------------------------------------
//SSH use custom error message
#define SSH_DEFAULT_PORT                            22
#define RPC_DEFAULT_PORT                            445

#define SSH2_SESSION_TIMEOUT                        4000   //4 seconds

#define SSH_ERROR_OK                                1
#define SSH_ERROR_NOT_TESTED                        0

#define SSH_ERROR_LIBINIT                           -1
#define SSH_ERROR_SESSIONINIT                       -2
#define SSH_ERROR_SESSIONSTART                      -3
#define SSH_ERROR_AUTHENT                           -4
#define SSH_ERROR_CHANNEL                           -5
#define SSH_ERROR_CHANNEL_OPEN                      -6
#define SSH_ERROR_CHANNEL_EXEC                      -7
#define SSH_ERROR_CHANNEL_READ                      -8

#define SSH_ERROR_SOCKET                            -10
#define SSH_ERROR_CONNECT                           -11
//----------------------------------------------------------------
#define DLG_NS                                      1000
#define GRP_DISCO                                   1001
#define ED_NET_LOGIN                                1002
#define ED_NET_PASSWORD                             1003
#define ED_NET_DOMAIN                               1004
#define CHK_NULL_SESSION                            1005
#define CHK_ALL_TEST                                1006
#define BT_LOAD_MDP_FILES                           1007

#define GRP_PERIMETER                               1015
#define IP1                                         1016
#define IP2                                         1017
#define BT_IP_CP                                    1018
#define CHK_LOAD_IP_FILE                            1019
#define BT_SAVE                                     1020

#define CB_infos                                    1031
#define LV_results                                  1032
#define CB_tests                                    1033

#define CB_IP                                       1040
#define CB_T_REGISTRY                               1041
#define CB_T_SERVICES                               1042
#define CB_T_SOFTWARE                               1043
#define CB_T_USB                                    1044
//#define CB_T_FILES                                  1045
#define CB_T_REGISTRY_W                             1046
#define CB_T_SSH                                    1047
#define CB_T_MULFILES                               1048
#define CB_DSC                                      1050


#define BT_START                                    1035
#define BT_RE                                       1036
//----------------------------------------------------------------
#define SAVE_TYPE_XML                               1
#define SAVE_TYPE_CSV                               2
#define SAVE_TYPE_HTML                              3
#define SAVE_TYPE_ALL                               4
#define SAVE_TYPE_LOG                               5
//----------------------------------------------------------------
#define COL_IP                                      0
#define COL_DSC                                     1
#define COL_DNS                                     2
#define COL_TTL                                     3
#define COL_OS                                      4
#define COL_CONFIG                                  5
#define COL_SHARE                                   6
#define COL_POLICY                                  7
#define COL_FILES                                   8
#define COL_REG                                     9
#define COL_SERVICE                                 10
#define COL_SOFTWARE                                11
#define COL_USB                                     12
#define COL_SSH                                     13
#define COL_STATE                                   14

#define NB_COLUMN                                   15
//----------------------------------------------------------------
#define CHK_TEST_ICMP                               0
#define CHK_TEST_DNS                                1
#define CHK_TEST_NETBIOS                            2
#define CHK_TEST_NETBIOS_POLICY                     3
#define CHK_TEST_NETBIOS_USERS                      4
#define SEPARATOR_1                                 5
#define CHK_TEST_FILES                              6
#define CHK_TEST_REGISTRY                           7
#define CHK_TEST_SERVICES                           8
#define CHK_TEST_SOFTWARE                           9
#define CHK_TEST_USB                                10
#define CHK_TEST_SSH                                11
#define SEPARATOR_2                                 12
#define CHK_TEST_WRITE_KEY                          13
//----------------------------------------------------------------
typedef struct sort_st
{
  HWND hlv;
  BOOL sort;
  unsigned int col;
}SORT_ST;
//----------------------------------------------------------------
typedef struct reg_line_st
{
  char path[LINE_SIZE];
  char value[LINE_SIZE];
  char data[LINE_SIZE];

  //format data
  BOOL data_dword;    //dword
  BOOL data_string;   //string

  char chkey[LINE_SIZE];
  char description[LINE_SIZE]; //description or value o check in write mode!

  //check
  BOOL check_equal;   // =
  BOOL check_inf;     // <
  BOOL check_sup;     // >
  BOOL check_diff;    // !
  BOOL check_content; // ?
  BOOL check_no_data; // *
  BOOL check_no_value;//
}REG_LINE_ST;
//----------------------------------------------------------------
typedef struct rg_st
{
  HKEY hk;
  char path[LINE_SIZE];
  char value[LINE_SIZE];
  char data[LINE_SIZE];
  unsigned short data_type;
}RG_ST;
//----------------------------------------------------------------
#define MAX_ACCOUNTS      4096
typedef struct accounts_st
{
  char domain[MAX_PATH];
  char login[MAX_PATH];
  char password[MAX_PATH];
}ACCOUNTS_ST;

typedef struct scanne_st
{
  //BOOL disco_arp;
  BOOL disco_icmp;
  BOOL disco_dns;
  BOOL disco_netbios;
  BOOL disco_netbios_policy;
  BOOL disco_users;
  BOOL disco_netbios_users;

  BOOL config_service;
  //BOOL config_user;
  BOOL config_software;
  BOOL config_USB;
  BOOL config_start;
  BOOL config_revers_SID;
  BOOL config_RPC;

  BOOL check_files;
  BOOL check_registry;
  BOOL check_services;
  BOOL check_software;
  BOOL check_USB;

  BOOL write_key;
  BOOL check_ssh;
  BOOL check_ssh_os;

  BOOL no_hash_check;

  BOOL global_ip_file; // = IP + desc + domain + login + mdp par ligne !!!

  int nb_accounts;
  ACCOUNTS_ST accounts[MAX_ACCOUNTS];

  //use or not local account
  BOOL local_account;
  char domain[MAX_PATH];
  char login[MAX_PATH];
  char password[MAX_PATH];
}SCANNE_ST, *PSCANNE_ST;
//----------------------------------------------------------------
typedef struct
{
  char c[MAX_LINE_SIZE];
}LINE_ITEM;
//----------------------------------------------------------------
BOOL scan_start, tri_order;
HANDLE h_thread_scan;

BOOL SHA1_enable;

HINSTANCE hinst;
HWND h_main, hdbclk_info;
HANDLE h_log;
WNDPROC wndproc_hdbclk_info;
BOOL save_done, save_current;
//----------------------------------------------------------------
//scan
#define MACH_LINUX                                  64
#define MACH_WINDOWS                                128
#define MACH_ROUTEUR                                256

//Threads
DWORD NB_MAX_DISCO_THREADS;
#define NB_MAX_DNS_THREADS                          100
DWORD NB_MAX_NETBIOS_THREADS;
DWORD NB_MAX_FILE_THREADS;
DWORD NB_MAX_REGISTRY_THREADS;
DWORD NB_MAX_SSH_THREADS;
DWORD NB_MAX_TCP_TEST_THREADS;
DWORD NB_MAX_THREAD;

CRITICAL_SECTION Sync, Sync_item, Sync_threads, Sync_threads_tcp, Sync_threads_end, Sync_threads_disco, Sync_threads_netbios, Sync_threads_registry, Sync_threads_files, Sync_threads_ssh;
HANDLE hs_threads,hs_disco,hs_netbios,hs_file,hs_registry,hs_ssh,hs_tcp;

long int hs_c_threads, hs_c_disco, hs_c_netbios, hs_c_file, hs_c_registry, hs_c_ssh, hs_c_tcp;
HANDLE hs_count;

SCANNE_ST config;
HINSTANCE richDll;
BOOL LOG_DISABLE, LOG_DNS_DISABLE, LOG_LOGIN_DISABLE, LOG_ERROR_VIEW_DISABLE,REG_REMOTE_SERVICE_STOP;

long long int emp_MIN_SZ;
long long int emp_MAX_SZ;

//----------------------------------------------------------------
//AUTO-SCAN
typedef struct auto_scanne_st
{
  //save
  BOOL save_CSV;
  BOOL save_XML;
  BOOL save_HTML;
  BOOL auto_close_after_save;

  BOOL NO_GUI;

  //check
  BOOL M_SEC;
  BOOL PATCH_UPDATED;
  BOOL WSUS_WORKS;
  BOOL MCAFEE_INSTALLED;
  BOOL MCAFEE_UPDATED;
  BOOL MCAFEE_SCAN;
  BOOL PASSWORD_POLICY;
  BOOL ADMIN_ACCOUNT;
  BOOL NULL_SESSION;
  BOOL REVERS_SID;
  BOOL AUTORUN;
  BOOL SHARE_ACCESS;

  //check_options
  char C_ADMIN_ACCOUNT[MAX_PATH];
  char MSEC_REG_PATH[MAX_PATH];
  char MSEC_REG_VALUE[MAX_PATH];
  unsigned int MCAFEE_SCAN_DAYS_INTERVAL;
  unsigned int MCAFEE_UPDATE_DAYS_INTERVAL;
  unsigned int PASSWORD_POLICY_MIN_AGE;
  unsigned int PASSWORD_POLICY_MAX_AGE;
  unsigned int PASSWORD_POLICY_MIN_LEN;
  unsigned int PASSWORD_POLICY_LOCKOUT_COUNT;
  BOOL PASSWORD_POLICY_COMPLEXITY_ENABLE;
  unsigned int PASSWORD_POLICY_HISTORY;

  BOOL DNS_DISCOVERY;
}AUTO_SCANNE_ST;

AUTO_SCANNE_ST auto_scan_config;
//----------------------------------------------------------------
//ICMP
typedef struct tagIPINFO
{
	unsigned char Ttl;
	unsigned char Tos;
	unsigned char Flags;
  	unsigned char OptionsSize;
  	unsigned char OptionsData;
}IPINFO;
typedef IPINFO* PIPINFO;

typedef struct tagICMPECHO
{
     DWORD Address;
     unsigned long  Status,RoundTripTime;
     unsigned char a[8];
     IPINFO  Options;
}ICMPECHO;

typedef LONG NTSTATUS;

typedef struct _IO_STATUS_BLOCK
{
  NTSTATUS Status;
  ULONG Information;
}IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef VOID WINAPI(*PIO_APC_ROUTINE)
(
  IN PVOID ApcContext,
  IN PIO_STATUS_BLOCK IoStatusBlock,
  IN ULONG Reserved
);

BOOL IcmpOk;
HANDLE hndlIcmp;
HANDLE (WINAPI *pIcmpCreateFile)(VOID);
BOOL (WINAPI *pIcmpCloseHandle)(HANDLE);
DWORD (WINAPI *pIcmpSendEcho) (HANDLE,DWORD,LPVOID,WORD, PIPINFO,LPVOID,DWORD,DWORD);
DWORD (WINAPI *pIcmpSendEcho2) (HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,IPAddr,LPVOID,WORD,PIP_OPTION_INFORMATION,LPVOID,DWORD,DWORD);

HMODULE hDLL_kernel32;
BOOL (WINAPI *Wow64DisableWow64FsRedirect)(PVOID *OldValue);
BOOL (WINAPI *Wow64RevertWow64FsRedirect)(PVOID *OldValue);
//----------------------------------------------------------------
DWORD nb_test_ip, nb_i, nb_files, nb_registry, nb_windows, nb_unknow;
//----------------------------------------------------------------
//GUI
void init(HWND hwnd);
void initthreadings();

void AddMsg(HWND hwnd, char *type, char *txt, char *info, BOOL inc);
void AddLSTVUpdateItem(char *add, DWORD column, DWORD iitem);
long int AddLSTVItem(char *ip, char *dsc, char *dns, char *ttl, char *os, char *config, char *share, char*policy, char *files, char *registry, char *Services, char *software, char *USB, char *state);
void c_Tri(HWND hlv, unsigned short colonne_ref, BOOL sort);
int CALLBACK CompareStringTri(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3);
void SetMainTitle( char *date, BOOL inc);
BOOL LSBExist(DWORD lsb, char *sst);
BOOL LSBExistC(DWORD lsb, char *sst);
BOOL CALLBACK DlgMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//RicheEdit
void RichSetTopPos(HWND HRichEdit);
void RichEditInit(HWND HRichEdit);
void RichEditCouleur(HWND HRichEdit,COLORREF couleur,char* txt);
void RichEditCouleurGras(HWND HRichEdit,COLORREF couleur,char* txt);

//string
BOOL compare_nocas(char *a, char *d);
char *ConvertLinuxToWindows(char *src, DWORD max_size);
char *charToLowChar(char *src);
long long int Contient(char*data, char*chaine);
BOOL Contient_nocas(char *data, char *chaine);
void replace_one_char(char *buffer, unsigned long int taille, char chtoreplace, char chreplace);
BOOL LinuxStart_msgOK(char *msg, char*cmd);
char *extractFileFromPath(char *path, char *file, unsigned int file_size_max);
char *extractPath(char *path, char *path_dst, unsigned int path_size_max);

//export
BOOL SaveLSTV(HWND hlv, char *file, unsigned int type, unsigned int nb_column);
BOOL SaveLV(HWND hlv, char *file);

//load files configuration
char* GetLocalPath(char *path, unsigned int sizeMax);
void loadFileIp(char *file);
DWORD WINAPI load_file_ip(LPVOID lParam);
DWORD load_file_list(DWORD lsb, char *file, BOOL reset);
void LoadAuthFile(char *file);
DWORD WINAPI load_file_accounts(LPVOID lParam);
DWORD WINAPI scan(LPVOID lParam);
DWORD WINAPI auto_scan(LPVOID lParam);

//IP
BOOL addIPTest(char *ip_format, char*dsc);
void addIPInterval(char *ip_src, char *ip_dst, char*dsc);
BOOL verifieName(char *name);

//Disco
BOOL ipIsLoclahost(char *ip);
int Ping(char *ip);
BOOL ResDNS(char *ip, char *name, unsigned int sz_max);

//Netbios
BOOL Netbios_NULLSession(char *ip, char *share);
BOOL Netbios_NULLSessionStart(char *ip, char *share);
void Netbios_NULLSessionStop(char *ip, char *share);

//BOOL TestReversSID(char *ip, char* user);
//void CheckReversSID(char *ip, char *results, DWORD max_size_results);
BOOL Netbios_Time(wchar_t *server, char *time, unsigned int sz_max);
BOOL Netbios_Share(wchar_t *server, DWORD iitem, DWORD col, char*ip, BOOL IPC_null_session);
BOOL Netbios_Policy(wchar_t *server, char *pol, unsigned int sz_max);
BOOL Netbios_OS(char *ip, char*txtOS, char *name, char *domain, unsigned int sz_max);

int Netbios_List_service(DWORD iitem, char *ip, BOOL check);
BOOL Netbios_List_users(DWORD iitem, char *ip);

BOOL EnumTestReversSID(DWORD iitem, char *ip, BOOL check_only, char* user, char* results, unsigned int sz_results);
BOOL Netbios_List_users_reversSID(DWORD iitem, char *ip, BOOL check_only, char*results, unsigned int max_size_results);

//Registry
BOOL StartRemoteRegistryService(char *ip, BOOL start);
BOOL parseLineToReg(char *line, REG_LINE_ST *reg_st, BOOL reg_write);
BOOL RegistryOS(DWORD iitem,HKEY hkey);
BOOL Registry_List_users(DWORD iitem, char *ip, HKEY hkey);

void RegistryScan(DWORD iitem,char *ip, HKEY hkey, char* chkey, BOOL hkey_users);
DWORD ReadValue(HKEY hk,char *path,char *value,void *data, DWORD data_size);
int RegistryServiceScan(DWORD iitem,char *ip, char *path, HKEY hkey);
int RegistrySoftwareScan(DWORD iitem,char *ip, char *path, HKEY hkey);
int RegistryUSBScan(DWORD iitem,char *ip, char *path, HKEY hkey);
void RegistryWriteKey(char *ip, HKEY hkey, char *chkey);
BOOL RemoteRegistryNetConnexion(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, BOOL windows_OS, long int *id_ok, BOOL users_check, BOOL os_check);
BOOL RemoteConnexionScan(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, BOOL windows_OS, long int *id_ok, BOOL users_check, BOOL os_check);

BOOL CheckServiceOpenSC(DWORD iitem, char *ip);

//File
void FileToMd5(HANDLE Hfic, char *md5);
void FileToSHA256(HANDLE Hfic, char *csha256);
BOOL RemoteAuthenticationFilesScan(DWORD iitem, char *ip, DWORD ip_id, char *remote_share, PSCANNE_ST config, long int *id_ok, DWORD id_cb, BOOL multi);
BOOL RemoteConnexionFilesScan(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, long int *id_ok);
void CheckFile(DWORD iitem, char *file, WIN32_FIND_DATA *data, char*source);
void CheckFileName(DWORD iitem, char*remote_name, char*chaine);
DWORD CheckRecursivFilesList(DWORD iitem, char *remote_name, DWORD cb_id);
BOOL RemoteFilesCopy(DWORD iitem, char *ip, char*remote_share, PSCANNE_ST config, char*pathToSave, char*file);

//backup
void backupRegKey(HKEY hkey, char *chkey,char *ckey, char *filetosave, HANDLE hfile, BOOL createf);
int BackupUSBList(HKEY hkey, char *ckey, char*fileToSave, char*ip, DWORD iitem);
int BackupSoftwareList(HKEY hkey, char *ckey, char*fileToSave, char*ip, DWORD iitem);
int BackupServiceList(HKEY hkey, char *ckey, char*fileToSave, char*ip, DWORD iitem);

//SSH
BOOL TCP_port_open(DWORD iitem, char *ip, unsigned int port, BOOL msg_OK);
int ssh_exec(DWORD iitem, char *ip, unsigned int port, char*username, char*password);
int ssh_exec_to_file(DWORD iitem, char *ip, unsigned int port, char*username, char*password, HANDLE hfile);
int ssh_exec_cmd(DWORD iitem,char *ip, unsigned int port, char*username, char*password, long int id_account, char *cmd, char *buffer, DWORD buffer_size, BOOL msg_OK, BOOL msg_auth);

//Scan
HANDLE NetConnexionAuthenticateTest(char *ip, DWORD id_ip, char*remote_name, PSCANNE_ST config, DWORD iitem, BOOL message, long int *id_ok);
DWORD WINAPI ScanIp(LPVOID lParam);
DWORD WINAPI remote_extract(LPVOID lParam);
#endif
//----------------------------------------------------------------



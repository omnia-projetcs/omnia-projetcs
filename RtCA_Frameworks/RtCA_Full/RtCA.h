//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#define _WIN32_WINNT			     0x0501  //fonctionne au minimum sous Windows 2000
#define _WIN32_IE              0x0501  //fonctionne avec ie5 min pour utilisation de LVS_EX_FULLROWSELECT

#define NOM_APPLI              "RtCA"
#define URL_APPLI              "http://code.google.com/p/omnia-projetcs/"

#define DEFAULT_SQLITE_FILE    "RtCA.sqlite"
#define DEFAULT_TM_SQLITE_FILE "RtCA.sqlite-journal"

#define IDR_VERSION                 1

#define ICON_APP                  100   //icone de l'application
#define ICON_LANGUAGE_EN          101
#define ICON_LANGUAGE_FR          102

#define ICON_FOLDER               103   //icone treeview
#define ICON_FILE                 104
#define ICON_FILE_BIN             105
#define ICON_FILE_DWORD           106
#define ICON_FILE_TXT             107
#define ICON_FILE_UNKNOW          108
//******************************************************************************
//debug mode dev test
#define DEV_DEBUG_MODE              1
//******************************************************************************
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "version.h"           //for version track
#include <commctrl.h>           //componants
#include <shlobj.h>             //browser
#include <iprtrmib.h>           //tracert PMIB_IPFORWARDTABLE
#include <lmshare.h>            //share
#include <tlhelp32.h>           //process
#include <psapi.h>              //process
#include "sqlite/sqlite3.h"     //sqlite
#include "sqlite/interface.h"

#include "Registry.h"           //registry

#include <Windns.h>             //DnsQuery

#include <Wincrypt.h>           //FOR DECODE CHROME/CHROMIUM password
#include "crypt/d3des.h"        //Crypto
//---------------------------------------------------------------------------
#define NB_MAX_THREAD           5 //thread nb
HANDLE hsemaphore;

PVOID OldValue_W64b;            //64bits OS
//---------------------------------------------------------------------------
char _SYSKEY[MAX_PATH];
//---------------------------------------------------------------------------
//size constant
#undef MAX_PATH                       //new value for max_path
#define MAX_PATH                 1024

#define DEFAULT_TMP_SIZE          256
#define MAX_LINE_SIZE      8*MAX_PATH
#define REQUEST_MAX_SIZE   MAX_LINE_SIZE
#define DEFAULT_EVENT_SIZE    1024*64

#define HEIGHT_SEARCH              30
#define DATE_SIZE_MAX              20
#define NUM_S_MAX                  10
#define NB_USERNAME_SIZE           21
#define DRIVE_SIZE                  4
#define COMPUTER_NAME_SIZE_MAX     16
#define IP_SIZE_MAX                16   //255.255.255.255   = 16
#define MAC_SIZE                   18   //AA:BB:CC:DD:EE:FF = 18
#define NB_MAX_PORTS            65536
#define SZ_PART_SYSKEY           0x21

#define DIXM             10*1024*1024    //10mo
//------------------------------------------------------------------------------
//debug message
#define NB_MSG_COLUMN               6

#define MSG_INFO               "INFO"
#define MSG_WARN               "WARN"
#define MSG_ERROR             "ERROR"
#define MSG_TEST            "TESTING"
//------------------------------------------------------------------------------
//TREVIEW STATE
#define TRV_STATE_CHECK             2
#define TRV_STATE_UNCHECK           1
//------------------------------------------------------------------------------
#define DLG_MAIN                 1000
#define LV_INFO                  1001

#define DLG_CONF                 2000
#define CB_LANG                  2001
#define GRP_CONF                 2002
#define ST_LANG                  2003
#define TRV_TEST                 2004
#define BT_START                 2005
#define BT_EXPORT                2006
#define TRV_FILES                2007
#define ST_SESSION               2008
#define CB_SESSION               2009

#define BT_ACL_FILE_CHK          2012
#define BT_SHA_FILE_CHK          2014
#define BT_ADS_FILE_CHK          2015
#define BT_REGISTRY_RECOV_MODE   2016

#define DLG_VIEW                 3000
#define LV_VIEW                  3001
#define TRV_VIEW                 3002
#define CB_VIEW                  3003
#define ED_SEARCH                3004
#define BT_SEARCH                3005
//------------------------------------------------------------------------------
#define MY_MENU                 10000
#define IDM_OPEN_FILE           10001
#define IDM_OPEN_DIRECTORY      10002
#define IDM_SAVE_ALL            10003
#define IDM_ONGLET_CONF         10004
#define IDM_ONGLET_VIEW         10005
#define IDM_CHECK_ALL_TESTS     10006
#define IDM_DEL_SESSION         10007
#define IDM_DEL_ALL_SESSION     10008
#define IDM_STAY_ON_TOP         10009
#define IDM_DEBUG_MODE          10010
#define IDM_TOOLS_CP_REGISTRY   10011
#define IDM_TOOLS_CP_AUDIT      10012
#define IDM_TOOLS_CP_AD         10013
#define IDM_TOOLS_CP_SELECT     10014
#define IDM_ABOUT               10015
#define IDM_RTCA_HOME           10016
#define IDM_DEBUG_LIST          10017
#define IDM_SAVE_DEBUG          10018
#define IDM_LOAD_SESSION_FILE   10019
#define IDM_SAVE_SESSION_FILE   10020
#define IDM_TOOLS_CP_HIBERNATE  10021
#define IDM_UNCHECK_ALL_TESTS   10022
//------------------------------------------------------------------------------
#define POPUP_TRV_FILES_REF_ITEMS_STRINGS         0
#define POPUP_TRV_FILES_REF_NB_ITEMS_STRINGS      7
#define POPUP_TRV_FILES_REF_NB_ITEMS_STRINGS_EXT  1
#define POPUP_TRV_FILES                       11000
#define POPUP_TRV_FILES_ADD_FILE              11001
#define POPUP_TRV_FILES_ADD_PATH              11002
#define POPUP_TRV_FILES_REMOVE_ITEMS          11003
#define POPUP_TRV_FILES_CLEAN_ALL             11004
#define POPUP_TRV_FILES_OPEN_PATH             11005
#define POPUP_TRV_FILES_AUTO_SEARCH           11006
#define POPUP_TRV_FILES_SAVE_LIST             11007

#define POPUP_EXPORT                          12000
#define POPUP_E_CSV                           12001
#define POPUP_E_HTML                          12002
#define POPUP_E_XML                           12003

#define POPUP_LSTV                            13000
#define POPUP_S_VIEW                          13001
#define POPUP_S_SELECTION                     13002
#define POPUP_O_PATH                          13003
#define POPUP_A_SEARCH                        13004

#define POPUP_I_00                            13010
#define POPUP_I_01                            13011
#define POPUP_I_02                            13012
#define POPUP_I_03                            13013
#define POPUP_I_04                            13014
#define POPUP_I_05                            13015
#define POPUP_I_06                            13016
#define POPUP_I_07                            13017
#define POPUP_I_08                            13018
#define POPUP_I_09                            13019
#define POPUP_I_10                            13020
#define POPUP_I_11                            13021
#define POPUP_I_12                            13022
#define POPUP_I_13                            13023
#define POPUP_I_14                            13024
#define POPUP_I_15                            13025
#define POPUP_I_16                            13026
#define POPUP_I_17                            13027
#define POPUP_I_18                            13028
#define POPUP_I_19                            13029

#define NB_POPUP_I                            20
//------------------------------------------------------------------------------
char NOM_FULL_APPLI[DEFAULT_TMP_SIZE];
//------------------------------------------------------------------------------
//save type
#define SAVE_TYPE_CSV                             1
#define SAVE_TYPE_XML                             2
#define SAVE_TYPE_HTML                            3
#define SAVE_TYPE_TXT                             4

unsigned int stat_export_column;
unsigned int nb_stat_export_column;
HANDLE MyhFile_export,h_Export;
unsigned int export_type;
unsigned int current_lang_id;
DWORD line;
BOOL ExportStart;
char current_test_export_path[MAX_PATH];
//------------------------------------------------------------------------------
//column count
#define NB_COLUMN_DEBUG                           5
//------------------------------------------------------------------------------
//general config
HWND h_main, h_conf, h_view;
WNDPROC TRV_SousClassement, LST_SousClassement;

//critical section for use DEBUG!
CRITICAL_SECTION Sync;

//tests
#define NB_MAX_TESTS                            256
#define NB_MAX_ITEMS_HEADERS_XML                256

#define TEST_FILES                                0
#define TEST_LOGS                                 1
#define TEST_REG_NETWORK                          5

#define TEST_ANDROID                             10
#define TEST_CHROME                              11
#define TEST_IE                                  12
#define TEST_FIREFOX                             13

#define TEST_REG_START                           14
#define TEST_REG_END                             25

#define TEST_REG_ANTIVIRUS                       29
#define TEST_REG_FIREWALL                        30

unsigned int NB_TESTS, nb_current_test;
HTREEITEM H_tests[NB_MAX_TESTS];          //list of tests
HANDLE h_thread_test[NB_MAX_TESTS];       //threads for tests
typedef struct
{
  char s[DEFAULT_TMP_SIZE];
}FORMAT_TESTS_STRING;
FORMAT_TESTS_STRING S_tests[NB_MAX_TESTS];// list of name of all threads

FORMAT_TESTS_STRING S_tests_XML_header[NB_MAX_ITEMS_HEADERS_XML];

#define NB_MX_TYPE_FILES_TITLE      4
#define FILES_TITLE_LOGS            0
#define FILES_TITLE_FILES           1
#define FILES_TITLE_REGISTRY        2
#define FILES_TITLE_APPLI           3
HTREEITEM TRV_HTREEITEM_CONF[NB_MX_TYPE_FILES_TITLE]; //list of files

//------------------------------------------------------------------------------
//parameters
BOOL CONSOL_ONLY;
BOOL DEBUG_VIEW;

BOOL DEBUG_MODE;
BOOL DEBUG_CMD_MODE;
BOOL STAY_ON_TOP;
BOOL FILE_ACL;
BOOL FILE_ADS;
BOOL FILE_SHA;
BOOL REGISTRY_RECOVERY;

BOOL LOCAL_SCAN;
//------------------------------------------------------------------------------
//scan stats
BOOL B_AUTOSEARCH;
HANDLE h_AUTOSEARCH;

//scan
BOOL start_scan;
BOOL scan_in_stop_state;
HANDLE h_thread_scan;
sqlite3 *db_scan;

//gui view
BOOL VIEW_RESULTS_DBL;    //for see trv+lstv
//------------------------------------------------------------------------------
//process
#define SIZE_ITEMS_PORT_MAX        20
typedef struct line_proc_item
{
  char protocol[SIZE_ITEMS_PORT_MAX];
  char state[SIZE_ITEMS_PORT_MAX];
  char IP_src[SIZE_ITEMS_PORT_MAX];
  char IP_dst[SIZE_ITEMS_PORT_MAX];
  char Port_src[SIZE_ITEMS_PORT_MAX];
  char Port_dst[SIZE_ITEMS_PORT_MAX];
}LINE_PROC_ITEM;

#define FIRST_PROCESS_ID  1
#define LAST_PROCESS_ID   32768
typedef struct
{
  DWORD pid;
  char args[MAX_PATH];
}PROCESS_INFOS_ARGS;
//------------------------------------------------------------------------------
//struct
typedef struct
{
  char c[MAX_LINE_SIZE];
}LINE_ITEM;
//------------------------------------------------------------------------------
//for sort in lstv
BOOL TRI_DEBUG_VIEW;
BOOL TRI_RESULT_VIEW;

typedef struct SORT_ST
{
  HANDLE hlv;
  BOOL sort;
  unsigned int col;
}sort_st;
//------------------------------------------------------------------------------
//for loading language in local component
#define NB_COMPONENT_STRING         19
#define COMPONENT_STRING_MAX_SIZE   DEFAULT_TMP_SIZE

#define REF_MSG                     8
#define TXT_BT_START                10
#define TXT_BT_STOP                 11
#define TXT_SAVE_TO                 12

#define TXT_BT_EXPORT_START         13
#define TXT_BT_EXPORT_STOP          14

#define TXT_POPUP_S_VIEW            15
#define TXT_POPUP_S_SELECTION       16
#define TXT_POPUP_CLIPBORAD         17
#define TXT_POPUP_A_SEARCH          18

typedef struct
{
  char c[COMPONENT_STRING_MAX_SIZE];
}COMPONENT_STRING;
COMPONENT_STRING cps[NB_COMPONENT_STRING];

#define SYSKEY_STRING_DEF          "604"
//------------------------------------------------------------------------------
//registry params
#define TYPE_VALUE_STRING           0
#define TYPE_VALUE_DWORD            1
#define TYPE_VALUE_MULTI_STRING     2
#define TYPE_VALUE_WSTRING          3

#define TYPE_VALUE_WIN_SERIAL       100
#define TYPE_ENUM_STRING_VALUE      200   //all string
#define TYPE_ENUM_STRING_RVALUE     201   //all string under one key
#define TYPE_ENUM_STRING_RRVALUE    202   //all string under thow key + key
#define TYPE_ENUM_STRING_R_VALUE    203   //all string under one key + key

#define SQLITE_REGISTRY_TYPE_SETTINGS 0x00000000
#define SQLITE_REGISTRY_TYPE_RUN      0x00000000
#define SQLITE_REGISTRY_TYPE_MRU      0x00000000
#define SQLITE_GUIDE                  0x00000000

//for test in guide functions
#define GUIDE_REG_TEST_IDENTIQUE  0x00000000
#define GUIDE_REG_TEST_CONTIENT   0x00000001
#define GUIDE_REG_TEST_EXIST      0x00000002
#define GUIDE_REG_TEST_NEXISTPAS  0x00000003

#define GUIDE_REG_OS_2000         "2000"
#define GUIDE_REG_OS_XP_32b       "XP"
#define GUIDE_REG_OS_XP_64b       "XP_64"
#define GUIDE_REG_OS_2003_32b     "2003"
#define GUIDE_REG_OS_2003_64b     "2003_64"
#define GUIDE_REG_OS_7_32b        "7"
#define GUIDE_REG_OS_7_64b        "7_64"
#define GUIDE_REG_OS_2008_32b     "2008"
#define GUIDE_REG_OS_2008_64b     "2008_64"
#define GUIDE_REG_OS_8_32b        "8"
#define GUIDE_REG_OS_8_64b        "8_64"
#define GUIDE_REG_OS_ALL          "ALL"
#define GUIDE_REG_OS_ALL_ONLY_32b "ALL_ONLY_32"
#define GUIDE_REG_OS_ALL_ONLY_64b "ALL_ONLY_64"

#define GUIDE_REG_TEST_OK         500
#define GUIDE_REG_TEST_NOK        501

char current_OS[DEFAULT_TMP_SIZE];
BOOL current_OS_BE_64b;
//------------------------------------------------------------------------------
#define NB_MAX_SQL_REQ        MAX_PATH

#define SPECIAL_CASE_CHROME_PASSWORD 4

typedef struct
{
  char sql[MAX_PATH];
  char params[MAX_PATH];
  DWORD test_string_id;
}SQL_REQ;

typedef struct
{
  sqlite3 *db;
  unsigned int session_id;
  DWORD index;
  char file[MAX_PATH];
}CALBACK_SQLITE_TEST_FORMAT;

unsigned int nb_sql_ANDROID, nb_sql_CHROME, nb_sql_FIREFOX;

SQL_REQ sql_ANDROID[NB_MAX_SQL_REQ];
SQL_REQ sql_CHROME[NB_MAX_SQL_REQ];
SQL_REQ sql_FIREFOX[NB_MAX_SQL_REQ];

//------------------------------------------------------------------------------
//CMD
unsigned int item_n;
#define CMD_TYPE_LIST_SESSION   0x00000001
#define CMD_TYPE_LIST_LANGUAGE  0x00000002
#define CMD_TYPE_LIST_TESTS     0x00000003
#define CMD_READ_SESSION        0x00000004

#define NB_MAX_ITEM_CMD         256
unsigned int nb_file_cmd;
unsigned int nb_path_cmd;

typedef struct
{
  char c[MAX_PATH];
}FORMAT_FILE_CMD;

FORMAT_FILE_CMD file_cmd[NB_MAX_ITEM_CMD], path_cmd[NB_MAX_ITEM_CMD];
//------------------------------------------------------------------------------
//columns
#define TYPE_SQL_STANDARD           0
#define TYPE_SQL_EXPLORER           1
#define TYPE_SQL_PROCESS            2
#define TYPE_SQL_STATE              3

#define MODE_SQL_NONE              99
#define MODE_SQL_FULL               0
#define MODE_SQL_LITE               1
#define MODE_SQL_LOGS               2

#define REQUEST_MAX_SIZE            MAX_LINE_SIZE
typedef struct
{
  int mode_simple;
  unsigned int nb_columns;
  char request[REQUEST_MAX_SIZE];
  unsigned int type;
}ST_COLUMS;

ST_COLUMS *columns_params;
unsigned int nb_current_columns, nb_columns_items;

//sessions
#define NB_MAX_SESSION              MAX_LINE_SIZE
unsigned int current_session_id;
unsigned int nb_session, session[NB_MAX_SESSION];

DWORD pos_search;
//------------------------------------------------------------------------------
//SQLITE functions
char *convertStringToSQL(char *data, unsigned int size_max);
int callback_write_sqlite(void *datas, int argc, char **argv, char **azColName);
BOOL SQLITE_LireData(FORMAT_CALBAK_READ_INFO *datas, char *sqlite_file);
BOOL SQLITE_WriteData(FORMAT_CALBAK_READ_INFO *datas, char *sqlite_file);
BOOL SQLITE_LoadSession(char *file);
BOOL SQLITE_SaveSession(char *file);
void addLogtoDB(char *eventname, char *indx, char *log_id,
                  char *send_date, char *write_date,
                  char *source, char *description, char *user, char *rid, char *sid,
                  char *state, char *critical, unsigned int session_id, sqlite3 *db);

//save function
BOOL SaveLSTV(HANDLE hlv, char *file, unsigned int type, unsigned int nb_column);
BOOL SaveLSTVSelectedItems(HANDLE hlv, char *file, unsigned int type, unsigned int nb_column);
BOOL SaveTRV(HANDLE htv, char *file, unsigned int type);
DWORD WINAPI SaveAll(LPVOID lParam);
DWORD WINAPI ChoiceSaveAll(LPVOID lParam);
void CopyDataToClipboard(HANDLE hlv, DWORD line, unsigned short column);

//usuals functions
void ReviewWOW64Redirect(PVOID OldValue_W64b);
unsigned long int Contient(char*data,char*chaine);
char *ReplaceEnv(char *var, char *path, unsigned int size_max);
void SetDebugPrivilege(BOOL enable);
BOOL startWith(char* txt, char *search);
char *filetimeToString(FILETIME FileTime, char *str, unsigned int string_size);
char *timeToString(DWORD time, char *str, unsigned int string_size);
char *convertUTF8toUTF16(char *src, DWORD max_size);
char *charToLowChar(char *src);
char *DataToHexaChar(char *data, unsigned int data_size, char *hexa_char, unsigned int hexa_char_size);
char *extractExtFromFile(char *file, char *ext, unsigned int ext_size_max);
char *extractFileFromPath(char *path, char *file, unsigned int file_size_max);
BOOL isDirectory(char *path);

//init functions
void InitSQLStrings();
void InitString();
void InitGlobalConfig(unsigned int params, BOOL debug, BOOL acl, BOOL ads, BOOL sha, BOOL recovery, BOOL local_scan);
void InitGUIConfig(HANDLE hwnd);
void EndGUIConfig(HANDLE hwnd);

//cmd function
int callback_sqlite_CMD(void *datas, int argc, char **argv, char **azColName);
void AddNewSession(BOOL local_only, sqlite3 *db);

//gui interfaces
void redimColumn(HANDLE f,int lv,int column,unsigned int col_size);
void Modify_Style(HANDLE hcomp, long style, BOOL add);
void AddComboBoxItem(HANDLE hcombo, char *txt, DWORD img);
void FileToTreeView(char *c_path);

int CALLBACK CompareStringTri(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3);
void c_Tri(HANDLE hlv, unsigned short colonne_ref, BOOL sort);
void AddtoLV(HANDLE hlv, unsigned int nb_column, LINE_ITEM *item, BOOL select);

void SupDoublon(HANDLE hf,DWORD trv,HTREEITEM htreeParent);
void check_childs_treeview(HANDLE Htree, int state);
void check_treeview(HANDLE Htree, HTREEITEM hitem, int state);
BOOL Ischeck_treeview(HANDLE Htree, HTREEITEM hitem);
HTREEITEM AddItemTreeView(HANDLE Htreeview,char *txt, HTREEITEM hparent);

//file function
void GetOwner(char *file, char* owner,char *rid, char *sid, unsigned int size_max);
void SidtoUser(PSID psid, char *user, char *rid, char *sid, unsigned int max_size);
void GetSIDFromUser(char *user, char* rid, char *sid, unsigned int max_size);
void CleanTreeViewFileView();
void CleanTreeViewFiles();
void AddItemFiletoTreeView(HANDLE htv, char *lowcase_file, char *path, char *global_path);
DWORD  WINAPI AutoSearchFiles(LPVOID lParam);

//gui global functions
char *GetTextFromTrv(HTREEITEM hitem, char *txt, DWORD item_size_max);
void RefreshSizeForm(HWND hwnd, unsigned int mWidth, unsigned int mHeight);
void Global_WM_COMMAND_wParam(WPARAM wParam, LPARAM lParam);
void AddDebugMessage(char *src, char *description, char *state, char*level);

//registry functions
HKEY hkStringtohkey(char *chkey);
void GetRegistryKeyOwner(HKEY hKey, char* owner,char *rid, char *sid, unsigned int size_max);
void ReadKeyUpdate(HKEY ENTETE,char *chemin, char *date, DWORD size_date);
DWORD ReadValue(HKEY hk,char *path,char *value,char *data, DWORD data_size);
long int ReadDwordValue(HKEY hk,char *path,char *value);
void ReadFILETIMEValue(HKEY hk,char *path,char *value, FILETIME *ft);

//registry function for raw files
unsigned int ExtractTextFromPathNb(char *path);
char *ExtractTextFromPath(char *path, char *txt, unsigned int txt_size_max, unsigned int index);
BOOL OpenRegFiletoMem(HK_F_OPEN *hks, char *file);
void CloseRegFiletoMem(HK_F_OPEN *hks);
HBIN_CELL_NK_HEADER *GetRegistryNK(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path);
DWORD GetSubNK(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h,
               DWORD pos_fhbin, unsigned int index, char *subkey, unsigned int subkey_size);
HBIN_CELL_NK_HEADER * GetSubNKtonk(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin, unsigned int index);

DWORD GetValueData(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin,
                   unsigned int index, char *value, unsigned int value_size, char *data, unsigned int data_size);

BOOL Readnk_Value(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                  char *read_value, char *data, unsigned int data_size);
DWORD ReadBinarynk_Value(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                         char *read_value, char *data, unsigned int *data_size);
BOOL Readnk_Class(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                  char *Class, unsigned int Class_size);
BOOL Readnk_Infos(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                  char *last_update, unsigned int last_update_size, char *rid, unsigned int rid_size,char *sid, unsigned int sid_size);
DWORD GetRegistryData(HBIN_CELL_VK_HEADER *vk_h, DWORD taille_fic, char *buffer, DWORD pos_fhbin, char *data, unsigned int data_size);

DWORD GetBinaryRegistryData(HBIN_CELL_VK_HEADER *vk_h, DWORD taille_fic, char *buffer, DWORD pos_fhbin, char *data, unsigned int *data_size);
DWORD GetBinaryValueData(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin,
                         unsigned int index, char *value, unsigned int value_size, char *data, unsigned int *data_size);

//log functions
void TraiterEventlogFileEvt(char * eventfile, sqlite3 *db, unsigned int session_id);
void TraiterEventlogFileLog(char * eventfile, sqlite3 *db, unsigned int session_id);
void TraiterEventlogFileEvtx(char *eventfile, sqlite3 *db, unsigned int session_id);

//subclass for resize direct of components
LRESULT CALLBACK TRV_proc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LST_proc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

//GUI functions
BOOL CALLBACK DialogProc_main(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_conf(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_view(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Scan function
DWORD WINAPI Scan_files(LPVOID lParam);
DWORD WINAPI Scan_log(LPVOID lParam);
DWORD WINAPI Scan_clipboard(LPVOID lParam);
DWORD WINAPI Scan_env(LPVOID lParam);
DWORD WINAPI Scan_disk(LPVOID lParam);
DWORD WINAPI Scan_network(LPVOID lParam);
DWORD WINAPI Scan_route(LPVOID lParam);
DWORD WINAPI Scan_share(LPVOID lParam);
DWORD WINAPI Scan_pipe(LPVOID lParam);
DWORD WINAPI Scan_process(LPVOID lParam);
DWORD WINAPI Scan_android_history(LPVOID lParam);
DWORD WINAPI Scan_chrome_history(LPVOID lParam);
DWORD WINAPI Scan_ie_history(LPVOID lParam);
DWORD WINAPI Scan_firefox_history(LPVOID lParam);
DWORD WINAPI Scan_registry_setting(LPVOID lParam);
DWORD WINAPI Scan_registry_software(LPVOID lParam);
DWORD WINAPI Scan_registry_update(LPVOID lParam);
DWORD WINAPI Scan_registry_service(LPVOID lParam);
DWORD WINAPI Scan_registry_start(LPVOID lParam);
DWORD WINAPI Scan_registry_usb(LPVOID lParam);
DWORD WINAPI Scan_registry_user(LPVOID lParam);
DWORD WINAPI Scan_registry_password(LPVOID lParam);
DWORD WINAPI Scan_registry_userassist(LPVOID lParam);
DWORD WINAPI Scan_registry_mru(LPVOID lParam);
DWORD WINAPI Scan_registry_path(LPVOID lParam);
DWORD WINAPI Scan_guide(LPVOID lParam);
DWORD WINAPI Scan_dns(LPVOID lParam);
DWORD WINAPI Scan_arp(LPVOID lParam);
DWORD WINAPI Scan_task(LPVOID lParam);
DWORD WINAPI Scan_antivirus(LPVOID lParam);
DWORD WINAPI Scan_firewall(LPVOID lParam);

DWORD WINAPI CMDScanNum(LPVOID lParam);
DWORD WINAPI CMDScan(LPVOID lParam);
DWORD WINAPI GUIScan(LPVOID lParam);
DWORD WINAPI StopGUIScan(LPVOID lParam);


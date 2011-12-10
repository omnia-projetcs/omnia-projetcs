//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#define _WIN32_WINNT			0x0501  //fonctionne au minimum sous Windows 2000
#define _WIN32_IE         0x0501  //fonctionne avec ie5 min pour utilisation LVS_EX_FULLROWSELECT
//------------------------------------------------------------------------------
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>   //composants
#include <shlobj.h>     // pour la gestion des browsers

#include <Psapi.h>      //GESTION DES PROCESSUS
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <lm.h>         //pour le chargement direct de DLL +liste des groupes
//------------------------------------------------------------------------------
#define NOM_APPLI             "RtCA v0.1.20 - http://code.google.com/p/omnia-projetcs/"
#define CONF_FILE             "RtCA.ini"

#define TAILLE_TMP            256
#define LIMIT_JOURNAUX        1000
#define MAX_LINE_SIZE         8*MAX_PATH
#define BUFFER_MIN            1024*1024*2     //2mo
#define DIXM                  10*1024*1024    //10mo

#define DATE_SIZE             20

#define ICON_APP              100//icone de l'application

#define ICON_DOSSIER          102//icones pour le treeview !
#define ICON_FICHIER_BIN      103
#define ICON_FICHIER_DWORD    104
#define ICON_FICHIER_TXT      105
#define ICON_FICHIER_UNKNOW   106
#define ICON_FICHIER          107

#define SZ_PART_SYSKEY        0x20

#define ICON_FILE_DOSSIER     0
#define ICON_FILE_BIN         1
#define ICON_FILE_NUM         2
#define ICON_FILE_STR         3
#define ICON_FILE_UNK         4
#define ICON_FILE             5

#define IDR_VERSION           1
//------------------------------------------------------------------------------
BOOL consol_mode;
char console_cmd[MAX_LINE_SIZE];
//------------------------------------------------------------------------------
#define DLG_MAIN               500
#define BT_MAIN_CONF           501
#define BT_MAIN_LOGS           502
#define BT_MAIN_FILES          503
#define BT_MAIN_REGISTRY       504
#define BT_MAIN_PROCESS        505
#define BT_MAIN_STATE          506
#define SB_MAIN                507

#define DLG_CONF              1000
#define TRV_CONF_TESTS        1001
#define CHK_CONF_TOP          1002
#define CHK_CONF_LOGS         1003
#define CHK_CONF_FILES        1004
#define CHK_CONF_REGISTRY     1005
#define CHK_CONF_LOCAL        1006
#define GRP_CONF              1007
#define GRP_CONF_ABOUT        1008
#define ST_CONF_ABOUT         1009
#define BT_CONF_START         1010
#define BT_CONF_EXPORT        1011
#define GRP_CONF_CONF         1012
#define CHK_CONF_REG_GLOBAL_LOCAL        1013
#define CHK_CONF_REG_FILE_RECOVERY       1014
#define CHK_CONF_ENABLE_STATE            1015
#define CHK_CONF_NO_ACL                  1016
#define CHK_CONF_NO_TYPE                 1017
#define CHK_CONF_MD5                     1018
BOOL State_Enable;
BOOL MD5_Enable;
BOOL ACL_Enable;
BOOL Type_Enable;
CRITICAL_SECTION Sync;

#define DLG_LOGS              2000
#define LV_LOGS_VIEW          7000
#define ED_SEARCH             2003
#define BT_VIEW_SEARCH        2005
#define BT_TREE_VIEW          2006
BOOL TV_REGISTRY_VISIBLE;
BOOL TV_FILES_VISBLE;

#define DLG_FILES             3000
#define LV_FILES_VIEW         7001

#define DLG_REGISTRY          4000
#define LV_REGISTRY_VIEW      4004
#define TV_VIEW               4005
#define LV_REGISTRY_CONF      7002
#define LV_REGISTRY_LOGICIEL  7003
#define LV_REGISTRY_MAJ       7004
#define LV_REGISTRY_SERVICES  7005
#define LV_REGISTRY_HISTORIQUE 7006
#define LV_REGISTRY_USB       7007
#define LV_REGISTRY_START     7008
#define LV_REGISTRY_LAN       7009
#define LV_REGISTRY_USERS     7010
#define LV_REGISTRY_PASSWORD  7011
#define LV_REGISTRY_MRU       7012
#define LV_REGISTRY_PATH      7013
#define CB_REGISTRY_VIEW      4015

#define DLG_PROCESS           8000
#define DLG_STATE             8100
#define LV_VIEW               8101
#define LV_VIEW_H             8102

#define DLG_INFO              5000

#define POPUP_TRV_CONF       10000
#define POPUP_TRV_CONF_ADD_FILE 10001
#define POPUP_TRV_CONF_ADD_PATH 10002
#define POPUP_TRV_CONF_REMOVE 10003
#define POPUP_TRV_CONF_CLEAN  10004
#define POPUP_TRV_CONF_AUTO_SEARCH_FILES 10005
#define POPUP_TRV_CONF_P     10006
#define POPUP_TRV_CONF_M     10007
#define POPUP_TRV_CONF_OFP   10008
#define POPUP_TRV_CONF_EXPORT_LIST_FILES  10010

#define POPUP_BACKUP         10020
#define POPUP_REG_CHECK      10021
#define POPUP_CLEAN_REGISTRY 10022
#define POPUP_CLEAN_LOGS     10023

#define POPUP_LV             11000
#define POPUP_LV_S_SELECTION 11001
#define POPUP_LV_S_VIEW      11002
#define POPUP_LV_S_DELETE    11003
#define POPUP_LV_AS_VIEW     11004
#define POPUP_LV_I_VIEW      11005
#define POPUP_LV_I           11006
#define POPUP_LV_C_VIEW      11007
#define POPUP_LV_PROCESS     11008
#define POPUP_LV_STATE       11009

#define POPUP_LV_CP_COL1     11010
#define POPUP_LV_CP_COL2     11011
#define POPUP_LV_CP_COL3     11012
#define POPUP_LV_CP_COL4     11013
#define POPUP_LV_CP_COL5     11014
#define POPUP_LV_CP_COL6     11015
#define POPUP_LV_CP_COL7     11016
#define POPUP_LV_CP_COL8     11017
#define POPUP_LV_CP_COL9     11018
#define POPUP_LV_CP_COL10    11019
#define POPUP_LV_CP_COL11    11020
#define POPUP_LV_CP_COL12    11021
#define POPUP_LV_CP_COL13    11022

#define ADD_DLL_INJECT_REMOTE_THREAD  11023
#define REM_DLL_INJECT_REMOTE_THREAD  11024
#define POPUP_LV_PROPERTIES           11025
#define POPUP_KILL_PROCESS            11026
#define POPUP_DUMP_MEMORY             11027
//#define POPUP_ALL_PROCESS_AND_THREAD  11028

#define POPUP_TV_EXPAND_ALL  11030
#define POPUP_LV_P_VIEW      11031

#define POPUP_TV_F           11040
#define POPUP_TV_CP_COMPLET_PATH 11041
#define POPUP_TV_OPEN        11042
#define POPUP_TV_ADD         11043
#define POPUP_TV_DELETE      11044
#define POPUP_TV_PROPERTIES  11045
#define POPUP_TV_COPY        11046
#define POPUP_TV_R           11047

#define POPUP_LV_INFO        11100

#define POPUPEXPORT          11101
#define POPUP_E_CSV          11102
#define POPUP_E_HTML         11103
#define POPUP_E_XML          11104

//type d'export
#define CSV_TYPE             0
#define HTML_TYPE            1
#define XML_TYPE             2

//------------------------------------------------------------------------------
#define SB_ONGLET_INFO           0
#define SB_ONGLET_LOGS           1
#define SB_ONGLET_FILES          2
#define SB_ONGLET_REGISTRY       3

#define NB_TABL                  8
#define TABL_MAIN                0
#define TABL_CONF                1
#define TABL_LOGS                2
#define TABL_FILES               3
#define TABL_REGISTRY            4
#define TABL_PROCESS             5
#define TABL_STATE               6

#define TABL_INFO                7

#define TAILLE_TXT_BT           15

HWND Tabl[NB_TABL];
HINSTANCE hInst;
unsigned int TABL_ID_VISIBLE;

unsigned int TABL_ID_REG_VISIBLE;

#define LV_NB                        32
#define LV_LOGS_VIEW_NB_COL           0
#define LV_FILES_VIEW_NB_COL          1
#define LV_REGISTRY_CONF_NB_COL       2
#define LV_REGISTRY_LOGICIEL_NB_COL   LV_REGISTRY_CONF_NB_COL+1
#define LV_REGISTRY_MAJ_NB_COL        LV_REGISTRY_CONF_NB_COL+2
#define LV_REGISTRY_SERVICES_NB_COL   LV_REGISTRY_CONF_NB_COL+3
#define LV_REGISTRY_HISTORIQUE_NB_COL LV_REGISTRY_CONF_NB_COL+4
#define LV_REGISTRY_USB_NB_COL        LV_REGISTRY_CONF_NB_COL+5
#define LV_REGISTRY_START_NB_COL      LV_REGISTRY_CONF_NB_COL+6
#define LV_REGISTRY_LAN_NB_COL        LV_REGISTRY_CONF_NB_COL+7
#define LV_REGISTRY_USERS_NB_COL      LV_REGISTRY_CONF_NB_COL+8
#define LV_REGISTRY_PASSWORD_NB_COL   LV_REGISTRY_CONF_NB_COL+9
#define LV_REGISTRY_MRU_NB_COL        LV_REGISTRY_CONF_NB_COL+10
#define LV_REGISTRY_PATH_NB_COL       LV_REGISTRY_CONF_NB_COL+11
#define LV_INFO_VIEW_NB_COL          28
#define LV_PROCESS_VIEW_NB_COL       29
#define LV_STATE_VIEW_NB_COL         30
#define LV_STATE_H_VIEW_NB_COL       31
unsigned int NB_COLONNE_LV[LV_NB];

#define TRV_CATEGORIES          3
#define TRV_LOGS                0
#define TRV_FILES               1
#define TRV_REGISTRY            2
HTREEITEM TRV_HTREEITEM [TRV_CATEGORIES];
//------------------------------------------------------------------------------
//structure pour les recherches de fichiers/ext...
typedef struct search_c
{
  char c[MAX_PATH];
}SEARCH_C;
HFONT PoliceGras; //bouton en gras !
//------------------------------------------------------------------------------
long int pos_search_logs;
long int pos_search_files;
long int pos_search_registry;
//------------------------------------------------------------------------------
//pour le tri de colonne
typedef struct stri
{
  HANDLE hwndFrom;
  unsigned short colonne_ref;
  unsigned short nb_colonne;
} STRI;
//------------------------------------------------------------------------------
//gestion des types de fichiers
unsigned int nb_ext_doc;
unsigned int nb_ext_med;
unsigned int nb_ext_img;
unsigned int nb_ext_exe;
unsigned int nb_ext_crypt;
unsigned int nb_ext_mail;
unsigned int nb_ext_other;

unsigned int nb_ext_source_code;
unsigned int nb_ext_lnk;
unsigned int nb_ext_compresse;
unsigned int nb_ext_web;
unsigned int nb_ext_configuration;
unsigned int nb_ext_bdd;
unsigned int nb_ext_audit;

#define NB_MAX_CAR_FILE_EXT     6
#define NB_MAX_EXT_TYPE 256

typedef struct ext_type
{
  char ext[NB_MAX_CAR_FILE_EXT];
}EXT_TYPE;

EXT_TYPE ext_doc[NB_MAX_EXT_TYPE],
         ext_med[NB_MAX_EXT_TYPE],
         ext_img[NB_MAX_EXT_TYPE],
         ext_exe[NB_MAX_EXT_TYPE],
         ext_crypt[NB_MAX_EXT_TYPE],
         ext_mail[NB_MAX_EXT_TYPE],
         ext_other[NB_MAX_EXT_TYPE],
         ext_source_code[NB_MAX_EXT_TYPE],
         ext_link[NB_MAX_EXT_TYPE],
         ext_compresse[NB_MAX_EXT_TYPE],
         ext_web[NB_MAX_EXT_TYPE],
         ext_configurationr[NB_MAX_EXT_TYPE],
         ext_bdd[NB_MAX_EXT_TYPE],
         ext_audit[NB_MAX_EXT_TYPE];
//------------------------------------------------------------------------------
//type_OS           //ann�e de sorti
#define OS_W95      1995
#define OS_NT4      1996
#define OS_W98      1998
#define OS_ME       1999
#define OS_2000     2000
#define OS_XP       2002
#define OS_2003     2003
#define OS_VISTA    2007
#define OS_2008     2008
#define OS_SEVEN    2009
int type_OS;
//------------------------------------------------------------------------------
//gestion du chargement de fichier de base de registre brute
#define EXTR_MAX_PATH 4*MAX_PATH
#define type_val_dword          0x01
#define type_val_sdword         0x02
#define type_val_longlong       0x03
#define type_val_chaine         0x04
#define type_val_bin            0x05

#define REG_ITEM_MAX_NAME_SIZE  MAX_PATH*2
#define REG_ITEM_MAX_PATH_SIZE  2048
#define REG_ITEM_MAX_COMPLET_PATH_SIZE  MAX_PATH*2
typedef struct val_item
{
  char name[REG_ITEM_MAX_NAME_SIZE];
  DWORD data_type;
  DWORD data_size;
  DWORD ls_offset;
  char value[MAX_LINE_SIZE];
}VAL_ITEM;

//r�f�rences utilis�es dans les donn�es de base de registre
DWORD nb_ref_items;
DWORD nb_alloc_items;
//------------------------------------------------------------------------------
//gestion de l'ajout d'item et du scan
typedef struct line_item
{
  char c[MAX_LINE_SIZE];
}LINE_ITEM;
#define SIZE_UTIL_ITEM    13

#define MAX_PROC_LINE_ITEM_SIZE 20
typedef struct line_proc_item
{
  char protocol[MAX_PROC_LINE_ITEM_SIZE];
  char state[MAX_PROC_LINE_ITEM_SIZE];
  char IP_src[MAX_PROC_LINE_ITEM_SIZE];
  char IP_dst[MAX_PROC_LINE_ITEM_SIZE];
  char Port_src[MAX_PROC_LINE_ITEM_SIZE];
  char Port_dst[MAX_PROC_LINE_ITEM_SIZE];
}LINE_PROC_ITEM;


typedef struct ThreadExport
{
  char path[MAX_PATH];
  unsigned int id_tabl;
  int lv;
  unsigned short nb_colonne;
}THREADEXPORT;

DWORD nb_items;
//------------------------------------------------------------------------------
typedef struct reg_ref_search
{
  char v[MAX_PATH];
}REG_REF_SEARCH;

#define NB_MAX_REF_SEARCH_AUTORUN       17
REG_REF_SEARCH ref_autorun_search[NB_MAX_REF_SEARCH_AUTORUN];

#define NB_MAX_REF_SEARCH_HIDDENLOG      1
REG_REF_SEARCH ref_hiddenlog_search[NB_MAX_REF_SEARCH_HIDDENLOG];

#define NB_MAX_REF_SEARCH_NETWORK        1
REG_REF_SEARCH ref_network_search[NB_MAX_REF_SEARCH_NETWORK];

#define NB_MAX_REF_SEARCH_SOFTWARE       1
REG_REF_SEARCH ref_software_search[NB_MAX_REF_SEARCH_SOFTWARE];
#define NB_MAX_REF_SEARCH_SOFTWARE_VAR   13
REG_REF_SEARCH ref_software_var_search[NB_MAX_REF_SEARCH_SOFTWARE_VAR];

#define NB_MAX_REF_SEARCH_UPDATE         2
REG_REF_SEARCH ref_update_search[NB_MAX_REF_SEARCH_UPDATE];

#define NB_MAX_REF_SEARCH_USB            1
REG_REF_SEARCH ref_usb_search[NB_MAX_REF_SEARCH_USB];

#define NB_MAX_REF_SEARCH_SERVICE        1
REG_REF_SEARCH ref_service_search[NB_MAX_REF_SEARCH_SERVICE];
#define NB_MAX_REF_SEARCH_SERVICE_VAR    5
REG_REF_SEARCH ref_service_var_search[NB_MAX_REF_SEARCH_SERVICE_VAR];

#define NB_MAX_REF_SEARCH_USERS          1
REG_REF_SEARCH ref_users_search[NB_MAX_REF_SEARCH_USERS];

#define NB_MAX_REF_SEARCH_CONF           8
REG_REF_SEARCH ref_conf_search[NB_MAX_REF_SEARCH_CONF];
#define NB_MAX_REF_SEARCH_CONF_VAR      54
REG_REF_SEARCH ref_conf_var_search[NB_MAX_REF_SEARCH_CONF_VAR];

#define NB_MAX_REF_SEARCH_MRU           30
REG_REF_SEARCH ref_mru_search[NB_MAX_REF_SEARCH_MRU];
#define NB_MAX_REF_SEARCH_MRU_VAR        NB_MAX_REF_SEARCH_MRU*2
REG_REF_SEARCH ref_mru_var_search[NB_MAX_REF_SEARCH_MRU_VAR];

#define NB_MAX_REF_SEARCH_MRU_WILD       3
REG_REF_SEARCH ref_mru_wild_search[NB_MAX_REF_SEARCH_MRU_WILD];
#define NB_MAX_REF_SEARCH_MRU_WILD_VAR   NB_MAX_REF_SEARCH_MRU_WILD*2
REG_REF_SEARCH ref_mru_var_wils_search[NB_MAX_REF_SEARCH_MRU_WILD_VAR];


char current_key_test[MAX_LINE_SIZE];
long int item_ref_current_key_test;
//------------------------------------------------------------------------------
#define HBIN_CELL_INC_PADDING 16
#define HBIN_CELL_DEFAULT_PADDING_SIZE  8
//------------------------------------------------------------------------------
//header du fichier
typedef struct regf_header{
  long int id;            // "regf" = 0x66676572
  long int sec_number1;
  long int sec_number2;
  FILETIME timestamp;     //Date de derni�re modification
  long int maj_version;
  long int min_version;
  long int file_type;     //0x0000 = normal, 0x0001 = transaction log
  long int format;
  long int sync;
  long int bin_data_size;
  long int control;       //0x0001
  char name[460];          //Name Hive en Unicode d�but a 0x30 et fin avec 0x0000
  long int sum;
} REGF_HEADER;
//------------------------------------------------------------------------------
//header hbin (taille : 32octets)
#define HBIN_HEADER_SIZE  0x20
typedef struct hbin_header{
  long int id;          // "hbin" = 0x6E696268
  DWORD hive_bin_last_header; //� partir de l'offset 0x1000+header size
  DWORD hive_bin_size;
  long int reserved1;
  long int reserved2;
  FILETIME timestamp;
  long int sum;
}HBIN_HEADER;
//------------------------------------------------------------------------------
//pre header de data
typedef struct hbin_cell_pre_header{
  char size[4];   //normalement 0x??FFFFFF
  short type; /*
              Sub keys list : lf = 0x666C , lh = 0x686C , li = 0x696C , ri = 0x6972
              Named key     : nk = 0x6B6E
              Security key  : sk = 0x6B73
              Value key     : vk = 0x6B76
              Data block key: db = 0x6264
              */
}HBIN_CELL_PRE_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_LF_DATA_SIZE  8 // 4 + 4 de taille
typedef struct hbin_cell_lf_data_header{
  DWORD nk_of;                //emplacement de la cl� nk � partir de l'octet de d�but de la structure hbin
  char name[4];               //les 4premi�res lettres de la chaine de NK li�e
}HBIN_CELL_LF_DATA_HEADER;
#define HBIN_CELL_LF_SIZE  8 // 4 + 4 de taille
typedef struct hbin_cell_lf_header{
  DWORD size;

  short type;                 //0x666C : debut standard de la zone de DATA
  short nb_keys;              //nombre de cl�
  HBIN_CELL_LF_DATA_HEADER hb_c[1];
}HBIN_CELL_LF_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_LH_SIZE  8 // 8 + 4 de taille
typedef struct hbin_cell_lh_header{
  DWORD size;

  short type;                 //0x686C : debut standard de la zone de DATA
  short nb_keys;              //nombre de cl�
}HBIN_CELL_LH_HEADER;
#define HBIN_CELL_LH_DATA_SIZE  8 // 4 + 4 de taille
typedef struct hbin_cell_lh_data_header{
  DWORD nk_of;                //emplacement de la cl� nk � partir de l'octet de d�but de la structure hbin
  char name[4];               //les 4premi�res lettres de la chaine de NK li�e
}HBIN_CELL_LH_DATA_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_LI_SIZE  12 // 8 + 4 de taille
typedef struct hbin_cell_li_header{
  DWORD size;

  short type;                 //0x696C : debut standard de la zone de DATA
  short nb_keys;              //nombre de cl�
  DWORD nk_of;                //emplacement de la cl� nk
}HBIN_CELL_LI_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_RI_SIZE  12 // 8 + 4 de taille
typedef struct hbin_cell_ri_header{
  DWORD size;

  short type;                 //0x6972 : debut standard de la zone de DATA
  short nb_keys;              //nombre de cl�
  DWORD li_of;                //emplacement de la cl� li
}HBIN_CELL_RI_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_NK_SIZE  80 // 76 + 4 de taille
typedef struct hbin_cell_nk_header{
  DWORD size;                 // 0xFFFFFFFF - size + le padding (size%8) = la taille

  short type;                 //0x6B6E : debut standard de la zone de DATA
  short format;               //root key = 0x2C, sinon 0x20, pas toujours fiable bien souven 0x20 m�me si root key
  FILETIME last_write;        //derni�re modification
  long int unknow1;
  DWORD parent_key;           //emplacement de la cl� parent
  DWORD nb_subkeys;           //nombre de sous cl�
  DWORD nb_vl_subkeys;        //nombre de sous cl� volatile
  DWORD lf_offset;           //-1 ou 0xffffffff = vide

  DWORD lsk_vol_offset;       //-1 ou 0xffffffff = vide
                              //emplacement de la liste de sk volatile � partir du d�but du hbin_data
  DWORD nb_values;
  DWORD val_ls_offset;
  DWORD sk_offset;            //-1 ou 0xffffffff = vide //emplacement de la liste de sk � partir du d�but du hbin_data
  DWORD class_name_offset;    //classe name offset -1 ou 0xffffffff = vide
  DWORD sbk_name_max_size;
  DWORD sbk_name_cl_max_size;
  DWORD value_name_max_size;
  DWORD value_data_max_size;
  DWORD unknow2;
  short key_name_size;
  short class_name_size;
}HBIN_CELL_NK_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_SK_SIZE  24 // 20 + 4 de taille
typedef struct hbin_cell_sk_header{
  DWORD size;

  short type;                 //0x6B73 : debut standard de la zone de DATA
  short unknow;
  DWORD last_sk;
  DWORD next_sk;
  DWORD ref_count;
  DWORD sk_size;
  //... NT security descriptor

}HBIN_CELL_SK_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_VK_SIZE  24 // 20 + 4 de taille
#define HBIN_CELL_VK_DATA_PADDING_SIZE  4
typedef struct hbin_cell_vk_header{
  DWORD size;

  short type;                 //0x6B76 : debut standard de la zone de DATA
  unsigned short name_size;
  unsigned short data_size;
  //unsigned short unknow;
  union
  {
    DWORD data_offset;
    char cdata_offset[4];
  };

  DWORD data_type;
  short flag;
  short padding;
  char value[1];
  //... Value Name
}HBIN_CELL_VK_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_ITEM_LS 4
typedef struct s_item_ls
{
  DWORD val_of;
}S_ITEM_LS;
//------------------------------------------------------------------------------
#define DATA_USER_HEADER_SIZE  8
 /*
  typedef struct data_user
  {
    char valid;       //0x01
    char nb_sid;      //0x05 ou 0x01
    char padding[5];  //0x0000 0000 00
    char type;        //0x05
    char sid[1];      //par defaut d�coup� en section de 2octet *nb_part
  }DATA_USER;
  */
//------------------------------------------------------------------------------
PVOID OldValue_W64b;
//------------------------------------------------------------------------------
//MD5
typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

/* Initialize the algorithm. */
void md5_init(md5_state_t *pms);

/* Append a string to the message. */
void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);

/* Finish the message and return the digest. */
void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
//------------------------------------------------------------------------------
BOOL syskey_;
char sk_c[MAX_LINE_SIZE];

BOOL secret_;
BYTE secret_c[MAX_LINE_SIZE];
//------------------------------------------------------------------------------
DWORD nb_process_SE_DEBUG;
//------------------------------------------------------------------------------
//fonctions syst�mes standard
void InitConfig(HWND hwnd);
void EndConfig();
void ErrorExit(LPTSTR lpszFunction);
void redimColumn(HANDLE f,int lv,int column,unsigned int col_size);

//fonctions de traitement
void GetACLS(char *file, char *acls, unsigned int res_taille_max, char* proprio, unsigned int prop_taille_max);
void GetMyDirectory(char *path, unsigned int TAILLE);
BOOL Compare(char*src,char*dst);
unsigned long int Contient(char*data,char*chaine);
void SupChar(char*src,char*dst, char separateur, unsigned int size);
void replace_to_char(char *buffer, unsigned long int taille, char a);
void ViewTabl(DWORD tabl_id);
void MiseEnGras(HWND hwnd,unsigned int bt,BOOL etat);
void CopyData(HANDLE hlv, DWORD ligne, unsigned short colonne);
void DessinerBouton(LPDRAWITEMSTRUCT lpdis);
void FileToTreeView(char *c_path);
void AddRep();
void AddFile();
void GetItemTxt(HTREEITEM hitem,HANDLE hdlg, int treeview,char *txt, unsigned int size);
void vncpwd(unsigned char *pwd, int bytelen);
void SidtoUser(PSID psid, char *user, char *sid, unsigned int taille_user,unsigned int taille_sid, BOOL onlySID);
void SupDoublon(HANDLE hf,DWORD trv,HTREEITEM htreeParent);
void SB_add(unsigned short colonne, char *desc, DWORD place, DWORD ref);
void SB_add_T(unsigned short colonne, char *desc);
void LVAllSearch(HANDLE hlv, unsigned short nb_col, char *search);
DWORD LVSearch(HANDLE hlv, unsigned short nb_col, char *search, DWORD start_id);
void LVDelete(unsigned int id_tabl, int lv);
void LVtoMessage(HANDLE hlv, unsigned short colonne);
void reg_liste_DataValeurSpec(HKEY hkey,char *chkey,char *path,char *exclu,char* description,HANDLE hlv);

DWORD AddToLVICON(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne, int img);
void AddToLV_Registry(LINE_ITEM *item);
void AddToLV_Registry2(char *date, char *user, char *from, char *data);
DWORD AddToLV_log(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
DWORD AddToLV_File(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
DWORD AddToLV(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
void AddToLVRegBin(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
HTREEITEM AjouterItemTreeView(HANDLE hdlg, int treeview, char *texte, HTREEITEM hparent);
HTREEITEM AjouterItemTreeViewFile(HANDLE hdlg, int treeview, char *texte, HTREEITEM hparent,DWORD img_id);
HTREEITEM AjouterItemTreeViewRep(HANDLE hdlg, int treeview, char *texte, HTREEITEM hparent);
void EnumProcess(HANDLE hlv, unsigned short nb_colonne);
BOOL AdministratorGroupName(char *group_name, unsigned short gn_max_size);

void TreeExport(HANDLE htree);
void ExportLVSelecttoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVtoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVSelecttoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVtoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVSelecttoXML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVtoXML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVColto(char *path, unsigned int id_tabl, int lv, unsigned short col); //pwdump ou col sp�cifique !!
void LVSaveAll(unsigned int id_tabl, int lv, unsigned short nb_colonne, BOOL selection_only,BOOL pwdump,BOOL front_registry);
void TraiterPopupSave(WPARAM wParam, LPARAM lParam, HWND hwnd, unsigned int nb_col);

BOOL TestSIDShadowCopy(HANDLE hlv, char *sid);
unsigned long HexaToDec(char *hexa,unsigned int taille);
void ROTTOASCII(char *donnee,char *resultat,int taille);
void FiltreRegData(LINE_ITEM *item);
void AnalyseFichierReg(char *fic);
void AnalyseFichierRegBin(char *fic);
void AnalyseFichierRegBin2(char *fic);
void AnalyseFichierRegWCEBin(char *fic);

HANDLE h_Tc_Tri;
BOOL Tc_TriStart;
void c_Tri(HANDLE hListView, unsigned short colonne_ref, unsigned short nb_colonne);

//popup treeview
void GetItemPath(HANDLE hparent, DWORD treeview, HTREEITEM hitem, char *path, DWORD max_size);
void CopyTVData(HANDLE hparent, DWORD treeview, HTREEITEM hitem);

//registre
int LireGValeur(HKEY ENTETE,char *chemin,char *nom,char *Valeur);
void OpenRegeditKey(char *key);
void CheckRegistryFile();

//process
void SetDebugPrivilege(BOOL enable);
DWORD WINAPI DumpProcessMemory(LPVOID lParam);
void ReadProcessInfo(HANDLE hlv, DWORD id);
BOOL WINAPI DllInjecteurA(DWORD dwPid,char * szDLLPath);
BOOL WINAPI DllEjecteurA(DWORD dwPid,char * szDLLPath);
void KilllvProcess(HANDLE hlv, DWORD id, unsigned int column);

//fonction d'extractions syskey + hash
DWORD WINAPI BackupRegFile(LPVOID lParam);
char HexaToDecS(char *hexa);
int TraiterUserDataFromSAM_V(LINE_ITEM *item);
void TraiterUserDataFromSAM_F(LINE_ITEM *item);
void TraiterGroupDataFromSAM_C(LINE_ITEM *item, HANDLE hlv);
void DecodeSAMHash(char *sk,char *hsc, int rid, char *user, BYTE *b_f);
BOOL registry_users_extract();
void registry_syskey(HANDLE hlv, char*sk);
void AddLvSyskey(char *path, char *cJD, char *cSkew1, char *cGBG, char *cData, char *sk);
void SyskeyExtract(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *path);

//event logs
BOOL EventIdtoDscr(unsigned int eventId, char *source, char *result, unsigned short max_size);
void TraiterlogFile(char *path, HANDLE hlv);
void TraiterEventlogFileEvt(char * eventfile, HANDLE hlv);
void TraiterEventlogFileEvtx(char *eventfile, HANDLE hlv);

//clean
void clean_registry();
void clean_logs();

//gestion des recherches automatique des fichiers � analyser
HANDLE h_AutoSearchFiles;
BOOL AutoSearchFilesStart;
DWORD WINAPI AutoSearchFiles(LPVOID lParam);

//export des r�sultats
HANDLE h_Export;
BOOL ExportStart;
DWORD WINAPI Export(LPVOID lParam);
DWORD WINAPI csvImport(LPVOID lParam);

//Scan
BOOL ScanStart;
HANDLE h_scan_logs;
HANDLE h_scan_files;
HANDLE h_scan_registry;
DWORD WINAPI Scan(LPVOID lParam);
DWORD WINAPI StopScan(LPVOID lParam);
DWORD WINAPI Scan_logs(LPVOID lParam);
DWORD WINAPI Scan_files(LPVOID lParam);
DWORD WINAPI Scan_registry(LPVOID lParam);

//gestion des messages sur les onglets
BOOL CALLBACK DialogProc_conf(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_logs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_files(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_registry(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_process(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_state(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_info(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//------------------------------------------------------------------------------
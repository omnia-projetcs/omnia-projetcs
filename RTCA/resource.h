//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http:\\omni.a.free.fr
// Licence              : GPL V3
//------------------------------------------------------------------------------
// Ressources :
//    - format du fichier SAM : http://chntpw.sourcearchive.com/documentation/0.99.5/sam_8h-source.html
//      http://www.hotline-pc.org/registre-securite.htm
//      http://webcache.googleusercontent.com/search?q=cache:pnxWpMqWISEJ:www.beginningtoseethelight.org/ntsecurity/+\SAM\Domains\Account\Users+F+V+account+logon+count&cd=2&hl=fr&ct=clnk&gl=fr&source=www.google.fr
//      http://msdn.microsoft.com/en-us/library/aa379649%28v=vs.85%29.aspx
//      différents types de data : http://msdn.microsoft.com/en-us/library/windows/desktop/ms724884%28v=vs.85%29.aspx
//    - exemple d'utilisation des API pour les journaux  : http://msdn.microsoft.com/en-us/library/aa363680%28v=vs.85%29.aspx
//    - format des fichiers evt : http://msdn.microsoft.com/en-us/library/bb309022%28v=vs.85%29.aspx
//    - BinXML (evtx) :  http://msdn.microsoft.com/en-us/library/cc231334%28v=prot.10%29.aspx (p65,134,142)
//    - BinXML (evtx) :  http://computer.forensikblog.de/en/2007/07/evtx_event_record.html
//    - BinXML (evtx) :  http://msdn.microsoft.com/en-us/library/cc231287%28v=prot.10%29.aspx
//    - BinXML (evtx) :  http://download.microsoft.com/download/9/5/e/95ef66af-9026-4bb0-a41d-a4f81802d92c/%5BMS-EVEN6%5D.pdf
//    - évènements code id (NT) : http://support.microsoft.com/kb/299475/en-us  et http://support.microsoft.com/kb/301677/en-us
//    - conversion des Sessions ID : http://www.eventid.net/display.asp?eventid=115&source= et http://www.ultimatewindowssecurity.com/securitylog/encyclopedia/default.aspx?i=j
//      http://www.ultimatewindowssecurity.com/securitylog/quickref/download.aspx http://www.myeventlog.com/
//      http://kb.monitorware.com/kbeventdb-top50.html
//      http://searchenterprisedesktop.techtarget.com/definition/Error-messages-for-Windows-XP-Pro?Offer=WCMlbscript
//    - outil de gestion des eventlogs :http://ctxadmtools.musumeci.com.ar/SearchEvent/SearchEvent11.html
// outils Citrix: http://ctxadmtools.musumeci.com.ar/CtxAdmTools/CtxAdmTools_Pack.html

//   doc ms : http://www.microsoft.com/technet/support/ee/ee_advanced.aspx

/*
PRIORITES !!!!
  - listeview complémentaire avec déclencheur  d'utilisation !!! : AddToLV_log(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne)
  - revoir la lstv avec la durée par jour par déclencheur ^^
  - dans la partie state :  Toutes les catégories de registre !!!
  - gestion de state avec l'import de fichiers ^^

Forensic urls :
http://isc.sans.edu/diary.html?storyid=6961
http://www.cert.org/forensics/tools/include/all_announcements.html
http://code.google.com/p/winforensicaanalysis/downloads/list
http://code.google.com/p/creddump/
outil pour copier le registre : http://www.larshederer.homepage.t-online.de/erunt/
http://www.e-fense.com/h3-enterprise.php


      * onglet : process :
        - liste des fonctions utilisés
        - décortication des drivers chargés dans les dlls kernels ^^
        - hook possible sur son fonctionnement : registre, file, réseau, pipe
        - non compatible Wine !!!!
        - ajout d'une console type DOS (inspirer de LUS !!!)
        - ajouter un open path process !! (avec gestion des début chaotiques et variables ^^

      * onglet : state :
        - mise à jour lors de l'ajout de journaux,userassits et éléments de base de registre ! : AddToLV_log (user assist faire une fonction complémentaire)
        - horaires d'utilisation par périodes, onglets :
          * revoir la gestion par déclencheur et non dates!! (id, etc..)
          * durée prenant en compte l'année + nb de jours/mois, etc..

      * onglet : registry :
        * Ajout de la configuration Wifi : même ligne que le réseau
        * revoir la gestion du Wifi (bug !!!)
        * ajouter la date de création/update des comptes et groupes avec la date de création du répertoire ^^
        * update pour la partie réseau aussi
        * user : vérifier si le même mot de passe !!!
        * treeviewajouter au popup : suppression/modification de valeur/ajout (local ou fichier)
        * ajout du secret dans l'onglet configuration + option de déchiffrement à partir de syskey externe
        * revoir le test des shadow copy pour ne pas prendre en compte les groupes ^^
        * export des certificats

        * prise en charge des fichiers de registre windowsmobile
        * export des clées lue au format REG
        * import de fichier reg pour vérification locale
        * import des hahs et compte de fichier et ad windows !!! : %SystemRoot%\ntds\NTDS.DIT  << base pour AD Windows 2000
        * évolution des relevés de configuration : précision si une variable n'existe pas !!!
        - possibilité d'importer une syskey (en binaire ou ascii) et de décoder les hash en md5
        - base de 1000 hash standard pour deviner le mot de passe !!!
        -----
        * lecture de fichier hv
        * recherche + ajout de fichier hv

      .???? nouvel onglet ?
        * manque historique de navigation : IE/Firefox/Chrome/Safari/Opera
        * manque mails : outlook/express/ThunHKEY_CURRENT_USER\Software\Microsoft\MediaPlayer\Player\RecentFileListderbird/LotusNotes
        * identification de logiciels de peer to peer ?

        gestion des fichiers .vol pour windows mobile

        "DirsToExclude"=multi_sz:"\\mxip_initdb.vol",
"\\mxip_system.vol",
"\\mxip_notify.vol",
"\\mxip_lang.vol",
"\\cemail.vol",
"\\pim.vol",
"\\Documents and Settings\\default\\user.hv",
"\\Documents and Settings\\system.hv",
"\\Windows\\Profiles\\guest\\Temporary Internet Files\\",
"\\Windows\\Profiles\\guest\\Cookies\\",
"\\Windows\\Profiles\\guest\\History\\",
"\\Windows\\Activesync\\"

      * onglet : logs :
        * ajouter de nouveaux ID !!!!
          http://www.eventlogxp.com
          http://blogs.technet.com/b/askperf/archive/2007/10/12/windows-vista-and-exported-event-log-files.aspx
          http://computer.forensikblog.de/en/2011/06/evtx_parser_1_0_8.html#more
          http://www.net-security.org/dl/insecure/INSECURE-Mag-18.pdf
          http://support.microsoft.com/kb/299475/en-us
          http://support.microsoft.com/kb/301677/en-us
          le type d'event peut êtr exploité comme suit : http://msdn.microsoft.com/en-gb/library/aa363651.aspx
          revoir tous les EventID : http://computer.forensikblog.de/en/2011/06/mac_os_x_memory_analysis_with_volafox.html#mo
        * utilisation d'une base externe fichier csv ?

      * onglet files : ajout d'autres types d'extension
        - ajouter en option la lecture des méta datas dans le popup/option de configuration (option)
        - acl complètes (option)
        - hash des fichiers (option)
        - récupération de certains fichiers systèmes automatiquement (applications en cours d'exécution + drivers + services)
        - ajout import + décodage des

      * Configuration :
        * utiliser des threads pour les parties sauvegarde et supprimer !! (surtout en cas de grand nombre d'item !)
        * export multi format (docx)
        * sauvegarde en contenanaire zip + possible mdp + de tous les exe + base SAM importants ^^
        * icone de l'application de grande qualité !!!
        * ajouter en mode console l'import de fichier SAM+SYSTEM
        * compilation en bas64

        - multi sélection pour suppression (liste de fichiers-treeview)
        - ajout de l'import de fichier log standards Windows
        - revérifier les zones de copie de data !!! (plantage potentiel)
---------------------------------------------------------------------------------------------
ABANDONNEES :
        - ajouter la possibiliter de faire un dd d'un disque sans redémarre
        - possibilité de faire un dd de la mémoire ^^  : \\.\PhysicalMemory
        - possibilité de faire un dd d'un support externe (type usb, disk, etc...)
        http://www.chrysocome.net/dd
        //building pe image :http://technet.microsoft.com/en-us/library/cc766066%28WS.10%29.aspx
      FileName := '\\.\' + Drive + ':';
      h := CreateFile(PChar(FileName), GENERIC_READ, FILE_SHARE_READ, nil, OPEN_EXISTING, 0, 0);
---------------------------------------------------------------------------------------------

char tmp_t[MAX_PATH];
sprintf(tmp_t,"pevlr->Length: %d",pevlr->Length);
MessageBox(0,tmp_t,"TEST!",MB_OK|MB_TOPMOST);
*/
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
//------------------------------------------------------------------------------
#define NOM_APPLI             "RtCA v0.26 - http:\\\\omni.a.free.fr"
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

#define SZ_PART_SYSKEY  0x20

#define ICON_FILE_DOSSIER  0
#define ICON_FILE_BIN 1
#define ICON_FILE_NUM 2
#define ICON_FILE_STR 3
#define ICON_FILE_UNK 4
#define ICON_FILE     5
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
BOOL State_Enable;
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
#define POPUP_BACKUP         10011

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

#define POPUP_TV_EXPAND_ALL  11030

#define POPUP_TV_F           11040
#define POPUP_TV_CP_COMPLET_PATH 11041
#define POPUP_TV_OPEN        11042
#define POPUP_TV_ADD         11043
#define POPUP_TV_DELETE      11044
#define POPUP_TV_PROPERTIES  11045
#define POPUP_TV_COPY        11046
#define POPUP_TV_R           11047

#define POPUP_LV_INFO        11100
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
//type_OS           //année de sorti
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

//références utilisées dans les données de base de registre
DWORD nb_ref_items;
DWORD nb_alloc_items;
//------------------------------------------------------------------------------
//gestion de l'ajout d'item et du scan
typedef struct line_item
{
  char c[MAX_LINE_SIZE];
}LINE_ITEM;
#define SIZE_UTIL_ITEM    13

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

#define NB_MAX_REF_SEARCH_HIDDENLOG      2
REG_REF_SEARCH ref_hiddenlog_search[NB_MAX_REF_SEARCH_HIDDENLOG];

#define NB_MAX_REF_SEARCH_NETWORK        1
REG_REF_SEARCH ref_network_search[NB_MAX_REF_SEARCH_NETWORK];

#define NB_MAX_REF_SEARCH_SOFTWARE       1
REG_REF_SEARCH ref_software_search[NB_MAX_REF_SEARCH_SOFTWARE];
#define NB_MAX_REF_SEARCH_SOFTWARE_VAR   4
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

#define NB_MAX_REF_SEARCH_CONF           7
REG_REF_SEARCH ref_conf_search[NB_MAX_REF_SEARCH_CONF];
#define NB_MAX_REF_SEARCH_CONF_VAR      46
REG_REF_SEARCH ref_conf_var_search[NB_MAX_REF_SEARCH_CONF_VAR];

#define NB_MAX_REF_SEARCH_MRU           27
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
  FILETIME timestamp;     //Date de dernière modification
  long int maj_version;
  long int min_version;
  long int file_type;     //0x0000 = normal, 0x0001 = transaction log
  long int format;
  long int sync;
  long int bin_data_size;
  long int control;       //0x0001
  char name[460];          //Name Hive en Unicode début a 0x30 et fin avec 0x0000
  long int sum;
} REGF_HEADER;
//------------------------------------------------------------------------------
//header hbin (taille : 32octets)
#define HBIN_HEADER_SIZE  0x20
typedef struct hbin_header{
  long int id;          // "hbin" = 0x6E696268
  DWORD hive_bin_last_header; //à partir de l'offset 0x1000+header size
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
  DWORD nk_of;                //emplacement de la clé nk à partir de l'octet de début de la structure hbin
  char name[4];               //les 4premières lettres de la chaine de NK liée
}HBIN_CELL_LF_DATA_HEADER;
#define HBIN_CELL_LF_SIZE  8 // 4 + 4 de taille
typedef struct hbin_cell_lf_header{
  DWORD size;

  short type;                 //0x666C : debut standard de la zone de DATA
  short nb_keys;              //nombre de clé
  HBIN_CELL_LF_DATA_HEADER hb_c[1];
}HBIN_CELL_LF_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_LH_SIZE  8 // 8 + 4 de taille
typedef struct hbin_cell_lh_header{
  DWORD size;

  short type;                 //0x686C : debut standard de la zone de DATA
  short nb_keys;              //nombre de clé
}HBIN_CELL_LH_HEADER;
#define HBIN_CELL_LH_DATA_SIZE  8 // 4 + 4 de taille
typedef struct hbin_cell_lh_data_header{
  DWORD nk_of;                //emplacement de la clé nk à partir de l'octet de début de la structure hbin
  char name[4];               //les 4premières lettres de la chaine de NK liée
}HBIN_CELL_LH_DATA_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_LI_SIZE  12 // 8 + 4 de taille
typedef struct hbin_cell_li_header{
  DWORD size;

  short type;                 //0x696C : debut standard de la zone de DATA
  short nb_keys;              //nombre de clé
  DWORD nk_of;                //emplacement de la clé nk
}HBIN_CELL_LI_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_RI_SIZE  12 // 8 + 4 de taille
typedef struct hbin_cell_ri_header{
  DWORD size;

  short type;                 //0x6972 : debut standard de la zone de DATA
  short nb_keys;              //nombre de clé
  DWORD li_of;                //emplacement de la clé li
}HBIN_CELL_RI_HEADER;
//------------------------------------------------------------------------------
#define HBIN_CELL_NK_SIZE  80 // 76 + 4 de taille
typedef struct hbin_cell_nk_header{
  DWORD size;                 // 0xFFFFFFFF - size + le padding (size%8) = la taille

  short type;                 //0x6B6E : debut standard de la zone de DATA
  short format;               //root key = 0x2C, sinon 0x20, pas toujours fiable bien souven 0x20 même si root key
  FILETIME last_write;        //dernière modification
  long int unknow1;
  DWORD parent_key;           //emplacement de la clé parent
  DWORD nb_subkeys;           //nombre de sous clé
  DWORD nb_vl_subkeys;        //nombre de sous clé volatile
  DWORD lf_offset;           //-1 ou 0xffffffff = vide

  DWORD lsk_vol_offset;       //-1 ou 0xffffffff = vide
                              //emplacement de la liste de sk volatile à partir du début du hbin_data
  DWORD nb_values;
  DWORD val_ls_offset;
  DWORD sk_offset;            //-1 ou 0xffffffff = vide //emplacement de la liste de sk à partir du début du hbin_data
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
    char sid[1];      //par defaut découpé en section de 2octet *nb_part
  }DATA_USER;
  */
//------------------------------------------------------------------------------
BOOL syskey_;
char sk_c[MAX_LINE_SIZE];

BOOL secret_;
BYTE secret_c[MAX_LINE_SIZE];
//------------------------------------------------------------------------------
//fonctions systèmes standard
void InitConfig(HWND hwnd);
void EndConfig();
void ErrorExit(LPTSTR lpszFunction);

//fonctions de traitement
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

void AddToLV_Registry(LINE_ITEM *item);
DWORD AddToLV_log(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
DWORD AddToLV_File(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
DWORD AddToLV(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
void AddToLVRegBin(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne);
HTREEITEM AjouterItemTreeView(HANDLE hdlg, int treeview, char *texte, HTREEITEM hparent);
HTREEITEM AjouterItemTreeViewFile(HANDLE hdlg, int treeview, char *texte, HTREEITEM hparent,DWORD img_id);
HTREEITEM AjouterItemTreeViewRep(HANDLE hdlg, int treeview, char *texte, HTREEITEM hparent);
void EnumProcess(HANDLE hlv, unsigned short nb_colonne);

void TreeExport(HANDLE htree);
void ExportLVSelecttoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVtoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVSelecttoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void ExportLVtoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne);
void LVSaveAll(unsigned int id_tabl, int lv, unsigned short nb_colonne, BOOL selection_only,BOOL pwdump);
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

//process
void ReadProcessInfo(DWORD pid, HANDLE hlv);

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
void EventIdtoDscr(unsigned int eventId, char *source, char *result, unsigned short max_size);
void TraiterlogFile(char *path, HANDLE hlv);
void TraiterEventlogFileEvt(char * eventfile, HANDLE hlv);
void TraiterEventlogFileEvtx(char *eventfile, HANDLE hlv);


//gestion des recherches automatique des fichiers à analyser
HANDLE h_AutoSearchFiles;
BOOL AutoSearchFilesStart;
DWORD WINAPI AutoSearchFiles(LPVOID lParam);

//export des résultats
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

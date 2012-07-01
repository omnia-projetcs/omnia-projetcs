//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void InitSQLStrings()
{
  //db for ANDROID, FIREFOX,CHROME
  nb_sql_ANDROID  = 0;
  nb_sql_CHROME   = 0;
  nb_sql_FIREFOX  = 0;

  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_ANDROID_INIT_STRINGS;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

  fcri.type = TYPE_SQLITE_FLAG_CHROME_INIT_STRINGS;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

  fcri.type = TYPE_SQLITE_FLAG_FIREFOX_INIT_STRINGS;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
}
//------------------------------------------------------------------------------
//load strings for internals components
void InitString()
{
  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_LANG_INIT_STRINGS;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
}
//------------------------------------------------------------------------------
//hide DOS form
//------------------------------------------------------------------------------
void hideDOSForm()
{
char pszOldWindowTitle[MAX_PATH];
GetConsoleTitle(pszOldWindowTitle, MAX_PATH);
ShowWindow(FindWindow(NULL, pszOldWindowTitle), SW_HIDE);
AddDebugMessage("init", "Hide DOS console", "OK", MSG_INFO);
}
//------------------------------------------------------------------------------
//init start configuration
//------------------------------------------------------------------------------
void InitGlobalConfig(unsigned int params, BOOL debug, BOOL acl, BOOL ads, BOOL sha, BOOL recovery, BOOL local_scan)
{
  //init globals var
  DEBUG_VIEW        = FALSE;
  STAY_ON_TOP       = FALSE;

  DEBUG_MODE        = debug;
  FILE_ACL          = acl;
  FILE_ADS          = ads;
  FILE_SHA          = sha;
  REGISTRY_RECOVERY = recovery;

  LOCAL_SCAN        = local_scan;
  start_scan        = FALSE;
  scan_in_stop_state= FALSE;
  export_type       = 0;

  _SYSKEY[0]        = 0;  //global syskey for decrypt hash of users

  //load data for db
  current_session_id= 0;
  InitSQLStrings();

  SetDebugPrivilege(TRUE);

  //init if 64b
  OldValue_W64b = FALSE;
  ReviewWOW64Redirect(OldValue_W64b);

  AddDebugMessage("init", "Global init config", "OK", MSG_INFO);
}
//------------------------------------------------------------------------------
//init GUI configuration
//------------------------------------------------------------------------------
void InitGUIConfig(HANDLE hwnd)
{
  InitializeCriticalSection(&Sync);

  //hidden DOS form
  #ifndef DEV_DEBUG_MODE
    hideDOSForm();
  #endif

  if (sqlite3_open(DEFAULT_SQLITE_FILE, &db_scan) != SQLITE_OK)
  {
    //if tmp sqlite file exist free !!
    if (GetFileAttributes(DEFAULT_TM_SQLITE_FILE) != INVALID_FILE_ATTRIBUTES)
    {
      DeleteFile(DEFAULT_TM_SQLITE_FILE);
    }

    if (sqlite3_open(DEFAULT_SQLITE_FILE, &db_scan) != SQLITE_OK)
    {
      AddDebugMessage("init", "Error to open sqlite file !", "NOK", MSG_ERROR);
    }
  }

  //init for LSTV components
  InitCommonControls();

  //load string for internals components
  InitString();

  //GUI DLG HANDLE
  h_main  = hwnd; SetWindowText(hwnd,NOM_FULL_APPLI);
  h_conf  = CreateDialog(0, MAKEINTRESOURCE(DLG_CONF) ,hwnd,DialogProc_conf);
  h_view  = CreateDialog(0, MAKEINTRESOURCE(DLG_VIEW) ,hwnd,DialogProc_view);

  //global init
  B_AUTOSEARCH      = FALSE;
  h_AUTOSEARCH      = NULL;
  ExportStart       = FALSE;
  DEBUG_CMD_MODE    = FALSE;
  pos_search        = 0;
  current_OS[0]     = 0;
  current_OS_BE_64b = FALSE;

  //init default view
  VIEW_RESULTS_DBL  = FALSE;
  TRI_DEBUG_VIEW    = FALSE;
  TRI_RESULT_VIEW   = FALSE;

  nb_current_columns= 0;
  ShowWindow(GetDlgItem(h_view,TRV_VIEW), SW_HIDE);

  //icon
  SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(ICON_APP)));

  //init view forms
  ShowWindow(GetDlgItem(hwnd,LV_INFO), SW_HIDE);
  ShowWindow(h_conf, SW_SHOW);
  RECT Rect;
  GetWindowRect(hwnd, &Rect);
  MoveWindow(hwnd,Rect.left,Rect.top,584+20,524+64,TRUE);

  //création de la liste d'image
  HANDLE htmp = GetDlgItem(h_conf,CB_LANG);
  HANDLE H_ImagList = ImageList_Create(GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , /*nb icones*/2, 0);
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_LANGUAGE_EN)));
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_LANGUAGE_FR)));
  ImageList_SetBkColor(H_ImagList, GetSysColor(COLOR_WINDOW));

  //init la combo box des langues
  SendMessage(htmp,CBEM_SETIMAGELIST,0,(LPARAM)H_ImagList);
  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_LANG_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
  SendMessage(htmp, CB_SETCURSEL,0,0);          //default select English

  //init default view for all application
  htmp = GetDlgItem(h_view,TRV_VIEW);
  H_ImagList = ImageList_Create(GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , /*nb icones*/6, 0);
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_FOLDER)));
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_FILE)));
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_FILE_BIN)));
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_FILE_DWORD)));
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_FILE_TXT)));
  ImageList_AddIcon(H_ImagList,(HICON)LoadIcon((HINSTANCE) GetModuleHandle(0), MAKEINTRESOURCE(ICON_FILE_UNKNOW)));
  ImageList_SetBkColor(H_ImagList, GetSysColor(COLOR_WINDOW));
  SendMessage(htmp,CBEM_SETIMAGELIST,0,(LPARAM)H_ImagList);

  //add extended style to listview
  LVCOLUMN lvc;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
  lvc.fmt = LVCFMT_LEFT;
  lvc.cx = 100;       //taille colonne
  lvc.pszText = "Date"; //texte de la colonne
  SendDlgItemMessage(h_main,LV_INFO,LVM_INSERTCOLUMN,(WPARAM)0, (LPARAM)&lvc);
  lvc.cx = 100;       //taille colonne
  lvc.pszText = "Source"; //texte de la colonne
  SendDlgItemMessage(h_main,LV_INFO,LVM_INSERTCOLUMN,(WPARAM)1, (LPARAM)&lvc);
  lvc.cx = 240;       //taille colonne
  lvc.pszText = "Description"; //texte de la colonne
  SendDlgItemMessage(h_main,LV_INFO,LVM_INSERTCOLUMN,(WPARAM)2, (LPARAM)&lvc);
  lvc.cx = 60;       //taille colonne
  lvc.pszText = "State"; //texte de la colonne
  SendDlgItemMessage(h_main,LV_INFO,LVM_INSERTCOLUMN,(WPARAM)3, (LPARAM)&lvc);
  lvc.cx = 60;       //taille colonne
  lvc.pszText = "Info"; //texte de la colonne
  SendDlgItemMessage(h_main,LV_INFO,LVM_INSERTCOLUMN,(WPARAM)4, (LPARAM)&lvc);
  SendDlgItemMessage(h_main,LV_INFO,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
  SendDlgItemMessage(h_view,LV_VIEW,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);

  //init lstv
  Modify_Style(GetDlgItem(h_view,LV_VIEW), WS_SIZEBOX, FALSE);

  //combox of choices + treeview
  NB_TESTS  = 0;
  fcri.type = TYPE_SQLITE_FLAG_TESTS_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
  AddDebugMessage("init", "Load of all tests", "OK", MSG_INFO);

  //init for number
  nb_columns_items = SendDlgItemMessage(h_view,CB_VIEW,CB_GETCOUNT,(WPARAM)NULL, (LPARAM)NULL);
  columns_params = malloc(sizeof(ST_COLUMS)*nb_columns_items);
  fcri.type = TYPE_SQLITE_FLAG_COLUMN_COUNT_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
  AddDebugMessage("init", "Load of all column count", "OK", MSG_INFO);

  //list of sessions !!!
  nb_session = 0;
  fcri.type  = TYPE_SQLITE_FLAG_SESSIONS_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
  AddDebugMessage("init", "Load of all sessions", "OK", MSG_INFO);
  SendDlgItemMessage(h_conf,CB_SESSION, CB_SETCURSEL,0,0);

  //list for files
  CleanTreeViewFileView();

  //for cross compilation bug in 64bit
  #ifndef GWL_WNDPROC
    #define GWL_WNDPROC (-4)
  #endif

  //subclassement for resize borders
  TRV_SousClassement = (WNDPROC)SetWindowLong(GetDlgItem(h_view,TRV_VIEW), GWL_WNDPROC,(LONG)TRV_proc);
  LST_SousClassement = (WNDPROC)SetWindowLong(GetDlgItem(h_main,LV_INFO), GWL_WNDPROC,(LONG)LST_proc);

  //msg
  AddDebugMessage("init", "GUI init config", "OK", MSG_INFO);

  //all others datas
  InitGlobalConfig(0, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);
}
//------------------------------------------------------------------------------
//End of GUI
//------------------------------------------------------------------------------
void EndGUIConfig(HANDLE hwnd)
{
  sqlite3_close(db_scan);
  ReviewWOW64Redirect(OldValue_W64b);

  //clean memory for header of lstv
  free(columns_params);
  SetDebugPrivilege(FALSE);
  DeleteCriticalSection(&Sync);
  EndDialog(hwnd, 0);
}


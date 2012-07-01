//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "RtCA.h"
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
//------------------------------------------------------------------------------
DWORD LVSearch(HANDLE hlv, unsigned short nb_col, char *search, DWORD start_id)
{
  DWORD i, j, nb_item = ListView_GetItemCount(hlv);
  char tmp[MAX_LINE_SIZE];
  for (i=start_id+1;i<nb_item;i++)
  {
    for (j=0;j<nb_col;j++)
    {
      //récupération du text
      tmp[0]=0;
      ListView_GetItemText(hlv,i,j,tmp,MAX_LINE_SIZE);

      //test si la recherche est présente dedans
      if (Contient(tmp,search))
      {
        //sélection
        SendMessage(hlv, LVM_ENSUREVISIBLE, i, 0);
        ListView_SetItemState(hlv, -1, 0, LVIS_SELECTED); // deselect all
        ListView_SetItemState(hlv,i,LVIS_SELECTED,LVIS_SELECTED);

        //on quitte
        return i;
      }
    }
  }
  return -1;
}
//------------------------------------------------------------------------------
void LVAllSearch(HANDLE hlv, unsigned short nb_col, char *search)
{
  DWORD i, j, nb_item = ListView_GetItemCount(hlv);
  char tmp[MAX_LINE_SIZE];

  ListView_SetItemState(hlv, -1, 0, LVIS_SELECTED); // deselect all

  for (i=0;i<nb_item;i++)
  {
    for (j=0;j<nb_col;j++)
    {
      //récupération du text
      tmp[0]=0;
      ListView_GetItemText(hlv,i,j,tmp,MAX_LINE_SIZE);

      //test si la recherche est présente dedans
      if (Contient(tmp,search))
      {
        //sélection
        ListView_SetItemState(hlv,i,LVIS_SELECTED,LVIS_SELECTED);
      }
    }
  }
}
//------------------------------------------------------------------------------
void Modify_Style(HANDLE hcomp, long style, BOOL add)
{
if(add) SetWindowLong(hcomp, GWL_STYLE, GetWindowLong(hcomp, GWL_STYLE) | style);
else SetWindowLong(hcomp, GWL_STYLE, GetWindowLong(hcomp, GWL_STYLE) & ~style);
}
//------------------------------------------------------------------------------
void FileToTreeView(char *c_path)
{
  if(isDirectory(c_path))
  {
    char path[MAX_PATH];
    if (c_path[strlen(c_path)-1]=='\\')strncpy(path,c_path,MAX_PATH);
    else snprintf(path,MAX_PATH,"%s\\",c_path);
    AddItemTreeView(GetDlgItem(h_conf,TRV_FILES),path, TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
    check_treeview(GetDlgItem(h_conf,TRV_TEST), H_tests[TEST_FILES], TRV_STATE_CHECK);
  }else
  {
    char file[MAX_PATH];
    extractFileFromPath(c_path, file, MAX_PATH);
    AddItemFiletoTreeView(GetDlgItem(h_conf,TRV_FILES), charToLowChar(file), NULL, c_path);
  }
}
//------------------------------------------------------------------------------
void CleanTreeViewFileView()
{
  TreeView_DeleteAllItems(GetDlgItem(h_conf,TRV_FILES));
  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_TITLE_FILE_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
  AddDebugMessage("proc_gui", "Load of all type menu files", "OK", MSG_INFO);
}
//------------------------------------------------------------------------------
void CleanTreeViewFiles()
{
  unsigned int i;
  //tri and clean
  for (i=0;i<NB_MX_TYPE_FILES_TITLE;i++)
  {
    SendDlgItemMessage(h_conf,TRV_FILES,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM_CONF[i]);
    SupDoublon(h_conf,TRV_FILES,TRV_HTREEITEM_CONF[i]);
    SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[i]);
  }
}
//------------------------------------------------------------------------------
int CALLBACK CompareStringTri(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
  sort_st *st = (sort_st *)lParam3;

  static char buf1[MAX_LINE_SIZE], buf2[MAX_LINE_SIZE];

  ListView_GetItemText(st->hlv, lParam1, st->col, buf1, MAX_LINE_SIZE);
  ListView_GetItemText(st->hlv, lParam2, st->col, buf2, MAX_LINE_SIZE);

  if (st->sort) return (strcmp(buf1, buf2));
  else return (strcmp(buf1, buf2)*-1);
}
//------------------------------------------------------------------------------
void c_Tri(HANDLE hlv, unsigned short colonne_ref, BOOL sort)
{
  static sort_st st;
  st.hlv  = hlv;
  st.sort = sort;
  st.col  = colonne_ref;

  ListView_SortItemsEx(st.hlv,CompareStringTri, (LPARAM)&st);
}
//------------------------------------------------------------------------------
void redimColumn(HANDLE f,int lv,int column,unsigned int col_size)
{
  LVCOLUMN lvc;
  lvc.mask = LVCF_WIDTH;
  lvc.cx = col_size;

  SendDlgItemMessage(f,lv,LVM_SETCOLUMN,(WPARAM)column, (LPARAM)&lvc);
}
//------------------------------------------------------------------------------
void AddtoLV(HANDLE hlv, unsigned int nb_column, LINE_ITEM *item, BOOL select)
{
  LVITEM lvi;
  lvi.mask = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem = 0;
  lvi.lParam = LVM_SORTITEMS;
  lvi.pszText="";
  lvi.iItem = ListView_GetItemCount(hlv);
  DWORD i=0, ref_item = ListView_InsertItem(hlv, &lvi);

  do{
    ListView_SetItemText(hlv,ref_item,i,item[i].c);
  }while (i++<nb_column);

  if (select)
  {
    //sélection of the last !!!
    SendMessage(hlv, LVM_ENSUREVISIBLE, lvi.iItem, 0);
  }

}
//------------------------------------------------------------------------------
HTREEITEM AddItemTreeView(HANDLE Htreeview,char *txt, HTREEITEM hparent)
{
  TV_INSERTSTRUCT tvinsertitem;
  tvinsertitem.hParent = hparent;
  tvinsertitem.hInsertAfter = TVI_ROOT;
  tvinsertitem.item.mask = TVIF_TEXT;
  tvinsertitem.item.pszText = txt;
  return (HTREEITEM)SendMessage(Htreeview,TVM_INSERTITEM ,(WPARAM) 0, (LPARAM)&tvinsertitem);
}
//------------------------------------------------------------------------------
//check treeview items state
BOOL Ischeck_treeview(HANDLE Htree, HTREEITEM hitem)
{
  TVITEM tvitem;
  tvitem.mask = TVIF_HANDLE | TVIF_STATE;
  tvitem.hItem = hitem;
  tvitem.stateMask = TVIS_STATEIMAGEMASK;
  TreeView_GetItem(Htree, &tvitem);

  return ((BOOL)(tvitem.state >> 12) - 1);
}
//------------------------------------------------------------------------------
//check treeview item
void check_treeview(HANDLE Htree, HTREEITEM hitem, int state)
{
   TV_ITEM tvitem;
   tvitem.mask = TVIF_HANDLE | TVIF_STATE;
   tvitem.hItem = hitem;
   tvitem.stateMask = TVIS_STATEIMAGEMASK;
   tvitem.state = INDEXTOSTATEIMAGEMASK(state);
   TreeView_SetItem(Htree, &tvitem);
}
//------------------------------------------------------------------------------
void check_childs_treeview(HANDLE Htree, int state)
{
  int i=0;
  while (i<NB_TESTS)
  {
    check_treeview(Htree, H_tests[i++], state);
  }
}
//------------------------------------------------------------------------------
void SupDoublon(HANDLE hf,DWORD trv,HTREEITEM htreeParent)
{
  HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage(hf,trv, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)htreeParent);
  HTREEITEM hprecitem = hitem;
  //on énumère tous les items fils du treeview
  char tmp[MAX_PATH],prec[MAX_PATH]="";
  do
  {
    //récupération du texte de l'item
    TVITEM tvitem;
    tmp[0]=0;
    tvitem.hItem = hitem;
    tvitem.mask = TVIF_TEXT;
    tvitem.pszText = tmp;
    tvitem.cchTextMax = MAX_PATH;
    if (SendDlgItemMessage(hf,trv, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
    {
      if (strcmp(tmp,prec)==0)
      {
        //on supprime l'item
        SendDlgItemMessage(hf,trv, TVM_DELETEITEM,(WPARAM)NULL, (LPARAM)hitem);
        hitem = hprecitem;
      }else
      {
        strcpy(prec,tmp);
        hprecitem = hitem;
      }
    }
  }while((hitem = (HTREEITEM)SendDlgItemMessage(hf,trv, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));
}
//------------------------------------------------------------------------------
void AddDebugMessage(char *src, char *description, char *state, char*level)
{
  if (!DEBUG_MODE)return;

  EnterCriticalSection(&Sync);
  //date format
  time_t dateEtHMs;
  time(&dateEtHMs);
  struct tm *today = localtime(&dateEtHMs);
  if (CONSOL_ONLY)
  {
    char date[DEFAULT_TMP_SIZE];
    strftime(date, DEFAULT_TMP_SIZE,"%Y/%m/%d %H:%M:%S", today);
    printf("[%s] %s (%s) : %s %s",date,level,src,description,state);
  }else
  {
    LINE_ITEM item[NB_MSG_COLUMN];
    strftime(item[0].c, MAX_LINE_SIZE,"%Y/%m/%d %H:%M:%S", today);

    //src
    strncpy(item[1].c, src, MAX_LINE_SIZE);

    //description
    strncpy(item[2].c, description, MAX_LINE_SIZE);

    //state
    strncpy(item[3].c, state, MAX_LINE_SIZE);

    //level
    strncpy(item[4].c, level, MAX_LINE_SIZE);

    AddtoLV(GetDlgItem(h_main,LV_INFO), NB_MSG_COLUMN, item, TRUE);
  }
  LeaveCriticalSection(&Sync);
}
//------------------------------------------------------------------------------
void AddComboBoxItem(HANDLE hcombo, char *txt, DWORD img)
{
  //add item in combobox
  COMBOBOXEXITEM item;
  item.mask         = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
  item.iItem        = SendMessage(hcombo,CB_GETCOUNT,0,0);
  item.iImage       = item.iSelectedImage = img;
  item.pszText      = txt;
  item.cchTextMax   = strlen(txt);
  item.iIndent      = 0;
  SendMessage(hcombo,CBEM_INSERTITEM,0,(LPARAM)&item);
}
//------------------------------------------------------------------------------
void RefreshSizeForm(HWND hwnd, unsigned int mWidth, unsigned int mHeight)
{
  //Debug
  if (DEBUG_VIEW)
  {
    //forms
    MoveWindow(h_conf,0,0,mWidth-2,mHeight-100,TRUE);
    MoveWindow(h_view,0,0,mWidth-2,mHeight-100,TRUE);
    MoveWindow(GetDlgItem(hwnd,LV_INFO), 0, mHeight-100, mWidth-2,100, TRUE);
  }else
  {
    MoveWindow(h_conf,0,0,mWidth-2,mHeight,TRUE);
    MoveWindow(h_view,0,0,mWidth-2,mHeight,TRUE);
    MoveWindow(GetDlgItem(hwnd,LV_INFO), 0, mHeight, 0,0, TRUE);
  }

  //resize column size
  redimColumn(h_main,LV_INFO,0,120);
  redimColumn(h_main,LV_INFO,1,120);
  redimColumn(h_main,LV_INFO,2,mWidth-400);
  redimColumn(h_main,LV_INFO,3,60);
  redimColumn(h_main,LV_INFO,4,60);
}
//-----------------------------------------------------------------------------
void IDM_SAFE_MODE_fct()
{
  DEBUG_MODE = !DEBUG_MODE;
  if (DEBUG_MODE)
  {
    CheckMenuItem(GetMenu(h_main),IDM_DEBUG_MODE,MF_BYCOMMAND|MF_CHECKED);
  }else
  {
    CheckMenuItem(GetMenu(h_main),IDM_DEBUG_MODE,MF_BYCOMMAND|MF_UNCHECKED);
  }
}
//-----------------------------------------------------------------------------
void IDM_STAY_ON_TOP_fct()
{
  STAY_ON_TOP = !STAY_ON_TOP;
  if (STAY_ON_TOP)
  {
    SetWindowPos(h_main,HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    CheckDlgButton(h_conf, IDM_STAY_ON_TOP, BST_CHECKED);
    CheckMenuItem(GetMenu(h_main),IDM_STAY_ON_TOP,MF_BYCOMMAND|MF_CHECKED);
  }else
  {
    SetWindowPos(h_main,HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
    CheckDlgButton(h_conf, IDM_STAY_ON_TOP, BST_UNCHECKED);
    CheckMenuItem(GetMenu(h_main),IDM_STAY_ON_TOP,MF_BYCOMMAND|MF_UNCHECKED);
  }
}
//------------------------------------------------------------------------------
//subclass for TRV in VIEW part for resize
LRESULT CALLBACK TRV_proc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL in_move;
  if(Msg == WM_SIZE && !in_move)
  {
    in_move = TRUE;
    RECT Rect;
    GetWindowRect(h_view, &Rect);

    unsigned int mWidth    = LOWORD(lParam);
    unsigned int mHeight   = Rect.bottom-Rect.top-HEIGHT_SEARCH-5;

    if (VIEW_RESULTS_DBL)
    {
      MoveWindow(GetDlgItem(h_view,TRV_VIEW),       2, HEIGHT_SEARCH, mWidth,mHeight, TRUE);
      MoveWindow(GetDlgItem(h_view,LV_VIEW), mWidth+4, HEIGHT_SEARCH, Rect.right-Rect.left-mWidth-6,mHeight, TRUE);
    }else
    {
      MoveWindow(GetDlgItem(h_view,LV_VIEW),        2, HEIGHT_SEARCH, mWidth,mHeight-HEIGHT_SEARCH-5, TRUE);
    }

    in_move = FALSE;
  }
  return CallWindowProc(TRV_SousClassement, hwnd, Msg, wParam, lParam);
}
//------------------------------------------------------------------------------
//subclass for LST VIEW of debug for resize
LRESULT CALLBACK LST_proc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL in_move;
  if(Msg == WM_SIZE && !in_move)
  {
    in_move = TRUE;
    DWORD nHeight  = HIWORD(lParam);
    RECT Rect;
    GetWindowRect(h_main, &Rect);

    MoveWindow(h_conf,0,0,Rect.right-Rect.left-10,Rect.bottom-Rect.top-70-nHeight,TRUE);
    MoveWindow(h_view,0,0,Rect.right-Rect.left-10,Rect.bottom-Rect.top-70-nHeight,TRUE);
    MoveWindow(GetDlgItem(hwnd,LV_INFO), 0, Rect.bottom-Rect.top-nHeight, Rect.right-Rect.left-2,nHeight, TRUE);
    in_move = FALSE;
  }
  return CallWindowProc(LST_SousClassement, hwnd, Msg, wParam, lParam);
}
//------------------------------------------------------------------------------
void Global_WM_COMMAND_wParam(WPARAM wParam, LPARAM lParam)
{
  switch(LOWORD(wParam))
  {
    //-----------------------------------------------------
    case IDM_OPEN_FILE:
    case POPUP_TRV_FILES_ADD_FILE:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter = "*.* \0*.*\0"
                        "*.log\0*.log\0"
                        "*.evt\0*.evt\0"
                        "*.evtx\0*.evtx\0"
                        "*.db\0*.db\0"
                        "*.sqlite\0*.sqlite\0"
                        "*.dat\0*.dat\0"
                        "ntds.dit\0ntds.dit\0"
                        "*.reg\0*.reg\0"
                        "sam\0sam\0"
                        "system\0system\0"
                        "software\0software\0"
                        "security\0security\0"
                        "default\0default\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt ="*.*";
      if (GetOpenFileName(&ofn)==TRUE)
      {
        FileToTreeView(file);
        CleanTreeViewFiles();
        //expend des branches
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      }
    }
    break;
    //-----------------------------------------------------
    case IDM_OPEN_DIRECTORY:
    case POPUP_TRV_FILES_ADD_PATH:
    {
      BROWSEINFO browser;
      LPITEMIDLIST lip;
      char path[MAX_PATH]     = "";
      browser.hwndOwner       = h_conf;
      browser.pidlRoot        = 0;
      browser.lpfn            = 0;
      browser.iImage          = 0;
      browser.lParam          = 0;
      browser.ulFlags         = BIF_NEWDIALOGSTYLE;
      browser.pszDisplayName  = path;
      browser.lpszTitle       = "";
      lip = SHBrowseForFolder(&browser);
      if (lip != NULL)
      {
        SHGetPathFromIDList(lip,path);
        FileToTreeView(path);
        CleanTreeViewFiles();
        //expend des branches
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      }
    }
    break;
    //-----------------------------------------------------
    case BT_EXPORT:
    {
      if (ExportStart)
      {
        ExportStart = FALSE;
        DWORD IDThread;
        GetExitCodeThread(h_Export,&IDThread);
        TerminateThread(h_Export,IDThread);
        SetWindowText(GetDlgItem(h_conf,BT_EXPORT),cps[TXT_BT_EXPORT_START].c);
        EnableMenuItem(GetSubMenu(GetMenu(h_main),0),7,MF_BYPOSITION|MF_ENABLED);
      }else
      {
        //view popup menu to choose : XML/HTML/CSV export ^^
        RECT rect;
        GetWindowRect(GetDlgItem(h_conf,BT_EXPORT), &rect);

        HMENU hmenu = LoadMenu(GetModuleHandle(0),MAKEINTRESOURCE(POPUP_EXPORT));
        if (hmenu != NULL)
        {
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0,rect.left, rect.bottom, h_conf, NULL);
          DestroyMenu(hmenu);
        }
      }
    }break;
    case POPUP_E_CSV:
      export_type = SAVE_TYPE_CSV;
      current_lang_id = SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1;
      CreateThread(NULL,0,ChoiceSaveAll,NULL,0,0);
    break;
    case POPUP_E_HTML:
      export_type = SAVE_TYPE_HTML;
      current_lang_id = SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1;
      CreateThread(NULL,0,ChoiceSaveAll,NULL,0,0);
    break;
    case POPUP_E_XML:
      export_type = SAVE_TYPE_XML;
      current_lang_id = SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1;
      CreateThread(NULL,0,ChoiceSaveAll,NULL,0,0);
    break;
    //-----------------------------------------------------
    case IDM_SAVE_DEBUG:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt =".csv\0";
      if (GetSaveFileName(&ofn)==TRUE)
      {
        if(!SaveLSTV(GetDlgItem(h_main,LV_INFO), file, ofn.nFilterIndex, NB_COLUMN_DEBUG))
          AddDebugMessage("proc_gui", "Fail to save LSTV DEBUG to file", "NOK", MSG_ERROR);
      }
    }
    break;
    //-----------------------------------------------------
    case IDM_LOAD_SESSION_FILE:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter ="*.sqlite \0*.sqlite\0*.* \0*.*\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt =".sqlite\0";

      if (GetOpenFileName(&ofn)==TRUE)
      {
        if(!SQLITE_LoadSession(file))AddDebugMessage("proc_gui", "Fail to open sqlite file", "NOK", MSG_ERROR);
      }
    }
    break;
    //-----------------------------------------------------
    case IDM_SAVE_SESSION_FILE:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter ="*.sqlite \0*.sqlite\0*.* \0*.*\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt =".sqlite\0";

      if (GetSaveFileName(&ofn)==TRUE)
      {
        if(!SQLITE_SaveSession(file))AddDebugMessage("proc_gui", "Fail to open sqlite file", "NOK", MSG_ERROR);
      }
    }
    break;
    //-----------------------------------------------------
    case IDM_ONGLET_CONF:ShowWindow(h_view, SW_HIDE);ShowWindow(h_conf, SW_SHOW);break;
    //-----------------------------------------------------
    case IDM_ONGLET_VIEW:ShowWindow(h_conf, SW_HIDE);ShowWindow(h_view, SW_SHOW);break;
    //-----------------------------------------------------
    case IDM_DEBUG_LIST:
    {
      DEBUG_VIEW = !DEBUG_VIEW;
      if (DEBUG_VIEW)
      {
        ShowWindow(GetDlgItem(h_main,LV_INFO), SW_SHOW);
        CheckMenuItem(GetMenu(h_main),IDM_DEBUG_LIST,MF_BYCOMMAND|MF_CHECKED);
      }else
      {
        ShowWindow(GetDlgItem(h_main,LV_INFO), SW_HIDE);
        CheckMenuItem(GetMenu(h_main),IDM_DEBUG_LIST,MF_BYCOMMAND|MF_UNCHECKED);
      }

      //force resize
      RECT Rect;
      GetWindowRect(h_main, &Rect);
      MoveWindow(h_main,Rect.left,Rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top,TRUE);
      RefreshSizeForm(h_main, Rect.right-Rect.left-8, Rect.bottom-Rect.top-60);
    }
    break;
    //-----------------------------------------------------
    case IDM_DEL_SESSION :
      if (nb_session<1)break;
      if (MessageBox(0,cps[REF_MSG].c,cps[REF_MSG+1].c,MB_ICONWARNING|MB_OKCANCEL) == IDOK)
      {
        AddDebugMessage("proc_gui", "Remove current session", "OK", MSG_WARN);

        FORMAT_CALBAK_READ_INFO fcri;
        fcri.type = TYPE_SQL_REMOVE_SESSION;
        SQLITE_WriteData(&fcri, DEFAULT_SQLITE_FILE);

        SendMessage(GetDlgItem(h_conf,CB_SESSION), CB_RESETCONTENT,0,0);
        nb_session = 0;
        fcri.type  = TYPE_SQLITE_FLAG_SESSIONS_INIT;
        SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
        AddDebugMessage("proc_gui", "Reload of all sessions", "OK", MSG_INFO);
        SendDlgItemMessage(h_conf,CB_SESSION, CB_SETCURSEL,0,0);
      }
      sqlite3_exec(db_scan,"VACUUM;", NULL, NULL, NULL);//compact database
    break;
    case IDM_DEL_ALL_SESSION :
      if (nb_session<1)break;
      if (MessageBox(0,cps[REF_MSG].c,cps[REF_MSG+1].c,MB_ICONWARNING|MB_OKCANCEL) == IDOK)
      {
        AddDebugMessage("proc_gui", "Remove all sessions", "OK", MSG_WARN);

        FORMAT_CALBAK_READ_INFO fcri;
        fcri.type = TYPE_SQL_REMOVE_ALL_SESSION;
        SQLITE_WriteData(&fcri, DEFAULT_SQLITE_FILE);

        SendMessage(GetDlgItem(h_conf,CB_SESSION), CB_RESETCONTENT,0,0);
      }
      sqlite3_exec(db_scan,"VACUUM;", NULL, NULL, NULL);//compact database
    break;
    //-----------------------------------------------------
    case IDM_STAY_ON_TOP:IDM_STAY_ON_TOP_fct();break;
    //-----------------------------------------------------
    case IDM_DEBUG_MODE:IDM_SAFE_MODE_fct();break;
    //-----------------------------------------------------
    case IDM_CHECK_ALL_TESTS:check_childs_treeview(GetDlgItem(h_conf,TRV_TEST), TRV_STATE_CHECK);break;
    case IDM_UNCHECK_ALL_TESTS:check_childs_treeview(GetDlgItem(h_conf,TRV_TEST), TRV_STATE_UNCHECK);break;

    /*case IDM_TOOLS_CP_REGISTRY : break;
    case IDM_TOOLS_CP_AUDIT : break;
    case IDM_TOOLS_CP_AD : break;
    case IDM_TOOLS_CP_SELECT :break;*/
    //-----------------------------------------------------
    case IDM_ABOUT:MessageBox(0,"to Read to Catch All :\n"
                                  "Licensed under the terms of the GNU\n"
                                  "General Public License version 3.\n\n"
                                  "Author: nicolas.hanteville@gmail.com\n"
                                  "http://code.google.com/p/omnia-projetcs/"
                                 ,"About",MB_ICONINFORMATION|MB_OK); break;
    //-----------------------------------------------------
    case IDM_RTCA_HOME:ShellExecute(NULL, "open", URL_APPLI, NULL, NULL, SW_SHOWNORMAL); break;
    //-----------------------------------------------------
    case POPUP_TRV_FILES_OPEN_PATH:
    {
      //get item txt
      char path[MAX_PATH];
      TVITEM tvitem;
      tvitem.mask = TVIF_HANDLE|TVIF_TEXT;
      tvitem.hItem = (HTREEITEM)SendDlgItemMessage(h_conf,TRV_FILES,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
      tvitem.cchTextMax = MAX_PATH;
      tvitem.pszText = path;
      if (SendDlgItemMessage(h_conf,TRV_FILES,TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        //directory or file
        if(isDirectory(path))ShellExecute(h_main, "explore", path, NULL,NULL,SW_SHOW);
        else
        {
          //extract
          char *c = path;
          while (*c++);
          while (*c != '\\')c--;
          c++;
          *c=0;
          ShellExecute(h_main, "explore", path, NULL,NULL,SW_SHOW);
        }
      }
    }
    break;
    //-----------------------------------------------------
    case POPUP_TRV_FILES_REMOVE_ITEMS:
      TreeView_DeleteItem(GetDlgItem(h_conf,TRV_FILES), (HTREEITEM)SendDlgItemMessage(h_conf,TRV_FILES,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0));
    break;
    //-----------------------------------------------------
    case POPUP_TRV_FILES_CLEAN_ALL:CleanTreeViewFileView(); break;
    //-----------------------------------------------------
    case POPUP_TRV_FILES_AUTO_SEARCH:
      if (B_AUTOSEARCH)
      {
        B_AUTOSEARCH = FALSE;
        DWORD IDThread;
        GetExitCodeThread(h_AUTOSEARCH,&IDThread);
        TerminateThread(h_AUTOSEARCH,IDThread);

        //clean results
        unsigned int i;
        for (i=0;i<NB_MX_TYPE_FILES_TITLE;i++)
        {
          //tri
          SendDlgItemMessage(h_conf,TRV_FILES,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM_CONF[i]);
          SupDoublon(h_conf,TRV_FILES,TRV_HTREEITEM_CONF[i]);
          SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[i]);
        }
      }else
      {
        B_AUTOSEARCH = TRUE;
        h_AUTOSEARCH = CreateThread(NULL,0,AutoSearchFiles,0,0,0);
      }
    break;
    //-----------------------------------------------------
    case POPUP_TRV_FILES_SAVE_LIST:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter ="*.txt \0*.txt\0*.csv\0*.csv\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt =".txt\0";
      if (GetSaveFileName(&ofn)==TRUE)
      {
        unsigned int index = SAVE_TYPE_TXT;
        if (ofn.nFilterIndex == 2) index = SAVE_TYPE_CSV;

        if(!SaveTRV(GetDlgItem(h_conf,TRV_FILES), file, index))
          AddDebugMessage("proc_gui", "Fail to save TRV file list to file", "NOK", MSG_ERROR);
      }
    }
    break;
    //-----------------------------------------------------
    case BT_START:
      if (start_scan)
      {
        CreateThread(NULL,0,StopGUIScan,0,0,0);
      }else if (scan_in_stop_state == FALSE)
      {
        start_scan = TRUE;

        //create new session and select it !
        SendDlgItemMessage(h_conf,CB_SESSION, CB_RESETCONTENT,0,0);
        FORMAT_CALBAK_READ_INFO fcri;
        fcri.type = TYPE_SQL_ADD_SESSION;
        SQLITE_WriteData(&fcri, DEFAULT_SQLITE_FILE);

        //modify txt button
        SetWindowText(GetDlgItem(h_conf,BT_START),cps[TXT_BT_STOP].c);

        //local or not ?
        if(TreeView_GetCount(GetDlgItem(h_conf,TRV_FILES)) == NB_MX_TYPE_FILES_TITLE)LOCAL_SCAN = TRUE;
        else LOCAL_SCAN = FALSE;

        //read state !
        if (IsDlgButtonChecked(h_conf,BT_ACL_FILE_CHK)==BST_CHECKED)FILE_ACL=TRUE;
        else FILE_ACL=FALSE;
        if (IsDlgButtonChecked(h_conf,BT_ADS_FILE_CHK)==BST_CHECKED)FILE_ADS=TRUE;
        else FILE_ADS=FALSE;
        if (IsDlgButtonChecked(h_conf,BT_SHA_FILE_CHK)==BST_CHECKED)FILE_SHA=TRUE;
        else FILE_SHA=FALSE;
        if (IsDlgButtonChecked(h_conf,BT_REGISTRY_RECOV_MODE)==BST_CHECKED)REGISTRY_RECOVERY=TRUE;
        else REGISTRY_RECOVERY=FALSE;

        //start scan
        h_thread_scan = CreateThread(NULL,0,GUIScan,0,0,0);
      }
    break;
    //-----------------------------------------------------
    //popup menu lstv in view part
    //-----------------------------------------------------
    case POPUP_S_VIEW:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt =".csv\0";
      if (GetSaveFileName(&ofn)==TRUE)
      {
        if(!SaveLSTV(GetDlgItem(h_view,LV_VIEW), file, ofn.nFilterIndex, nb_current_columns))
          AddDebugMessage("proc_gui", "Fail to save LSTV in VIEW form to file", "NOK", MSG_ERROR);
      }
    }
    break;
    //-----------------------------------------------------
    case POPUP_S_SELECTION:
    {
      char file[MAX_PATH]="";
      OPENFILENAME ofn;
      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = h_main;
      ofn.lpstrFile = file;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
      ofn.nFilterIndex = 1;
      ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
      ofn.lpstrDefExt =".csv\0";
      if (GetSaveFileName(&ofn)==TRUE)
      {
        if(!SaveLSTVSelectedItems(GetDlgItem(h_view,LV_VIEW), file, ofn.nFilterIndex, nb_current_columns))
          AddDebugMessage("proc_gui", "Fail to save LSTV selected items in VIEW form to file", "NOK", MSG_ERROR);
      }
    }
    break;
    //-----------------------------------------------------
    case POPUP_A_SEARCH:
    {
      char tmp[MAX_PATH];
      SendDlgItemMessage(h_view,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
      LVAllSearch(GetDlgItem(h_view,LV_VIEW), nb_current_columns, tmp);
    }
    break;
    //-----------------------------------------------------
    case POPUP_I_00:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 0);break;
    case POPUP_I_01:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 1);break;
    case POPUP_I_02:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 2);break;
    case POPUP_I_03:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 3);break;
    case POPUP_I_04:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 4);break;
    case POPUP_I_05:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 5);break;
    case POPUP_I_06:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 6);break;
    case POPUP_I_07:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 7);break;
    case POPUP_I_08:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 8);break;
    case POPUP_I_09:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 9);break;
    case POPUP_I_10:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 10);break;
    case POPUP_I_11:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 11);break;
    case POPUP_I_12:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 12);break;
    case POPUP_I_13:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 13);break;
    case POPUP_I_14:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 14);break;
    case POPUP_I_15:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 15);break;
    case POPUP_I_16:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 16);break;
    case POPUP_I_17:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 17);break;
    case POPUP_I_18:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 18);break;
    case POPUP_I_19:CopyDataToClipboard(GetDlgItem(h_view,LV_VIEW), SendMessage(GetDlgItem(h_view,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 19);break;
    //-----------------------------------------------------
    //-----------------------------------------------------
    case BT_SEARCH: //search
    {
      char tmp[MAX_PATH];
      SendDlgItemMessage(h_view,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
      pos_search = LVSearch(GetDlgItem(h_view,LV_VIEW), nb_current_columns, tmp, pos_search);
    }
    break;
  }
}

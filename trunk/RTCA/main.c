//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_conf(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HTREEITEM hitem;

    if (uMsg == WM_SIZE)
    {
        unsigned int mWidth = LOWORD(lParam)-4;  // width of client area
        unsigned int mHeight = HIWORD(lParam);  // width of client area

        MoveWindow(GetDlgItem(hwnd,TRV_CONF_TESTS),5,0,mWidth-245,mHeight-5,TRUE);

        MoveWindow(GetDlgItem(hwnd,GRP_CONF),mWidth-235,0,230,182,TRUE);

        MoveWindow(GetDlgItem(hwnd,CHK_CONF_LOGS),mWidth-225,15,60,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_FILES),mWidth-225,35,60,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_NO_ACL),mWidth-154,35,130,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_NO_TYPE),mWidth-154,50,130,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_MD5)    ,mWidth-154,65,130,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_ADS)    ,mWidth-154,80,130,15,TRUE);

        MoveWindow(GetDlgItem(hwnd,CHK_CONF_REGISTRY),mWidth-225,110,60,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_REG_GLOBAL_LOCAL) ,mWidth-154,110,144,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_REG_FILE_RECOVERY),mWidth-154,125,140,15,TRUE);

        MoveWindow(GetDlgItem(hwnd,CHK_CONF_CONFIGURATION)     ,mWidth-225,147,180,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_ENABLE_STATE)      ,mWidth-225,162,140,15,TRUE);

        MoveWindow(GetDlgItem(hwnd,GRP_CONF_CONF)             ,mWidth-235,182,230,50,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_LOCAL)            ,mWidth-225,197,80,15,TRUE);
        MoveWindow(GetDlgItem(hwnd,CHK_CONF_TOP)              ,mWidth-225,212,80,15,TRUE);

        MoveWindow(GetDlgItem(hwnd,GRP_CONF_ABOUT),mWidth-235,232,230,110,TRUE);
        MoveWindow(GetDlgItem(hwnd,ST_CONF_ABOUT),mWidth-225,247,210,90,TRUE);

        MoveWindow(GetDlgItem(hwnd,BT_CONF_EXPORT),mWidth-235,347,110,30,TRUE);
        MoveWindow(GetDlgItem(hwnd,BT_CONF_START),mWidth-115,347,110,30,TRUE);

        //pour correction du bug d'affichage
        InvalidateRect(hwnd, NULL, TRUE);
    }else if (uMsg == WM_COMMAND)
    {
      if (HIWORD(wParam) == BN_CLICKED)
      {
        switch(LOWORD(wParam))
        {
          case BT_CONF_START:
            if (ScanStart)
            {
              //on arrête le scan
              ScanStart = FALSE;
              CreateThread(NULL,0,StopScan,0,0,0);
              SetWindowText(GetDlgItem(hwnd,BT_CONF_START),"Start");
              EnableWindow(GetDlgItem(hwnd,BT_CONF_START),FALSE);

              SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_INFO, (LPARAM)"");
              SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_LOGS, (LPARAM)"");
              SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_FILES, (LPARAM)"");
              SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_REGISTRY, (LPARAM)"");
            }else
            {
              //activation ou non des state
              if (IsDlgButtonChecked(hwnd,CHK_CONF_ENABLE_STATE) ==BST_CHECKED)State_Enable = TRUE;
              else State_Enable = FALSE;

              if (IsDlgButtonChecked(hwnd,CHK_CONF_MD5) ==BST_CHECKED)MD5_Enable = TRUE;
              else MD5_Enable = FALSE;

              if (IsDlgButtonChecked(hwnd,CHK_CONF_NO_ACL) ==BST_CHECKED)ACL_Enable = TRUE;
              else ACL_Enable = FALSE;

              if (IsDlgButtonChecked(hwnd,CHK_CONF_NO_TYPE) ==BST_CHECKED)Type_Enable = TRUE;
              else Type_Enable = FALSE;

              ScanStart = TRUE;
              CreateThread(NULL,0,Scan,0,0,0);
              SetWindowText(GetDlgItem(hwnd,BT_CONF_START),"Stop");
            }
          break;
          case BT_CONF_EXPORT:
            if (ExportStart)
            {
              //on arrête le scan
              ExportStart = FALSE;
              DWORD IDThread;
              GetExitCodeThread(h_Export,&IDThread);
              TerminateThread(h_Export,IDThread);
              SetWindowText(GetDlgItem(hwnd,BT_CONF_EXPORT),"Export");
            }else
            {
              //on affiche le popup
              RECT rect;
              //récupération des coordonnées du bouton appelé
              GetWindowRect(GetDlgItem(hwnd,BT_CONF_EXPORT), &rect);

              //afficher la popup
              HMENU hmenu = LoadMenu(hInst,MAKEINTRESOURCE(POPUPEXPORT));
              if (hmenu != NULL)
              {
                //affichage du popup
                TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0,rect.left, rect.bottom, hwnd, NULL);
                DestroyMenu(hmenu);
              }
            }
          break;
          case POPUP_E_CSV:
              ExportStart = TRUE;
              h_Export = CreateThread(NULL,0,Export,(PVOID)CSV_TYPE,0,0);
              SetWindowText(GetDlgItem(hwnd,BT_CONF_EXPORT),"Stop Export");
          break;
          case POPUP_E_HTML:
              ExportStart = TRUE;
              h_Export = CreateThread(NULL,0,Export,(PVOID)HTML_TYPE,0,0);
              SetWindowText(GetDlgItem(hwnd,BT_CONF_EXPORT),"Stop Export");
          break;
          case POPUP_E_XML:
              ExportStart = TRUE;
              h_Export = CreateThread(NULL,0,Export,(PVOID)XML_TYPE,0,0);
              SetWindowText(GetDlgItem(hwnd,BT_CONF_EXPORT),"Stop Export");
          break;

          case POPUP_TRV_CONF_ADD_FILE: AddFile();break;
          case POPUP_TRV_CONF_ADD_PATH: AddRep();break;
          case POPUP_TRV_CONF_REMOVE:SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);break;
          case POPUP_TRV_CONF_CLEAN:
            //on supprime tous les items
            SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)TVI_ROOT);

            //on ajoute les root !
            TRV_HTREEITEM[TRV_LOGS] = AjouterItemTreeView(Tabl[TABL_CONF], TRV_CONF_TESTS,"Audit logs",TVI_ROOT);
            TRV_HTREEITEM[TRV_FILES] = AjouterItemTreeView(Tabl[TABL_CONF], TRV_CONF_TESTS,"Files path",TVI_ROOT);
            TRV_HTREEITEM[TRV_REGISTRY] = AjouterItemTreeView(Tabl[TABL_CONF], TRV_CONF_TESTS,"Registry files",TVI_ROOT);
            TRV_HTREEITEM[TRV_CONF] = AjouterItemTreeView(Tabl[TABL_CONF], TRV_CONF_TESTS,"Applications files",TVI_ROOT);
          break;
          //déplacer un item !!!
          case POPUP_TRV_MV_LOGS:
          {
            char tmp[MAX_PATH];
            GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
            AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_LOGS]);
            SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
          }
          break;
          case POPUP_TRV_MV_REGISTRY:
          {
            char tmp[MAX_PATH];
            GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
            AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_REGISTRY]);
            SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
          }
          break;
          case POPUP_TRV_MV_CONF:
          {
            char tmp[MAX_PATH];
            GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
            AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_CONF]);
            SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
          }
          break;
          case POPUP_TRV_CONF_AUTO_SEARCH_FILES:
            if (AutoSearchFilesStart)
            {
              //on arrête le scan
              AutoSearchFilesStart = FALSE;
              DWORD IDThread;
              GetExitCodeThread(h_AutoSearchFiles,&IDThread);
              TerminateThread(h_AutoSearchFiles,IDThread);

              //on tri + sup les doublons + affiche les résultats
              //tri
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM[TRV_FILES]);
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM[TRV_LOGS]);
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM[TRV_CONF]);

              SupDoublon(Tabl[TABL_CONF],TRV_CONF_TESTS,TRV_HTREEITEM[TRV_FILES]);
              SupDoublon(Tabl[TABL_CONF],TRV_CONF_TESTS,TRV_HTREEITEM[TRV_LOGS]);
              SupDoublon(Tabl[TABL_CONF],TRV_CONF_TESTS,TRV_HTREEITEM[TRV_REGISTRY]);
              SupDoublon(Tabl[TABL_CONF],TRV_CONF_TESTS,TRV_HTREEITEM[TRV_CONF]);

              //on affiche les résultats
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_FILES]);
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_LOGS]);
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);
              SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_CONF]);

            }else
            {
              AutoSearchFilesStart = TRUE;
              h_AutoSearchFiles = CreateThread(NULL,0,AutoSearchFiles,0,0,0);
            }
          break;
          case POPUP_TRV_CONF_EXPORT_LIST_FILES:TreeExport(GetDlgItem(hwnd, TRV_CONF_TESTS));break;
          case POPUP_BACKUP: CreateThread(NULL,0,BackupRegFile,0,0,0);break;
          case POPUP_BACKUP_LOGS : CreateThread(NULL,0,BackupEvtFile,0,0,0);break;
          case POPUP_TRV_CONF_P:
          {
            //récupération de la sélection
            hitem = (HTREEITEM)SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
            //récupération du parent
            HTREEITEM hparent = (HTREEITEM)SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_GETNEXTITEM,(WPARAM)TVGN_PARENT, (LPARAM)hitem);

            if (hparent == TRV_HTREEITEM[TRV_FILES])
            {
              //lecture du texte de l'item
              char tmp[MAX_PATH];
              GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
              //ajout de l'item
              AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_LOGS]);
              //suppression de l'item
              SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
            }
            else if (hparent == TRV_HTREEITEM[TRV_REGISTRY])
            {
              //lecture du texte de l'item
              char tmp[MAX_PATH];
              GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
              //ajout de l'item
              AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_FILES]);
              //suppression de l'item
              SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
            }
          }
          break;
          case POPUP_TRV_CONF_M:
          {
            //récupération de la sélection
            hitem = (HTREEITEM)SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
            //récupération du parent
            HTREEITEM hparent = (HTREEITEM)SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_GETNEXTITEM,(WPARAM)TVGN_PARENT, (LPARAM)hitem);

            if (hparent == TRV_HTREEITEM[TRV_LOGS])
            {
              //lecture du texte de l'item
              char tmp[MAX_PATH];
              GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
              //ajout de l'item
              AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_FILES]);
              //suppression de l'item
              SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
            }
            else if (hparent == TRV_HTREEITEM[TRV_FILES])
            {
              //lecture du texte de l'item
              char tmp[MAX_PATH];
              GetItemTxt(hitem,hwnd, TRV_CONF_TESTS,tmp, MAX_PATH);
              //ajout de l'item
              AjouterItemTreeView(hwnd, TRV_CONF_TESTS, tmp, TRV_HTREEITEM[TRV_REGISTRY]);
              //suppression de l'item
              SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
            }
          }
          break;
          case CHK_CONF_TOP:
            if (IsDlgButtonChecked(Tabl[TABL_CONF],CHK_CONF_TOP)==BST_CHECKED)//stay on top
              SetWindowPos(Tabl[TABL_MAIN],HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
            else//on enlève
              SetWindowPos(Tabl[TABL_MAIN],HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
          break;
          case POPUP_TRV_CONF_OFP:
          {
            //read tree item
            char tmp[MAX_PATH+1];
            TVITEM tvitem;
            tvitem.mask = TVIF_HANDLE|TVIF_TEXT;
            tvitem.hItem = (HTREEITEM)SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
            tvitem.cchTextMax = MAX_PATH;
            tvitem.pszText = tmp;
            if (SendDlgItemMessage(hwnd,TRV_CONF_TESTS,TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
            {
              unsigned int size = strlen(tmp);
              if (size)
              {
                //on vérifie si un fichier ou directory
                if (tmp[size-1]=='\\')//directory
                  ShellExecute(Tabl[TABL_MAIN], "explore", tmp, NULL,NULL,SW_SHOW);
                else
                {
                  //on récupère le path
                  char *c = tmp+size-1;
                  while(*c != '\\')c--;
                  c++;
                  *c=0;
                  ShellExecute(Tabl[TABL_MAIN], "explore", tmp, NULL,NULL,SW_SHOW);
                }
              }
            }
          }
          break;
          case CHK_CONF_ENABLE_STATE:
            if (IsDlgButtonChecked(hwnd,CHK_CONF_ENABLE_STATE) ==BST_CHECKED)ShowWindow(GetDlgItem(Tabl[TABL_MAIN],BT_MAIN_STATE), SW_SHOW);
            else ShowWindow(GetDlgItem(Tabl[TABL_MAIN],BT_MAIN_STATE), SW_HIDE);
          break;

          case POPUP_CLEAN_REGISTRY: clean_registry();break;
          case POPUP_CLEAN_LOGS: clean_logs();break;

          case POPUP_REG_CHECK: CheckRegistryFile();break;

          case POPUP_POL_REG:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "DisableRegistryTools", 1, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "DisableRegistryTools", 1, 0);
          break;
          case POPUP_POL_CMD:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Policies\\Microsoft\\Windows\\System", "DisableCMD", 1, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\WindowsSystem", "DisableCMD", 1, 0);
          break;
          case POPUP_POL_TASKMGR:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "DisableTaskMgr", 1, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "DisableTaskMgr", 1, 0);
          break;
          case POPUP_POL_DRIVES:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoDrives", 0x3FFFFFF, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoDrives", 0x3FFFFFF, 0);
          break;
          case POPUP_POL_DESKTOP:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoViewContextMenu", 1, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoViewContextMenu", 1, 0);
          break;
          case POPUP_POL_DESKTOP_:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoActiveDesktop", 1, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", "NoActiveDesktop", 1, 0);
          break;
          case POPUP_POL_CPL:
            RegistryTestDWORD(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "NODispCPL", 1, 0);
            RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", "NODispCPL", 1, 0);
          break;

          case POPUP_POL_USB:     RegistryTestDWORD(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\UsbStor", "Start", 3, 4);break;
          case POPUP_POL_SCR:     RegistryTestSTRING(HKEY_CURRENT_USER, "Control Panel\\Desktop", "ScreenSaveActive", "1", "0");break;
          case POPUP_POL_SCR_PWD: RegistryTestSTRING(HKEY_CURRENT_USER, "Control Panel\\Desktop", "ScreenSaverIsSecure", "1", "0");break;
        }
      }
    }else if (uMsg == WM_CONTEXTMENU)
    {
      //récupération de l'emplacement du click
      TV_HITTESTINFO tvh_info;
      tvh_info.pt.x = LOWORD(lParam);
      tvh_info.pt.y = HIWORD(lParam);

      //sélection de l'item
      HANDLE htree = GetDlgItem(hwnd, TRV_CONF_TESTS);
      ScreenToClient(htree, &(tvh_info.pt));
      hitem = TreeView_HitTest(htree, &tvh_info);
      TreeView_SelectItem(htree,hitem);

      //affichage du popup menu
      HMENU hmenu;
      if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(POPUP_TRV_CONF)))!= NULL)
      {
        //si aucun item de sélectionné ou une ruche on désactive des options
        if (hitem == NULL || hitem ==TRV_HTREEITEM[TRV_LOGS] || hitem ==TRV_HTREEITEM[TRV_FILES] || hitem ==TRV_HTREEITEM[TRV_REGISTRY])
        {
          EnableMenuItem(hmenu,POPUP_TRV_CONF_REMOVE,MF_BYCOMMAND|MF_GRAYED);
          EnableMenuItem(hmenu,POPUP_TRV_CONF_P,MF_BYCOMMAND|MF_GRAYED);
          EnableMenuItem(hmenu,POPUP_TRV_CONF_M,MF_BYCOMMAND|MF_GRAYED);
          EnableMenuItem(hmenu,POPUP_TRV_CONF_OFP,MF_BYCOMMAND|MF_GRAYED);
        }

        if (TreeView_GetCount(htree)<4) //3 = les roots
        {
          EnableMenuItem(hmenu,POPUP_TRV_CONF_EXPORT_LIST_FILES,MF_BYCOMMAND|MF_GRAYED);
        }

        //pour le cas de la recherche de fichier, on modifie le titre si actif
        if (AutoSearchFilesStart)ModifyMenu(hmenu,POPUP_TRV_CONF_AUTO_SEARCH_FILES,MF_BYCOMMAND|MF_STRING,0,"STOP search files");

        //affichage du popup menu
        TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
        DestroyMenu(hmenu);
      }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_logs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SIZE)
    {
        unsigned int mWidth = LOWORD(lParam);  // width of client area
        unsigned int mHeight = HIWORD(lParam);  // width of client area
        MoveWindow(GetDlgItem(hwnd,LV_LOGS_VIEW),5,0,mWidth-10,mHeight-31,TRUE);

        MoveWindow(GetDlgItem(hwnd,ED_SEARCH),5,mHeight-26,mWidth/4,21,TRUE);
        MoveWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH),mWidth/4+10,mHeight-26,100,21,TRUE);

        //redimmensionnement des colonnes
        unsigned int col_size = (mWidth-330)/3;
        redimColumn(hwnd,LV_LOGS_VIEW,4,col_size);
        redimColumn(hwnd,LV_LOGS_VIEW,5,col_size);
        redimColumn(hwnd,LV_LOGS_VIEW,7,col_size);
        redimColumn(hwnd,LV_LOGS_VIEW,8,20);

        InvalidateRect(hwnd, NULL, TRUE);
    }else if (uMsg == WM_COMMAND)
    {
      if (HIWORD(wParam) == BN_CLICKED)
      {
        switch(LOWORD(wParam))
        {
          case POPUP_LV_S_SELECTION : LVSaveAll(TABL_ID_VISIBLE, LV_LOGS_VIEW, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL], TRUE, FALSE, FALSE);break;
          case POPUP_LV_S_VIEW : LVSaveAll(TABL_ID_VISIBLE, LV_LOGS_VIEW, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
          case POPUP_LV_S_DELETE : LVDelete(TABL_ID_VISIBLE,LV_LOGS_VIEW);break;
          case POPUP_LV_I_VIEW : CreateThread(NULL,0,csvImport,0,0,0);break;
          case POPUP_LV_C_VIEW : ListView_DeleteAllItems(GetDlgItem(hwnd,LV_LOGS_VIEW));break;
          case POPUP_LV_AS_VIEW :
            {
              char tmp[MAX_PATH];
              SendDlgItemMessage(hwnd,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
              LVAllSearch(GetDlgItem(hwnd,LV_LOGS_VIEW), NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL], tmp);
            }
          break;
          case BT_VIEW_SEARCH:
            {
              char tmp[MAX_PATH];
              SendDlgItemMessage(hwnd,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
              pos_search_logs = LVSearch(GetDlgItem(hwnd,LV_LOGS_VIEW), NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL], tmp, pos_search_logs);
            }
          break;
          case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
          case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
          case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
          case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
          case POPUP_LV_CP_COL5:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),4);break;
          case POPUP_LV_CP_COL6:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),5);break;
          case POPUP_LV_CP_COL7:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),6);break;
          case POPUP_LV_CP_COL8:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),7);break;
          case POPUP_LV_CP_COL9:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),8);break;
          case POPUP_LV_CP_COL10:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),9);break;
          case POPUP_LV_CP_COL11:CopyData(GetDlgItem(hwnd,LV_LOGS_VIEW), SendMessage(GetDlgItem(hwnd,LV_LOGS_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),10);break;
        }
      }
    }else if (uMsg == WM_CONTEXTMENU)TraiterPopupSave(wParam, lParam, hwnd,LV_LOGS_VIEW_NB_COL);
    else if (uMsg == WM_NOTIFY)
    {
      if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
      {
        if (!h_scan_logs)c_Tri(GetDlgItem(hwnd,LV_LOGS_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);
      }else if (((LPNMHDR)lParam)->code == NM_DBLCLK)
      {
        switch(LOWORD(wParam))
        {
          case LV_LOGS_VIEW:
            LVtoMessage(GetDlgItem(hwnd,LV_LOGS_VIEW), NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]);
          break;
        }
      }else if (((LPNMHDR)lParam)->code == EN_CHANGE)
      {
        if (LOWORD(wParam) == ED_SEARCH)pos_search_logs = 0;
      }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_files(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SIZE)
    {
        unsigned int mWidth = LOWORD(lParam);  // width of client area
        unsigned int mHeight = HIWORD(lParam);  // width of client area
        MoveWindow(GetDlgItem(hwnd,LV_FILES_VIEW),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,TV_VIEW),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,BT_TREE_VIEW),mWidth/4+120,mHeight-26,100,21,TRUE);

        MoveWindow(GetDlgItem(hwnd,ED_SEARCH),5,mHeight-26,mWidth/4,21,TRUE);
        MoveWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH),mWidth/4+10,mHeight-26,100,21,TRUE);

        //redimmensionnement des colonnes
        unsigned int col_size = (mWidth-210)/7;
        if (col_size>100)
        {
          col_size = (mWidth-570)/5;
          redimColumn(hwnd,LV_FILES_VIEW,1,col_size);
          redimColumn(hwnd,LV_FILES_VIEW,3,col_size);
          redimColumn(hwnd,LV_FILES_VIEW,7,col_size);
          redimColumn(hwnd,LV_FILES_VIEW,13,col_size/2);
          redimColumn(hwnd,LV_FILES_VIEW,14,col_size/2);
          redimColumn(hwnd,LV_FILES_VIEW,15,col_size);

          redimColumn(hwnd,LV_FILES_VIEW,4,100);
          redimColumn(hwnd,LV_FILES_VIEW,5,100);
          redimColumn(hwnd,LV_FILES_VIEW,6,100);
        }else
        {
          col_size = (mWidth-370)/5;
          redimColumn(hwnd,LV_FILES_VIEW,1,col_size);
          redimColumn(hwnd,LV_FILES_VIEW,3,col_size);
          redimColumn(hwnd,LV_FILES_VIEW,7,col_size);
          redimColumn(hwnd,LV_FILES_VIEW,13,col_size/2);
          redimColumn(hwnd,LV_FILES_VIEW,14,col_size/2);
          redimColumn(hwnd,LV_FILES_VIEW,15,col_size);

          redimColumn(hwnd,LV_FILES_VIEW,4,33);
          redimColumn(hwnd,LV_FILES_VIEW,5,33);
          redimColumn(hwnd,LV_FILES_VIEW,6,33);
        }
        InvalidateRect(hwnd, NULL, TRUE);
    }else if (uMsg == WM_COMMAND)
    {
      if (HIWORD(wParam) == BN_CLICKED)
      {
        switch(LOWORD(wParam))
        {
          case POPUP_LV_S_SELECTION : LVSaveAll(TABL_ID_VISIBLE, LV_FILES_VIEW, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL], TRUE, FALSE, FALSE);break;
          case POPUP_LV_S_VIEW : LVSaveAll(TABL_ID_VISIBLE, LV_FILES_VIEW, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
          case POPUP_LV_S_DELETE : LVDelete(TABL_ID_VISIBLE,LV_FILES_VIEW);break;
          case POPUP_LV_I_VIEW : CreateThread(NULL,0,csvImport,0,0,0);break;
          case POPUP_LV_C_VIEW : ListView_DeleteAllItems(GetDlgItem(hwnd,LV_FILES_VIEW));break;
          case POPUP_LV_AS_VIEW :
            {
              char tmp[MAX_PATH];
              SendDlgItemMessage(hwnd,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
              LVAllSearch(GetDlgItem(hwnd,LV_FILES_VIEW), NB_COLONNE_LV[LV_FILES_VIEW_NB_COL], tmp);
            }
          break;
          case BT_VIEW_SEARCH:
            {
              char tmp[MAX_PATH];
              SendDlgItemMessage(hwnd,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
              pos_search_files = LVSearch(GetDlgItem(hwnd,LV_FILES_VIEW), NB_COLONNE_LV[LV_FILES_VIEW_NB_COL], tmp, pos_search_files);
            }
          break;
          case BT_TREE_VIEW:
            if (TV_FILES_VISBLE)
            {
              TV_FILES_VISBLE = FALSE;
              ShowWindow(GetDlgItem(hwnd,TV_VIEW), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,LV_FILES_VIEW), SW_SHOW);
              ShowWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH), SW_SHOW);
              ShowWindow(GetDlgItem(hwnd,ED_SEARCH), SW_SHOW);
            }else
            {
              TV_FILES_VISBLE = TRUE;
              ShowWindow(GetDlgItem(hwnd,LV_FILES_VIEW), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,ED_SEARCH), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,TV_VIEW), SW_SHOW);
            }
          break;
          case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
          case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
          case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
          case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
          case POPUP_LV_CP_COL5:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),4);break;
          case POPUP_LV_CP_COL6:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),5);break;
          case POPUP_LV_CP_COL7:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),6);break;
          case POPUP_LV_CP_COL8:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),7);break;
          case POPUP_LV_CP_COL9:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),8);break;
          case POPUP_LV_CP_COL10:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),9);break;
          case POPUP_LV_CP_COL11:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),10);break;
          case POPUP_LV_CP_COL12:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),11);break;
          case POPUP_LV_CP_COL13:CopyData(GetDlgItem(hwnd,LV_FILES_VIEW), SendMessage(GetDlgItem(hwnd,LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),12);break;

          //popup treeview
          case POPUP_TV_CP_COMPLET_PATH:CopyTVData(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0));break;
          case POPUP_TV_OPEN:
          {
            //lecture du path du fichier
            char file[MAX_PATH];
            GetItemPath(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0), file, MAX_PATH);
            //ouverture ^^
            ShellExecute(Tabl[TABL_MAIN], "open", file, NULL,NULL,SW_SHOW);
          }
          break;
          case POPUP_TV_ADD:
          {
            //lecture du path du fichier
            char file[MAX_PATH];
            GetItemPath(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0), file, MAX_PATH);

            OPENFILENAME ofn;
            char tmp[MAX_PATH]="";
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = Tabl[TABL_MAIN];
            ofn.lpstrFile = tmp;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter ="All files (*.*)\0*.*";
            ofn.nFilterIndex = 1;
            ofn.Flags =OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST| OFN_ALLOWMULTISELECT | OFN_EXPLORER;
            ofn.lpstrDefExt ="*.*\0";

            if (GetOpenFileName(&ofn)==TRUE)
            {
              if (ofn.nFileOffset > 0)
              {
                //ajout du nom du fichier à copier ^^
                strncat(file,"\\\0",MAX_PATH);
                char *c = tmp;
                while (*c)c++;
                while (*c!='\\')c--;
                c++;
                strncat(file,c,MAX_PATH);
                strncat(file,"\0",MAX_PATH);

                CopyFile(tmp,file,TRUE);
              }
            }
          }
          break;
          case POPUP_TV_COPY:
          {
            //lecture du path du fichier
            char file[MAX_PATH];
            GetItemPath(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0), file, MAX_PATH);

            BROWSEINFO browser;
            LPITEMIDLIST lip;
            char tmp[MAX_PATH+1]="";
            browser.hwndOwner = Tabl[TABL_MAIN];;
            browser.pidlRoot = 0;
            browser.lpfn = 0;
            browser.iImage = 0;
            browser.ulFlags = BIF_NEWDIALOGSTYLE; //permet la création d'un dossier
            browser.lParam = 0;
            browser.pszDisplayName = tmp;  //résultat ici
            browser.lpszTitle = "Copy to...";
            lip = SHBrowseForFolder(&browser);
            if (lip != NULL)
            {
              SHGetPathFromIDList(lip,tmp);

              //ajout du nom du fichier à copier ^^
              strncat(tmp,"\\\0",MAX_PATH);
              char *c = file;
              while (*c)c++;
              while (*c!='\\')c--;
              c++;
              strncat(tmp,c,MAX_PATH);
              strncat(tmp,"\0",MAX_PATH);

              if (!CopyFile(file,tmp,TRUE))
              {
                if(MessageBox(0,"Force the copy ?","File copy error !",MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
                  CopyFile(file,tmp,FALSE);
              }
            }
          }
          break;
          case POPUP_TV_DELETE:
          {
            //lecture du path du fichier
            char file[MAX_PATH];
            GetItemPath(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0), file, MAX_PATH);

            if(MessageBox(0,file,"Delete ?",MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
            {
              //test si un répertoire ou un fichier ^^ + suppression
              BOOL ok = FALSE;
              if((GetFileAttributes(file) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)ok = RemoveDirectory(file);
              else ok = DeleteFile(file);

              //suppression de l'item !!!
              if(ok)SendDlgItemMessage(hwnd, TV_VIEW, TVM_DELETEITEM, (WPARAM)NULL, (LPARAM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0));
            }
          }
          break;
          case POPUP_LV_P_VIEW: //open registry path
          {
            HANDLE hlv = GetDlgItem(hwnd,LV_FILES_VIEW);
            char path[MAX_PATH];

            ListView_GetItemText(hlv,SendMessage(hlv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0,path,MAX_PATH);
            ShellExecute(Tabl[TABL_MAIN], "open","explorer",path,NULL,SW_SHOW);
          }
          break;
        }
      }
    }else if (uMsg == WM_CONTEXTMENU)TraiterPopupSave(wParam, lParam, hwnd,LV_FILES_VIEW_NB_COL);
    else if (uMsg == WM_NOTIFY)
    {
      if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
      {
        if (!h_scan_files)c_Tri(GetDlgItem(hwnd,LV_FILES_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);
      }else if (((LPNMHDR)lParam)->code == EN_CHANGE)
      {
        if (LOWORD(wParam) == ED_SEARCH)pos_search_files = 0;
      }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_registry(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SIZE)
    {
        unsigned int mWidth = LOWORD(lParam);  // width of client area
        unsigned int mHeight = HIWORD(lParam);  // width of client area

        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_PATH),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_MRU),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_PASSWORD),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_USERS),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_LAN),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_START),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_USB),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_HISTORIQUE),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_SERVICES),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_MAJ),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_LOGICIEL),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_CONF),5,0,mWidth-10,mHeight-31,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_REGISTRY_VIEW),5,0,mWidth-10,mHeight-31,TRUE);

        MoveWindow(GetDlgItem(hwnd,TV_VIEW),5,0,mWidth-10,mHeight-31,TRUE);

        MoveWindow(GetDlgItem(hwnd,ED_SEARCH),5,mHeight-26,mWidth/4,21,TRUE);
        MoveWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH),mWidth/4+10,mHeight-26,100,21,TRUE);
        MoveWindow(GetDlgItem(hwnd,BT_TREE_VIEW),mWidth/4+120,mHeight-26,100,21,TRUE);

        MoveWindow(GetDlgItem(hwnd,CB_REGISTRY_VIEW),mWidth/4+225,mHeight-26,200,220,TRUE);

        //redimmensionnement des colonnes
        unsigned int col_size = (mWidth-290)/2;
        redimColumn(hwnd,LV_REGISTRY_VIEW,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_VIEW,3,col_size);

        col_size = (mWidth-250)/3;
        redimColumn(hwnd,LV_REGISTRY_CONF,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_CONF,3,col_size);
        redimColumn(hwnd,LV_REGISTRY_CONF,4,col_size);

        col_size = (mWidth-390)/3;
        redimColumn(hwnd,LV_REGISTRY_LOGICIEL,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_LOGICIEL,3,col_size);
        redimColumn(hwnd,LV_REGISTRY_LOGICIEL,4,col_size);

        col_size = (mWidth-330)/3;
        redimColumn(hwnd,LV_REGISTRY_MAJ,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_MAJ,3,col_size);
        redimColumn(hwnd,LV_REGISTRY_MAJ,4,col_size);

        col_size = (mWidth-350)/3;
        redimColumn(hwnd,LV_REGISTRY_SERVICES,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_SERVICES,4,col_size);
        redimColumn(hwnd,LV_REGISTRY_SERVICES,5,col_size);

        redimColumn(hwnd,LV_REGISTRY_HISTORIQUE,2,mWidth-390);

        col_size = (mWidth-300)/2;
        redimColumn(hwnd,LV_REGISTRY_USB,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_USB,3,col_size);

        col_size = (mWidth-190)/2;
        redimColumn(hwnd,LV_REGISTRY_START,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_START,3,col_size);

        col_size = (mWidth-520)/2;
        redimColumn(hwnd,LV_REGISTRY_LAN,1,col_size);
        redimColumn(hwnd,LV_REGISTRY_LAN,7,col_size);

        col_size = (mWidth-340)/4;
        redimColumn(hwnd,LV_REGISTRY_USERS,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_USERS,3,col_size);
        redimColumn(hwnd,LV_REGISTRY_USERS,4,col_size);
        redimColumn(hwnd,LV_REGISTRY_USERS,9,col_size);

        col_size = (mWidth-390)/2;
        redimColumn(hwnd,LV_REGISTRY_PASSWORD,4,col_size);
        redimColumn(hwnd,LV_REGISTRY_PASSWORD,5,col_size);

        col_size = (mWidth-190)/4;
        redimColumn(hwnd,LV_REGISTRY_MRU,2,col_size);
        redimColumn(hwnd,LV_REGISTRY_MRU,3,col_size);
        redimColumn(hwnd,LV_REGISTRY_MRU,4,col_size);
        redimColumn(hwnd,LV_REGISTRY_MRU,5,col_size);

        col_size = (mWidth-190)/2;
        redimColumn(hwnd,LV_REGISTRY_PATH,0,50);
        redimColumn(hwnd,LV_REGISTRY_PATH,1,col_size);
        redimColumn(hwnd,LV_REGISTRY_PATH,2,100);
        redimColumn(hwnd,LV_REGISTRY_PATH,3,col_size);

        InvalidateRect(hwnd, NULL, TRUE);
    }else if (uMsg == WM_COMMAND)
    {
      switch(HIWORD(wParam))
      {
        case BN_CLICKED:
          switch(LOWORD(wParam))
          {
            case POPUP_LV_S_SELECTION :
              if (TABL_ID_REG_VISIBLE == 0)LVSaveAll(TABL_ID_VISIBLE, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL], TRUE, FALSE,TRUE);
              else if (TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL == LV_REGISTRY_USERS_NB_COL)LVSaveAll(TABL_ID_VISIBLE, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL], TRUE, TRUE, FALSE);
              else LVSaveAll(TABL_ID_VISIBLE, TABL_ID_REG_VISIBLE+LV_FILES_VIEW, NB_COLONNE_LV[TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL], TRUE, FALSE, FALSE);
            break;
            case POPUP_LV_S_VIEW :
              if (TABL_ID_REG_VISIBLE == 0)LVSaveAll(TABL_ID_VISIBLE, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL], FALSE, FALSE, TRUE);
              else if (TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL == LV_REGISTRY_USERS_NB_COL)LVSaveAll(TABL_ID_VISIBLE, TABL_ID_REG_VISIBLE+LV_FILES_VIEW, NB_COLONNE_LV[TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL], FALSE, TRUE, FALSE);
              else LVSaveAll(TABL_ID_VISIBLE, TABL_ID_REG_VISIBLE+LV_FILES_VIEW, NB_COLONNE_LV[TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL], FALSE, FALSE, FALSE);
            break;
            case POPUP_LV_S_DELETE :
              if (TABL_ID_REG_VISIBLE == 0)LVDelete(TABL_ID_VISIBLE, LV_REGISTRY_VIEW);
              else LVDelete(TABL_ID_VISIBLE, TABL_ID_REG_VISIBLE+LV_FILES_VIEW);
            break;
          case POPUP_LV_I_VIEW : CreateThread(NULL,0,csvImport,0,0,0);break;
          case POPUP_LV_C_VIEW :
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_VIEW));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_CONF));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_LOGICIEL));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_MAJ));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_SERVICES));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_HISTORIQUE));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_USB));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_START));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_LAN));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_USERS));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_PASSWORD));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_MRU));
            ListView_DeleteAllItems(GetDlgItem(hwnd,LV_REGISTRY_PATH));

            SendDlgItemMessage(hwnd,TV_VIEW,TVM_DELETEITEM,(WPARAM)0, (LPARAM)TVI_ROOT);
          break;
          case POPUP_LV_AS_VIEW :
            {
              char tmp[MAX_PATH];
              SendDlgItemMessage(hwnd,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
              if (TABL_ID_REG_VISIBLE==0)LVAllSearch(GetDlgItem(hwnd,LV_REGISTRY_VIEW), NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL], tmp);
              else LVAllSearch(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE+LV_FILES_VIEW), NB_COLONNE_LV[TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL], tmp);
            }
          break;
          case POPUP_LV_P_VIEW: //open registry path
          {
            HANDLE hlv;
            if (TABL_ID_REG_VISIBLE==0)hlv = GetDlgItem(hwnd,LV_REGISTRY_VIEW);
            else hlv = GetDlgItem(hwnd,TABL_ID_REG_VISIBLE+LV_FILES_VIEW);

            char path[MAX_PATH];
            ListView_GetItemText(hlv,SendMessage(hlv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,path,MAX_PATH);
            OpenRegeditKey(path);
          }
          break;
          case BT_VIEW_SEARCH:
            {
              char tmp[MAX_PATH];
              SendDlgItemMessage(hwnd,ED_SEARCH,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
              if (TABL_ID_REG_VISIBLE==0)pos_search_registry = LVSearch(GetDlgItem(hwnd,LV_REGISTRY_VIEW), NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL], tmp, pos_search_registry);
              else pos_search_registry = LVSearch(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE+LV_FILES_VIEW), NB_COLONNE_LV[TABL_ID_REG_VISIBLE+LV_FILES_VIEW_NB_COL], tmp, pos_search_registry);
            }
          break;
          case BT_TREE_VIEW:
            if (TV_REGISTRY_VISIBLE)
            {
              TV_REGISTRY_VISIBLE = FALSE;
              ShowWindow(GetDlgItem(hwnd,TV_VIEW), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,LV_REGISTRY_VIEW), SW_SHOW);
              ShowWindow(GetDlgItem(hwnd,CB_REGISTRY_VIEW), SW_SHOW);
              ShowWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH), SW_SHOW);
              ShowWindow(GetDlgItem(hwnd,ED_SEARCH), SW_SHOW);
            }else
            {
              TV_REGISTRY_VISIBLE = TRUE;
              ShowWindow(GetDlgItem(hwnd,LV_REGISTRY_VIEW), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,CB_REGISTRY_VIEW), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,BT_VIEW_SEARCH), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,ED_SEARCH), SW_HIDE);
              ShowWindow(GetDlgItem(hwnd,TV_VIEW), SW_SHOW);
            }
          break;
          case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
          case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
          case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
          case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
          case POPUP_LV_CP_COL5:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),4);break;
          case POPUP_LV_CP_COL6:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),5);break;
          case POPUP_LV_CP_COL7:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),6);break;
          case POPUP_LV_CP_COL8:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),7);break;
          case POPUP_LV_CP_COL9:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                         , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),8);break;
          case POPUP_LV_CP_COL10:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                          , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),9);break;
          case POPUP_LV_CP_COL11:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                          , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),10);break;
          case POPUP_LV_CP_COL12:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                          , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),11);break;
          case POPUP_LV_CP_COL13:CopyData(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW)
                                          , SendMessage(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE==0?LV_REGISTRY_VIEW:TABL_ID_REG_VISIBLE+LV_FILES_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),12);break;

          case POPUP_TV_CP_COMPLET_PATH:CopyTVData(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0));break;
          case POPUP_TV_OPEN:OpenTVRegistryPath(hwnd,TV_VIEW, (HTREEITEM)SendDlgItemMessage(hwnd, TV_VIEW, TVM_GETNEXTITEM, TVGN_CARET, 0));break;

          break;
          }
        break;
        case CBN_SELCHANGE:
          //en cas de changement de filtre pour les informations à afficher
          if (LOWORD(wParam) == CB_REGISTRY_VIEW)
          {
            DWORD index = SendDlgItemMessage(hwnd,CB_REGISTRY_VIEW, CB_GETCURSEL, 0, 0);
            if (TABL_ID_REG_VISIBLE != index && !TV_REGISTRY_VISIBLE)
            {
              if (TABL_ID_REG_VISIBLE == 0)
              {
                ShowWindow(GetDlgItem(hwnd,LV_REGISTRY_VIEW), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd,BT_TREE_VIEW), SW_HIDE);
              }else ShowWindow(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE+LV_FILES_VIEW), SW_HIDE);

              TABL_ID_REG_VISIBLE = index;
              if (TABL_ID_REG_VISIBLE == 0)
              {
                ShowWindow(GetDlgItem(hwnd,LV_REGISTRY_VIEW), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd,BT_TREE_VIEW), SW_SHOW);
              }else
              {
                ShowWindow(GetDlgItem(hwnd,TABL_ID_REG_VISIBLE+LV_FILES_VIEW), SW_SHOW);
              }
            }
          }
        break;
      }
    }else if (uMsg == WM_CONTEXTMENU)TraiterPopupSave(wParam, lParam, hwnd,LV_REGISTRY_CONF_NB_COL);
    else if (uMsg == WM_NOTIFY)
    {
      if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
      {
        //suivant l'index visible ^^
        if (!h_scan_registry)c_Tri(((LPNMHDR)lParam)->hwndFrom,((LPNMLISTVIEW)lParam)->iSubItem);
      }else if (((LPNMHDR)lParam)->code == EN_CHANGE)
      {
        if (LOWORD(wParam) == ED_SEARCH)pos_search_registry = 0;
      }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_process(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SIZE)
  {
      unsigned int mWidth = LOWORD(lParam);  // width of client area
      unsigned int mHeight = HIWORD(lParam);  // width of client area
      MoveWindow(GetDlgItem(hwnd,LV_VIEW),5,0,mWidth-10,mHeight-5,TRUE);

      //redimmensionnement des colonnes
      unsigned int col_size = (mWidth-380)/3;
      redimColumn(hwnd,LV_VIEW,2,col_size);
      redimColumn(hwnd,LV_VIEW,4,col_size);

      col_size = col_size/9;
      redimColumn(hwnd,LV_VIEW,6,col_size);
      redimColumn(hwnd,LV_VIEW,7,col_size*2);
      redimColumn(hwnd,LV_VIEW,8,col_size);
      redimColumn(hwnd,LV_VIEW,9,col_size*2);
      redimColumn(hwnd,LV_VIEW,10,col_size);
      redimColumn(hwnd,LV_VIEW,11,col_size*2);

      InvalidateRect(hwnd, NULL, TRUE);
  }else if (uMsg == WM_COMMAND)
  {
    if (HIWORD(wParam) == BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
        //gestion des injections de dll
        case ADD_DLL_INJECT_REMOTE_THREAD:
        case REM_DLL_INJECT_REMOTE_THREAD:
        {
          //lecture du pid du processus
          char tmp[MAX_PATH]="";
          HANDLE hlv = GetDlgItem(hwnd,LV_VIEW);
          ListView_GetItemText(hlv,SendMessage(hlv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,tmp,MAX_PATH);
          DWORD pid = atoi(tmp);

          //choix de la DLL
          tmp[0]=0;
          OPENFILENAME ofnFile;
          ZeroMemory(&ofnFile,sizeof(OPENFILENAME));
          ofnFile.lStructSize   = sizeof(OPENFILENAME);
          ofnFile.hwndOwner     = Tabl[TABL_MAIN];
          ofnFile.lpstrFile     = tmp;
          ofnFile.nMaxFile      = MAX_PATH;
          ofnFile.lpstrFilter   = "dll (*.dll)\0*.dll\0";
          ofnFile.nFilterIndex  = 1;
          ofnFile.lpstrTitle    = "Chose DLL";
          ofnFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
          if(GetOpenFileName(&ofnFile))
          {
            //élévation de privilège
            //SetDebugPrivilege(TRUE);

            //injection or not ^^
            if (LOWORD(wParam) == ADD_DLL_INJECT_REMOTE_THREAD)DllInjecteurA(pid,tmp);
            else DllEjecteurA(pid,tmp);

            //SetDebugPrivilege(FALSE);
          }
        }
        break;
        case POPUP_LV_S_SELECTION : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW, NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL], TRUE, FALSE, FALSE);break;
        case POPUP_LV_S_VIEW : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW, NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
        case POPUP_LV_S_DELETE : LVDelete(TABL_ID_VISIBLE, LV_VIEW);break;

        case POPUP_DUMP_MEMORY :
        {
          //lecture du pid du process
          HANDLE hlv = GetDlgItem(hwnd,LV_VIEW);
          char cpid[MAX_PATH];
          ListView_GetItemText(hlv,SendMessage(hlv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,cpid,MAX_PATH);

          CreateThread(NULL,0,DumpProcessMemory,(PVOID)atoi(cpid),0,0);
        }
        break;
        /*case POPUP_ALL_PROCESS_AND_THREAD:
          EnumProcessAndThread(GetDlgItem(Tabl[TABL_PROCESS],LV_VIEW), NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL]);
        break;*/

        case POPUP_KILL_PROCESS : KilllvProcess(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 1);break;
        case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
        case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
        case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
        case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
        case POPUP_LV_CP_COL5:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),4);break;
        case POPUP_LV_CP_COL6:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),5);break;
        case POPUP_LV_CP_COL7:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),6);break;
        case POPUP_LV_P_VIEW: //open registry path
        {
          HANDLE hlv = GetDlgItem(hwnd,LV_VIEW);
          char path[MAX_PATH];

          ListView_GetItemText(hlv,SendMessage(hlv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2,path,MAX_PATH);

          //test du path
          if (strlen(path)>0)
          {
            if (path[0]=='\\' && path[1]!='\\')
            {
              if (path[1]=='?' && path[4]!='\\')
              {
                //on supprime les 4 premiers données ^^
                char ext_path[MAX_PATH];
                strcpy(ext_path,path+4);

                //suppression du nom de fichier ^^
                char *c = ext_path;
                while(*c++);
                while(*c!='\\')c--;
                *c =0;

                ShellExecute(Tabl[TABL_MAIN], "open","explorer",ext_path,NULL,SW_SHOW);
              }else if (path[0]=='\\' && path[1]=='S' && path[2]=='y' && path[3]=='s' && path[4]=='t' && path[5]=='e' && path[6]=='m' && path[7]=='R')
              {
                //variable système
                char path2[MAX_PATH];
                if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion","SystemRoot",path2))
                {
                  //ajout du path lue
                  strncat(path2,path+11,MAX_PATH);
                  strncat(path2,"\0",MAX_PATH);

                  //suppression du nom de fichier ^^
                  char *c = path2;
                  while(*c++);
                  while(*c!='\\')c--;
                  *c =0;

                  ShellExecute(Tabl[TABL_MAIN], "open","explorer",path2,NULL,SW_SHOW);
                }
              }
            }else
            {
                //suppression du nom de fichier ^^
                char *c = path;
                while(*c++);
                while(*c!='\\')c--;
                *c =0;

                ShellExecute(Tabl[TABL_MAIN], "open","explorer",path,NULL,SW_SHOW);
            }
          }
        }
        break;
      }
    }
  }else if (uMsg == WM_CONTEXTMENU)
  {
    //popup pour sauvegarde + sup d'item
    HMENU hmenu;
    if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(POPUP_LV_PROCESS)))!= NULL)
    {
      //si pas d'item
      if (ListView_GetItemCount((HANDLE)wParam) <1)
      {
        RemoveMenu(hmenu,POPUP_LV_S_VIEW,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_S_SELECTION,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_P_VIEW,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_KILL_PROCESS,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_S_DELETE,MF_BYCOMMAND|MF_GRAYED);

        RemoveMenu(hmenu,ADD_DLL_INJECT_REMOTE_THREAD,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,REM_DLL_INJECT_REMOTE_THREAD,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_DUMP_MEMORY,MF_BYCOMMAND|MF_GRAYED);

        RemoveMenu(hmenu,POPUP_LV_CP_COL1,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL2,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL3,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL4,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL5,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL6,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL7,MF_BYCOMMAND|MF_GRAYED);
      }else if (ListView_GetSelectedCount((HANDLE)wParam)<1)
      {
        //si pas de sélection
        RemoveMenu(hmenu,POPUP_LV_S_SELECTION,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_P_VIEW,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_KILL_PROCESS,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_S_DELETE,MF_BYCOMMAND|MF_GRAYED);

        RemoveMenu(hmenu,ADD_DLL_INJECT_REMOTE_THREAD,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,REM_DLL_INJECT_REMOTE_THREAD,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_DUMP_MEMORY,MF_BYCOMMAND|MF_GRAYED);

        RemoveMenu(hmenu,POPUP_LV_CP_COL1,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL2,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL3,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL4,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL5,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL6,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL7,MF_BYCOMMAND|MF_GRAYED);
      }
      //affichage du popup menu
      TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
      DestroyMenu(hmenu);
    }
  }else if (uMsg == WM_NOTIFY)
  {
    if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
    {
      c_Tri(GetDlgItem(hwnd,LV_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);
    }if ((LOWORD(wParam) == LV_VIEW) && (((LPNMHDR)lParam)->code == NM_DBLCLK))
    {
      //chargement des infos du processus
      ReadProcessInfo(GetDlgItem(hwnd,LV_VIEW),SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED));

      //affichage de la fenêtre
      ShowWindow(Tabl[TABL_INFO], SW_SHOW);
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_state(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SIZE)
  {
      unsigned int mWidth = LOWORD(lParam);  // width of client area
      unsigned int mHeight = HIWORD(lParam);  // width of client area

      MoveWindow(GetDlgItem(hwnd,LV_VIEW),5,0,mWidth-10,mHeight-31,TRUE);
      MoveWindow(GetDlgItem(hwnd,LV_VIEW_H),5,0,mWidth-10,mHeight-31,TRUE);
      MoveWindow(GetDlgItem(hwnd,LV_VIEW_CRITICAL),5,0,mWidth-10,mHeight-31,TRUE);
      MoveWindow(GetDlgItem(hwnd,CB_STATE_VIEW),mWidth/4+225,mHeight-26,200,200,TRUE);

      //redimmensionnement des colonnes
      unsigned int col_size = (mWidth-220)/2;
      redimColumn(hwnd,LV_VIEW,0,120);
      redimColumn(hwnd,LV_VIEW,1,60);
      redimColumn(hwnd,LV_VIEW,2,col_size);
      redimColumn(hwnd,LV_VIEW,3,col_size);
      redimColumn(hwnd,LV_VIEW_CRITICAL,0,120);
      redimColumn(hwnd,LV_VIEW_CRITICAL,1,60);
      redimColumn(hwnd,LV_VIEW_CRITICAL,2,col_size);
      redimColumn(hwnd,LV_VIEW_CRITICAL,3,col_size);
      redimColumn(hwnd,LV_VIEW_H,0,(col_size+190)/3);
      redimColumn(hwnd,LV_VIEW_H,1,(col_size+190)/3);
      redimColumn(hwnd,LV_VIEW_H,2,(col_size+190)/3);
      redimColumn(hwnd,LV_VIEW_H,3,col_size);

      InvalidateRect(hwnd, NULL, TRUE);
  }else if (uMsg == WM_COMMAND)
  {
    switch(HIWORD(wParam))
    {
      case BN_CLICKED:
        switch(LOWORD(wParam))
        {
          case POPUP_LV_S_SELECTION : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW+TABL_ID_STATE_VISIBLE, NB_COLONNE_LV[LV_STATE_VIEW_NB_COL], TRUE, FALSE, FALSE);break;
          case POPUP_LV_S_VIEW : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW+TABL_ID_STATE_VISIBLE, NB_COLONNE_LV[LV_STATE_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
          //case POPUP_LV_AS_VIEW : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW_H, NB_COLONNE_LV[LV_STATE_H_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
          case POPUP_LV_S_DELETE : LVDelete(TABL_ID_VISIBLE, LV_VIEW+TABL_ID_STATE_VISIBLE);break;
          case POPUP_LV_C_VIEW : ListView_DeleteAllItems(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE));break;

          case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE), SendMessage(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
          case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE), SendMessage(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
          case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE), SendMessage(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
          case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE), SendMessage(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
        }
      break;
      case CBN_SELCHANGE:
        //en cas de changement de filtre pour les informations à afficher
        if (LOWORD(wParam) == CB_STATE_VIEW)
        {
          DWORD index = SendDlgItemMessage(hwnd,CB_STATE_VIEW, CB_GETCURSEL, 0, 0);
          if (TABL_ID_STATE_VISIBLE != index)
          {
            //on masque l'ancien ^^
            switch(TABL_ID_STATE_VISIBLE)
            {
              case 0: ShowWindow(GetDlgItem(hwnd,LV_VIEW), SW_HIDE); break;//all
              case 1: ShowWindow(GetDlgItem(hwnd,LV_VIEW_CRITICAL), SW_HIDE); break;//critical
              case 2: ShowWindow(GetDlgItem(hwnd,LV_VIEW_H), SW_HIDE); break;//time
            }

            //on affiche le nouveau
            switch(index)
            {
              case 0: ShowWindow(GetDlgItem(hwnd,LV_VIEW), SW_SHOW); break;//all
              case 1: ShowWindow(GetDlgItem(hwnd,LV_VIEW_CRITICAL), SW_SHOW); break;//critical
              case 2: ShowWindow(GetDlgItem(hwnd,LV_VIEW_H), SW_SHOW); break;//time
            }
            TABL_ID_STATE_VISIBLE = index;
          }
        }
      break;
    }
  }else if (uMsg == WM_CONTEXTMENU)
  {
    if (ListView_GetItemCount(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE)) != 0)
    {
      //popup pour sauvegarde + sup d'item
      HMENU hmenu;
      if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(POPUP_LV_STATE)))!= NULL)
      {
        if (ListView_GetSelectedCount(GetDlgItem(hwnd,LV_VIEW+TABL_ID_STATE_VISIBLE))<1)
        {
          //si pas de sélection
          RemoveMenu(hmenu,POPUP_LV_S_SELECTION,MF_BYCOMMAND|MF_GRAYED);
          RemoveMenu(hmenu,POPUP_LV_S_DELETE,MF_BYCOMMAND|MF_GRAYED);

          RemoveMenu(hmenu,POPUP_LV_CP_COL1,MF_BYCOMMAND|MF_GRAYED);
          RemoveMenu(hmenu,POPUP_LV_CP_COL2,MF_BYCOMMAND|MF_GRAYED);
          RemoveMenu(hmenu,POPUP_LV_CP_COL3,MF_BYCOMMAND|MF_GRAYED);
          RemoveMenu(hmenu,POPUP_LV_CP_COL4,MF_BYCOMMAND|MF_GRAYED);
        }else if (TABL_ID_STATE_VISIBLE == 2)//si dans le 3ème listv
        {
          ModifyMenu(hmenu,POPUP_LV_CP_COL1,MF_BYCOMMAND|MF_STRING,POPUP_LV_CP_COL1,"Copy : Start");
          ModifyMenu(hmenu,POPUP_LV_CP_COL2,MF_BYCOMMAND|MF_STRING,POPUP_LV_CP_COL2,"Copy : End");
          ModifyMenu(hmenu,POPUP_LV_CP_COL3,MF_BYCOMMAND|MF_STRING,POPUP_LV_CP_COL3,"Copy : Duration");
          ModifyMenu(hmenu,POPUP_LV_CP_COL4,MF_BYCOMMAND|MF_STRING,POPUP_LV_CP_COL4,"Copy : User");
        }

        //affichage du popup menu
        TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
        DestroyMenu(hmenu);
      }
    }
  }else if (uMsg == WM_NOTIFY)
  {
    if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
    {
      switch(TABL_ID_STATE_VISIBLE)
      {
        case 0: c_Tri(GetDlgItem(hwnd,LV_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);break;
        case 1: c_Tri(GetDlgItem(hwnd,LV_VIEW_CRITICAL),((LPNMLISTVIEW)lParam)->iSubItem);break;
        case 2: c_Tri(GetDlgItem(hwnd,LV_VIEW_H),((LPNMLISTVIEW)lParam)->iSubItem);break;
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_configuration(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SIZE)
  {
      unsigned int mWidth = LOWORD(lParam);  // width of client area
      unsigned int mHeight = HIWORD(lParam);  // width of client area
      MoveWindow(GetDlgItem(hwnd,LV_VIEW),5,0,mWidth-10,mHeight-5,TRUE);

      //redimmensionnement des colonnes
      unsigned int col_size = (mWidth-40)/3;
      redimColumn(hwnd,LV_VIEW,2,col_size);
      redimColumn(hwnd,LV_VIEW,5,col_size);

      redimColumn(hwnd,LV_VIEW,0,col_size/6);
      redimColumn(hwnd,LV_VIEW,1,col_size/6);
      redimColumn(hwnd,LV_VIEW,3,col_size/6);
      redimColumn(hwnd,LV_VIEW,4,col_size/6);
      redimColumn(hwnd,LV_VIEW,6,col_size/6);
      redimColumn(hwnd,LV_VIEW,7,col_size/6);

      InvalidateRect(hwnd, NULL, TRUE);
  }else if (uMsg == WM_COMMAND)
  {
    if (HIWORD(wParam) == BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
        case POPUP_LV_S_SELECTION : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL], TRUE, FALSE, FALSE);break;
        case POPUP_LV_S_VIEW : LVSaveAll(TABL_ID_VISIBLE, LV_VIEW, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL], FALSE, FALSE, FALSE);break;
        case POPUP_LV_S_DELETE : LVDelete(TABL_ID_VISIBLE, LV_VIEW);break;

        case POPUP_LV_I_VIEW : CreateThread(NULL,0,csvImport,0,0,0);break;
        case POPUP_LV_C_VIEW : ListView_DeleteAllItems(GetDlgItem(hwnd,LV_VIEW));break;

        case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
        case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
        case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
        case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
        case POPUP_LV_CP_COL5:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),4);break;
        case POPUP_LV_CP_COL6:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),5);break;
        case POPUP_LV_CP_COL7:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),6);break;
        case POPUP_LV_CP_COL8:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),7);break;
      }
    }
  }else if (uMsg == WM_CONTEXTMENU)TraiterPopupSave(wParam, lParam, hwnd,LV_CONFIGURATION_NB_COL);
  else if (uMsg == WM_NOTIFY)
  {
    if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
    {
      if(!h_scan_configuration)c_Tri(GetDlgItem(hwnd,LV_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_info(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_SIZE)
  {
      unsigned int mWidth = LOWORD(lParam);  // width of client area
      unsigned int mHeight = HIWORD(lParam);  // width of client area

      MoveWindow(GetDlgItem(hwnd,LV_VIEW),5,0,mWidth-10,mHeight-5,TRUE);
      //redimmensionnement des colonnes
      unsigned int col_size = (mWidth-70)/9;
      redimColumn(hwnd,LV_VIEW,0,col_size);
      redimColumn(hwnd,LV_VIEW,1,col_size*2);
      redimColumn(hwnd,LV_VIEW,2,col_size);
      redimColumn(hwnd,LV_VIEW,3,col_size);
      redimColumn(hwnd,LV_VIEW,4,col_size);
      redimColumn(hwnd,LV_VIEW,5,col_size);
      redimColumn(hwnd,LV_VIEW,6,col_size);
      redimColumn(hwnd,LV_VIEW,7,col_size);

      InvalidateRect(hwnd, NULL, TRUE);
  }
  else if (uMsg == WM_COMMAND)
  {
    if (HIWORD(wParam) == BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
        case POPUP_LV_S_SELECTION : LVSaveAll(TABL_INFO, LV_VIEW, NB_COLONNE_LV[LV_INFO_VIEW_NB_COL], TRUE, FALSE, FALSE);break;
        case POPUP_LV_S_VIEW : LVSaveAll(TABL_INFO, LV_VIEW, NB_COLONNE_LV[LV_INFO_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
        case POPUP_LV_S_DELETE : LVDelete(TABL_INFO,LV_VIEW);break;

        case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
        case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
        case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
        case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
        case POPUP_TRV_CONF_OFP:
        {
          if (SendDlgItemMessage(hwnd,LV_VIEW,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0))
          {
            char tmp[MAX_PATH]="";
            ListView_GetItemText(GetDlgItem(hwnd,LV_VIEW),SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,tmp,MAX_PATH);

            unsigned int size = strlen(tmp);
            if (size)
            {
              //on récupère le path
              char *c = tmp+size-1;
              while(*c != '\\')c--;
              c++;
              *c=0;
              ShellExecute(Tabl[TABL_MAIN], "explore", tmp, NULL,NULL,SW_SHOW);
            }
          }
        }
        break;
        case POPUP_LV_PROPERTIES:
          {
            //lecture du path du fichier
            char file[MAX_PATH];
            ListView_GetItemText(GetDlgItem(hwnd,LV_VIEW),SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,file,MAX_PATH);

            if (file[1]==':' || (file[1]=='\\' && file[1]=='\\'))
            {
              //properties
              SHELLEXECUTEINFO se;
              ZeroMemory(&se,sizeof(se));
              se.cbSize = sizeof(se);
              se.fMask = SEE_MASK_INVOKEIDLIST;
              se.lpFile = file;
              se.lpVerb = "properties";
              ShellExecuteEx(&se);
            }
          }
        break;
      }
    }
  }else if (uMsg == WM_CONTEXTMENU)
  {
    HMENU hmenu;
    if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(POPUP_LV_INFO)))!= NULL)
    {
      //vérification si un item est sélectionné ^^
      if (ListView_GetSelectedCount((HANDLE)wParam)<1)
      {
        RemoveMenu(hmenu,POPUP_LV_S_SELECTION,MF_BYCOMMAND|MF_GRAYED);

        RemoveMenu(hmenu,POPUP_LV_CP_COL1,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL2,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL3,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL4,MF_BYCOMMAND|MF_GRAYED);
      }

      //affichage du popup menu
      TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
      DestroyMenu(hmenu);
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
    {
      c_Tri(GetDlgItem(hwnd,LV_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);
    }
  }else if (uMsg == WM_CLOSE)ShowWindow(hwnd, SW_HIDE);

  return FALSE;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_main(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_CTLCOLORBTN :return DefWindowProc(hwnd, WM_CTLCOLORBTN, wParam, lParam);
    case WM_DRAWITEM:DessinerBouton((LPDRAWITEMSTRUCT)lParam);break;
    case WM_SIZE:
    {
      unsigned int mWidth = LOWORD(lParam);  // width of client area
      unsigned int mHeight = HIWORD(lParam);  // width of client area

      //controle de la taille minimum
      if (mWidth<584 ||mHeight<524)
      {
        RECT Rect;
        GetWindowRect(hwnd, &Rect);
        MoveWindow(hwnd,Rect.left,Rect.top,584+20,524+64,TRUE);
      }else
      {
        //redimmensionnement des onglets ^^
        MoveWindow(Tabl[TABL_CONF]    ,0,38,mWidth,mHeight-65,TRUE);
        MoveWindow(Tabl[TABL_LOGS]    ,0,38,mWidth,mHeight-65,TRUE);
        MoveWindow(Tabl[TABL_FILES]   ,0,38,mWidth,mHeight-65,TRUE);
        MoveWindow(Tabl[TABL_REGISTRY],0,38,mWidth,mHeight-65,TRUE);
        MoveWindow(Tabl[TABL_PROCESS],0,38,mWidth,mHeight-65,TRUE);
        MoveWindow(Tabl[TABL_CONFIGURATION],0,38,mWidth,mHeight-65,TRUE);
        MoveWindow(Tabl[TABL_STATE],0,38,mWidth,mHeight-65,TRUE);

        //de la status barre
        MoveWindow(GetDlgItem(hwnd,SB_MAIN), 0, mHeight-5, mWidth,0, TRUE);
        //init de la barre de status
        int TaillePart[4];
        TaillePart[0] = mWidth/4; // messages d'information
        TaillePart[1] = mWidth/2; // état du scanne logs
        TaillePart[2] = (mWidth/4)*3; // état du scanne fichiers
        TaillePart[3] = mWidth; // état du scanne registre
        SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETPARTS,(WPARAM)4, (LPARAM)TaillePart);
      }
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
    case WM_COMMAND:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        switch(LOWORD(wParam))
        {
          case BT_MAIN_CONF : ViewTabl(TABL_CONF);break;
          case BT_MAIN_LOGS : ViewTabl(TABL_LOGS);break;
          case BT_MAIN_FILES : ViewTabl(TABL_FILES);break;
          case BT_MAIN_REGISTRY : ViewTabl(TABL_REGISTRY);break;
          case BT_MAIN_PROCESS : CreateThread(NULL,0,EnumProcess,0,0,0);ViewTabl(TABL_PROCESS);break;
          case BT_MAIN_CONFIGURATION : ViewTabl(TABL_CONFIGURATION);break;
          case BT_MAIN_STATE : ViewTabl(TABL_STATE);break;
        }
      }
    break;
    case WM_DROPFILES://gestion du drag and drop de fichier ^^
      {
         HDROP H_DropInfo=(HDROP)wParam;//récupération de la liste
         char tmp[MAX_PATH];
         DWORD i,nb_path = DragQueryFile(H_DropInfo, 0xFFFFFFFF, tmp, MAX_PATH);
         for (i=0;i<nb_path;i++)
         {
           //traitement des données ^^
           DragQueryFile(H_DropInfo, i, tmp, MAX_PATH);

           //vérification du type de fichier + add^^
           FileToTreeView(tmp);
         }
         DragFinish(H_DropInfo);//libération de la mémoire

         //expend des branches
         SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_FILES]);
         SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_LOGS]);
         SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);
         SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM[TRV_CONF]);
      }
    break;
    case WM_INITDIALOG:InitConfig(hwnd); break;
    case WM_CLOSE:EndConfig();break;
  }
  return FALSE;
}
//------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  hInst = hInstance;

  consol_mode = FALSE;
  console_cmd[0]=0;

  if (lpCmdLine[0]!=0)
  {
    switch(lpCmdLine[0])
    {
      case 'r':
      case 'e':
      case 'p':
      case 'a':
      case 'c':
      case 'l':
      case 'g':
      case 'f':
      case 's':
        consol_mode=TRUE;
        snprintf(console_cmd,MAX_LINE_SIZE,"%s",lpCmdLine);
      break;
      default:
      case 'h': MessageBox(0,"Read to Catch All :\n"
                        "Licensed under the terms of the GNU\n"
                        "General Public License version 3.\n\n"
                        "Author: Nicolas Hanteville\n"
                        "Home: http://code.google.com/p/omnia-projetcs/\n"
                        "----------------------------------\n"
                        "Uses:\n\n"
                        "h : Display this help.\n\n"

                        "r : Read recursively all files and ACL from all system directory and export to CSV/XML/HTML.\n"
                        "    example: r \"c:\\file_to_save.csv\"\n\n"
                        "e : Read recursively all files and ACL from directory and export to CSV/XML/HTML.\n"
                        "    example: e \"c:\\directory_to_read\" \"c:\\file_to_save.csv\"\n\n"

                        "p : Read all process and informations and export to CSV/XML/HTML.\n"
                        "    example: p \"c:\\file_to_save.csv\"\n\n"

                        "a : Read all audit logs and export to CSV/XML/HTML.\n"
                        "    example: a \"c:\\file_to_save.csv\"\n\n"
                        "l : Read audit file (evt, evtx) and export to CSV/XML/HTML.\n"
                        "    example: l \"c:\\file_to_read.evt\" \"c:\\file_to_save.csv\"\n\n"

                        "g : Read local registry and export to CSV/XML/HTML.\n"
                        "    example: g \"c:\\directory_to_save\\\"\n\n"
                        "f : Read registry file (reg, binary registry) and export to CSV.\n"
                        "    example: f \"c:\\file_to_read.reg\" \"c:\\directory_to_save\\\"\n\n"
                        "s : Read SAM+SYTEM binary registry from directory and export to CSV.\n"
                        "    example: f \"c:\\SAM_end_SYSTEM_directory\\\" \"c:\\directory_to_save\\\""

                        "c : Read software configuration and export to CSV/XML/HTML.\n"
                        "    example: c \"c:\\file_to_read.evt\" \"c:\\file_to_save.csv\"\n\n"

                        ,"HELP",MB_OK|MB_TOPMOST);
      break;
    }
  }
  return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DialogProc_main);
}

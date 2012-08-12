//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_conf(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_COMMAND:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        switch(LOWORD(wParam))
        {
          case BT_START:
            if (start_scan) CreateThread(NULL,0,StopGUIScan,0,0,0);
            else
            {
              //init
              start_scan = TRUE;
              //local or not ?
              if(TreeView_GetCount(htrv_files) == NB_MX_TYPE_FILES_TITLE)LOCAL_SCAN = TRUE;
              else LOCAL_SCAN = FALSE;
              //read state !
              if (IsDlgButtonChecked(h_conf,BT_ACL_FILE_CHK)==BST_CHECKED)FILE_ACL=TRUE;
              else FILE_ACL=FALSE;
              if (IsDlgButtonChecked(h_conf,BT_ADS_FILE_CHK)==BST_CHECKED)FILE_ADS=TRUE;
              else FILE_ADS=FALSE;
              if (IsDlgButtonChecked(h_conf,BT_SHA_FILE_CHK)==BST_CHECKED)FILE_SHA=TRUE;
              else FILE_SHA=FALSE;

              EnableWindow(htrv_files,FALSE);
              EnableWindow(GetDlgItem((HWND)h_conf,BT_ACL_FILE_CHK),FALSE);
              EnableWindow(GetDlgItem((HWND)h_conf,BT_ADS_FILE_CHK),FALSE);
              EnableWindow(GetDlgItem((HWND)h_conf,BT_SHA_FILE_CHK),FALSE);

              //create new session and select it !
              SendMessage(hCombo_session, CB_RESETCONTENT,0,0);
              FORMAT_CALBAK_READ_INFO fcri;
              fcri.type = TYPE_SQL_ADD_SESSION;
              SQLITE_WriteData(&fcri, DEFAULT_SQLITE_FILE);

              //start
              h_thread_scan = CreateThread(NULL,0,GUIScan,0,0,0);
              SetWindowText(GetDlgItem((HWND)h_conf,BT_START),cps[TXT_BT_STOP].c);
            }
          break;
          //----------------------------------------
          case POPUP_TRV_FILES_ADD_FILE:
          {
            char file[MAX_PATH]="";
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = h_conf;
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

              //tri and clean
              CleanTreeViewFiles(htrv_files);

              //expend des branches
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
            }
          }
          break;
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
              if (SHGetPathFromIDList(lip,path))
              {
                FileToTreeView(path);

                //tri and clean
                CleanTreeViewFiles(htrv_files);

                //expend all
                SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
                SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
                SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
                SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
              }
            }
          }
          break;
          case POPUP_TRV_FILES_UP:
            {
              //get selection + parent
              HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
              HTREEITEM hparent = (HTREEITEM)SendMessage(htrv_files,TVM_GETNEXTITEM,(WPARAM)TVGN_PARENT, (LPARAM)hitem);
              //get txt
              char tmp[MAX_PATH];
              GetItemTreeView(hitem,htrv_files,tmp, MAX_PATH);
              //add item
              if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_LOGS])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
              else if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_FILES])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
              else if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
              else if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_APPLI])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);

              //remove item
              SendMessage(htrv_files,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
              //expend all
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
            }
          break;
          case POPUP_TRV_FILES_DOWN:
            {
              //get selection + parent
              HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
              HTREEITEM hparent = (HTREEITEM)SendMessage(htrv_files,TVM_GETNEXTITEM,(WPARAM)TVGN_PARENT, (LPARAM)hitem);
              //get txt
              char tmp[MAX_PATH];
              GetItemTreeView(hitem,htrv_files,tmp, MAX_PATH);
              //add item
              if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_LOGS])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
              else if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_FILES])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
              else if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
              else if (hparent == TRV_HTREEITEM_CONF[FILES_TITLE_APPLI])AddItemTreeView(htrv_files,tmp, TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);

              //remove item
              SendMessage(htrv_files,TVM_DELETEITEM,(WPARAM)0, (LPARAM)hitem);
              //expend all
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
              SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);

            }
          break;
          case POPUP_TRV_FILES_REMOVE_ITEMS:TreeView_DeleteItem(htrv_files, (HTREEITEM)SendMessage(htrv_files,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0));break;
          case POPUP_TRV_FILES_CLEAN_ALL:
            SendMessage(htrv_files,TVM_DELETEITEM,(WPARAM)0, (LPARAM)TVI_ROOT);
            check_childs_treeview(htrv_test, FALSE);

            TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]      = AddItemTreeView(htrv_files,cps[TXT_FILE_AUDIT].c, TVI_ROOT);
            TRV_HTREEITEM_CONF[FILES_TITLE_FILES]     = AddItemTreeView(htrv_files,cps[TXT_FILE_REP].c, TVI_ROOT);
            TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]  = AddItemTreeView(htrv_files,cps[TXT_FILE_REGISTRY].c, TVI_ROOT);
            TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]     = AddItemTreeView(htrv_files,cps[TXT_FILE_APPLI].c, TVI_ROOT);
          break;
          case POPUP_TRV_FILES_OPEN_PATH:
          {
            //get item txt
            char path[MAX_PATH];
            TVITEM tvitem;
            tvitem.mask = TVIF_HANDLE|TVIF_TEXT;
            tvitem.hItem = (HTREEITEM)SendMessage(htrv_files,TVM_GETNEXTITEM,(WPARAM)TVGN_CARET, (LPARAM)0);
            tvitem.cchTextMax = MAX_PATH;
            tvitem.pszText = path;
            if (SendMessage(htrv_files,TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
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
                SendMessage(htrv_files,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM_CONF[i]);
                SupDoublon(htrv_files,TRV_HTREEITEM_CONF[i]);
                SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[i]);
              }
            }else
            {
              B_AUTOSEARCH = TRUE;
              h_AUTOSEARCH = CreateThread(NULL,0,AutoSearchFiles,0,0,0);
            }
          break;
          case POPUP_TRV_FILES_SAVE_LIST:
          {
            char file[MAX_PATH]="";
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = h_conf;
            ofn.lpstrFile = file;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter ="*.txt \0*.txt\0*.csv\0*.csv\0";
            ofn.nFilterIndex = 1;
            ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt =".txt\0";
            if (GetSaveFileName(&ofn)==TRUE)
            {
              if (ofn.nFilterIndex == 2) SaveTRV(htrv_files, file, SAVE_TYPE_CSV);
              else SaveTRV(htrv_files, file, SAVE_TYPE_TXT);
            }
          }
          break;
          case POPUP_TRV_CHECK_ALL:check_childs_treeview(htrv_test, TRUE);break;
          case POPUP_TRV_UNCHECK_ALL:check_childs_treeview(htrv_test, FALSE);break;
        }
      }
    break;
    case WM_NOTIFY:
      if (((LPNMHDR)lParam)->code == NM_CLICK && ((LPNMHDR)lParam)->hwndFrom == htrv_test)
      {
        //selected item and state
        TV_HITTESTINFO Struct;
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(htrv_test, &pt);
        Struct.pt = pt;
        HTREEITEM hItemSelect = TreeView_HitTest(htrv_test, &Struct);
        if (hItemSelect != NULL)
        {
          if (Struct.flags == TVHT_ONITEMSTATEICON)TreeView_SelectItem(htrv_test,hItemSelect);
          else//check case
          {
            if (Ischeck_treeview(htrv_test, hItemSelect))check_treeview(htrv_test, hItemSelect, TRV_STATE_CHECK);
            else check_treeview(htrv_test, hItemSelect, TRV_STATE_UNCHECK);
          }
        }
      }
    break;
    case WM_CONTEXTMENU://popup menu
      if ((HWND)wParam == htrv_test)
      {
        //select trv item
        TV_HITTESTINFO tvh_info;
        tvh_info.pt.x = LOWORD(lParam);
        tvh_info.pt.y = HIWORD(lParam);
        ScreenToClient(htrv_test, &(tvh_info.pt));
        HTREEITEM hItemSelect = TreeView_HitTest(htrv_test, &tvh_info);
        if (hItemSelect != 0)TreeView_SelectItem(htrv_test,hItemSelect);

        //popup
        HMENU hmenu;
        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_TRV_TEST)))!= NULL)
        {
          //set text !!!
          ModifyMenu(hmenu,POPUP_TRV_CHECK_ALL          ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_CHECK_ALL          ,cps[TXT_POPUP_CHECK_ALL].c);
          ModifyMenu(hmenu,POPUP_TRV_UNCHECK_ALL        ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_UNCHECK_ALL        ,cps[TXT_POPUP_UNCHECK_ALL].c);

          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }else if ((HWND)wParam == htrv_files)
      {
        //select trv item
        TV_HITTESTINFO tvh_info;
        tvh_info.pt.x = LOWORD(lParam);
        tvh_info.pt.y = HIWORD(lParam);
        ScreenToClient(htrv_files, &(tvh_info.pt));
        HTREEITEM hItemSelect = TreeView_HitTest(htrv_files, &tvh_info);
        if (hItemSelect != NULL)TreeView_SelectItem(htrv_files,hItemSelect);

        //popup
        HMENU hmenu;
        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_TRV_FILES)))!= NULL)
        {
          //set text !!!
          ModifyMenu(hmenu,POPUP_TRV_FILES_ADD_FILE     ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_ADD_FILE     ,cps[TXT_POPUP_ADD_FILE].c);
          ModifyMenu(hmenu,POPUP_TRV_FILES_ADD_PATH     ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_ADD_PATH     ,cps[TXT_POPUP_ADD_PATH].c);

          if (B_AUTOSEARCH)ModifyMenu(hmenu,POPUP_TRV_FILES_AUTO_SEARCH  ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_AUTO_SEARCH  ,cps[TXT_POPUP_AUTO_SEARCH_STOP].c);
          else ModifyMenu(hmenu,POPUP_TRV_FILES_AUTO_SEARCH  ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_AUTO_SEARCH  ,cps[TXT_POPUP_AUTO_SEARCH].c);

          if (SendMessage(htrv_files,TVM_GETCOUNT,(WPARAM)0,(LPARAM)0) > 4)
          {
            ModifyMenu(hmenu,POPUP_TRV_FILES_UP           ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_UP           ,cps[TXT_POPUP_UP].c);
            ModifyMenu(hmenu,POPUP_TRV_FILES_DOWN         ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_DOWN         ,cps[TXT_POPUP_DOWN].c);
            ModifyMenu(hmenu,POPUP_TRV_FILES_REMOVE_ITEMS ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_REMOVE_ITEMS ,cps[TXT_POPUP_REMOVE_ITEMS].c);
            ModifyMenu(hmenu,POPUP_TRV_FILES_CLEAN_ALL    ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_CLEAN_ALL    ,cps[TXT_POPUP_CLEAN_ALL].c);
            ModifyMenu(hmenu,POPUP_TRV_FILES_OPEN_PATH    ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_OPEN_PATH    ,cps[TXT_POPUP_OPEN_PATH].c);
            ModifyMenu(hmenu,POPUP_TRV_FILES_SAVE_LIST    ,MF_BYCOMMAND|MF_STRING ,POPUP_TRV_FILES_SAVE_LIST    ,cps[TXT_POPUP_SAVE_LIST].c);
          }else
          {
            RemoveMenu(GetSubMenu(hmenu,0),2              ,MF_BYPOSITION);

            RemoveMenu(hmenu,POPUP_TRV_FILES_UP           ,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_DOWN         ,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_REMOVE_ITEMS ,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_CLEAN_ALL    ,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_OPEN_PATH    ,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_SAVE_LIST    ,MF_BYCOMMAND);
          }

          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }
    break;
    case WM_DROPFILES://gestion du drag and drop de fichier ^^
    {
      if (B_AUTOSEARCH)break;
      HDROP H_DropInfo=(HDROP)wParam;
      char tmp[MAX_PATH];
      DWORD i,nb_path = DragQueryFile(H_DropInfo, 0xFFFFFFFF, tmp, MAX_PATH);
      for (i=0;i<nb_path;i++)
      {
        //get data
        DragQueryFile(H_DropInfo, i, tmp, MAX_PATH);
        //add
        FileToTreeView(tmp);
      }
      DragFinish(H_DropInfo);

      //tri and clean
      CleanTreeViewFiles(htrv_files);

      //expend des branches
      SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
      SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
      SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
      SendMessage(htrv_files,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
    }
    break;
    case WM_INITDIALOG:
      //add language correction
      SetWindowText(GetDlgItem(hwnd,BT_ACL_FILE_CHK),cps[TXT_CHECK_ACL].c);
      SetWindowText(GetDlgItem(hwnd,BT_SHA_FILE_CHK),cps[TXT_CHECK_SHA].c);
      SetWindowText(GetDlgItem(hwnd,BT_ADS_FILE_CHK),cps[TXT_CHECK_ADS].c);
      SetWindowText(GetDlgItem(hwnd,BT_START),cps[TXT_BT_START].c);

      //check all tests
      CheckDlgButton(hwnd,BT_ACL_FILE_CHK,BST_CHECKED);
      CheckDlgButton(hwnd,BT_SHA_FILE_CHK,BST_CHECKED);
      CheckDlgButton(hwnd,BT_ADS_FILE_CHK,BST_CHECKED);

      //add icon
      SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(ICON_APP)));

      //add files owners
      htrv_files = GetDlgItem(hwnd,TRV_FILES);
      SendMessage(htrv_files,CBEM_SETIMAGELIST,0,(LPARAM)H_ImagList_icon);
      TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]      = AddItemTreeView(htrv_files,cps[TXT_FILE_AUDIT].c, TVI_ROOT);
      TRV_HTREEITEM_CONF[FILES_TITLE_FILES]     = AddItemTreeView(htrv_files,cps[TXT_FILE_REP].c, TVI_ROOT);
      TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]  = AddItemTreeView(htrv_files,cps[TXT_FILE_REGISTRY].c, TVI_ROOT);
      TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]     = AddItemTreeView(htrv_files,cps[TXT_FILE_APPLI].c, TVI_ROOT);

      //add list of test
      //get all tests in list of rubriques
      htrv_test = GetDlgItem(hwnd,TRV_TEST);
      unsigned int i;
      NB_TESTS = SendMessage(hlstbox, LB_GETCOUNT,0,0);
      char tmp[DEFAULT_TMP_SIZE];
      for (i=0;i<NB_TESTS;i++)
      {
        if (SendMessage(hlstbox, LB_GETTEXTLEN,i,0) < DEFAULT_TMP_SIZE)
        {
          tmp[0] = 0;
          if(SendMessage(hlstbox, LB_GETTEXT,i,(LPARAM)tmp) > 0)
          {
            //add item
            H_tests[i] = AddItemTreeView(htrv_test,tmp, TVI_ROOT);
          }
        }
      }
    break;
    case WM_CLOSE:
      ShowWindow (h_main, SW_SHOW);
      EndDialog(hwnd, 0);
    break;
  }
  return 0;
}

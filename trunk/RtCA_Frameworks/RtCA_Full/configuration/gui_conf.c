//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
//view form
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_conf(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_SIZE:
    {
      unsigned int mWidth     = LOWORD(lParam);
      unsigned int mHeight    = HIWORD(lParam);
      unsigned int Width_conf = mWidth/4;
      if (Width_conf<250)Width_conf=250;

      MoveWindow(GetDlgItem(hwnd,ST_SESSION), mWidth-Width_conf+2, 0,Width_conf-2,45, TRUE);
      MoveWindow(GetDlgItem(hwnd,CB_SESSION), mWidth-Width_conf+8, 15, Width_conf-14,240, TRUE);

      MoveWindow(GetDlgItem(hwnd,TRV_FILES), 2, 0, mWidth-Width_conf-5,mHeight-5, TRUE);
      MoveWindow(GetDlgItem(hwnd,TRV_TEST), mWidth-Width_conf+2, 50, Width_conf-2,mHeight-250, TRUE);

      MoveWindow(GetDlgItem(hwnd,GRP_CONF), mWidth-Width_conf+2, mHeight-200, Width_conf-2,155, TRUE);

      MoveWindow(GetDlgItem(hwnd,BT_START), mWidth-Width_conf+2, mHeight-43, Width_conf/2-2,38, TRUE);
      MoveWindow(GetDlgItem(hwnd,BT_EXPORT), mWidth-Width_conf+2+Width_conf/2, mHeight-43, Width_conf/2-2,38, TRUE);

      MoveWindow(GetDlgItem(hwnd,ST_LANG), mWidth-Width_conf+10, mHeight-182, 90,20, TRUE);
      MoveWindow(GetDlgItem(hwnd,CB_LANG), mWidth-Width_conf+100, mHeight-186, 140,240, TRUE);


      MoveWindow(GetDlgItem(hwnd,BT_ACL_FILE_CHK), mWidth-Width_conf+10, mHeight-164, Width_conf-15,18, TRUE);
      MoveWindow(GetDlgItem(hwnd,BT_ADS_FILE_CHK), mWidth-Width_conf+10, mHeight-148, Width_conf-15,20, TRUE);
      MoveWindow(GetDlgItem(hwnd,BT_SHA_FILE_CHK), mWidth-Width_conf+10, mHeight-132, Width_conf-15,20, TRUE);
      MoveWindow(GetDlgItem(hwnd,BT_REGISTRY_RECOV_MODE), mWidth-Width_conf+10, mHeight-116, Width_conf-15,20, TRUE);
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
    case WM_COMMAND:
      switch (HIWORD(wParam))
      {
        case BN_CLICKED:Global_WM_COMMAND_wParam(wParam, lParam);break;
        case CBN_SELCHANGE:
          //for change in combobox selection
          switch(LOWORD(wParam))
          {
            case CB_LANG :
            {
              FORMAT_CALBAK_READ_INFO fcri;
              fcri.type = TYPE_SQLITE_FLAG_LANGUAGE_CHANGE;
              SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

              fcri.type = TYPE_SQLITE_FLAG_TESTS_UPDATE;
              SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

              fcri.type = TYPE_SQLITE_FLAG_TESTS_INIT;
              SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

              InitString();
              AddDebugMessage("gui_conf", "Change of language", "OK", MSG_INFO);

              ShowWindow(h_main, SW_HIDE);
              ShowWindow(h_main, SW_SHOW);
            }
            break;
            case CB_SESSION :
              current_session_id = session[SendDlgItemMessage(h_conf,CB_SESSION,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)];
            break;
          }
        break;
      }
    break;
    case WM_NOTIFY:
      if (((LPNMHDR)lParam)->code == NM_CLICK && ((LPNMHDR)lParam)->idFrom == TRV_TEST)
      {
        //selected item and state
        HANDLE Htree = GetDlgItem(hwnd,TRV_TEST);
        TV_HITTESTINFO Struct;
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(Htree, &pt);
        Struct.pt = pt;

        HTREEITEM hItemSelect = TreeView_HitTest(Htree, &Struct);
        if (hItemSelect != NULL)
        {
          if (Struct.flags == TVHT_ONITEMSTATEICON)TreeView_SelectItem(Htree,hItemSelect);
          else//check case
          {
            if (Ischeck_treeview(Htree, hItemSelect))check_treeview(Htree, hItemSelect, TRV_STATE_CHECK);
            else check_treeview(Htree, hItemSelect, TRV_STATE_UNCHECK);
          }
        }
      }
    break;
    case WM_CONTEXTMENU:
    {
      //test if in treeview !!!
      HANDLE htv = GetDlgItem(h_conf, TRV_FILES);
      if (htv == (HWND) wParam)
      {
        TV_HITTESTINFO tvh_info;
        tvh_info.pt.x = LOWORD(lParam);
        tvh_info.pt.y = HIWORD(lParam);

        //select item
        ScreenToClient(htv, &(tvh_info.pt));
        HTREEITEM hitem = TreeView_HitTest(htv, &tvh_info);
        TreeView_SelectItem(htv,hitem);

        //popup
        HMENU hmenu;
        if ((hmenu = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(POPUP_TRV_FILES)))!= NULL)
        {
          //if no select item
          if (hitem == NULL)
          {
            //if count !!
            if(TreeView_GetCount(htv) == NB_MX_TYPE_FILES_TITLE)
            {
              RemoveMenu(GetSubMenu(hmenu,0),5,MF_BYPOSITION);
              RemoveMenu(hmenu,POPUP_TRV_FILES_CLEAN_ALL,MF_BYCOMMAND);
              RemoveMenu(hmenu,POPUP_TRV_FILES_SAVE_LIST,MF_BYCOMMAND);
            }
            RemoveMenu(hmenu,POPUP_TRV_FILES_REMOVE_ITEMS,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_OPEN_PATH,MF_BYCOMMAND);

          }else if (TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]      == hitem ||
                    TRV_HTREEITEM_CONF[FILES_TITLE_FILES]     == hitem ||
                    TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]  == hitem ||
                    TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]     == hitem)
          {// if parent item !!
            RemoveMenu(GetSubMenu(hmenu,0),5,MF_BYPOSITION);
            RemoveMenu(hmenu,POPUP_TRV_FILES_REMOVE_ITEMS,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_OPEN_PATH,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_CLEAN_ALL,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_TRV_FILES_SAVE_LIST,MF_BYCOMMAND);
          }

          //case for autosearch
          if (B_AUTOSEARCH)ModifyMenu(hmenu,POPUP_TRV_FILES_AUTO_SEARCH,MF_BYCOMMAND|MF_STRING,POPUP_TRV_FILES_AUTO_SEARCH,cps[POPUP_TRV_FILES_REF_ITEMS_STRINGS+POPUP_TRV_FILES_REF_NB_ITEMS_STRINGS].c);

          //rewrite txt item for language
          unsigned int i;
          for (i=POPUP_TRV_FILES_REF_ITEMS_STRINGS;i<POPUP_TRV_FILES_REF_ITEMS_STRINGS+POPUP_TRV_FILES_REF_NB_ITEMS_STRINGS;i++)
          {
            ModifyMenu(hmenu,i+POPUP_TRV_FILES+1,MF_BYCOMMAND|MF_STRING,i+POPUP_TRV_FILES+1,cps[i].c);
          }

          //show popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), h_conf, NULL);
          DestroyMenu(hmenu);
        }
      }
    }
    break;
  }
  return 0;
}

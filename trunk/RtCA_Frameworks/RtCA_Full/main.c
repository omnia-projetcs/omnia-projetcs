//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "RtCA.h"
//------------------------------------------------------------------------------
//main dialogue function
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
      case WM_COMMAND:
        switch (HIWORD(wParam))
        {
          case BN_CLICKED:
            switch(LOWORD(wParam))
            {
              case IDM_NEW_SESSION:
                //create dialogue !!!
                h_conf = CreateDialog(0, MAKEINTRESOURCE(DLG_CONF), 0, DialogProc_conf);
                SetWindowText(h_conf,NOM_FULL_APPLI);
                ShowWindow (h_main, SW_HIDE);

                //stay on top
                if (STAY_ON_TOP) SetWindowPos(h_conf,HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
              break;
              //-------------------------------------------------
              case IDM_DEL_SESSION :
                if (nb_session<1)break;
                if (MessageBox(0,cps[REF_MSG].c,cps[REF_MSG+1].c,MB_ICONWARNING|MB_OKCANCEL) == IDOK)
                {
                  FORMAT_CALBAK_READ_INFO fcri;
                  fcri.type = TYPE_SQL_REMOVE_SESSION;
                  SQLITE_WriteData(&fcri, DEFAULT_SQLITE_FILE);

                  SendMessage(hCombo_session, CB_RESETCONTENT,0,0);
                  nb_session = 0;
                  fcri.type  = TYPE_SQLITE_FLAG_SESSIONS_INIT;
                  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
                  SendMessage(hCombo_session, CB_SETCURSEL,0,0);
                }
                sqlite3_exec(db_scan,"VACUUM;", NULL, NULL, NULL);//compact database
              break;
              case IDM_DEL_ALL_SESSION :
                if (nb_session<1)break;
                if (MessageBox(0,cps[REF_MSG].c,cps[REF_MSG+1].c,MB_ICONWARNING|MB_OKCANCEL) == IDOK)
                {
                  FORMAT_CALBAK_READ_INFO fcri;
                  fcri.type = TYPE_SQL_REMOVE_ALL_SESSION;
                  SQLITE_WriteData(&fcri, DEFAULT_SQLITE_FILE);

                  SendMessage(hCombo_session, CB_RESETCONTENT,0,0);
                }
                sqlite3_exec(db_scan,"VACUUM;", NULL, NULL, NULL);//compact database
              break;
              //-------------------------------------------------
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
                  SQLITE_LoadSession(file);
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
                  SQLITE_SaveSession(file);
                }
              }
              break;
              //-------------------------------------------------
              case POPUP_E_CSV:
                export_type = SAVE_TYPE_CSV;
                CreateThread(NULL,0,ChoiceSaveAll,NULL,0,0);
              break;
              case POPUP_E_HTML:
                export_type = SAVE_TYPE_HTML;
                CreateThread(NULL,0,ChoiceSaveAll,NULL,0,0);
              break;
              case POPUP_E_XML:
                export_type = SAVE_TYPE_XML;
                CreateThread(NULL,0,ChoiceSaveAll,NULL,0,0);
              break;
              //-------------------------------------------------
              case BT_SEARCH:
              {
                char tmp[MAX_PATH];
                SendMessage(he_search,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
                pos_search = LVSearch(hlstv, nb_current_columns, tmp, pos_search);
              }
              break;
              case IDM_STAY_ON_TOP:IDM_STAY_ON_TOP_fct(hwnd);break;
              case IDM_ABOUT:MessageBox(0,"to Read to Catch All :\n"
                                            "Licensed under the terms of the GNU\n"
                                            "General Public License version 3.\n\n"
                                            "Author: nicolas.hanteville@gmail.com\n"
                                            "http://code.google.com/p/omnia-projetcs/"
                                           ,"About",MB_ICONINFORMATION|MB_OK); break;
              //-----------------------------------------------------
              case IDM_RTCA_HOME:ShellExecute(NULL, "open", URL_APPLI, NULL, NULL, SW_SHOWNORMAL); break;
              //-----------------------------------------------------
              //popup menu
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
                  SaveLSTV(hlstv, file, ofn.nFilterIndex, nb_current_columns);
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
                  SaveLSTVSelectedItems(hlstv, file, ofn.nFilterIndex, nb_current_columns);
                }
              }
              break;
              //-----------------------------------------------------
              case POPUP_A_SEARCH:
              {
                char tmp[MAX_PATH];
                SendMessage(he_search,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);
                LVAllSearch(hlstv, nb_current_columns, tmp);
              }
              break;
              //-----------------------------------------------------
              case POPUP_I_00:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 0);break;
              case POPUP_I_01:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 1);break;
              case POPUP_I_02:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 2);break;
              case POPUP_I_03:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 3);break;
              case POPUP_I_04:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 4);break;
              case POPUP_I_05:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 5);break;
              case POPUP_I_06:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 6);break;
              case POPUP_I_07:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 7);break;
              case POPUP_I_08:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 8);break;
              case POPUP_I_09:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 9);break;
              case POPUP_I_10:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 10);break;
              case POPUP_I_11:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 11);break;
              case POPUP_I_12:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 12);break;
              case POPUP_I_13:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 13);break;
              case POPUP_I_14:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 14);break;
              case POPUP_I_15:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 15);break;
              case POPUP_I_16:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 16);break;
              case POPUP_I_17:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 17);break;
              case POPUP_I_18:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 18);break;
              case POPUP_I_19:CopyDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 19);break;
            }
          break;
          case CBN_SELCHANGE:
            //for change in combobox selection
            switch(LOWORD(wParam))
            {
              case CB_LANG :      InitGlobalLangueString(SendMessage(hCombo_lang,CB_GETCURSEL,0,0)+1);break;
              case CB_SESSION :
                current_session_id = session[SendMessage(hCombo_session,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)];
                //get items infos + items
                ListView_DeleteAllItems(hlstv);
                FORMAT_CALBAK_READ_INFO fcri;
                fcri.type = TYPE_SQLITE_FLAG_GET_ITEMS_INFO;
                SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

                char tmp_infos[DEFAULT_TMP_SIZE];
                snprintf(tmp_infos,DEFAULT_TMP_SIZE,"Item(s) : %d",ListView_GetItemCount(hlstv));
                SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)tmp_infos);
              break;
              case LV_BOX:
                current_item_selected = SendMessage(hlstbox, LB_GETCURSEL, 0, 0);
                TRI_RESULT_VIEW       = FALSE;
                column_tri            = -1;
                pos_search            = 0;

                if (current_item_selected > -1)
                {
                  FORMAT_CALBAK_READ_INFO fcri;

                  //get column count
                  fcri.type = TYPE_SQLITE_FLAG_GET_COLUM_COUNT;
                  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

                  //get column + text
                  fcri.type = TYPE_SQLITE_FLAG_VIEW_CHANGE;
                  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

                  //get items infos + items
                  ListView_DeleteAllItems(hlstv);
                  fcri.type = TYPE_SQLITE_FLAG_GET_ITEMS_INFO;
                  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

                  char tmp_infos[DEFAULT_TMP_SIZE];
                  snprintf(tmp_infos,DEFAULT_TMP_SIZE,"Item(s) : %d",ListView_GetItemCount(hlstv));
                  SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)tmp_infos);
                }
              break;
            }
          break;
        }
      break;
      case WM_NOTIFY:
      {
        if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)
        {
          TRI_RESULT_VIEW = !TRI_RESULT_VIEW;
          c_Tri(hlstv,((LPNMLISTVIEW)lParam)->iSubItem,TRI_RESULT_VIEW);
        }
      }
      break;
      case WM_CONTEXTMENU://popup menu
      //if item
      if (ListView_GetItemCount(hlstv) > 0 && (HWND)wParam == hlstv)
      {
        HMENU hmenu;
        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_LSTV)))!= NULL)
        {
          //set text !!!
          ModifyMenu(hmenu,POPUP_S_VIEW           ,MF_BYCOMMAND|MF_STRING ,POPUP_S_VIEW           ,cps[TXT_POPUP_S_VIEW].c);
          ModifyMenu(hmenu,POPUP_S_SELECTION      ,MF_BYCOMMAND|MF_STRING ,POPUP_S_SELECTION      ,cps[TXT_POPUP_S_SELECTION].c);
          ModifyMenu(hmenu,POPUP_A_SEARCH         ,MF_BYCOMMAND|MF_STRING ,POPUP_A_SEARCH         ,cps[TXT_POPUP_A_SEARCH].c);
          ModifyMenu(GetSubMenu(hmenu, 0),POPUP_COPY_TO_CLIPBORD ,MF_BYPOSITION|MF_STRING,POPUP_COPY_TO_CLIPBORD ,cps[TXT_POPUP_CLIPBORAD].c);

          //load column text
          char buffer[DEFAULT_TMP_SIZE]="";
          LVCOLUMN lvc;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = DEFAULT_TMP_SIZE;
          lvc.pszText = buffer;

          unsigned int i=0;
          while (SendMessage(hlstv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))
          {
            ModifyMenu(hmenu,POPUP_I_00+i,MF_BYCOMMAND|MF_STRING,POPUP_I_00+i,buffer);

            //reinit
            buffer[0] = 0;
            lvc.mask = LVCF_TEXT;
            lvc.cchTextMax = DEFAULT_TMP_SIZE;
            lvc.pszText = buffer;
            i++;
          }

          //remove other items
          for (;i<NB_POPUP_I;i++)RemoveMenu(hmenu,POPUP_I_00+i,MF_BYCOMMAND|MF_GRAYED);

          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }
      break;
      case WM_SIZE:
      {
        unsigned int mWidth  = LOWORD(lParam);
        unsigned int mHeight = HIWORD(lParam);

        //controle de la taille minimum
        if (mWidth<800 ||mHeight<600)
        {
          RECT Rect;
          GetWindowRect(hwnd, &Rect);
          MoveWindow(hwnd,Rect.left,Rect.top,800+20,600+64,TRUE);
        }else
        {
          MoveWindow(htoolbar,0,0,mWidth,22,TRUE);
          MoveWindow(hstatus_bar,0,0,mWidth,22,TRUE);

          MoveWindow(hlstbox,0,32,230,mHeight-55,TRUE);
          MoveWindow(hlstv,232,32,mWidth-234,mHeight-55,TRUE);

          //column resise
          unsigned int i;
          if (nb_current_columns>0)for (i=0;i<nb_current_columns;i++)redimColumnH(hlstv,i,(mWidth-274)/nb_current_columns);
        }
        InvalidateRect(hwnd, NULL, TRUE);
      }
      break;
      case WM_CLOSE:EndGUIConfig(hwnd);break;
      default:
          return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
//------------------------------------------------------------------------------
//main function
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  //init name
  snprintf(NOM_FULL_APPLI,DEFAULT_TMP_SIZE,"%s %s%s - %s",NOM_APPLI,FULLVERSION_STRING,STATUS_SHORT,URL_APPLI);

  //check if console or GUI mode
  if (argc == 1)//no params = GUI mode
  {
    CONSOL_ONLY = FALSE;
    char szClassName[ ] = "RtCA_Class";

    hinst = GetModuleHandle(0);
    MSG msg;
    WNDCLASSEX wincl;

    wincl.hInstance     = hinst;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc   = WindowProcedure;
    wincl.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wincl.cbSize        = sizeof (WNDCLASSEX);
    wincl.hIcon         = (HICON)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP));
    wincl.hIconSm       = (HICON)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP));
    wincl.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName  = (LPCTSTR)MY_MENU;
    wincl.cbClsExtra    = 0;
    wincl.cbWndExtra    = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_WINDOW;

    if (!RegisterClassEx (&wincl))return 0;
    h_main = CreateWindowEx(0,szClassName,NOM_FULL_APPLI,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,800,600,HWND_DESKTOP,NULL,hinst,NULL);

    //create Accelerators
    HACCEL hcl = LoadAccelerators(hinst, MAKEINTRESOURCE(MY_ACCEL));

  //forms
    htoolbar = CreateWindowEx(WS_EX_CLIENTEDGE, TOOLBARCLASSNAME, NULL, WS_CHILD|WS_VISIBLE|WS_BORDER|TBSTYLE_FLAT, 0,0,0,0,h_main,0,hinst,NULL);
    TBADDBITMAP tbab;
    TBBUTTON    tbb[6];

    //init
    SendMessage(htoolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON),0);
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(htoolbar, TB_ADDBITMAP, 0,(LPARAM)&tbab);

    //set buttons
    ZeroMemory(tbb, sizeof(tbb));
    tbb[0].fsStyle    = TBSTYLE_SEP;
    tbb[0].idCommand  = CB_LANG;

    tbb[1].fsStyle    = TBSTYLE_SEP;
    tbb[1].idCommand  = CB_SESSION;

    tbb[2].iBitmap    = STD_PROPERTIES;
    tbb[2].fsState    = TBSTATE_ENABLED;
    tbb[2].fsStyle    = TBSTYLE_BUTTON;
    tbb[2].idCommand  = IDM_NEW_SESSION;
    tbb[3].fsStyle    = TBSTYLE_SEP;

    tbb[4].fsStyle    = TBSTYLE_SEP;
    tbb[4].idCommand  = ED_SEARCH;
    tbb[5].iBitmap    = STD_FIND;
    tbb[5].fsState    = TBSTATE_ENABLED;
    tbb[5].fsStyle    = TBSTYLE_BUTTON;
    tbb[5].idCommand  = BT_SEARCH;

    SendMessage(htoolbar, TB_ADDBUTTONS, 6, (LPARAM)&tbb);

    //resize items
    TBBUTTONINFO tbinfo;
    ZeroMemory(&tbinfo, sizeof(TBBUTTONINFO));
    tbinfo.cbSize = sizeof(TBBUTTONINFO);
    tbinfo.dwMask = TBIF_SIZE;
    tbinfo.cx     = 150;
    SendMessage(htoolbar, TB_SETBUTTONINFO, CB_LANG, (LPARAM)&tbinfo);

    ZeroMemory(&tbinfo, sizeof(TBBUTTONINFO));
    tbinfo.cbSize = sizeof(TBBUTTONINFO);
    tbinfo.dwMask = TBIF_SIZE;
    tbinfo.cx     = 300;
    SendMessage(htoolbar, TB_SETBUTTONINFO, CB_SESSION, (LPARAM)&tbinfo);

    ZeroMemory(&tbinfo, sizeof(TBBUTTONINFO));
    tbinfo.cbSize = sizeof(TBBUTTONINFO);
    tbinfo.dwMask = TBIF_SIZE;
    tbinfo.cx     = 270;
    SendMessage(htoolbar, TB_SETBUTTONINFO, ED_SEARCH, (LPARAM)&tbinfo);

    //add items
    RECT rect;
    InitCommonControls();

    SendMessage(htoolbar, TB_GETITEMRECT, SendMessage(htoolbar, TB_COMMANDTOINDEX, CB_LANG, 0), (LPARAM)&rect);
    hCombo_lang = CreateWindowEx(0x00, WC_COMBOBOXEX, NULL,0x50010003,rect.left,rect.top+2,rect.right - rect.left,200,
                                  htoolbar,(HMENU)CB_LANG, hinst, NULL);

    SendMessage(htoolbar, TB_GETITEMRECT, SendMessage(htoolbar, TB_COMMANDTOINDEX, CB_SESSION, 0), (LPARAM)&rect);
    hCombo_session = CreateWindow("Combobox", NULL,0x50210003,rect.left,rect.top+2,rect.right - rect.left,200,
                                  htoolbar,(HMENU)CB_SESSION, hinst, NULL);

    SendMessage(htoolbar, TB_GETITEMRECT, SendMessage(htoolbar, TB_COMMANDTOINDEX, ED_SEARCH, 0), (LPARAM)&rect);
    he_search = CreateWindow("Edit", NULL,0x50810080,rect.left,rect.top+2,rect.right - rect.left,22,
                                  htoolbar,(HMENU)ED_SEARCH, hinst, NULL);
    //status bar
    hstatus_bar = CreateWindow(STATUSCLASSNAME, NULL,0x50000000,0,0,0,40,
                               h_main,NULL, hinst, NULL);
    //listeview resultats
    hlstv = CreateWindowEx(0x200,WC_LISTVIEW,NULL,LVS_REPORT|WS_VISIBLE|WS_CHILD,202,32,590,493,h_main,(HMENU)LV_VIEW, hinst, NULL);

    hlstbox = CreateWindowEx(0x200,WC_LISTBOX,NULL,0x50310141,0,32,200,493,h_main,(HMENU)LV_BOX, hinst, NULL);

    CreateThread(NULL,0,InitGUIConfig,NULL,0,0);

    ShowWindow(hCombo_lang, SW_SHOW);
    ShowWindow(hCombo_session, SW_SHOW);current_lang_id = SendMessage(hCombo_lang,CB_GETCURSEL,0,0)+1;
    ShowWindow (h_main, SW_SHOW);

    while (GetMessage (&msg, NULL, 0, 0))
    {
      if(!TranslateAccelerator(h_main, hcl, &msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    return msg.wParam;

  }else
  {
    if (sqlite3_open(DEFAULT_SQLITE_FILE, &db_scan) != SQLITE_OK)
    {
      //if tmp sqlite file exist free !!
      if (GetFileAttributes(DEFAULT_TM_SQLITE_FILE) != INVALID_FILE_ATTRIBUTES)
      {
        DeleteFile(DEFAULT_TM_SQLITE_FILE);
      }
      sqlite3_open(DEFAULT_SQLITE_FILE, &db_scan);
    }
    SetDebugPrivilege(TRUE);

    nb_file_cmd         = 0;
    nb_path_cmd         = 0;
    export_type         = SAVE_TYPE_CSV;
    current_session_id  = 0;
    start_scan          = FALSE;

    DEBUG_CMD_MODE      = TRUE;
    CONSOL_ONLY         = TRUE;
    LOCAL_SCAN          = TRUE;
    DEBUG_MODE          = FALSE;
    FILE_ACL            = FALSE;
    FILE_ADS            = FALSE;
    FILE_SHA            = FALSE;

    FORMAT_CALBAK_READ_INFO fcri;
    char request[MAX_LINE_SIZE];

    //params
    unsigned int i;
    for (i=0;i<argc;i++)
    {
      if (argv[i][0] == '-') //params
      {
        switch(argv[i][1])
        {
          case 'h'://help
            printf(
                   "*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "SYNOPSIS\n"
                   "\tRtCA.exe [-l|-L|-t]\n"
                   "\t         [-d][-r][-0][-1][-2]\n"
                   "\t         [-f \"...\"]\n"
                   "\t         [-p \"...\"]\n"
                   "\t         [-a|-A|-s ...]\n"
                   "\t         [-o \"...\"][-F [CSV|XML|HTML]]\n"
                   "\n"
                   "DESCRITPION\n"
                   "\tTool To help forensic investigations :)\n"
                   "\n"
                   "OPTIONS\n"
                   "\t-l  List all sessions or select the session number.\n\t    Exemple: -l 125\n\n"
                   "\t-L  List all languages or select it for export.\n\t    Exemple for english export (default): -L 1\n\n"
                   "\t-t  List all tests.\n\n"
                   "\n"
                   "\t-d  Enable Debug mode.\n\n"
                   "\t-0  Enable ACL check in files test.\n\n"
                   "\t-1  Enable ADS check in files test.\n\n"
                   "\t-2  Enable SHA in files test.\n\n"
                   "\n"
                   "\t-f  Import file to check.\n\t    Exemple: -f \"c:\\file.reg\"\n\n"
                   "\t-p  Import path to check in file test.\n\t    Exemple: -p \"c:\\directory\\\"\n\n"
                   "\n"
                   "\t-a  Start all tests.\n\n"
                   "\t-A  Start all tests in safe mode with no Files and no Logs test.\n\n"
                   "\t-s  Select test to start.\n\t    Exemple: -s 0 1 2 3 4 5 6\n\n"
                   "\n"
                   "\t-o  Export all data to path.\n\t    Exemple: -o \"c:\\\"\n"
                   "\t-F  Format to export : CSV (default), XML or HTML\n"
                   "\n"
                   ,NOM_FULL_APPLI);
            system("PAUSE");
          break;
          case 'l'://list of all sessions + select
            printf("*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "List of sessions :\n",NOM_FULL_APPLI);
            if (i+1<argc)
            {
              i++;
              if (argv[i][0]!='-')
              {
                current_session_id = atoi(argv[i]);//select a session
              }else
              {
                fcri.type = CMD_TYPE_LIST_SESSION;
                snprintf(request,MAX_LINE_SIZE,"SELECT id,name FROM session;");
                sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
              }
            }else
            {
              fcri.type = CMD_TYPE_LIST_SESSION;
              snprintf(request,MAX_LINE_SIZE,"SELECT id,name FROM session;");
              sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            }
            system("PAUSE");
          break;
          case 'L'://list of all languages supported + select
            item_n = 0;
            printf("*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "List of languages :\n",NOM_FULL_APPLI);
            if (i+1<argc)
            {
              i++;
              if (argv[i][0]!='-')
              {
                current_session_id = atoi(argv[i]);//select a session
              }else
              {
                fcri.type = CMD_TYPE_LIST_LANGUAGE;
                snprintf(request,MAX_LINE_SIZE,"SELECT name FROM language;");
                sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
              }
            }else
            {
              fcri.type = CMD_TYPE_LIST_LANGUAGE;
              snprintf(request,MAX_LINE_SIZE,"SELECT id,name FROM language;");
              sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            }
            system("PAUSE");
          break;
          case 't'://list of all tests
            printf("*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "List of tests :\n",NOM_FULL_APPLI);
            fcri.type = CMD_TYPE_LIST_TESTS;
            snprintf(request,MAX_LINE_SIZE,"SELECT id_item,string FROM language_strings WHERE id_language=1 ORDER BY id_item;");
            sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            system("PAUSE");
          break;
          case 'd'://enable mode debug
            DEBUG_MODE = TRUE;
          break;
          case '0'://enable ACL for tests files
            FILE_ACL = TRUE;
          break;
          case '1'://enable ADS for tests files
            FILE_ADS = TRUE;
          break;
          case '2'://enable SHA hashes for tests files
            FILE_SHA = TRUE;
          break;

          case 'f'://import file to scan
            for (++i;i<argc;i++)
            {
              if (argv[i][0] == '-'){i--;break;}

              //get files
              if (nb_file_cmd<NB_MAX_ITEM_CMD)
              {
                strncpy(file_cmd[nb_file_cmd++].c,argv[i],MAX_PATH);
              }else break;
              LOCAL_SCAN = FALSE;
            }
          break;
          case 'p'://scan path for file test
            for (++i;i<argc;i++)
            {
              if (argv[i][0] == '-'){i--;break;}

              //get files
              if (nb_path_cmd<NB_MAX_ITEM_CMD)
              {
                strncpy(path_cmd[nb_path_cmd++].c,argv[i],MAX_PATH);
              }else break;
              LOCAL_SCAN = FALSE;
            }
          break;

          case 'a'://scan with all tests
            CMDScan((LPVOID)FALSE);
          break;
          case 'A'://scan with all tests in safe mode with no log and no file test
            CMDScan((LPVOID)TRUE);
          break;
          case 's'://scan for test with number 0 1 2 3
            //generate new session
            AddNewSession(LOCAL_SCAN,db_scan);

            for (++i;i<argc;i++)
            {
              if (argv[i][0] == '-'){i--;break;}

              CMDScanNum((LPVOID)atoi(argv[i]));
            }
          break;
          case 'o'://save alls data session to path
            if (i+1<argc)
            {
              i++;
              if (argv[i][0] == '-'){i--;break;}

              strncpy(current_test_export_path,argv[i],MAX_PATH);
              ExportStart = TRUE;
              SaveAll(NULL);
            }
          break;
          case 'F'://save alls data session to path
            if (i+1<argc)
            {
              i++;
              if (argv[i][0] == '-'){i--;break;}

              switch(argv[i][0])
              {
                case 'x':
                case 'X':export_type     = SAVE_TYPE_XML;break;
                case 'h':
                case 'H':export_type     = SAVE_TYPE_HTML;break;
              }
            }
          break;
        }
      }else continue;//error
    }
    SetDebugPrivilege(FALSE);
    return 0;
  }
}

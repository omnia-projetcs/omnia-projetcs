//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "RtCA.h"
//------------------------------------------------------------------------------
//subclass of hdbclk_info
//------------------------------------------------------------------------------
LRESULT APIENTRY subclass_hdbclk_info(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CLOSE)ShowWindow (hdbclk_info, SW_HIDE);
  else return CallWindowProc(wndproc_hdbclk_info, hwnd, uMsg, wParam, lParam);
  return 0;
}
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
              case IDM_QUIT:EndGUIConfig(hwnd);break;
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
                  ListView_DeleteAllItems(hlstv);
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

                //select lstv
                SetFocus(hlstv);
                SendMessage(he_search,WM_GETTEXT ,(WPARAM)MAX_PATH, (LPARAM)tmp);

                if (GetMenuState(GetMenu(h_main),BT_SEARCH_MATCH_CASE,MF_BYCOMMAND) != MF_CHECKED) pos_search = LVSearchNoCass(hlstv, nb_current_columns, tmp, pos_search);
                else pos_search = LVSearch(hlstv, nb_current_columns, tmp, pos_search);
              }
              break;
              case IDM_STAY_ON_TOP:IDM_STAY_ON_TOP_fct();break;
              case BT_SEARCH_MATCH_CASE:
                if (GetMenuState(GetMenu(h_main),BT_SEARCH_MATCH_CASE,MF_BYCOMMAND) == MF_CHECKED)
                  CheckMenuItem(GetMenu(h_main),BT_SEARCH_MATCH_CASE,MF_BYCOMMAND|MF_UNCHECKED);
                else
                  CheckMenuItem(GetMenu(h_main),BT_SEARCH_MATCH_CASE,MF_BYCOMMAND|MF_CHECKED);
                break;

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
                unsigned int lb_item = SendMessage(hlstbox, LB_GETCURSEL, 0, 0);
                if (SendMessage(hlstbox, LB_GETTEXTLEN, lb_item, 0) < MAX_PATH)
                  SendMessage(hlstbox, LB_GETTEXT, lb_item, (LPARAM)file);

                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = h_main;
                ofn.lpstrFile = file;
                ofn.nMaxFile = MAX_PATH;
                if(SendMessage(hlstbox, LB_GETCURSEL, 0, 0) == INDEX_REG_PASSWORD)
                  ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0*.pwdump \0*.pwdump\0";
                else
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
                unsigned int lb_item = SendMessage(hlstbox, LB_GETCURSEL, 0, 0);
                if (SendMessage(hlstbox, LB_GETTEXTLEN, lb_item, 0) < MAX_PATH)
                  SendMessage(hlstbox, LB_GETTEXT, lb_item, (LPARAM)file);

                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = h_main;
                ofn.lpstrFile = file;
                ofn.nMaxFile = MAX_PATH;
                if(SendMessage(hlstbox, LB_GETCURSEL, 0, 0) == INDEX_REG_PASSWORD)
                  ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0*.pwdump \0*.pwdump\0";
                else
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
              case POPUP_CP_LINE:CopyAllDataToClipboard(hlstv, SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), nb_current_columns);break;

              //-----------------------------------------------------
              case POPUP_H_00:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_01:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_02:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_03:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_04:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_05:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_06:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_07:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_08:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_09:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_10:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_11:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_12:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_13:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_14:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_15:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_16:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_17:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_18:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              case POPUP_H_19:if(ListView_GetColumnWidth(hlstv,LOWORD(wParam)-POPUP_H_00) > 20)redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,0);else redimColumnH(hlstv,LOWORD(wParam)-POPUP_H_00,50);break;
              //-----------------------------------------------------
              case POPUP_OPEN_PATH:
              {
                char path[MAX_PATH]="";
                ListView_GetItemText(hlstv,SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0,path,MAX_PATH);
                if (path[0]!=0)ShellExecute(h_main, "explore", path, NULL,NULL,SW_SHOW);
              }
              break;
              case POPUP_OPEN_FILE_PATH:
              {
                char path[MAX_PATH]="";
                ListView_GetItemText(hlstv,SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0,path,MAX_PATH);
                if (path[0]!=0)
                {
                  char *c = path;
                  while (*c++);
                  while (*c != '\\' && *c != '/')c--;
                  c++;
                  *c=0;
                  ShellExecute(h_main, "explore", path, NULL,NULL,SW_SHOW);
                }
              }
              break;
              case POPUP_OPEN_REG_PATH:
              {
                char path[MAX_PATH]="",chk[MAX_PATH]="";
                ListView_GetItemText(hlstv,SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,chk,MAX_PATH);
                ListView_GetItemText(hlstv,SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2,path,MAX_PATH);

                if (path[0]!=0 && chk[0]!=0)OpenRegeditKey(chk, path);
              }
              break;
              case IDM_TOOLS_CP_REGISTRY: CreateThread(NULL,0,BackupRegFile,NULL,0,0);break;
              case IDM_TOOLS_CP_AUDIT:    CreateThread(NULL,0,BackupEvtFile,NULL,0,0);break;
              case IDM_TOOLS_CP_AD:       CreateThread(NULL,0,BackupNTDIS,NULL,0,0);break;
              case IDM_TOOLS_CP_FILE:     CreateThread(NULL,0,BackupFile,NULL,0,0);break;
              case IDM_TOOLS_PROCESS:
              {
                LoadPRocessList(hlstv_process);

                //set language
                LVCOLUMN lvc;
                lvc.mask = LVCF_TEXT;
                unsigned int i;
                for (i=0;i<nb_column_process_view && i+TXT_COLUMN_PROCESS_REF< NB_COMPONENT_STRING;i++)
                {
                  lvc.pszText = cps[i+TXT_COLUMN_PROCESS_REF].c; //texte de la colonne
                  SendMessage(hlstv_process,LVM_SETCOLUMN,(WPARAM)i, (LPARAM)&lvc);
                }

                ShowWindow(h_process,SW_SHOW);
              }
              break;
              case IDM_TOOLS_SNIFF:
              {
                //set text
                SetWindowText(GetDlgItem((HWND)h_sniff,DLG_NS_BT_START),cps[TXT_BT_START].c);
                SetWindowText(GetDlgItem((HWND)h_sniff,DLG_NS_BT_SAVE),cps[TXT_BT_EXPORT_START].c);

                SendDlgItemMessage(h_sniff,DLG_NS_SNIFF_LB_FILTRE, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
                SendDlgItemMessage(h_sniff,DLG_NS_SNIFF_LB_FILTRE, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)cps[TXT_SNIFF_FILTRE].c);

                ShowWindow(h_sniff,SW_SHOW);
              }
              break;
              case IDM_TOOLS_REG_EXPLORER:
                InitDlgRegfile();
                ShowWindow(h_reg_file, SW_SHOW);
              break;
              case IDM_TOOLS_DATE:
                ShowWindow(h_date, SW_SHOW);
              break;
              case IDM_TOOLS_ANALYSER:
                InitGuiState();
                ShowWindow(h_state, SW_SHOW);
              break;
              case POPUP_FILE_IMPORT_FILE:CreateThread(NULL,0,ImportCVSorSHA256deep,0,0,0);break;
              case POPUP_FILE_REMOVE_ITEM:LVDelete(hlstv);break;
              case POPUP_FILE_VIRUSTOTAL:
                if (VIRUSTTAL)
                {
                  DWORD IDThread;
                  GetExitCodeThread(h_VIRUSTTAL,&IDThread);
                  TerminateThread(h_VIRUSTTAL,IDThread);
                  VIRUSTTAL = FALSE;
                }else
                {
                  VIRUSTTAL = TRUE;
                  h_VIRUSTTAL = CreateThread(NULL,0,CheckSelectedItemToVirusTotal,0,0,0);
                }
              break;
              case POPUP_FILE_VIRUSTOTAL_ALL:
                if (AVIRUSTTAL)
                {
                  DWORD IDThread;
                  GetExitCodeThread(h_AVIRUSTTAL,&IDThread);
                  TerminateThread(h_AVIRUSTTAL,IDThread);
                  AVIRUSTTAL = FALSE;
                }else
                {
                  AVIRUSTTAL = TRUE;
                  h_AVIRUSTTAL = CreateThread(NULL,0,CheckAllFileToVirusTotal,0,0,0);
                }
              break;
            }
          break;
          case CBN_SELCHANGE:
            //for change in combobox selection
            switch(LOWORD(wParam))
            {
              case CB_LANG :
                InitGlobalLangueString(SendMessage(hCombo_lang,CB_GETCURSEL,0,0)+1);

                //update tooltips
                ModifyToolTip(htoolbar, htooltip, hinst, 2, NULL, cps[TXT_TOOLTIP_NEW_SESSION].c);
                ModifyToolTip(htoolbar, htooltip, hinst, 5, NULL, cps[TXT_TOOLTIP_SEARCH].c);
              break;
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
                  FORMAT_CALBAK_READ_INFO fcri2;

                  //get column count
                  fcri2.type = TYPE_SQLITE_FLAG_GET_COLUM_COUNT;
                  SQLITE_LireData(&fcri2, DEFAULT_SQLITE_FILE);

                  //get column + text
                  fcri2.type = TYPE_SQLITE_FLAG_VIEW_CHANGE;
                  SQLITE_LireData(&fcri2, DEFAULT_SQLITE_FILE);

                  //get items infos + items
                  ListView_DeleteAllItems(hlstv);
                  fcri2.type = TYPE_SQLITE_FLAG_GET_ITEMS_INFO;
                  SQLITE_LireData(&fcri2, DEFAULT_SQLITE_FILE);

                  char tmp_infos2[DEFAULT_TMP_SIZE];
                  snprintf(tmp_infos2,DEFAULT_TMP_SIZE,"Item(s) : %d",ListView_GetItemCount(hlstv));
                  SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)tmp_infos2);
                }
              break;
            }
          break;
        }
      break;
      case WM_NOTIFY:
      {
        switch(((LPNMHDR)lParam)->code)
        {
          case LVN_COLUMNCLICK:
            TRI_RESULT_VIEW = !TRI_RESULT_VIEW;
            c_Tri(hlstv,((LPNMLISTVIEW)lParam)->iSubItem,TRI_RESULT_VIEW);
          break;
          case NM_DBLCLK:
            if (LOWORD(wParam) == LV_VIEW)
            {
              long nitem = SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
              if (nitem > -1)
              {
                char tmp[MAX_LINE_SIZE]="", buffer[MAX_LINE_SIZE]="";

                //get test text
                int selected_cb_item = SendMessage(hlstbox, LB_GETCURSEL, 0, 0);
                if (SendMessage(hlstbox, LB_GETTEXTLEN, selected_cb_item, 0) < MAX_LINE_SIZE)
                {
                  SendMessage(hlstbox, LB_GETTEXT, selected_cb_item, (LPARAM)buffer);
                }
                strncat(buffer,"\r\n--------------------",MAX_LINE_SIZE);

                //for each column
                unsigned int i;
                LVCOLUMN lvc;
                lvc.mask = LVCF_TEXT;
                lvc.cchTextMax = MAX_LINE_SIZE;
                lvc.pszText = tmp;

                for(i=0;i<nb_current_columns;i++)
                {
                  //column header
                  strncat(buffer,"\r\n\r\n[",MAX_LINE_SIZE);
                  SendMessage(hlstv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc);
                  strncat(buffer,tmp,MAX_LINE_SIZE);
                  strncat(buffer,"]\r\n",MAX_LINE_SIZE);
                  tmp[0] = 0;
                  lvc.mask = LVCF_TEXT;
                  lvc.cchTextMax = MAX_LINE_SIZE;
                  lvc.pszText = tmp;

                  //datas
                  ListView_GetItemText(hlstv,nitem,i,tmp,MAX_LINE_SIZE);
                  strncat(buffer,tmp,MAX_LINE_SIZE);
                  tmp[0] = 0;
                }
                strncat(buffer,"\0",MAX_LINE_SIZE);

                //set text
                SetWindowText(hdbclk_info, buffer);
                ShowWindow (hdbclk_info, SW_SHOW);
              }
            }
          break;
          //popup menu for view column !!
          case NM_RCLICK:
          {
            //src code : http://support.microsoft.com/kb/125694
            //get click pos
            DWORD dwPos = GetMessagePos();
            POINT pointScreen;
            pointScreen.x = LOWORD (dwPos);
            pointScreen.y = HIWORD (dwPos);

            //set to lstv pos
            ScreenToClient (hlstv, &pointScreen);
            HANDLE hChildWnd = ChildWindowFromPoint(hlstv, pointScreen);

            if (hChildWnd != hlstv) //header have been clicked
            {
              //view popup menu
              HMENU hmenu;
              if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_LSTV_HDR)))!= NULL)
              {
                //load column text
                char buffer[DEFAULT_TMP_SIZE]="";
                LVCOLUMN lvc;
                lvc.mask = LVCF_TEXT|LVCF_WIDTH;
                lvc.cchTextMax = DEFAULT_TMP_SIZE;
                lvc.pszText = buffer;
                lvc.cx = 0;

                unsigned int i=0;
                while (SendMessage(hlstv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))
                {
                  ModifyMenu(hmenu,POPUP_H_00+i,MF_BYCOMMAND|MF_STRING,POPUP_H_00+i,buffer);
                  if(lvc.cx > 20 )CheckMenuItem(hmenu,POPUP_H_00+i,MF_BYCOMMAND|MF_CHECKED);
                  else CheckMenuItem(hmenu,POPUP_H_00+i,MF_BYCOMMAND|MF_UNCHECKED);

                  //reinit
                  buffer[0] = 0;
                  lvc.mask = LVCF_TEXT|LVCF_WIDTH;
                  lvc.cchTextMax = DEFAULT_TMP_SIZE;
                  lvc.pszText = buffer;
                  lvc.cx = 0;
                  i++;
                }

                //remove other items
                for (;i<NB_POPUP_I;i++)RemoveMenu(hmenu,POPUP_H_00+i,MF_BYCOMMAND);

                //view popup
                TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(dwPos), HIWORD(dwPos), hwnd, NULL);
                DestroyMenu(hmenu);
              }
              disable_m_context = TRUE;
            }
          }
          break;
        }
      }
      break;
      case WM_CONTEXTMENU://popup menu
      //if item
      if ((HWND)wParam == hlstv)
      {
        HMENU hmenu;
        if (ListView_GetItemCount(hlstv) < 1)
        {
          if (SendMessage(hlstbox, LB_GETCURSEL, 0, 0) == INDEX_FILE)
          {
            if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_LSTV_EMPTY_FILE)))!= NULL)
            {
              ModifyMenu(hmenu,POPUP_FILE_IMPORT_FILE,MF_BYCOMMAND|MF_STRING,POPUP_FILE_IMPORT_FILE,cps[TXT_LOAD_FILE].c);

              TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
              DestroyMenu(hmenu);
            }
          }
          break;
        }

        if (disable_m_context)
        {
          disable_m_context = FALSE;
          break;
        }

        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_LSTV)))!= NULL)
        {
          //set text !!!
          ModifyMenu(hmenu,POPUP_S_VIEW           ,MF_BYCOMMAND|MF_STRING ,POPUP_S_VIEW           ,cps[TXT_POPUP_S_VIEW].c);
          ModifyMenu(hmenu,POPUP_S_SELECTION      ,MF_BYCOMMAND|MF_STRING ,POPUP_S_SELECTION      ,cps[TXT_POPUP_S_SELECTION].c);
          ModifyMenu(hmenu,POPUP_A_SEARCH         ,MF_BYCOMMAND|MF_STRING ,POPUP_A_SEARCH         ,cps[TXT_POPUP_A_SEARCH].c);
          ModifyMenu(hmenu,POPUP_CP_LINE          ,MF_BYCOMMAND|MF_STRING ,POPUP_CP_LINE          ,cps[TXT_POPUP_CP_LINE].c);
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
          for (;i<NB_POPUP_I;i++)RemoveMenu(hmenu,POPUP_I_00+i,MF_BYCOMMAND);

          //add specific menu
          switch(SendMessage(hlstbox, LB_GETCURSEL, 0, 0))
          {
            case INDEX_FILE:
              //add menu
              InsertMenu(GetSubMenu(hmenu,0),2,MF_BYPOSITION|MF_SEPARATOR,0,"");
              InsertMenu(GetSubMenu(hmenu,0),3,MF_BYPOSITION|MF_STRING,POPUP_FILE_IMPORT_FILE,cps[TXT_LOAD_FILE].c);
              InsertMenu(GetSubMenu(hmenu,0),3,MF_BYPOSITION|MF_STRING,POPUP_FILE_REMOVE_ITEM,cps[TXT_REMOVE_FILE].c);

              if (AVIRUSTTAL)
              {
                InsertMenu(GetSubMenu(hmenu,0),5,MF_BYPOSITION|MF_STRING,POPUP_FILE_VIRUSTOTAL_ALL,cps[TXT_STOP_CHK_ALL_SHA256].c);
              }else if (VIRUSTTAL) InsertMenu(GetSubMenu(hmenu,0),5,MF_BYPOSITION|MF_STRING,POPUP_FILE_VIRUSTOTAL,cps[TXT_STOP_CHK_SHA256].c);
              else
              {
                InsertMenu(GetSubMenu(hmenu,0),5,MF_BYPOSITION|MF_STRING,POPUP_FILE_VIRUSTOTAL,cps[TXT_CHK_SHA256].c);
                InsertMenu(GetSubMenu(hmenu,0),6,MF_BYPOSITION|MF_STRING,POPUP_FILE_VIRUSTOTAL_ALL,cps[TXT_CHK_ALL_SHA256].c);
              }

            case INDEX_ANTIVIRUS:
              //openpath
              ModifyMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND|MF_STRING,POPUP_OPEN_PATH,cps[TXT_OPEN_PATH].c);
            break;

            case INDEX_LOG:
            case INDEX_ENV:
            case INDEX_LAN:
            case INDEX_SHARE:
            case INDEX_REG_USERS:
            case INDEX_REG_PASSWORD:
            {
              //check path
              char _tmp[MAX_PATH]="";
              ListView_GetItemText(hlstv,SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0,_tmp,MAX_PATH);
              if (_tmp[0]!=0 && _tmp[1]==':'&& _tmp[2]=='\\')
              {
                //open file path
                ModifyMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND|MF_STRING,POPUP_OPEN_FILE_PATH,cps[TXT_OPEN_PATH].c);
              }else
              {
                //supp menu
                RemoveMenu(GetSubMenu(hmenu,0),4,MF_BYPOSITION);
                RemoveMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND);
              }
            }
            break;
            case INDEX_REG_CONF:
            case INDEX_REG_SERVICES:
            case INDEX_REG_USB:
            case INDEX_REG_SOFTWARE:
            case INDEX_REG_UPDATE:
            case INDEX_REG_START:
            case INDEX_REG_USERASSIST:
            case INDEX_REG_MRU:
            case INDEX_REG_PATH:
            case INDEX_REG_GUIDE:
            case INDEX_REG_FIREWALL:
            {
              //check if path in first or registry case !!!
              char _tmp[MAX_PATH]="";
              ListView_GetItemText(hlstv,SendMessage(hlstv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,_tmp,MAX_PATH);
              if (_tmp[0] != 0)
              {
                //open registry path
                ModifyMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND|MF_STRING,POPUP_OPEN_REG_PATH,cps[TXT_OPEN_REG_PATH].c);
              }else
              {
                //open file path
                ModifyMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND|MF_STRING,POPUP_OPEN_FILE_PATH,cps[TXT_OPEN_PATH].c);
              }
            }

            break;
            case INDEX_NAV_FIREFOX:
            case INDEX_NAV_CHROME:
            case INDEX_NAV_IE:
            case INDEX_ANDROID:
            case INDEX_PREFETCH:
              //open file path
              ModifyMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND|MF_STRING,POPUP_OPEN_FILE_PATH,cps[TXT_OPEN_PATH].c);
            break;
            default:
              //supp menu
              RemoveMenu(GetSubMenu(hmenu,0),4,MF_BYPOSITION);
              RemoveMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND);
            break;
          }

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

          MoveWindow(hlstbox,0,32,230,mHeight-55,TRUE);
          MoveWindow(hlstv,232,32,mWidth-234,mHeight-55,TRUE);

          MoveWindow(hstatus_bar,0,mHeight-22,mWidth,22,TRUE);

          //column resise
          unsigned int i;
          if (nb_current_columns)
          {
            DWORD column_sz = (mWidth-274)/nb_current_columns;
            for (i=0;i<nb_current_columns;i++)
            {
              redimColumnH(hlstv,i,column_sz);
            }
          }
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
    htoolbar = CreateWindowEx(WS_EX_CLIENTEDGE, TOOLBARCLASSNAME, NULL, WS_CHILD|WS_VISIBLE|WS_BORDER|TBSTYLE_FLAT|WS_TABSTOP, 0,0,0,0,h_main,0,hinst,NULL);
    TBADDBITMAP tbab;
    TBBUTTON    tbb[6];

    //init
    SendMessage(htoolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON),0);
    tbab.hInst        = HINST_COMMCTRL;
    tbab.nID          = IDB_STD_SMALL_COLOR;
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
    tbinfo.cbSize     = sizeof(TBBUTTONINFO);
    tbinfo.dwMask     = TBIF_SIZE;
    tbinfo.cx         = 150;
    SendMessage(htoolbar, TB_SETBUTTONINFO, CB_LANG, (LPARAM)&tbinfo);

    ZeroMemory(&tbinfo, sizeof(TBBUTTONINFO));
    tbinfo.cbSize     = sizeof(TBBUTTONINFO);
    tbinfo.dwMask     = TBIF_SIZE;
    tbinfo.cx         = 300;
    SendMessage(htoolbar, TB_SETBUTTONINFO, CB_SESSION, (LPARAM)&tbinfo);

    ZeroMemory(&tbinfo, sizeof(TBBUTTONINFO));
    tbinfo.cbSize     = sizeof(TBBUTTONINFO);
    tbinfo.dwMask     = TBIF_SIZE;
    tbinfo.cx         = 270;
    SendMessage(htoolbar, TB_SETBUTTONINFO, ED_SEARCH, (LPARAM)&tbinfo);

    //add items
    RECT rect;
    InitCommonControls();

    SendMessage(htoolbar, TB_GETITEMRECT, SendMessage(htoolbar, TB_COMMANDTOINDEX, CB_LANG, 0), (LPARAM)&rect);
    hCombo_lang       = CreateWindowEx(0x00, WC_COMBOBOXEX, NULL,0x50010003|WS_TABSTOP,rect.left,rect.top+2,rect.right - rect.left,200,
                                       htoolbar,(HMENU)CB_LANG, hinst, NULL);

    SendMessage(htoolbar, TB_GETITEMRECT, SendMessage(htoolbar, TB_COMMANDTOINDEX, CB_SESSION, 0), (LPARAM)&rect);
    hCombo_session    = CreateWindow("Combobox", NULL,0x50210003|WS_TABSTOP,rect.left,rect.top+2,rect.right - rect.left,200,
                                     htoolbar,(HMENU)CB_SESSION, hinst, NULL);

    SendMessage(htoolbar, TB_GETITEMRECT, SendMessage(htoolbar, TB_COMMANDTOINDEX, ED_SEARCH, 0), (LPARAM)&rect);
    he_search         = CreateWindow("Edit", NULL,0x50810080|WS_TABSTOP,rect.left,rect.top+2,rect.right - rect.left,22,
                                     htoolbar,(HMENU)ED_SEARCH, hinst, NULL);
    //status bar
    hstatus_bar       = CreateWindow(STATUSCLASSNAME, NULL,0x50000000,0,0,0,40, h_main,NULL, hinst, NULL);
    //listeview resultats
    hlstv             = CreateWindowEx(0x200,WC_LISTVIEW,NULL,LVS_REPORT|WS_VISIBLE|WS_CHILD,202,32,590,493,h_main,
                                       (HMENU)LV_VIEW, hinst, NULL);

    hlstbox           = CreateWindowEx(0x200,WC_LISTBOX,NULL,0x50310141|WS_TABSTOP,0,32,200,493,h_main,(HMENU)LV_BOX, hinst, NULL);

    htooltip          = CreateWindow(TOOLTIPS_CLASS, NULL, WS_POPUP|TTS_NOPREFIX|TTS_BALLOON|TTS_ALWAYSTIP,CW_USEDEFAULT,CW_USEDEFAULT,
                                     CW_USEDEFAULT,CW_USEDEFAULT,h_main,NULL,hinst,NULL);
    //edit dblclick
    hdbclk_info = CreateWindowEx(0x200|WS_EX_CLIENTEDGE, WC_EDIT, NOM_FULL_APPLI, 0x00E80844|WS_SIZEBOX|WS_MAXIMIZEBOX,
                                 GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3,
                                 GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3,
                                 h_main, NULL, hinst, NULL);

    SendMessage(hdbclk_info, WM_SETFONT,(WPARAM)CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New"), TRUE);
    SendMessage(hdbclk_info, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    #ifdef _WIN64_VERSION_
      wndproc_hdbclk_info = (WNDPROC)SetWindowLongPtr(hdbclk_info, GWL_WNDPROC,(LONG)subclass_hdbclk_info);
    #else
      wndproc_hdbclk_info = (WNDPROC)SetWindowLong(hdbclk_info, GWL_WNDPROC,(LONG)subclass_hdbclk_info);
    #endif

    //dialogue for process
    h_process     = CreateDialog(0, MAKEINTRESOURCE(DLG_PROCESS) ,NULL,DialogProc_info);
    hlstv_process = GetDlgItem(h_process,LV_VIEW);
    SendMessage(h_process, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    SetWindowText(h_process,NOM_FULL_APPLI);

    //set columns !!!
    LVCOLUMN lvc;
    unsigned int i;
    nb_column_process_view  = NB_COLUMN_PROCESS_DEF;
    lvc.mask                = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
    lvc.fmt                 = LVCFMT_LEFT;
    lvc.cx                  = 100;

    for (i=0;i<nb_column_process_view && i+TXT_COLUMN_PROCESS_REF< NB_COMPONENT_STRING;i++)
    {
      lvc.pszText = cps[i+TXT_COLUMN_PROCESS_REF].c; //texte de la colonne
      SendMessage(hlstv_process,LVM_INSERTCOLUMN,(WPARAM)i, (LPARAM)&lvc);
    }
    SendMessage(hlstv_process,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);

    //dialogue for network sniff
    h_sniff = CreateDialog(0, MAKEINTRESOURCE(DLG_NETWORK_SNIFF), NULL,DialogProc_sniff);
    SendMessage(h_sniff, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    SetWindowText(h_sniff,NOM_FULL_APPLI);

    //add columns
    lvc.cx      = 110;
    lvc.pszText = "IP src";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 0, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 0, &lvc);
    lvc.cx      = 80;
    lvc.pszText = "Port src";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 1, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 1, &lvc);
    lvc.cx      = 110;
    lvc.pszText = "IP dst";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 2, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 2, &lvc);
    lvc.cx      = 80;
    lvc.pszText = "Port dst";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 3, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 3, &lvc);
    lvc.cx      = 110;
    lvc.pszText = "Protocol";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 4, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 4, &lvc);
    lvc.cx      = 200;
    lvc.pszText = "Desc";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 5, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 5, &lvc);
    lvc.cx      = 50;
    lvc.pszText = "ID";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_PAQUETS), 6, &lvc);
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV_FILTRE) , 6, &lvc);

    lvc.cx      = 110;
    lvc.pszText = "IP";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV), 0, &lvc);
    lvc.pszText = "Name";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV), 1, &lvc);
    lvc.pszText = "TTL";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV), 2, &lvc);
    lvc.pszText = "OS";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV), 3, &lvc);
    lvc.pszText = "Protocol";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV), 4, &lvc);
    lvc.pszText = "Malware";
    ListView_InsertColumn(GetDlgItem(h_sniff,DLG_NS_LSTV), 5, &lvc);

    SendDlgItemMessage(h_sniff,DLG_NS_LSTV_FILTRE,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
    SendDlgItemMessage(h_sniff,DLG_NS_LSTV_PAQUETS,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
    SendDlgItemMessage(h_sniff,DLG_NS_LSTV,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);

    //init all network interfaces
    SendDlgItemMessage(h_sniff,DLG_CONF_INTERFACE, CB_RESETCONTENT,0, 0);
    struct hostent *local;
    struct in_addr addr;

    WSADATA WSAData;
    WSAStartup(0x02, &WSAData);
    char name[DEFAULT_TMP_SIZE]="";
    if (gethostname(name, DEFAULT_TMP_SIZE) != SOCKET_ERROR)
    {
      if ((local = gethostbyname(name))!=0)
      {
        for (i = 0; local->h_addr_list[i] != 0; ++i)
        {
          memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
          snprintf(name,IP_SIZE_MAX,"%s",inet_ntoa(addr));
          SendDlgItemMessage(h_sniff,DLG_CONF_INTERFACE, CB_INSERTSTRING,(WPARAM)-1, (LPARAM)name);
        }
        SendDlgItemMessage(h_sniff,DLG_CONF_INTERFACE, CB_SETCURSEL,0,0);
      }
    }
    WSACleanup();

    //init brush interfaces
    Hb_green = CreateSolidBrush(RGB(204, 255, 204));
    Hb_blue = CreateSolidBrush(RGB(153, 255, 255));
    Hb_pink = CreateSolidBrush(RGB(255, 204, 204));
    Hb_violet = CreateSolidBrush(RGB(204, 204, 255));

    //create fom for infos
    //edit dblclick
    hdbclk_sniff = CreateWindowEx(0x200|WS_EX_CLIENTEDGE, WC_EDIT, NOM_FULL_APPLI, 0x00E80844|WS_SIZEBOX|WS_MAXIMIZEBOX,
                                 GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3,
                                 GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3,
                                 h_sniff, NULL, hinst, NULL);

    SendMessage(hdbclk_sniff, WM_SETFONT,(WPARAM)CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New"), TRUE);
    SendMessage(hdbclk_sniff, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    #ifdef _WIN64_VERSION_
      wndproc_hdbclk_sniff = (WNDPROC)SetWindowLongPtr(hdbclk_sniff, GWL_WNDPROC,(LONG)subclass_hdbclk_sniff);
    #else
      wndproc_hdbclk_sniff = (WNDPROC)SetWindowLong(hdbclk_sniff, GWL_WNDPROC,(LONG)subclass_hdbclk_sniff);
    #endif

    //if no interface disable sniff
    if (SendDlgItemMessage(h_sniff,DLG_CONF_INTERFACE, CB_GETCOUNT,0,0) == 0)
      EnableMenuItem(GetMenu(h_main),IDM_TOOLS_SNIFF,MF_BYCOMMAND|MF_GRAYED);

    //registry tool
    h_reg_file = CreateDialog(0, MAKEINTRESOURCE(DLG_REGISTRY_EXPLORER_FILE), NULL,DialogProc_reg_file);
    SendMessage(h_reg_file, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    SetWindowText(h_reg_file,NOM_FULL_APPLI);

    h_reg = CreateDialog(0, MAKEINTRESOURCE(DLG_REGISTRY_EXPLORER_VIEW), NULL,DialogProc_reg);
    SendMessage(h_reg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    SetWindowText(h_reg,NOM_FULL_APPLI);
    SendDlgItemMessage(h_reg,LV_VIEW,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
    ShowWindow(GetDlgItem(h_reg,TV_VIEW), SW_HIDE);
    ShowWindow(GetDlgItem(h_reg,LV_VIEW), SW_SHOW);

    lvc.cx      = 110;
    lvc.pszText = "File";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 0, &lvc);
    lvc.pszText = "Key";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 1, &lvc);
    lvc.pszText = "Value";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 2, &lvc);
    lvc.pszText = "Data";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 3, &lvc);
    lvc.pszText = "Type";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 4, &lvc);
    lvc.pszText = "Parent key update (UTC)";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 5, &lvc);
    lvc.pszText = "Owner SID";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 6, &lvc);
    lvc.pszText = "Deleted";
    ListView_InsertColumn(GetDlgItem(h_reg,LV_VIEW), 7, &lvc);

    //trv
    HIMAGELIST hImageList=ImageList_Create(16,16,ILC_COLORDDB | ILC_MASK,6,0);
    if (hImageList != NULL)
    {
      HCURSOR Hicon = LoadIcon(hinst,(LPCTSTR)ICON_FOLDER);
      ImageList_AddIcon(hImageList,Hicon);
      DeleteObject(Hicon);
      Hicon = LoadIcon(hinst,(LPCTSTR)ICON_FILE);
      ImageList_AddIcon(hImageList,Hicon);
      DeleteObject(Hicon);
      Hicon = LoadIcon(hinst,(LPCTSTR)ICON_FILE_BIN);
      ImageList_AddIcon(hImageList,Hicon);
      DeleteObject(Hicon);
      Hicon = LoadIcon(hinst,(LPCTSTR)ICON_FILE_DWORD);
      ImageList_AddIcon(hImageList,Hicon);
      DeleteObject(Hicon);
      Hicon = LoadIcon(hinst,(LPCTSTR)ICON_FILE_TXT);
      ImageList_AddIcon(hImageList,Hicon);
      DeleteObject(Hicon);
      Hicon = LoadIcon(hinst,(LPCTSTR)ICON_FILE_UNKNOW);
      ImageList_AddIcon(hImageList,Hicon);
      DeleteObject(Hicon);

      //add to treeview
      TreeView_SetImageList(GetDlgItem(h_reg,TV_VIEW),hImageList,TVSIL_NORMAL);
    }

    //date
    h_date = CreateDialog(0, MAKEINTRESOURCE(DLG_DATE), NULL,DialogProc_date);
    SendMessage(h_date, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    SetWindowText(h_date,NOM_FULL_APPLI);
    last_bt = 0;

    //init combobox
    //http://fr.wikipedia.org/wiki/Liste_des_fuseaux_horaires
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-12:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-11:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-10:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-09:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-09:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-08:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-07:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-06:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-05:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-04:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-04:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-03:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-03:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-02:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC-01:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+00:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+01:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+02:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+03:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+03:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+04:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+04:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+05:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+05:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+05:45");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+06:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+06:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+07:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+08:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+08:45");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+09:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+09:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+10:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+10:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+11:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+11:30");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+12:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+12:45");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+13:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_ADDSTRING,0,(LPARAM)"UTC+14:00");
    SendDlgItemMessage(h_date,DLG_DATE_CB_UTC,CB_SETCURSEL,12+3,0);

    h_state = CreateDialog(0, MAKEINTRESOURCE(DLG_STATE), NULL,DialogProc_state);
    SendMessage(h_state, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));
    SetWindowText(h_state,NOM_FULL_APPLI);

    lvc.cx      = 110;
    lvc.pszText = "Date";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 0, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 0, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 0, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_TIME_ZONE), 0, &lvc);
    lvc.pszText = "Origine";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 1, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 1, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 1, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_TIME_ZONE), 1, &lvc);
    lvc.pszText = "Source";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 2, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 2, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 2, &lvc);
    lvc.pszText = "State";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_TIME_ZONE), 2, &lvc);
    lvc.pszText = "Info";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 3, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 3, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 3, &lvc);
    lvc.pszText = "Description";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 4, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 4, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 4, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_TIME_ZONE), 3, &lvc);
    lvc.pszText = "Owner/User";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 5, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 5, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 5, &lvc);
    lvc.pszText = "SID";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 6, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 6, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 6, &lvc);
    lvc.pszText = "Session";
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_ALL), 7, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), 7, &lvc);
    ListView_InsertColumn(GetDlgItem(h_state,DLG_STATE_LV_FILTER), 7, &lvc);

    SendDlgItemMessage(h_state,DLG_STATE_LV_ALL,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
    SendDlgItemMessage(h_state,DLG_STATE_LV_CRITICAL,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
    SendDlgItemMessage(h_state,DLG_STATE_LV_TIME_ZONE,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);
    SendDlgItemMessage(h_state,DLG_STATE_LV_FILTER,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES);

    CreateThread(NULL,0,InitGUIConfig,NULL,0,0);
    ShowWindow(hCombo_lang, SW_SHOW);
    ShowWindow(hCombo_session, SW_SHOW);
    ShowWindow(h_main, SW_SHOW);

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
    printf("CONSOLE OPTION IN DEV : DISABLE !!!!\n");
/*
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
    stop_scan           = FALSE;

    CONSOL_ONLY         = TRUE;
    LOCAL_SCAN          = TRUE;
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
          default:
          case 'h'://help
            printf(
                   "*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "SYNOPSIS\n"
                   "\tRtCA.exe [-l|-L|-t]\n"
                   "\t         [-r][-0][-1][-2]\n"
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
                   "\t-0  Enable ACL check in files test.\n\n"
                   "\t-1  Enable ADS check in files test.\n\n"
                   "\t-2  Enable SHA in files test.\n\n"
                   "\t-T  Export in UTC time.\n\n"
                   "\n"
                   "\t-f  Import file to check.\n\t    Exemple: -f \"c:\\NTUSER.DAT\"\n\n"
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
            snprintf(request,MAX_LINE_SIZE,"SELECT id_item, string FROM language_strings WHERE id_language=1 ORDER BY id_item;");
            sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            system("PAUSE");
          break;
          case 'T': UTC_TIME = TRUE;
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
    SetDebugPrivilege(FALSE);*/
    return 0;
  }
}

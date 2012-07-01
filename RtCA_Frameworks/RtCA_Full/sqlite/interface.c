//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
int callback_write_sqlite(void *datas, int argc, char **argv, char **azColName)
{
  return TRUE;
}
//------------------------------------------------------------------------------
int callback_sqlite(void *datas, int argc, char **argv, char **azColName)
{
  FORMAT_CALBAK_TYPE *type = datas;
  switch(type->type)
  {
    //----------------------------------------
    case TYPE_SQLITE_FLAG_LANGUAGE_CHANGE:
    {
      unsigned int id_component = atoi(argv[1]);
      switch(atoi(argv[0]))
      {
        case TYPE_ST:
          if      (id_component >= DLG_MAIN && id_component<DLG_CONF) SetWindowText(GetDlgItem(h_main,id_component),convertUTF8toUTF16(argv[2], strlen(argv[2])+1));
          else if (id_component >= DLG_CONF && id_component<DLG_VIEW) SetWindowText(GetDlgItem(h_conf,id_component),convertUTF8toUTF16(argv[2], strlen(argv[2])+1));
          else if (id_component >= DLG_VIEW && id_component<MY_MENU)  SetWindowText(GetDlgItem(h_view,id_component),convertUTF8toUTF16(argv[2], strlen(argv[2])+1));
        break;
        case TYPE_MENU:ModifyMenu(GetMenu(h_main),id_component,MF_BYCOMMAND|MF_STRING,id_component,convertUTF8toUTF16(argv[2], strlen(argv[2])+1));break;
        case TYPE_MENU_TITLE:
          ModifyMenu(GetMenu(h_main),id_component,MF_BYPOSITION|MF_STRING,id_component,convertUTF8toUTF16(argv[2], strlen(argv[2])+1));
        break;
        case TYPE_MENU_SUB_TITLE:
        {
          char menu_id[2]="\0\0";
          menu_id[0]      = argv[1][0];
          char *c = argv[1];
          id_component    = atoi(++c);
          ModifyMenu(GetSubMenu(GetMenu(h_main),atoi(menu_id)),id_component,MF_BYCOMMAND|MF_STRING,id_component,convertUTF8toUTF16(argv[2], strlen(argv[2])+1));
        }
        break;
        case TYPE_COL_DEBUG:
        {
          LVCOLUMN lvc;
          lvc.mask      = LVCF_TEXT;
          lvc.pszText   = convertUTF8toUTF16(argv[2], strlen(argv[2])+1);
          SendDlgItemMessage(h_main,LV_INFO,LVM_SETCOLUMN,(WPARAM)id_component, (LPARAM)&lvc);
        }
        break;
        case TYPE_FILES_TITLE:
        {
          TV_ITEM tvitem;
          tvitem.mask     = TVIF_HANDLE|TVIF_TEXT;
          tvitem.hItem    = TRV_HTREEITEM_CONF[atoi(argv[1])];
          tvitem.pszText  = convertUTF8toUTF16(argv[2], strlen(argv[2])+1);
          SendDlgItemMessage(h_conf,TRV_FILES,TVM_SETITEM,(WPARAM)NULL, (LPARAM)&tvitem);
        }
        break;
      }
    }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_SESSIONS_INIT:
      if (nb_session<NB_MAX_SESSION)
      {
        session[nb_session++] = atoi(argv[0]);
        SendDlgItemMessage(h_conf,CB_SESSION, CB_INSERTSTRING,(WPARAM)-1, (LPARAM)convertUTF8toUTF16(argv[1], strlen(argv[1])+1));
      }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_TESTS_INIT:
      SendDlgItemMessage(h_view,CB_VIEW, CB_INSERTSTRING,(WPARAM)-1, (LPARAM)convertUTF8toUTF16(argv[0], strlen(argv[0])+1));
      strncpy(S_tests[NB_TESTS].s,argv[0],DEFAULT_TMP_SIZE);
      H_tests[NB_TESTS++] = AddItemTreeView(GetDlgItem(h_conf,TRV_TEST), argv[0], TVI_ROOT);
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_LANG_INIT_STRINGS:
    {
      int id_item = atoi(argv[1]);
      if (id_item< NB_COMPONENT_STRING)
      {
        strncpy(cps[id_item].c,convertUTF8toUTF16(argv[0], strlen(argv[0])+1),COMPONENT_STRING_MAX_SIZE);
      }else
      {
        char tmp[MAX_PATH];
        snprintf(tmp, MAX_PATH,"Item id out of range (%s/%d) : %s",argv[1],NB_COMPONENT_STRING,convertUTF8toUTF16(argv[0], strlen(argv[0])+1));
        AddDebugMessage("interface", tmp, "OK", MSG_WARN);
      }
    }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_TITLE_FILE_INIT:TRV_HTREEITEM_CONF[atoi(argv[1])] = AddItemTreeView(GetDlgItem(h_conf,TRV_FILES),convertUTF8toUTF16(argv[2], strlen(argv[2])+1),TVI_ROOT);break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_LANG_INIT:AddComboBoxItem(GetDlgItem(h_conf,CB_LANG), convertUTF8toUTF16(argv[0], strlen(argv[0])+1), atoi(argv[2]));break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_COLUMN_COUNT_INIT:
    {
      unsigned int index = atoi(argv[0]);
      columns_params[index].nb_columns = atoi(argv[1]);
      columns_params[index].mode_simple = atoi(argv[3]);
      columns_params[index].type = atoi(argv[4]);
      strncpy(columns_params[index].request,convertUTF8toUTF16(argv[2], strlen(argv[2])+1),REQUEST_MAX_SIZE);
    }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_VIEW_CHANGE:
    {
      LVCOLUMN lvc;
      lvc.mask      = LVCF_TEXT;
      lvc.pszText   = convertUTF8toUTF16(argv[1], strlen(argv[1])+1);
      SendDlgItemMessage(h_view,LV_VIEW,LVM_SETCOLUMN,(WPARAM)atoi(argv[0]), (LPARAM)&lvc);
    }
    break;
    //----------------------------------------
    case TYPE_SQLITE_TEST:
    {
      //add line
      HANDLE hlv = GetDlgItem(h_view,LV_VIEW);
      LVITEM lvi;
      lvi.mask = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem = 0;
      lvi.lParam = LVM_SORTITEMS;
      lvi.pszText="";
      lvi.iItem = ListView_GetItemCount(hlv);
      unsigned int i=0, ref_item = ListView_InsertItem(hlv, &lvi);

      for (i=0;i<argc;i++)
        ListView_SetItemText(hlv,ref_item,i,argv[i]);
    }
    break;
    //---------------------------------------
    case TYPE_SQLITE_FLAG_ANDROID_INIT_STRINGS:
      if (nb_sql_ANDROID>=NB_MAX_SQL_REQ)break;
      strncpy(sql_ANDROID[nb_sql_ANDROID].sql,argv[0],MAX_PATH);
      strncpy(sql_ANDROID[nb_sql_ANDROID].params,argv[1],MAX_PATH);
      sql_ANDROID[nb_sql_ANDROID].test_string_id = atoi(argv[2]);
      nb_sql_ANDROID++;
    break;
    case TYPE_SQLITE_FLAG_CHROME_INIT_STRINGS:
      if (nb_sql_CHROME>=NB_MAX_SQL_REQ)break;
      strncpy(sql_CHROME[nb_sql_CHROME].sql,argv[0],MAX_PATH);
      strncpy(sql_CHROME[nb_sql_CHROME].params,argv[1],MAX_PATH);
      sql_CHROME[nb_sql_CHROME].test_string_id = atoi(argv[2]);
      nb_sql_CHROME++;
    break;
    case TYPE_SQLITE_FLAG_FIREFOX_INIT_STRINGS:
      if (nb_sql_FIREFOX>=NB_MAX_SQL_REQ)break;
      strncpy(sql_FIREFOX[nb_sql_FIREFOX].sql,argv[0],MAX_PATH);
      strncpy(sql_FIREFOX[nb_sql_FIREFOX].params,argv[1],MAX_PATH);
      sql_FIREFOX[nb_sql_FIREFOX].test_string_id = atoi(argv[2]);
      nb_sql_FIREFOX++;
    break;
  }
  return 0;
}
//------------------------------------------------------------------------------
BOOL SQLITE_Data(FORMAT_CALBAK_READ_INFO *datas, char *sqlite_file, DWORD flag)
{
  sqlite3 *db = db_scan;
  BOOL ret = TRUE;
  char request[MAX_LINE_SIZE];

  switch(datas->type)
  {
    //----------------------------------------
    case TYPE_SQLITE_FLAG_VIEW_CHANGE:    //select results from data base
      //clean la liste view + treeview
      ListView_DeleteAllItems(GetDlgItem(h_view,LV_VIEW));
      TreeView_DeleteAllItems(GetDlgItem(h_view,TRV_VIEW));

      //delete all header in the listeview
      unsigned int i, index = SendDlgItemMessage(h_view,CB_VIEW,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
      for (i=0;i<nb_current_columns;i++)ListView_DeleteColumn(GetDlgItem(h_view,LV_VIEW),1);
      nb_current_columns = columns_params[index].nb_columns;

      //read actual index box + string
      char tmp[MAX_PATH]="";
      GetWindowText(GetDlgItem(h_view,CB_VIEW),tmp,MAX_PATH);

      if (strlen(tmp)!=0)
      {
        //mode explorer or not ?
        RECT Rect;
        GetWindowRect(h_view, &Rect);

        switch(columns_params[index].type)
        {
          case TYPE_SQL_PROCESS:
          case TYPE_SQL_STANDARD:
            VIEW_RESULTS_DBL  = FALSE;
            ShowWindow(GetDlgItem(h_view,TRV_VIEW), SW_HIDE);
            Modify_Style(GetDlgItem(h_view,TRV_VIEW), WS_SIZEBOX, FALSE);

            //resize
            MoveWindow(GetDlgItem(h_view,LV_VIEW), 2, HEIGHT_SEARCH, Rect.right-Rect.left,Rect.bottom-Rect.top-HEIGHT_SEARCH-5, TRUE);
          break;
          case TYPE_SQL_EXPLORER:
            VIEW_RESULTS_DBL  = TRUE;
            ShowWindow(GetDlgItem(h_view,TRV_VIEW), SW_SHOW);
            Modify_Style(GetDlgItem(h_view,TRV_VIEW), WS_SIZEBOX, TRUE);

            MoveWindow(GetDlgItem(h_view,TRV_VIEW), 2, HEIGHT_SEARCH, (Rect.right-Rect.left)/2,Rect.bottom-Rect.top-HEIGHT_SEARCH-5, TRUE);
            MoveWindow(GetDlgItem(h_view,LV_VIEW), (Rect.right-Rect.left)/2+2, HEIGHT_SEARCH, (Rect.right-Rect.left)/2-4,Rect.bottom-Rect.top-HEIGHT_SEARCH-5, TRUE);
          break;
        }

        //set column
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
        lvc.fmt = LVCFMT_LEFT;
        lvc.cx = VIEW_RESULTS_DBL?(Rect.right-Rect.left/2-50)/columns_params[index].nb_columns:(Rect.right-Rect.left-50)/columns_params[index].nb_columns;
        lvc.pszText = "";

        for (i=0;i<columns_params[index].nb_columns;i++)
        {
          SendDlgItemMessage(h_view,LV_VIEW,LVM_INSERTCOLUMN,(WPARAM)i, (LPARAM)&lvc);
        }
        ListView_DeleteColumn(GetDlgItem(h_view,LV_VIEW),nb_current_columns); // for clean the last column in modify!

        //set column title
        snprintf(request, MAX_LINE_SIZE,"SELECT column, string "
                                        "FROM language_strings_columns_string, language "
                                        "WHERE language.id =%d AND language.id = id_language AND id_item=%d;",(int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1,index);
        sqlite3_exec(db, request, callback_sqlite, datas, NULL);

        //add item in the lstv !!!
        FORMAT_CALBAK_READ_INFO fcri;
        fcri.type = TYPE_SQLITE_TEST;

        //request + id language
        unsigned int num_session = SendDlgItemMessage(h_conf,CB_SESSION,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
        switch(columns_params[index].mode_simple)
        {
          case MODE_SQL_NONE:break;
          case MODE_SQL_FULL:
            snprintf(request, MAX_LINE_SIZE,"%s%d AND session_id=%d;",columns_params[index].request,
                     (int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1,session[num_session]);
            sqlite3_exec(db, request, callback_sqlite, &fcri, NULL);
          break;
          case MODE_SQL_LITE:
            snprintf(request, MAX_LINE_SIZE,"%s session_id=%d;",columns_params[index].request,session[num_session]);
            sqlite3_exec(db, request, callback_sqlite, &fcri, NULL);
          break;
          case MODE_SQL_LOGS: //special stat for audit log
            snprintf(request, MAX_LINE_SIZE,"%s%d AND session_id=%d WHERE log_string_description_id.log_id Is Null AND name Is Null AND language_id Is Null;",columns_params[index].request,
                     (int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1,session[num_session]);
            sqlite3_exec(db, request, callback_sqlite, &fcri, NULL);
          break;
        }
      }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_COLUMN_COUNT_INIT:
        snprintf(request, MAX_LINE_SIZE,"SELECT id_item,count,request,mode,type "
                                        "FROM language_strings_columns_settings;");
        sqlite3_exec(db, request, callback_sqlite, datas, NULL);
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_LANGUAGE_CHANGE:
      if (MAX_LINE_SIZE > SendDlgItemMessage(h_conf,CB_LANG,CB_GETITEMHEIGHT,(WPARAM)-1, (LPARAM)NULL))
      {
        snprintf(request, MAX_LINE_SIZE,"SELECT language_components.type,language_components.id_component,language_components.string,language_components.id_language,language.name,language.id "
                                        "FROM language, language_components "
                                        "WHERE language.id =%d AND language.id = language_components.id_language;",(int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1);
        sqlite3_exec(db, request, callback_sqlite, datas, NULL);

        //for start button
        if (start_scan)SetWindowText(GetDlgItem(h_main,BT_START),cps[TXT_BT_STOP].c);
      }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_TITLE_FILE_INIT:
      if (MAX_LINE_SIZE > SendDlgItemMessage(h_conf,CB_LANG,CB_GETITEMHEIGHT,(WPARAM)-1, (LPARAM)NULL))
      {
        snprintf(request, MAX_LINE_SIZE,"SELECT language_components.type,language_components.id_component,language_components.string,language_components.id_language,language.name,language.id "
                                        "FROM language, language_components "
                                        "WHERE language.id =%d AND language.id = language_components.id_language AND language_components.type=%d;",(int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1,TYPE_FILES_TITLE);
        sqlite3_exec(db, request, callback_sqlite, datas, NULL);
      }
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_SESSIONS_INIT:
      sqlite3_exec(db, "SELECT id,name FROM session;", callback_sqlite, datas, NULL);
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_TESTS_UPDATE:
    case TYPE_SQLITE_FLAG_TESTS_INIT:
      if (MAX_LINE_SIZE > SendDlgItemMessage(h_conf,CB_LANG,CB_GETITEMHEIGHT,(WPARAM)-1, (LPARAM)NULL))
      {
        SendDlgItemMessage(h_view,CB_VIEW, CB_RESETCONTENT,(WPARAM)0, (LPARAM)0);
        TreeView_DeleteAllItems(GetDlgItem(h_conf,TRV_TEST));
        snprintf(request, MAX_LINE_SIZE,"SELECT language_strings.string,language_strings.id_language,language.name,language.id,language_strings.id_item "
                                        "FROM language, language_strings "
                                        "WHERE language.id =%d AND language.id = language_strings.id_language ORDER BY id_item;",(int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1);
        sqlite3_exec(db, request, callback_sqlite, datas, NULL);
      }
    break;
    case TYPE_SQLITE_FLAG_LANG_INIT_STRINGS:
        snprintf(request, MAX_LINE_SIZE,"SELECT language_strings_load.string,language_strings_load.id_item,language_strings_load.id_language,language.name,language.id "
                                        "FROM language, language_strings_load "
                                        "WHERE language.id =%d AND language.id = language_strings_load.id_language ORDER BY id_item;",(int)SendDlgItemMessage(h_conf,CB_LANG,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)+1);
        sqlite3_exec(db, request, callback_sqlite, datas, NULL);
    break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_LANG_INIT:sqlite3_exec(db, "SELECT name, id, index_img FROM language;", callback_sqlite, datas, NULL);break;
    //----------------------------------------
    case TYPE_SQLITE_FLAG_ANDROID_INIT_STRINGS:sqlite3_exec(db, "SELECT sql, params, id_tests_string FROM extract_android_request;", callback_sqlite, datas, NULL);break;
    case TYPE_SQLITE_FLAG_CHROME_INIT_STRINGS:sqlite3_exec(db, "SELECT sql, params, id_tests_string FROM extract_chrome_request;", callback_sqlite, datas, NULL);break;
    case TYPE_SQLITE_FLAG_FIREFOX_INIT_STRINGS:sqlite3_exec(db, "SELECT sql, params, id_tests_string FROM extract_firefox_request;", callback_sqlite, datas, NULL);break;
    //----------------------------------------

    //****************************************
    //delete
    case TYPE_SQL_REMOVE_SESSION:
    {
      unsigned int num_session = current_session_id;
      snprintf(request, MAX_LINE_SIZE,"DELETE from session WHERE id=%d;"
                                      "DELETE from extract_IE WHERE session_id=%d;"
                                      "DELETE from extract_android WHERE session_id=%d;"
                                      "DELETE from extract_antivirus WHERE session_id=%d;"
                                      "DELETE from extract_arp WHERE session_id=%d;"
                                      "DELETE from extract_chrome WHERE session_id=%d;"
                                      "DELETE from extract_clipboard WHERE session_id=%d;"
                                      "DELETE from extract_disk WHERE session_id=%d;"
                                      "DELETE from extract_env WHERE session_id=%d;"
                                      "DELETE from extract_file WHERE session_id=%d;"
                                      "DELETE from extract_firefox WHERE session_id=%d;"
                                      "DELETE from extract_firewall WHERE session_id=%d;"
                                      "DELETE from extract_guide WHERE session_id=%d;"
                                      "DELETE from extract_host WHERE session_id=%d;"
                                      "DELETE from extract_log WHERE session_id=%d;"
                                      "DELETE from extract_network WHERE session_id=%d;"
                                      "DELETE from extract_pipe WHERE session_id=%d;"
                                      "DELETE from extract_policy WHERE session_id=%d;"
                                      "DELETE from extract_process WHERE session_id=%d;"
                                      "DELETE from extract_route WHERE session_id=%d;"
                                      "DELETE from extract_share WHERE session_id=%d;"
                                      "DELETE from extract_tache WHERE session_id=%d;"
                                      "DELETE from extract_registry_account_password WHERE session_id=%d;"
                                      "DELETE from extract_registry_mru WHERE session_id=%d;"
                                      "DELETE from extract_registry_path WHERE session_id=%d;"
                                      "DELETE from extract_registry_service_driver WHERE session_id=%d;"
                                      "DELETE from extract_registry_settings WHERE session_id=%d;"
                                      "DELETE from extract_registry_software WHERE session_id=%d;"
                                      "DELETE from extract_registry_start WHERE session_id=%d;"
                                      "DELETE from extract_registry_update WHERE session_id=%d;"
                                      "DELETE from extract_registry_usb WHERE session_id=%d;"
                                      "DELETE from extract_registry_user WHERE session_id=%d;"
                                      "DELETE from extract_registry_userassist WHERE session_id=%d;"
                                      ,session[num_session],session[num_session],session[num_session],session[num_session],session[num_session]
                                      ,session[num_session],session[num_session],session[num_session],session[num_session],session[num_session]
                                      ,session[num_session],session[num_session],session[num_session],session[num_session],session[num_session]
                                      ,session[num_session],session[num_session],session[num_session],session[num_session],session[num_session]
                                      ,session[num_session],session[num_session],session[num_session],session[num_session],session[num_session]
                                      ,session[num_session],session[num_session],session[num_session],session[num_session],session[num_session]
                                      ,session[num_session],session[num_session],session[num_session]);
      sqlite3_exec(db,request, callback_sqlite, datas, NULL);
    }
    break;
    //----------------------------------------
    case TYPE_SQL_REMOVE_ALL_SESSION:
      sqlite3_exec(db,"DELETE from session;"
                      "DELETE from extract_IE;"
                      "DELETE from extract_android;"
                      "DELETE from extract_antivirus;"
                      "DELETE from extract_arp;"
                      "DELETE from extract_chrome;"
                      "DELETE from extract_clipboard;"
                      "DELETE from extract_disk;"
                      "DELETE from extract_env;"
                      "DELETE from extract_file;"
                      "DELETE from extract_firefox;"
                      "DELETE from extract_firewall;"
                      "DELETE from extract_guide;"
                      "DELETE from extract_host;"
                      "DELETE from extract_log;"
                      "DELETE from extract_network;"
                      "DELETE from extract_pipe;"
                      "DELETE from extract_policy;"
                      "DELETE from extract_process;"
                      "DELETE from extract_route;"
                      "DELETE from extract_share;"
                      "DELETE from extract_tache;"
                      "DELETE from extract_registry_account_password;"
                      "DELETE from extract_registry_mru;"
                      "DELETE from extract_registry_path;"
                      "DELETE from extract_registry_service_driver;"
                      "DELETE from extract_registry_settings;"
                      "DELETE from extract_registry_software;"
                      "DELETE from extract_registry_start;"
                      "DELETE from extract_registry_update;"
                      "DELETE from extract_registry_usb;"
                      "DELETE from extract_registry_user;"
                      "DELETE from extract_registry_userassist;", callback_sqlite, datas, NULL);
    break;
    //----------------------------------------

    //****************************************
    //add
    case TYPE_SQL_ADD_SESSION:
    {
      if(TreeView_GetCount(GetDlgItem(h_conf,TRV_FILES)) == NB_MX_TYPE_FILES_TITLE)
        AddNewSession(FALSE, db);
      else AddNewSession(TRUE, db);

      //read all sessions
      nb_session = 0;
      FORMAT_CALBAK_READ_INFO fcri;
      fcri.type  = TYPE_SQLITE_FLAG_SESSIONS_INIT;
      SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
      AddDebugMessage("Interface", "Reload of all sessions", "OK", MSG_INFO);

      //select last item
      SendDlgItemMessage(h_conf,CB_SESSION, CB_SETCURSEL,SendDlgItemMessage(h_conf,CB_SESSION, CB_GETCOUNT,0,0)-1,0);
    }
    break;
    //----------------------------------------
    default:ret = FALSE;break;
  }
  return ret;
}
//------------------------------------------------------------------------------
BOOL SQLITE_LireData(FORMAT_CALBAK_READ_INFO *datas, char *sqlite_file)
{
  return SQLITE_Data(datas, sqlite_file, SQLITE_OPEN_READONLY);
}
//------------------------------------------------------------------------------
BOOL SQLITE_WriteData(FORMAT_CALBAK_READ_INFO *datas, char *sqlite_file)
{
  return SQLITE_Data(datas, sqlite_file, SQLITE_OPEN_READWRITE);
}

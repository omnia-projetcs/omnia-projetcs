//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "RtCA.h"
//------------------------------------------------------------------------------
int callback_sqlite_export(void *d, int argc, char **argv, char **azColName)
{
  FORMAT_CALBAK_TYPE *type = d;
  char datas[MAX_LINE_SIZE];
  DWORD copiee;

  switch(type->type)
  {
    case TYPE_SQL_EXPORT_HEAD:
      switch(export_type)
      {
        case SAVE_TYPE_CSV:
          snprintf(datas,MAX_LINE_SIZE,"\"%s\";",convertUTF8toUTF16(argv[0], strlen(argv[0])+1));
          stat_export_column++;
          WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
        break;
        case SAVE_TYPE_XML:
          if (stat_export_column<NB_MAX_ITEMS_HEADERS_XML)
            strncpy(S_tests_XML_header[stat_export_column++].s,convertUTF8toUTF16(argv[0], strlen(argv[0])+1),DEFAULT_TMP_SIZE);
        break;
        case SAVE_TYPE_HTML:
          snprintf(datas,MAX_LINE_SIZE,"    <th>%s</th>",convertUTF8toUTF16(argv[0], strlen(argv[0])+1));
          stat_export_column++;
          WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
        break;
      }
    break;
    case TYPE_SQL_EXPORT_DATAS:
      switch(export_type)
      {
        case SAVE_TYPE_CSV:
          for (stat_export_column=0;stat_export_column<argc;stat_export_column++)
          {
            if (stat_export_column+1==argc)
            {
              if (argv[stat_export_column]!=0)
                snprintf(datas,MAX_LINE_SIZE,"\"%s\";\r\n",convertUTF8toUTF16(argv[stat_export_column], strlen(argv[stat_export_column])+1));
              else strncpy(datas,"\"\";",MAX_LINE_SIZE);
            }else
            {
              if (argv[stat_export_column]!=0)
                snprintf(datas,MAX_LINE_SIZE,"\"%s\";",convertUTF8toUTF16(argv[stat_export_column], strlen(argv[stat_export_column])+1));
              else strncpy(datas,"\"\";",MAX_LINE_SIZE);
            }
            WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
          }
        break;
        case SAVE_TYPE_XML:
          strncpy(datas,"  <Data>\r\n",MAX_LINE_SIZE);
          WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
          for (stat_export_column=0;stat_export_column<argc;stat_export_column++)
          {
            if (argv[stat_export_column]!=0)
              snprintf(datas,MAX_LINE_SIZE,"     <%s><![CDATA[%s]]></%s>\r\n",
                       S_tests_XML_header[stat_export_column].s,
                       convertUTF8toUTF16(argv[stat_export_column], strlen(argv[stat_export_column])+1),
                       S_tests_XML_header[stat_export_column].s);
            else snprintf(datas,MAX_LINE_SIZE,"     <%s><![CDATA[]]></%s>\r\n",
                       S_tests_XML_header[stat_export_column].s,S_tests_XML_header[stat_export_column].s);

            WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
          }
          strncpy(datas,"  </Data>\r\n",MAX_LINE_SIZE);
          WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
        break;
        case SAVE_TYPE_HTML:
          //line header
          if (line%2==1)strncpy(datas,"  <tr bgcolor=\"#ddddff\">\r\n",MAX_LINE_SIZE);
          else strncpy(datas,"  <tr>\r\n",MAX_LINE_SIZE);
          WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);

          for (stat_export_column=0;stat_export_column<argc;stat_export_column++)
          {
            if (argv[stat_export_column]!=0)
              snprintf(datas,MAX_LINE_SIZE,"    <td>%s</td>",convertUTF8toUTF16(argv[stat_export_column], strlen(argv[stat_export_column])+1));
            else strncpy(datas,"    <td></td>",MAX_LINE_SIZE);

            WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
          }

          //line foot
          strncpy(datas,"\r\n  </tr>\r\n",MAX_LINE_SIZE);
          WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
          line++;
        break;
        case SAVE_TYPE_PWDUMP:
          if (argv[0]!=0)
          {
            snprintf(datas,MAX_LINE_SIZE,"%s\r\n",argv[0]);
            WriteFile(MyhFile_export,datas,strlen(datas),&copiee,0);
          }
        break;
      }
    break;
  }
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI SaveAll(LPVOID lParam)
{
  //get current time
  time_t date;
  time(&date);
  struct tm *today = localtime(&date);
  char date_today[DATE_SIZE_MAX];
  strftime(date_today, DATE_SIZE_MAX,"%Y-%m-%d_%H.%M.%S",today);

  //set file path for all file
  char path[MAX_PATH],file[MAX_PATH];
  snprintf(path,MAX_PATH,"%s\\RtCA[%s]",current_test_export_path,date_today);

  //request sqlite for enumerate the datas
  char request[MAX_LINE_SIZE];
  DWORD copiee;
  unsigned int i,j;
  FORMAT_CALBAK_READ_INFO fcri;
  sqlite3 *db = (sqlite3 *)db_scan;

  //headers
  char XML_head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n  <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
  char XML_foot[]="</RtCA>";

  char HTML_head[]="<html>\r\n<head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n <table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n  <tr bgcolor=\"#CCCCCC\">\r\n";
  char HTML_end_header_column[]="\r\n  </tr>\r\n";

  char HTML_foot[]=" </table>\r\n</html>";

  char CSV_end_header_column[]="\r\n";

  unsigned long int cur_session_id = current_session_id;

  //export_type = 0;
  unsigned int nb_test = SendMessage(hlstbox, LB_GETCOUNT, 0, 0);
  char test_name[DEFAULT_TMP_SIZE];
  for (i=0;i<nb_test;i++)
  {
    //get test name :
    if (SendMessage(hlstbox, LB_GETTEXTLEN, i, 0)>=DEFAULT_TMP_SIZE)continue;
    if (SendMessage(hlstbox, LB_GETTEXT, i, (LPARAM)test_name) == LB_ERR)test_name[0] = 0;

    //get infos
    GetColumnInfo(i);

    //set file name :
    switch(export_type)
    {
      case SAVE_TYPE_CSV:snprintf(file,MAX_PATH,"%s_%s.csv",path,test_name);break;
      case SAVE_TYPE_XML:snprintf(file,MAX_PATH,"%s_%s.xml",path,test_name);break;
      case SAVE_TYPE_HTML:snprintf(file,MAX_PATH,"%s_%s.html",path,test_name);break;
    }

    //open file
    MyhFile_export = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (MyhFile_export == INVALID_HANDLE_VALUE)continue;

    //file header
    switch(export_type)
    {
      //case SAVE_TYPE_CSV:break; //no header
      case SAVE_TYPE_XML:WriteFile(MyhFile_export,XML_head,strlen(XML_head),&copiee,0);break;
      case SAVE_TYPE_HTML:WriteFile(MyhFile_export,HTML_head,strlen(HTML_head),&copiee,0);break;
    }

    //get columns
    stat_export_column = 0;
    fcri.type = TYPE_SQL_EXPORT_HEAD;
    snprintf(request, MAX_LINE_SIZE,"SELECT string "
                                    "FROM language_strings_columns_string, language "
                                    "WHERE language.id =%d AND language.id = id_language AND id_item=%d order by column;",current_lang_id,i);

    sqlite3_exec(db, request, callback_sqlite_export, &fcri, NULL);

    //end of header
    switch(export_type)
    {
      case SAVE_TYPE_CSV:WriteFile(MyhFile_export,CSV_end_header_column,strlen(CSV_end_header_column),&copiee,0);break;
      case SAVE_TYPE_XML:
        for (j=0;j<st_columns.nb_columns;j++)replace_one_char(S_tests_XML_header[j].s,strlen(S_tests_XML_header[j].s),' ','_');
      break;
      case SAVE_TYPE_HTML:WriteFile(MyhFile_export,HTML_end_header_column,strlen(HTML_end_header_column),&copiee,0);break;
    }

    //generate SQL request and go !!!
    fcri.type     = TYPE_SQL_EXPORT_DATAS;
    line          = 0;

    switch(st_columns.mode_simple)
    {
        case MODE_SQL_NONE:break;
        case MODE_SQL_FULL:
          snprintf(request, MAX_LINE_SIZE,"%s%d AND session_id=%lu;",st_columns.request,
                   current_lang_id,cur_session_id);
          sqlite3_exec(db, request, callback_sqlite_export, &fcri, NULL);
        break;
        case MODE_SQL_LITE:
          snprintf(request, MAX_LINE_SIZE,"%s session_id=%lu;",st_columns.request,cur_session_id);
          sqlite3_exec(db, request, callback_sqlite_export, &fcri, NULL);
        break;
        case MODE_SQL_LOGS: //special stat for audit log
          snprintf(request, MAX_LINE_SIZE,"%s%d AND session_id=%lu WHERE log_string_description_id.log_id Is Null AND name Is Null AND language_id Is Null;",st_columns.request,
                   current_lang_id,cur_session_id);
          sqlite3_exec(db, request, callback_sqlite_export, &fcri, NULL);
        break;
    }

    //file foot
    switch(export_type)
    {
      //case SAVE_TYPE_CSV:break; //no foot
      case SAVE_TYPE_XML:WriteFile(MyhFile_export,XML_foot,strlen(XML_foot),&copiee,0);break;
      case SAVE_TYPE_HTML:WriteFile(MyhFile_export,HTML_foot,strlen(HTML_foot),&copiee,0);break;
    }

    CloseHandle(MyhFile_export);
  }

  //pwdump_users export
  snprintf(file,MAX_PATH,"%s_%s_pwdump.txt",path,test_name);

  //open file
  MyhFile_export = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile_export != INVALID_HANDLE_VALUE)
  {
    FORMAT_CALBAK_READ_INFO fcri2;
    export_type = SAVE_TYPE_PWDUMP;
    fcri2.type   = TYPE_SQL_EXPORT_DATAS;

    snprintf(request, MAX_LINE_SIZE,"SELECT raw_password FROM extract_registry_account_password WHERE session_id = %lu;",cur_session_id);
    sqlite3_exec(db, request, callback_sqlite_export, &fcri2, NULL);
    CloseHandle(MyhFile_export);
  }

  ExportStart = FALSE;
  EnableMenuItem(GetSubMenu(GetMenu(h_main),0),8,MF_BYPOSITION|MF_ENABLED);
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI ChoiceSaveAll(LPVOID lParam)
{
  BROWSEINFO browser;
  ITEMIDLIST *lip;

  browser.hwndOwner          = h_conf;
  browser.pidlRoot           = NULL;
  browser.pszDisplayName     = current_test_export_path;
  browser.lpszTitle          = cps[TXT_SAVE_TO].c;
  browser.ulFlags            = BIF_NEWDIALOGSTYLE; //permet la création de répertoire en direct
  browser.lpfn               = NULL;
  if((lip=SHBrowseForFolder(&browser))!=NULL)
  {
    current_test_export_path[0]=0;
    SHGetPathFromIDList(lip,current_test_export_path);
    if (strlen(current_test_export_path)>0)
    {
      ExportStart = TRUE;
      EnableMenuItem(GetSubMenu(GetMenu(h_main),0),8,MF_BYPOSITION|MF_GRAYED);

      h_Export = CreateThread(NULL,0,SaveAll,NULL,0,0);
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
BOOL SaveTRV(HANDLE htv, char *file, unsigned int type)
{
  if(TreeView_GetCount(htv) == NB_MX_TYPE_FILES_TITLE)return FALSE;

  //open file
  HANDLE hfile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (hfile == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  char lines[MAX_LINE_SIZE]="", buffer[MAX_LINE_SIZE]="";
  DWORD copiee;
  TVITEM tvitem;
  HTREEITEM hitem;
  unsigned int i;

  switch(type)
  {
    case SAVE_TYPE_TXT:
      for (i=0;i<NB_MX_TYPE_FILES_TITLE;i++)
      {
        hitem = (HTREEITEM)SendMessage(htv, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[i]);
        do
        {
          buffer[0]=0;
          tvitem.hItem = hitem;
          tvitem.mask = TVIF_TEXT;
          tvitem.pszText = buffer;
          tvitem.cchTextMax = MAX_LINE_SIZE;
          if (SendMessage(htv, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
          {
            snprintf(lines,MAX_LINE_SIZE,"%s\r\n",buffer);
            WriteFile(hfile,lines,strlen(lines),&copiee,0);
          }
        }while((hitem = (HTREEITEM)SendMessage(htv, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));
      }
    break;
    case SAVE_TYPE_CSV:
    {
      char title[DEFAULT_TMP_SIZE];
      for (i=0;i<NB_MX_TYPE_FILES_TITLE;i++)
      {
        hitem = (HTREEITEM)SendMessage(htv, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[i]);

        title[0]=0;
        tvitem.hItem = TRV_HTREEITEM_CONF[i];
        tvitem.mask = TVIF_TEXT;
        tvitem.pszText = title;
        tvitem.cchTextMax = MAX_LINE_SIZE;
        SendMessage(htv, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem);
        do
        {
          buffer[0]=0;
          tvitem.hItem = hitem;
          tvitem.mask = TVIF_TEXT;
          tvitem.pszText = buffer;
          tvitem.cchTextMax = MAX_LINE_SIZE;
          if (SendMessage(htv, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
          {
            snprintf(lines,MAX_LINE_SIZE,"\"%s\";\"%s\";\r\n",title,buffer);
            WriteFile(hfile,lines,strlen(lines),&copiee,0);
          }
        }while((hitem = (HTREEITEM)SendMessage(htv, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));
      }
    }
    break;
  }
  CloseHandle(hfile);
  return TRUE;
}
//------------------------------------------------------------------------------
BOOL SaveLSTV(HANDLE hlv, char *file, unsigned int type, unsigned int nb_column)
{
  //get item count
  unsigned int nb_items = ListView_GetItemCount(hlv);
  if (nb_items > 0 && nb_column > 0)
  {
    //open file
    HANDLE hfile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
      return FALSE;
    }

    char lines[MAX_LINE_SIZE]="", buffer[MAX_LINE_SIZE]="";
    DWORD copiee;
    unsigned int i=0,j=0;

    LVCOLUMN lvc;
    lvc.mask        = LVCF_TEXT;
    lvc.cchTextMax  = MAX_LINE_SIZE;
    lvc.pszText     = buffer;

    switch(type)
    {
      case SAVE_TYPE_CSV:
        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }
        strncat(lines,"\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          lines[0]=0;
          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"\";");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
      break;
      case SAVE_TYPE_XML:
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        LINE_ITEM lv_line[nb_column+1];

        //title line
        for (i=0;i<nb_column;i++)
        {
          lv_line[i].c[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[i].c;
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          replace_one_char(lv_line[i].c, strlen(lv_line[i].c), ' ', '_');
        }

        //save all line
        for (j=0;j<nb_items;j++)
        {
          WriteFile(hfile," <Data>\r\n",9,&copiee,0);
          for (i=0;i<nb_column;i++)
          {
            copiee = 0;
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines,MAX_LINE_SIZE,"  <%s><![CDATA[%s]]></%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(hfile,lines,strlen(lines),&copiee,0);
            }
          }
          WriteFile(hfile," </Data>\r\n",10,&copiee,0);
        }
        WriteFile(hfile,"</RtCA>",7,&copiee,0);
      }
      break;
      case SAVE_TYPE_HTML:
      {
        char head[]="<html>\r\n <head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n <table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n  <tr bgcolor=\"#CCCCCC\">\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"  <th>%s</th>",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }

        strncat(lines,"\r\n  </tr>\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          if (j%2==1)strcpy(lines,"  <tr bgcolor=\"#ddddff\">");
          else strcpy(lines,"  <tr>");

          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td>%s</td>",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td></td>");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"</tr>\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
        WriteFile(hfile," </table>\r\n</html>",17,&copiee,0);
      }
      break;
    }
    CloseHandle(hfile);
    return TRUE;
  }else return FALSE;
}
//------------------------------------------------------------------------------
BOOL SaveLSTVSelectedItems(HANDLE hlv, char *file, unsigned int type, unsigned int nb_column)
{
  //get item count
  unsigned int nb_items = ListView_GetItemCount(hlv);
  if (nb_items > 0 && nb_column > 0)
  {
    //open file
    HANDLE hfile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
      return FALSE;
    }

    char lines[MAX_LINE_SIZE]="", buffer[MAX_LINE_SIZE]="";
    DWORD copiee;
    unsigned int i=0,j=0;

    LVCOLUMN lvc;
    lvc.mask        = LVCF_TEXT;
    lvc.cchTextMax  = MAX_LINE_SIZE;
    lvc.pszText     = buffer;

    switch(type)
    {
      case SAVE_TYPE_CSV:
        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }
        strncat(lines,"\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          if (SendMessage(hlv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) != LVIS_SELECTED)continue;
          lines[0]=0;
          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"\";");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
      break;
      case SAVE_TYPE_XML:
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        LINE_ITEM lv_line[nb_column+1];

        //title line
        for (i=0;i<nb_column;i++)
        {
          lv_line[i].c[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[i].c;
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
        }

        //save all line
        for (j=0;j<nb_items;j++)
        {
          if (SendMessage(hlv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) != LVIS_SELECTED)continue;
          WriteFile(hfile," <Data>\r\n",9,&copiee,0);
          for (i=0;i<nb_column;i++)
          {
            copiee = 0;
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines,MAX_LINE_SIZE,"  <%s><![CDATA[%s]]></%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(hfile,lines,strlen(lines),&copiee,0);
            }
          }
          WriteFile(hfile," </Data>\r\n",10,&copiee,0);
        }
        WriteFile(hfile,"</RtCA>",7,&copiee,0);
      }
      break;
      case SAVE_TYPE_HTML:
      {
        char head[]="<html>\r\n <head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n <table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n  <tr bgcolor=\"#CCCCCC\">\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"  <th>%s</th>",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }

        strncat(lines,"\r\n  </tr>\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        DWORD k=0;
        for (j=0;j<nb_items;j++)
        {
          if (SendMessage(hlv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) != LVIS_SELECTED)continue;
          if (k%2==1)strcpy(lines,"  <tr bgcolor=\"#ddddff\">");
          else strcpy(lines,"  <tr>");

          k++;

          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td>%s</td>",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td></td>");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"</tr>\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
        WriteFile(hfile," </table>\r\n</html>",17,&copiee,0);
      }
      break;
    }
    CloseHandle(hfile);
    return TRUE;
  }else return FALSE;
}
//------------------------------------------------------------------------------
void CopyDataToClipboard(HANDLE hlv, DWORD nline, unsigned short column)
{
  char tmp[MAX_LINE_SIZE]="";
  ListView_GetItemText(hlv,nline,column,tmp,MAX_LINE_SIZE);

  //copy to clipbord
  if(OpenClipboard(NULL))
  {
    EmptyClipboard();
    HANDLE hGlobal = GlobalAlloc(GHND | GMEM_SHARE, strlen(tmp)+1);
    if (hGlobal!=NULL)
    {
      char *p = (char *)GlobalLock(hGlobal);
      if (p != NULL)
      {
        strcpy(p, tmp);
      }
      GlobalUnlock(hGlobal);
      SetClipboardData(CF_TEXT, hGlobal);
    }
    CloseClipboard();
  }
}

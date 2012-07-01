//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
char tmp_file_android[MAX_PATH];
//------------------------------------------------------------------------------
void addAndroidtoDB(char *file, char *parameter, char *data, char *date, DWORD id_language_description, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_android (file,parameter,data,date,id_language_description,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%lu\",%d);",
           file,parameter,data,date,id_language_description,session_id);

  if (!CONSOL_ONLY || DEBUG_CMD_MODE)AddDebugMessage("test_android", request, "-", MSG_INFO);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
int callback_sqlite_android(void *datas, int argc, char **argv, char **azColName)
{
  FORMAT_CALBAK_READ_INFO *type = (FORMAT_CALBAK_READ_INFO *)datas;
  char tmp[MAX_PATH]="";
  char date[DATE_SIZE_MAX]="";
  unsigned int i,size=0;
  if (type->type > 0 && type->type < nb_sql_ANDROID)
  {
    //copy datas
    for (i=0;i<argc && MAX_PATH-size > 0;i++)
    {
      if (argv[i] == NULL)continue;

      //date or not ?
      if (strlen(argv[i]) == DATE_SIZE_MAX-1)
      {
        if (argv[i][4] == '/' && argv[i][13] == ':')
        {
          if (strcmp("1970/01/01 01:00:00",argv[i])!=0)strncpy(date,argv[i],DATE_SIZE_MAX);
          continue;
        }
      }
      if (i>0)snprintf(tmp+size,MAX_PATH-size,", %s",convertUTF8toUTF16(argv[i], strlen(argv[i])+1));
      else snprintf(tmp+size,MAX_PATH-size,"%s",convertUTF8toUTF16(argv[i], strlen(argv[i])+1));
      size = strlen(tmp);
    }
    //get datas and write it
    convertStringToSQL(tmp, MAX_PATH);
    addAndroidtoDB(tmp_file_android,sql_ANDROID[type->type].params,tmp,date,sql_ANDROID[type->type].test_string_id,current_session_id,db_scan);
  }
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_android_history(LPVOID lParam)
{
  FORMAT_CALBAK_READ_INFO data;
  char tmp_msg[MAX_PATH];

  //get child
  HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage((HWND)h_conf,TRV_FILES, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
  sqlite3 *db_tmp;
  WaitForSingleObject(hsemaphore,INFINITE);
  AddDebugMessage("test_android", "Scan Android history - START", "OK", MSG_INFO);

  while(hitem!=NULL)
  {
    //get item txt
    GetTextFromTrv(hitem, tmp_file_android, MAX_PATH);
    //test to open file
    if (sqlite3_open(tmp_file_android, &db_tmp) == SQLITE_OK)
    {
      for (data.type =0;data.type <nb_sql_ANDROID && start_scan;data.type = data.type+1)
      {
        sqlite3_exec(db_tmp, sql_ANDROID[data.type].sql, callback_sqlite_android, &data, NULL);
      }
      sqlite3_close(db_tmp);
      snprintf(tmp_msg,MAX_PATH,"Scan Android file : %s",tmp_file_android);
      AddDebugMessage("test_android", tmp_msg, "OK", MSG_INFO);
    }
    hitem = (HTREEITEM)SendDlgItemMessage((HWND)h_conf,TRV_FILES, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
  }

  AddDebugMessage("test_android", "Scan Android history - DONE", "OK", MSG_INFO);
  check_treeview(GetDlgItem((HWND)h_conf,TRV_TEST), H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  ReleaseSemaphore(hsemaphore,1,NULL);
  return 0;
}

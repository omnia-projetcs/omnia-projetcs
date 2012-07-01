//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addEnvtoDB(char *str, char *user, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_env (string,user,session_id) "
           "VALUES(\"%s\",\"%s\",%d);",
           str,user,session_id);
  if (!CONSOL_ONLY || DEBUG_CMD_MODE)AddDebugMessage("test_env", request, "-", MSG_INFO);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_env(LPVOID lParam)
{
  //check if local or not :)
  if (SendDlgItemMessage(h_conf,TRV_FILES, TVM_GETCOUNT,(WPARAM)0, (LPARAM)0) > NB_MX_TYPE_FILES_TITLE+1)return 0;

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  WaitForSingleObject(hsemaphore,INFINITE);
  AddDebugMessage("test_env", "Scan ENV - START", "OK", MSG_INFO);

  unsigned int session_id = current_session_id;
  DWORD s=NB_USERNAME_SIZE;
  char user[NB_USERNAME_SIZE+1]="",str[MAX_LINE_SIZE];
  GetUserName(user,&s);

  char *c = GetEnvironmentStrings();
  while (*c)
  {
    str[0]=0;
    strncpy(str,c,MAX_LINE_SIZE);

    if (str[0]!=0)
    {
      convertStringToSQL(str, MAX_LINE_SIZE);
      addEnvtoDB(str, user, session_id, db);
    }
    c += strlen(c)+1;
  }

  AddDebugMessage("test_env", "Scan ENV - DONE", "OK", MSG_INFO);
  check_treeview(GetDlgItem(h_conf,TRV_TEST), H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  ReleaseSemaphore(hsemaphore,1,NULL);
  return 0;
}

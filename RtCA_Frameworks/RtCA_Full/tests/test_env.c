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
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_env(LPVOID lParam)
{
  //check if local or not :)
  if (!LOCAL_SCAN)
  {
    h_thread_test[(unsigned int)lParam] = 0;
    check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
    return 0;
  }

  //init
  sqlite3 *db = (sqlite3 *)db_scan;

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

  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

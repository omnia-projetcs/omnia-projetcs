//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addPipetoDB(char *pipe, char *owner, char *rid, char *sid,unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_pipe (pipe,owner,rid,sid,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           pipe,owner,rid,sid,session_id);
  if (!CONSOL_ONLY || DEBUG_CMD_MODE)AddDebugMessage("test_pipe", request, "-", MSG_INFO);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_pipe(LPVOID lParam)
{
  //check if local or not :)
  if (SendDlgItemMessage(h_conf,TRV_FILES, TVM_GETCOUNT,(WPARAM)0, (LPARAM)0) > NB_MX_TYPE_FILES_TITLE+1)return 0;

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  WIN32_FIND_DATA data;
  HANDLE hfic = FindFirstFile("\\\\.\\pipe\\*", &data);
  if (hfic == INVALID_HANDLE_VALUE)return 0;

  WaitForSingleObject(hsemaphore,INFINITE);
  AddDebugMessage("test_pipe", "Scan PIPE - START", "OK", MSG_INFO);

  char pipe[MAX_PATH],owner[DEFAULT_TMP_SIZE],rid[DEFAULT_TMP_SIZE],sid[DEFAULT_TMP_SIZE];

  do
  {
    //owner
    snprintf(pipe,MAX_PATH,"\\\\.\\pipe\\%s",data.cFileName);
    GetOwner(pipe,owner,rid,sid,DEFAULT_TMP_SIZE);

    addPipetoDB(pipe, owner, rid, sid, session_id, db);
  }while(FindNextFile (hfic,&data));

  AddDebugMessage("test_pipe", "Scan PIPE - DONE", "OK", MSG_INFO);
  check_treeview(GetDlgItem(h_conf,TRV_TEST), H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  ReleaseSemaphore(hsemaphore,1,NULL);
  return 0;
}

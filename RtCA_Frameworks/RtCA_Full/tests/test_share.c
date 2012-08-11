//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addSharetoDB(char *share, char *path, char *description, char *type, char *connexion, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_share (share,path,description,type,connexion,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           share,path,description,type,connexion,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_share(LPVOID lParam)
{
  if (SendMessage(htrv_files, TVM_GETCOUNT,(WPARAM)0, (LPARAM)0) > NB_MX_TYPE_FILES_TITLE+1)return 0;

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;
  HMODULE hDLL = LoadLibrary("NETAPI32.dll");
  if (hDLL == NULL)return 0;

  typedef NET_API_STATUS (WINAPI *NETAPIBUFFERFREE)(LPVOID Buffer);
  NETAPIBUFFERFREE NetApiBufferFree = (NETAPIBUFFERFREE) GetProcAddress(hDLL,"NetApiBufferFree");

  typedef NET_API_STATUS (WINAPI *NETSHAREENUM)(LPWSTR servername, DWORD level, LPBYTE* bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries, LPDWORD resume_handle);
  NETSHAREENUM NetShareEnum = (NETSHAREENUM) GetProcAddress(hDLL,"NetShareEnum");

  if (NetApiBufferFree != NULL && NetShareEnum != NULL )
  {
    NET_API_STATUS res;
    PSHARE_INFO_502 buffer,p;
    DWORD nb=0,tr=0,i;
    char share[DEFAULT_TMP_SIZE], path[MAX_PATH], description[MAX_PATH], type[DEFAULT_TMP_SIZE], connexion[DEFAULT_TMP_SIZE];

    do
    {
      res = NetShareEnum (0, 502, (LPBYTE *) &buffer,MAX_PREFERRED_LENGTH, &nb, &tr,0);
      if(res != ERROR_SUCCESS && res != ERROR_MORE_DATA)break;

      for(i=1,p=buffer;i<=nb;i++,p++)
      {
        snprintf(share,DEFAULT_TMP_SIZE,"%S",p->shi502_netname);
        snprintf(path,MAX_PATH,"%S",p->shi502_path);
        snprintf(description,MAX_PATH,"%S",p->shi502_remark);

        switch(p->shi502_type)
        {
          case STYPE_DISKTREE:  strncpy(type,"DISKTREE",DEFAULT_TMP_SIZE);break;
          case STYPE_PRINTQ:    strncpy(type,"PRINT",DEFAULT_TMP_SIZE);break;
          case STYPE_DEVICE:    strncpy(type,"DEVICE",DEFAULT_TMP_SIZE);break;
          case STYPE_IPC:       strncpy(type,"IPC",DEFAULT_TMP_SIZE);break;
          case STYPE_SPECIAL:   strncpy(type,"SPECIAL",DEFAULT_TMP_SIZE);break;
          case 0x40000000:      strncpy(type,"TEMPORARY",DEFAULT_TMP_SIZE);break;
          case -2147483645:     strncpy(type,"RPC",DEFAULT_TMP_SIZE);break;
          default :             snprintf(type,DEFAULT_TMP_SIZE,"UNKNOW (%lu)",p->shi502_type);break;
        }

        if (p->shi502_max_uses==-1)
          snprintf(connexion,DEFAULT_TMP_SIZE,"%lu/-",p->shi502_current_uses);
        else snprintf(connexion,DEFAULT_TMP_SIZE,"%lu/%lu",p->shi502_current_uses,p->shi502_max_uses);

        convertStringToSQL(path, MAX_PATH);
        convertStringToSQL(description, MAX_PATH);
        addSharetoDB(share, path, description, type, connexion, session_id, db);
      }
    }while(res==ERROR_MORE_DATA);
  }
  FreeLibrary(hDLL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  return 0;
}

//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addTasktoDB(char *id_ev, char *type, char *data, char*next_run, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_tache (id_ev,type,next_run,data,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           id_ev,type,data,next_run,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_task(LPVOID lParam)
{
  //check if local or not :)
  if (!LOCAL_SCAN || WINE_OS)
  {
    h_thread_test[(unsigned int)lParam] = 0;
    check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
    MessageBox(0,"OK","test",MB_OK);
    return 0;
  }

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  HMODULE hDLL = LoadLibrary( "NETAPI32.dll");
  if (hDLL!=NULL)
  {
    typedef NET_API_STATUS (WINAPI *NETSCHEDULEJOBENUM)(LPCWSTR servername,LPBYTE* PointerToBuffer,DWORD PreferredMaximumLength,LPDWORD EntriesRead,LPDWORD TotalEntries,LPDWORD ResumeHandle );
    NETSCHEDULEJOBENUM NetScheduleJobEnum = (NETSCHEDULEJOBENUM) GetProcAddress(hDLL,"NetScheduleJobEnum");

    typedef NET_API_STATUS (WINAPI *NETAPIBUFFERFREE)(LPVOID Buffer);
    NETAPIBUFFERFREE NetApiBufferFree = (NETAPIBUFFERFREE) GetProcAddress(hDLL,"NetApiBufferFree");

    if (NetScheduleJobEnum && NetApiBufferFree)
    {
      //get datas
      typedef struct _AT_ENUM {
        DWORD JobId;
        DWORD JobTime;
        DWORD DaysOfMonth;
        UCHAR DaysOfWeek;
        UCHAR Flags;
        LPWSTR Command;
      }AT_ENUM;

      AT_ENUM *bufAtEnum,*b;
      DWORD EntriesRead=0,TotalEntries=0,ResumeHandle=0;
      NET_API_STATUS res = NetScheduleJobEnum(0,(LPBYTE*)&bufAtEnum,MAX_PREFERRED_LENGTH,&EntriesRead,&TotalEntries,&ResumeHandle);

      if(res == 0 && TotalEntries && EntriesRead && bufAtEnum)
      {
        char id_ev[DEFAULT_TMP_SIZE],type[DEFAULT_TMP_SIZE],data[MAX_PATH],next_run[DEFAULT_TMP_SIZE];

        for(b = bufAtEnum;EntriesRead>0;EntriesRead--)
        {
          if (b->Command > 0)
          {
            id_ev[0]    = 0;
            type[0]     = 0;
            data[0]     = 0;
            next_run[0] = 0;

            snprintf(id_ev,DEFAULT_TMP_SIZE,"%08lu",b->JobId);
            snprintf(data,MAX_PATH,"%S",b->Command);
            convertStringToSQL(data, MAX_PATH);
            snprintf(next_run,DEFAULT_TMP_SIZE,"%lu:%02d:%02d",b->JobTime/3600000,(unsigned int)(b->JobTime%3600000)/60000,(unsigned int)((b->JobTime%3600000)%60000)/1000);

            switch(b->Flags)
            {
              case 1: addTasktoDB(id_ev,"JOB_RUN_PERIODICALLY",data,next_run,session_id,db);break;
              case 2: addTasktoDB(id_ev,"JOB_EXEC_ERROR",data,next_run,session_id,db);break;
              case 4: addTasktoDB(id_ev,"JOB_RUNS_TODAY",data,next_run,session_id,db);break;
              case 16:addTasktoDB(id_ev,"JOB_NONINTERACTIVE",data,next_run,session_id,db);break;
              default:
                snprintf(type,DEFAULT_TMP_SIZE,"UNKNOW (%d)",b->Flags);
                addTasktoDB(id_ev,type,data,next_run,session_id,db);
              break;
            }
          }
          b++;
        }
      }
    }
    FreeLibrary(hDLL);
  }
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

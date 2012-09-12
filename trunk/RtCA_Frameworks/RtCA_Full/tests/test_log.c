//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addLogtoDB(char *eventname, char *indx, char *log_id,
                  char *send_date, char *write_date,
                  char *source, char *description, char *user, char *rid, char *sid,
                  char *state, char *critical, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_log (event,indx,log_id,send_date,write_date,source,description,user,rid,sid,state,critical,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           eventname,indx,log_id,send_date,write_date,source,description,user,rid,sid,state,critical,session_id);

  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
void LireEvent(HANDLE Heventlog, char *eventname, sqlite3 *db, unsigned int session_id, DWORD cRecords)
{
  char indx[DEFAULT_TMP_SIZE], log_id[DEFAULT_TMP_SIZE],
  send_date[DATE_SIZE_MAX], write_date[DATE_SIZE_MAX],
  source[MAX_PATH], description[MAX_LINE_SIZE],
  user[DEFAULT_TMP_SIZE], rid[DEFAULT_TMP_SIZE], sid[DEFAULT_TMP_SIZE],
  state[DEFAULT_TMP_SIZE], critical[DEFAULT_TMP_SIZE];

  EVENTLOGRECORD *pevlr;
  DWORD cbBuffer = 1024*64;
  BYTE bBuffer[(1024*64)+1];
  pevlr = (EVENTLOGRECORD *) &bBuffer;
  DWORD dwRead, dwNeeded;

  unsigned long int i=1, size_real, size_max, uSize, x, uStringOffset, uStepOfString;
  LPBYTE szExpandedString, pStrings;

  while (ReadEventLog(Heventlog,EVENTLOG_BACKWARDS_READ |EVENTLOG_SEQUENTIAL_READ,0,&bBuffer,cbBuffer,&dwRead,&dwNeeded) && i < cRecords && start_scan)
  {
    pevlr = (EVENTLOGRECORD *) &bBuffer;
    if (pevlr!=NULL && pevlr->Length>0)
    {
      while (dwRead > 0 && i < cRecords && start_scan)
      {
        //record number
        snprintf(indx,DEFAULT_TMP_SIZE,"%08lu",pevlr->RecordNumber);

        //Type
        switch(pevlr->EventType)
        {
          case 0x01 : strcpy(state,"ERROR"); break;
          case 0x02 : strcpy(state,"WARNING"); break;
          case 0x04 : strcpy(state,"INFORMATION"); break;
          case 0x08 : strcpy(state,"AUDIT_SUCCESS"); break;
          case 0x10 : strcpy(state,"AUDIT_FAILURE"); break;
          default :state[0]=0;break;
        }

        if (state[0]!=0)
        {
          //date : send_date
          timeToString(pevlr->TimeGenerated, send_date, DATE_SIZE_MAX);

          //date : write_date
          timeToString(pevlr->TimeWritten, write_date, DATE_SIZE_MAX);

          //source
          source[0]=0;
          if (sizeof(EVENTLOGRECORD) < pevlr->Length)
            strncpy(source,(char *)pevlr+sizeof(EVENTLOGRECORD),DEFAULT_TMP_SIZE);

          //ID
          snprintf(log_id,DEFAULT_TMP_SIZE,"%08lu",pevlr->EventID& 0xFFFF);

          //user+rid+sid
          user[0] = 0;
          rid[0]  = 0;
          sid[0]  = 0;
          SidtoUser((PSID)((LPBYTE) pevlr + pevlr->UserSidOffset), user, rid, sid, DEFAULT_TMP_SIZE);

          //descriptions strings !!!
          uSize         = pevlr->DataOffset - pevlr->StringOffset;
          uStringOffset = pevlr->StringOffset;
          if (uSize>0 && uStringOffset>0)
          {
            pStrings = (LPBYTE)GlobalAlloc(GPTR, uSize * sizeof(BYTE));
            if (pStrings > 0)
            {
               memcpy(pStrings, (LPBYTE)pevlr + uStringOffset, uSize);
               uStepOfString   = 0;

               szExpandedString = (LPBYTE)GlobalAlloc(GPTR, (uSize + MAX_LINE_SIZE) * sizeof(BYTE));
               size_max = uSize + MAX_LINE_SIZE;
               if (szExpandedString > 0)
               {
                 for(x = 0,size_real=0; x < pevlr->NumStrings && size_max>size_real && uStepOfString < uSize; x++)
                 {
                    snprintf((char*)(szExpandedString+size_real),size_max-size_real,"%s,",pStrings + uStepOfString);
                    size_real = size_real + strlen((char*)szExpandedString);
                    uStepOfString = strlen((char*)(pStrings + uStepOfString))+1;
                 }
                 strncpy(description,(char*)szExpandedString,MAX_LINE_SIZE);
                 GlobalFree(szExpandedString);
               }
              GlobalFree(pStrings);
            }
          }

          if (strcmp(send_date,write_date) != 0)strncpy(critical,"X",DEFAULT_TMP_SIZE);
          else critical[0]=0;

          //add
          convertStringToSQL(source, MAX_PATH);
          convertStringToSQL(description, MAX_LINE_SIZE);

          addLogtoDB(eventname, indx, log_id,
                     send_date, write_date, source, description,
                     user, rid, sid, state, critical, session_id, db);
        }
        pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr + pevlr->Length);
        dwRead = dwRead-pevlr->Length;
        i++;
      }
    }
    break;
  }
}
//------------------------------------------------------------------------------
void OpenDirectEventLog(char *eventname, sqlite3 *db, unsigned int session_id)
{
  //backup puis event log
  HANDLE Heventlog = OpenBackupEventLog(NULL,eventname);
  DWORD cRecords=0;
  if (Heventlog!=NULL)
  {
    if (GetNumberOfEventLogRecords(Heventlog, &cRecords))
    {
      if (cRecords>0)
      {
        LireEvent(Heventlog,eventname,db,session_id,cRecords);
        CloseEventLog(Heventlog);
      }
    }
  }

  Heventlog = OpenEventLog(NULL,eventname);
  cRecords=0;
  if (Heventlog!=NULL)
  {
    if (GetNumberOfEventLogRecords(Heventlog, &cRecords))
    {
      if (cRecords>0)
      {
        LireEvent(Heventlog,eventname,db,session_id,cRecords);
        CloseEventLog(Heventlog);
      }
    }
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_log(LPVOID lParam)
{
  unsigned int session_id = current_session_id;
  //db
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  sqlite3 *db = (sqlite3 *)db_scan;
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
  if (hitem!=NULL || !LOCAL_SCAN)
  {
    char tmp[MAX_PATH];
    char ext[10];
    while(hitem!=NULL)
    {
      //get item txt
      GetTextFromTrv(hitem, tmp, MAX_PATH);

      //get extension on verify
      ext[0] = 0;
      extractExtFromFile(tmp, ext, 10);
      if (strcmp("evt",ext) == 0)TraiterEventlogFileEvt(tmp, db, session_id);
      else if (strcmp("evtx",ext) == 0)TraiterEventlogFileEvtx(tmp, db, session_id);
      else if (strcmp("log",ext) == 0)TraiterEventlogFileLog(tmp, db, session_id);

      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }else
  {
    //read in the registry of all event
    char eventname[MAX_PATH];
    HKEY CleTmp=0;
    DWORD nbSubKey=0,i=0, TailleNomSubKey;

    BOOL ok = FALSE;
    if (RegOpenKey((HKEY)HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Eventlog",&CleTmp)==ERROR_SUCCESS)
    {
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        if (nbSubKey > 0)
        {
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey = MAX_PATH;
            if (RegEnumKeyEx (CleTmp,i,eventname,(LPDWORD)&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              ok=TRUE;
              OpenDirectEventLog(eventname,db,session_id);
            }
          }
        }
      }
      RegCloseKey(CleTmp);
    }
    if (!ok)
    {
      OpenDirectEventLog("Application",db,session_id);       //journal application
      OpenDirectEventLog("Security",db,session_id);          //journal sécurité
      OpenDirectEventLog("System",db,session_id);            //journal système
      OpenDirectEventLog("Internet Explorer",db,session_id); //Internet Explorer
      OpenDirectEventLog("OSession",db,session_id);          //session Office
    }

  }

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
char malware_check[MAX_PATH];
//------------------------------------------------------------------------------
/*
  - malware liste : https://easylist-downloads.adblockplus.org/malwaredomains_full.txt
                    http://mirror1.malwaredomains.com/files/domains.txt
*/
void addHosttoDB(char*file, char*ip, char*name, char*last_file_update, unsigned int session_id, sqlite3 *db)
{
  //chek name if malware or not
  MalwareCheck(name, malware_check, MAX_PATH);

  char request[MAX_LINE_SIZE+DEFAULT_TMP_SIZE];
  snprintf(request,MAX_LINE_SIZE+DEFAULT_TMP_SIZE,
           "INSERT INTO extract_host (file,ip,name,last_file_update,malware_check,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           file,ip,name,last_file_update,malware_check,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
int callback_sqlite_malware(void *datas, int argc, char **argv, char **azColName)
{
  if (strlen(malware_check) > 0)strncat(malware_check,"\r\n",MAX_PATH);
  strncat(malware_check,argv[0],MAX_PATH);
  strncat(malware_check,"\0",MAX_PATH);
  return 0;
}
//------------------------------------------------------------------------------
void MalwareCheck(char*name, char*malware_check, unsigned int malware_check_max_size)
{
  malware_check[0] = 0;
  char request[MAX_LINE_SIZE];
  char *c = name;

  //get only last 2 parts xxx.xxx
  while (*c++);
  while (*c != name[0] && *c != '.')c--; //fisrt
  if (*c != name[0])c--;
  while (*c != name[0] && *c != '.')c--; //second
  if (*c == '.')
  {
    c++;
    snprintf(request, MAX_LINE_SIZE,"SELECT description FROM malware_list WHERE domain LIKE \"%%%s\";",c);
  }else snprintf(request, MAX_LINE_SIZE,"SELECT description FROM malware_list WHERE domain LIKE \"%%%s\";",name);

  FORMAT_CALBAK_READ_INFO fcri;
  sqlite3_exec(db_scan, request, callback_sqlite_malware, &fcri, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_dns(LPVOID lParam)
{
  //check if local or not :)
  if (!LOCAL_SCAN || WINE_OS)
  {
    h_thread_test[(unsigned int)lParam] = 0;
    check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
    return 0;
  }

  //init
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  //make file directory
  char file[MAX_PATH]="";
  char ip[IPV6_SIZE_MAX],name[MAX_PATH];
  snprintf(file,MAX_PATH,"%s\\WINDOWS\\system32\\drivers\\etc\\hosts",getenv("SYSTEMDRIVE"));

  //open host file and read all hosts
  HANDLE Hfic = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic != INVALID_HANDLE_VALUE)
  {
    DWORD taille_fic = GetFileSize(Hfic,NULL);
    if (taille_fic != INVALID_FILE_SIZE)
    {
      char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
      if (buffer != NULL)
      {
        //get last update
        char last_file_update[DATE_SIZE_MAX]="";
        FILETIME LastWriteTime;
        if(GetFileTime(Hfic,NULL,NULL,&LastWriteTime))filetimeToString_GMT(LastWriteTime, last_file_update, DATE_SIZE_MAX);

        //read data line by line
        DWORD copiee;
        char lines[MAX_PATH];
        if (ReadFile(Hfic, buffer, taille_fic,&copiee,0))
        {
          char *r = buffer;
          char *s,*c;
          while (*r)
          {
            //read line
            memset(lines,0,MAX_PATH);
            strncpy(lines,r,MAX_PATH);
            s = lines;
            while (*s && *s != '\r')s++;
            *s = 0;
            r = r+strlen(lines)+2;

            //comment or not :)
            if (lines[0]!='#' && strlen(lines) > 8)
            {
              //get IP
              strncpy(ip,lines,IPV6_SIZE_MAX);
              c = ip;

              while (*c && *c != ' ' && *c!= '\t' && (*c == '.' || *c == ':' || (*c<='9' && *c>='0')))c++;
              if (*c)
              {
                *c = 0;
                //get name
                c = lines+strlen(ip);
                while (*c && (*c == ' ' || *c == '\t'))c++;

                memset(name,0,MAX_PATH);
                strncpy(name,c,MAX_PATH);
                addHosttoDB(file, ip, name, last_file_update,session_id,db);
              }
            }
          }
        }
        HeapFree(GetProcessHeap(), 0, buffer);
      }
    }
    CloseHandle(Hfic);
  }

  //get cache
  HMODULE hDLL = LoadLibrary( "DNSAPI.DLL" );
  if (!hDLL) return 0;

  //function
  typedef int(*DNS_GET_CACHE_DATA_TABLE)(PDNS_RECORD*);
  DNS_GET_CACHE_DATA_TABLE DnsGetCacheDataTable = (DNS_GET_CACHE_DATA_TABLE)GetProcAddress(hDLL,"DnsGetCacheDataTable");

  if (DnsGetCacheDataTable != NULL)
  {
    PDNS_RECORD pcache = NULL;
    DNS_RECORD* dnsRecords = NULL, *dnsr;
    IN_ADDR ipAddress;
    char last_file_update[DATE_SIZE_MAX]="";

    if (DnsGetCacheDataTable(&pcache) == TRUE)
    {
      PDNS_RECORD cache = pcache;
      while (cache)
      {
        memset(name,0,MAX_PATH);
        snprintf(name,MAX_PATH,"%S",cache->pName);
        if (name[0] != 0)
        {
          //get IP + TTL
          if(DnsQuery(name,DNS_TYPE_A,0/*0x10 : DNS_QUERY_CACHE_ONLY*/,NULL,&dnsRecords,NULL) == ERROR_SUCCESS)
          {
            dnsr = dnsRecords;
            while (dnsr != NULL)
            {
              ipAddress.S_un.S_addr = dnsr->Data.A.IpAddress;
              if (inet_ntoa(ipAddress) != NULL)
              {
                snprintf(ip,IP_SIZE_MAX,"%s",inet_ntoa(ipAddress));
                snprintf(last_file_update,DATE_SIZE_MAX,"%lu (s)",dnsr->dwTtl);
                addHosttoDB("", ip, name, last_file_update,session_id,db);
              }
              dnsr = dnsr->pNext;
            }
            //free
            DnsRecordListFree(dnsRecords,DnsFreeRecordList);
          }else
          {
            addHosttoDB("", "", name, "",session_id,db);
          }
        }
        cache = cache->pNext;
      }
    }
  }
  FreeLibrary(hDLL);

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

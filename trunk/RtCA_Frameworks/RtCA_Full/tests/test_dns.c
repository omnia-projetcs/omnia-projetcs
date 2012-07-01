//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addHosttoDB(char*file, char*ip, char*name, char*last_file_update, unsigned int session_id, sqlite3 *db)
{
  char request[MAX_LINE_SIZE+DEFAULT_TMP_SIZE];
  snprintf(request,MAX_LINE_SIZE+DEFAULT_TMP_SIZE,
           "INSERT INTO extract_host (file,ip,name,last_file_update,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           file,ip,name,last_file_update,session_id);
  if (!CONSOL_ONLY || DEBUG_CMD_MODE)AddDebugMessage("test_dns", request, "-", MSG_INFO);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_dns(LPVOID lParam)
{
  //check if local or not :)
  if (SendDlgItemMessage(h_conf,TRV_FILES, TVM_GETCOUNT,(WPARAM)0, (LPARAM)0) > NB_MX_TYPE_FILES_TITLE+1)return 0;

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;
  WaitForSingleObject(hsemaphore,INFINITE);
  AddDebugMessage("test_dns", "Scan DNS resolv  - START", "OK", MSG_INFO);

  //make file directory
  char file[MAX_PATH]="";
  char ip[IP_SIZE_MAX],name[MAX_PATH];
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
        if(GetFileTime(Hfic,NULL,NULL,&LastWriteTime))filetimeToString(LastWriteTime, last_file_update, DATE_SIZE_MAX);

        //read data line by line
        DWORD copiee;
        char line[MAX_PATH];
        if (ReadFile(Hfic, buffer, taille_fic,&copiee,0))
        {
          char *r = buffer;
          char *s,*c;
          while (*r)
          {
            //read line
            line[0] = 0;
            strncpy(line,r,MAX_PATH);
            s = line;
            while (*s && *s != '\r')s++;
            *s = 0;
            r = r+strlen(line)+2;

            //comment or not :)
            if (line[0]!='#' && strlen(line) > 8)
            {
              //get IP
              strncpy(ip,line,IP_SIZE_MAX);
              c = ip;
              while (*c && *c != ' ' && *c!= '\t' && (*c == '.'||*c<58 && *c>47))c++;
              if (*c)
              {
                c = 0;
                //get name
                c = line+strlen(ip);
                while (*c && (*c == ' ' || *c == '\t'))c++;

                strncpy(name,c,IP_SIZE_MAX);
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
  typedef struct _DNS_CACHE_ENTRY {
      struct _DNS_CACHE_ENTRY* pNext;     // Pointer to next entry
      char* pszName;                      // DNS Record Name
      unsigned short wType;               // DNS Record Type
      unsigned short wDataLength;         // Not referenced
      unsigned long dwFlags;              // DNS Record Flags
  } DNSCACHEENTRY, *PDNSCACHEENTRY;

  typedef int(*DNS_GET_CACHE_DATA_TABLE)(PDNSCACHEENTRY*);
  DNS_GET_CACHE_DATA_TABLE DnsGetCacheDataTable = (DNS_GET_CACHE_DATA_TABLE)GetProcAddress(hDLL,"DnsGetCacheDataTable");

  if (DnsGetCacheDataTable != NULL)
  {
    PDNSCACHEENTRY cache = NULL;
    DNS_RECORD* dnsRecords = NULL, *dnsr;
    IN_ADDR ipAddress;

    char last_file_update[DATE_SIZE_MAX]="";

    if (DnsGetCacheDataTable(&cache) == TRUE)
    {
      while (cache != NULL)
      {
        snprintf(name,MAX_PATH,"%S",cache->pszName);

        //get IP + TTL
        if(DnsQuery(name,DNS_TYPE_A,0x10/*DNS_QUERY_CACHE_ONLY*/,NULL,&dnsRecords,NULL) == ERROR_SUCCESS)
        {
          dnsr = dnsRecords;
          while (dnsRecords != NULL)
          {
            ipAddress.S_un.S_addr = dnsRecords->Data.A.IpAddress;
            if (inet_ntoa(ipAddress) != NULL)
            {
              snprintf(ip,IP_SIZE_MAX,"%s",inet_ntoa(ipAddress));
              snprintf(last_file_update,DATE_SIZE_MAX,"%lu (s)",dnsRecords->dwTtl);

              addHosttoDB("", ip, name, last_file_update,session_id,db);
            }
            dnsRecords = dnsRecords->pNext;
          }
          //free
          DnsRecordListFree(dnsr,DnsFreeRecordList);
        }
        cache = cache->pNext;
      }
    }
  }
  FreeLibrary(hDLL);

  AddDebugMessage("test_dns", "Scan DNS resolv  - DONE", "OK", MSG_INFO);
  check_treeview(GetDlgItem(h_conf,TRV_TEST), H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  ReleaseSemaphore(hsemaphore,1,NULL);
  return 0;
}

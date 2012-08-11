//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addARPtoDB(char *ip, char *mac, char *type, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_arp (ip,mac,type,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",%d);",
           ip,mac,type,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_arp(LPVOID lParam)
{
  //check if local or not :)
  if (SendMessage(htrv_files, TVM_GETCOUNT,(WPARAM)0, (LPARAM)0) > NB_MX_TYPE_FILES_TITLE+1)return 0;

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  //load route table);
  HMODULE hDLL = LoadLibrary( "IPHLPAPI.DLL" );
  if (!hDLL) return 0;

  //declaration load function
  typedef DWORD (WINAPI *GETIPNETTABLE)(PMIB_IPNETTABLE pIpNetTable, PULONG pdwSize, BOOL bOrder);
  GETIPNETTABLE GetIpNetTable = (GETIPNETTABLE) GetProcAddress(hDLL,"GetIpNetTable");

  if (GetIpNetTable != NULL)
  {
    DWORD dwSize = 0,i;
    //allocate momery
    if (GetIpNetTable(NULL, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
    {
      if (dwSize > 0)
      {
        PMIB_IPNETTABLE pIpNetTable = (MIB_IPNETTABLE *) malloc (dwSize);
        if (pIpNetTable != NULL)
        {
          //Get datas
          if(GetIpNetTable(pIpNetTable, &dwSize, 0) == NO_ERROR)
          {
            char ip[IP_SIZE_MAX], mac[MAC_SIZE], type[DEFAULT_TMP_SIZE];

            for (i=0; i<pIpNetTable->dwNumEntries; i++)
            {
              snprintf(ip,IP_SIZE_MAX,"%s",inet_ntoa(*(struct in_addr*)&pIpNetTable->table[i].dwAddr));
              snprintf(mac,MAC_SIZE,"%02X:%02X:%02X:%02X:%02X:%02X",
                       pIpNetTable->table[i].bPhysAddr[0] & 0xFF,
                       pIpNetTable->table[i].bPhysAddr[1] & 0xFF,
                       pIpNetTable->table[i].bPhysAddr[2] & 0xFF,
                       pIpNetTable->table[i].bPhysAddr[3] & 0xFF,
                       pIpNetTable->table[i].bPhysAddr[4] & 0xFF,
                       pIpNetTable->table[i].bPhysAddr[5] & 0xFF);

              switch(pIpNetTable->table[i].dwType)
              {
                case 1:addARPtoDB(ip, mac, "MIB_IPNET_TYPE_OTHER", session_id, db);break;
                case 2:addARPtoDB(ip, mac, "MIB_IPNET_TYPE_INVALID", session_id, db);break;
                case 3:addARPtoDB(ip, mac, "MIB_IPNET_TYPE_DYNAMIC", session_id, db);break;
                case 4:addARPtoDB(ip, mac, "MIB_IPNET_TYPE_STATIC", session_id, db);break;
                default:
                  snprintf(type,DEFAULT_TMP_SIZE,"UNKNOW (code:%ld)",pIpNetTable->table[i].dwType);
                  addARPtoDB(ip, mac, type, session_id, db);break;
              }
            }
          }
          free(pIpNetTable);
        }
      }
    }
  }

  //free
  FreeLibrary(hDLL);

  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  return 0;
}

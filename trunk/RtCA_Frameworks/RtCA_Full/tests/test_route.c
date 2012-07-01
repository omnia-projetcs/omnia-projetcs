//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addRoutetoDB(char *destination, char *netmask, char *gateway, DWORD metric, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_route (destination,netmask,gateway,metric,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%lu\",%d);",
           destination,netmask,gateway,metric,session_id);
  if (!CONSOL_ONLY || DEBUG_CMD_MODE)AddDebugMessage("test_route", request, "-", MSG_INFO);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_route(LPVOID lParam)
{
  //check if local or not :)
  if (SendDlgItemMessage(h_conf,TRV_FILES, TVM_GETCOUNT,(WPARAM)0, (LPARAM)0) > NB_MX_TYPE_FILES_TITLE+1)return 0;

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  //load route table);
  HANDLE hDLL = LoadLibrary( "IPHLPAPI.DLL" );
  if (!hDLL) return 0;

  WaitForSingleObject(hsemaphore,INFINITE);
  AddDebugMessage("test_route", "Scan route  - START", "OK", MSG_INFO);

  //declaration load function
  typedef DWORD (WINAPI *GETIPFORWARDTABLE)(PMIB_IPFORWARDTABLE pIpForwardTable, PULONG pdwSize, BOOL bOrder);
  GETIPFORWARDTABLE GetIpForwardTable = (GETIPFORWARDTABLE) GetProcAddress(hDLL,"GetIpForwardTable");

  if (GetIpForwardTable!= NULL)
  {
    //load all table
    PMIB_IPFORWARDTABLE pIpForwardTable = (MIB_IPFORWARDTABLE*) HeapAlloc(GetProcessHeap(), 0, (sizeof(MIB_IPFORWARDTABLE)));
    if (pIpForwardTable != NULL)
    {
      //alloc memory
      DWORD i, dwSize = 0;
      if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
      {
        HeapFree(GetProcessHeap(), 0,pIpForwardTable);
        pIpForwardTable = (MIB_IPFORWARDTABLE*) HeapAlloc(GetProcessHeap(), 0,dwSize);

        if (pIpForwardTable == NULL)
        {
          FreeLibrary(hDLL);
          ReleaseSemaphore(hsemaphore,1,NULL);
          return 0;
        }
      }

      //get datas
      if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == NO_ERROR)
      {
        struct in_addr IpAddr_dst;
        struct in_addr IpAddr_msk;
        struct in_addr IpAddr_hop;

        char destination[IP_SIZE_MAX];
        char netmask[IP_SIZE_MAX];
        char gateway[IP_SIZE_MAX];

        for (i = 0; i < (int) pIpForwardTable->dwNumEntries; i++)
        {
          IpAddr_dst.S_un.S_addr = (u_long) pIpForwardTable->table[i].dwForwardDest;
          snprintf(destination,IP_SIZE_MAX,"%s",inet_ntoa(IpAddr_dst));

          IpAddr_msk.S_un.S_addr = (u_long) pIpForwardTable->table[i].dwForwardMask;
          snprintf(netmask,IP_SIZE_MAX,"%s",inet_ntoa(IpAddr_msk));

          IpAddr_hop.S_un.S_addr = (u_long) pIpForwardTable->table[i].dwForwardNextHop;
          snprintf(gateway,IP_SIZE_MAX,"%s",inet_ntoa(IpAddr_hop));

          addRoutetoDB(destination,
                       netmask,
                       gateway,
                       pIpForwardTable->table[i].dwForwardMetric1,session_id,db);
        }
      }
      HeapFree(GetProcessHeap(), 0,pIpForwardTable);
    }
  }
  //free
  FreeLibrary(hDLL);

  AddDebugMessage("test_route", "Scan route  - DONE", "OK", MSG_INFO);
  check_treeview(GetDlgItem(h_conf,TRV_TEST), H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  ReleaseSemaphore(hsemaphore,1,NULL);
  return 0;
}

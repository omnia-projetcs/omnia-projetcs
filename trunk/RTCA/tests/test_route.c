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
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_route(LPVOID lParam)
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

  //load route table);
  HANDLE hDLL = LoadLibrary( "IPHLPAPI.DLL" );
  if (!hDLL) return 0;

  //declaration load function
  typedef DWORD (WINAPI *GETIPFORWARDTABLE)(PMIB_IPFORWARDTABLE pIpForwardTable, PULONG pdwSize, BOOL bOrder);
  GETIPFORWARDTABLE GetIpForwardTable = (GETIPFORWARDTABLE) GetProcAddress(hDLL,"GetIpForwardTable");
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
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

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

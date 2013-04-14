//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addProcesstoDB(char *process, char *pid, char *path, char *cmd,
                    char *owner, char *rid, char *sid, char *start_date,
                    char *protocol, char *ip_src, char *port_src,
                    char *ip_dst, char *port_dst, char *state,
                    char *hidden,char *parent_process, char *parent_pid, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_process (process,pid,path,cmd,owner,"
           "rid,sid,start_date,protocol,ip_src,port_src,ip_dst,port_dst,state,hidden,parent_process,parent_pid,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           process,pid,path,cmd,owner,rid,sid,start_date,protocol,ip_src,port_src,ip_dst,port_dst,state,hidden,parent_process,parent_pid,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
BOOL GetProcessArg(HANDLE hProcess, char* arg, unsigned int size)
{
  arg[0]=0;
  BOOL ret = FALSE;

  typedef struct _PEB
  {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[229];
    PVOID Reserved3[59];
    ULONG SessionId;
  } *PPEB;

  typedef struct _PROCESS_BASIC_INFORMATION
  {
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
  }PROCESS_BASIC_INFORMATION;

  typedef enum _PROCESSINFOCLASS
  {
    ProcessBasicInformation
  }PROCESSINFOCLASS;

  typedef LONG (WINAPI LPNTQUERYINFOPROCESS)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

  typedef struct ___PEB
  {
    DWORD dwFiller[4];
    DWORD dwInfoBlockAddress;
  }__PEB;

  typedef struct ___INFOBLOCK
  {
    DWORD dwFiller[16];
    WORD wLength;
    WORD wMaxLength;
    DWORD dwCmdLineAddress;
  }__INFOBLOCK;


  PROCESS_BASIC_INFORMATION pbi;
  LPNTQUERYINFOPROCESS *lpfnNtQueryInformationProcess;
  __PEB PEB;
  __INFOBLOCK Block;

  //load de la ddl
  HMODULE hDLL = GetModuleHandle("ntdll.dll");
  if (hDLL == NULL)return ret;

    lpfnNtQueryInformationProcess = (LPNTQUERYINFOPROCESS *)GetProcAddress(hDLL, "NtQueryInformationProcess");
    if (lpfnNtQueryInformationProcess == NULL)lpfnNtQueryInformationProcess = (LPNTQUERYINFOPROCESS *)GetProcAddress(hDLL, "ZwQueryInformationProcess");

    if (lpfnNtQueryInformationProcess != NULL)
    {
      DWORD dwSize=0;
      if(lpfnNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize) == 0)
      {
        //lecture de la mémoire
        if (ReadProcessMemory(hProcess, pbi.PebBaseAddress, &PEB, sizeof(PEB), &dwSize) != 0)
        {
          if (ReadProcessMemory(hProcess, (LPVOID)PEB.dwInfoBlockAddress, &Block, sizeof(Block), &dwSize) != 0)
          {
            wchar_t *cmd = NULL;
            cmd = (wchar_t*)malloc(Block.wMaxLength+1 * sizeof(wchar_t));
            if (cmd!=NULL)
            {
              if (ReadProcessMemory(hProcess, (LPVOID)Block.dwCmdLineAddress, cmd, Block.wMaxLength, &dwSize) != 0)
              {
                snprintf(arg,size,"%S",cmd);
                ret = TRUE;
              }
              free(cmd);
            }
          }
        }
      }
    }

  FreeLibrary(hDLL);
  return ret;
}
//------------------------------------------------------------------------------
void GetProcessOwner(DWORD pid, char *owner, char *rid, char *sid, DWORD size_max)
{
  //init
  HANDLE hProcess = OpenProcess(READ_CONTROL,0,pid);
  owner[0] = 0;
  rid[0] = 0;
  sid[0] = 0;

  //user
  if (hProcess!=NULL)
  {
    PSECURITY_DESCRIPTOR psDesc = (PSECURITY_DESCRIPTOR)GlobalAlloc(GMEM_FIXED,MAX_PATH);
    DWORD dwSize = 0;
    PSID psid=0;
    BOOL proprio = FALSE;
    if (psDesc != NULL)
    {
      //info de l'objet
      if (GetKernelObjectSecurity(hProcess,OWNER_SECURITY_INFORMATION,psDesc,1000,&dwSize))
      {
        //security descriptor
        if (GetSecurityDescriptorOwner(psDesc,&psid, &proprio))
        {
          SidtoUser(psid, owner, rid, sid, size_max);
        }
      }
      GlobalFree(psDesc);
    }
    CloseHandle(hProcess);
  }
}
//------------------------------------------------------------------------------
DWORD GetPortsFromPID(DWORD pid, LINE_PROC_ITEM *port_line, unsigned int nb_item_max,unsigned int taille_max_line)
{
  HMODULE hLibrary = LoadLibrary( "IPHLPAPI.DLL" );
  if (hLibrary == NULL)return 0;

  DWORD nb_item = 0;

  //load function
  typedef enum  {
    TCP_TABLE_BASIC_LISTENER,
    TCP_TABLE_BASIC_CONNECTIONS,
    TCP_TABLE_BASIC_ALL,
    TCP_TABLE_OWNER_PID_LISTENER,
    TCP_TABLE_OWNER_PID_CONNECTIONS,
    TCP_TABLE_OWNER_PID_ALL,
    TCP_TABLE_OWNER_MODULE_LISTENER,
    TCP_TABLE_OWNER_MODULE_CONNECTIONS,
    TCP_TABLE_OWNER_MODULE_ALL
  }TCP_TABLE_CLASS, *PTCP_TABLE_CLASS;

  typedef enum  {
    UDP_TABLE_BASIC,
    UDP_TABLE_OWNER_PID,
    UDP_TABLE_OWNER_MODULE
  }UDP_TABLE_CLASS, *PUDP_TABLE_CLASS;

  typedef DWORD (WINAPI TypeGetExtendedTcpTable)(PVOID, PDWORD, BOOL, ULONG, TCP_TABLE_CLASS, ULONG);
  TypeGetExtendedTcpTable *MyGetExtendedTcpTable = (TypeGetExtendedTcpTable *)GetProcAddress(hLibrary, "GetExtendedTcpTable");

  typedef DWORD (WINAPI TypeGetExtendedUdpTable)(PVOID, PDWORD, BOOL, ULONG, UDP_TABLE_CLASS, ULONG);
  TypeGetExtendedUdpTable *MyGetExtendedUdpTable = (TypeGetExtendedUdpTable *)GetProcAddress(hLibrary, "GetExtendedUdpTable");

  if (MyGetExtendedTcpTable ==NULL || MyGetExtendedUdpTable ==NULL)
  {
    FreeLibrary(hLibrary);
    return 0;
  }

  //TCP
  PVOID pTCPTable = NULL;
  DWORD i, size = 0;
  if (MyGetExtendedTcpTable(pTCPTable, &size, TRUE, AF_INET, TCP_TABLE_OWNER_MODULE_ALL, 0) == ERROR_INSUFFICIENT_BUFFER)
  {
    //struct
    #define TCPIP_OWNING_MODULE_SIZE 16
    typedef struct _MIB_TCPROW_OWNER_MODULE
    {
      DWORD         dwState;
      DWORD         dwLocalAddr;
      DWORD         dwLocalPort;
      DWORD         dwRemoteAddr;
      DWORD         dwRemotePort;
      DWORD         dwOwningPid;
      LARGE_INTEGER liCreateTimestamp;
      ULONGLONG     OwningModuleInfo[TCPIP_OWNING_MODULE_SIZE];
    }MIB_TCPROW_OWNER_MODULE, *PMIB_TCPROW_OWNER_MODULE;

    typedef struct {
    DWORD                   dwNumEntries;
    MIB_TCPROW_OWNER_MODULE table[ANY_SIZE];
    }MIB_TCPTABLE_OWNER_MODULE, *PMIB_TCPTABLE_OWNER_MODULE;

    pTCPTable = malloc(size+1);
    if (pTCPTable != NULL)
    {
      if (MyGetExtendedTcpTable(pTCPTable, &size, TRUE, AF_INET, TCP_TABLE_OWNER_MODULE_ALL, 0) == NO_ERROR)
      {
        for(i=0; i<((PMIB_TCPTABLE_OWNER_MODULE)pTCPTable)->dwNumEntries && nb_item<nb_item_max;i++)
        {
          MIB_TCPROW_OWNER_MODULE module = ((PMIB_TCPTABLE_OWNER_MODULE)pTCPTable)->table[i];
          if (pid == module.dwOwningPid)
          {
            strncpy(port_line[nb_item].protocol,"TCP",taille_max_line);
            snprintf(port_line[nb_item].IP_src,taille_max_line,"%s",inet_ntoa(*(struct in_addr *)&module.dwLocalAddr));
            snprintf(port_line[nb_item].IP_dst,taille_max_line,"%s",inet_ntoa(*(struct in_addr *)&module.dwRemoteAddr));
            snprintf(port_line[nb_item].Port_src,taille_max_line,"%d",htons((u_short) module.dwLocalPort));
            snprintf(port_line[nb_item].Port_dst,taille_max_line,"%d",htons((u_short) module.dwRemotePort));

            switch(module.dwState)
            {
              case 1:strncpy(   port_line[nb_item].state,"CLOSED"       ,taille_max_line);break;
              case 2:strncpy(   port_line[nb_item].state,"LISTENING"    ,taille_max_line);break;
              case 3:strncpy(   port_line[nb_item].state,"SYN_SENT"     ,taille_max_line);break;
              case 4:strncpy(   port_line[nb_item].state,"SYN_RECIVED"  ,taille_max_line);break;
              case 5:strncpy(   port_line[nb_item].state,"ESTABLISHED"  ,taille_max_line);break;
              case 6:
              case 7:strncpy(   port_line[nb_item].state,"FIN_WAIT"     ,taille_max_line);break;
              case 8:strncpy(   port_line[nb_item].state,"CLOSE_WAIT"   ,taille_max_line);break;
              case 9:strncpy(   port_line[nb_item].state,"CLOSING"      ,taille_max_line);break;
              case 10:strncpy(  port_line[nb_item].state,"LAST_ACK"     ,taille_max_line);break;
              case 11:strncpy(  port_line[nb_item].state,"TIME_WAIT"    ,taille_max_line);break;
              case 12:strncpy(  port_line[nb_item].state,"DELETE_TCB"   ,taille_max_line);break;
              default:snprintf( port_line[nb_item].state,taille_max_line,"UNKNOW (%lu)",module.dwState);break;
            }
            nb_item++;
          }
        }
      }
      free(pTCPTable);
    }
  }

  //UDP
  PVOID pUDPTable = NULL;
  size = 0;
  if (MyGetExtendedUdpTable(pUDPTable, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0) == ERROR_INSUFFICIENT_BUFFER)
  {
    typedef struct _MIB_UDPROW_EX {
       DWORD dwLocalAddr;
       DWORD dwLocalPort;
       DWORD dwProcessId;
    }MIB_UDPROW_EX, *PMIB_UDPROW_EX;

    typedef struct _MIB_UDPTABLE_EX
    {
       DWORD dwNumEntries;
       MIB_UDPROW_EX table[1];
    }MIB_UDPTABLE_EX, *PMIB_UDPTABLE_EX;

    pUDPTable = malloc(size+1);
    if(pUDPTable != NULL)
    {
      if (MyGetExtendedUdpTable(pUDPTable, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0) == NO_ERROR)
      {
        PMIB_UDPTABLE_EX UDP_table = (PMIB_UDPTABLE_EX)pUDPTable;
        for (i=0; i<UDP_table->dwNumEntries && nb_item<nb_item_max;i++)
        {
          if (pid == UDP_table->table[i].dwProcessId)
          {
            strncpy(port_line[nb_item].protocol,"UDP",taille_max_line);
            snprintf(port_line[nb_item].IP_src,taille_max_line,"%s",inet_ntoa(*(struct in_addr *)&UDP_table->table[i].dwLocalAddr));
            strncpy(port_line[nb_item].IP_dst,"*.*",taille_max_line);
            snprintf(port_line[nb_item].Port_src,taille_max_line,"%d",htons((u_short) UDP_table->table[i].dwLocalPort));
            port_line[nb_item].Port_dst[0]=0;
            strncpy(port_line[nb_item].state,"LISTENING",taille_max_line);
            nb_item++;
          }
        }
      }
      free(pUDPTable);
    }
  }
  FreeLibrary(hLibrary);
  return nb_item;
}
//------------------------------------------------------------------------------
void EnumProcessAndThread(DWORD nb_process, PROCESS_INFOS_ARGS *process_info,unsigned int session_id,sqlite3 *db)
{
  HANDLE hProcess;
  DWORD d_pid, i, j, k, cbNeeded;
  BOOL ok;
  LINE_PROC_ITEM port_line[MAX_PATH];
  HMODULE hMod[MAX_PATH];
  FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime;
  char process[DEFAULT_TMP_SIZE],
       pid[DEFAULT_TMP_SIZE],
       path[MAX_PATH],
       cmd[MAX_PATH],
       owner[DEFAULT_TMP_SIZE],
       rid[DEFAULT_TMP_SIZE],
       sid[DEFAULT_TMP_SIZE],
       start_date[DATE_SIZE_MAX]/*,
       parent_pid[DEFAULT_TMP_SIZE],
       parent_path[MAX_PATH]*/;

  //force enumerate all process by id !
  for (d_pid=FIRST_PROCESS_ID;d_pid<LAST_PROCESS_ID && start_scan;d_pid++)
  {
    //open process info
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,d_pid);
    if (hProcess == NULL)continue;

    //cmd
    cmd[0]=0;
    GetProcessArg(hProcess, cmd, MAX_PATH);

    if (cmd[0]==0)
    {
      ok = FALSE;
    }else
    {
      ok = TRUE;
      //verify if exist or not
      for (i=0;i<nb_process;i++)
      {
        if (process_info[i].pid == d_pid){ok = FALSE;break;}
        else if (strcmp(cmd,process_info[i].args) == 0){ok = FALSE;break;}
      }
    }

    if (ok)
    {
      //process
      process[0] = 0;
      GetModuleBaseName(hProcess,NULL,process,DEFAULT_TMP_SIZE);

      //pid
      snprintf(pid,DEFAULT_TMP_SIZE,"%05lu",d_pid);

      //path
      path[0]=0;
      if (EnumProcessModules(hProcess,hMod, MAX_PATH,&cbNeeded))
      {
        if (GetModuleFileNameEx(hProcess,hMod[0],path,MAX_PATH) == 0)path[0] = 0;
      }

      //owner
      GetProcessOwner(d_pid, owner, rid, sid, DEFAULT_TMP_SIZE);

      //start date process
      start_date[0] = 0;
      if (GetProcessTimes(hProcess, &lpCreationTime,&lpExitTime, &lpKernelTime, &lpUserTime))
      {
        //traitement de la date
        if (lpCreationTime.dwHighDateTime != 0 && lpCreationTime.dwLowDateTime != 0)
        {
          filetimeToString_GMT(lpCreationTime, start_date, DATE_SIZE_MAX);
        }
      }

      //ports !
      j=GetPortsFromPID(d_pid, port_line, MAX_PATH, SIZE_ITEMS_PORT_MAX);

      convertStringToSQL(path, MAX_PATH);
      convertStringToSQL(cmd, MAX_PATH);

      //add items !
      if (j == 0)addProcesstoDB(process, pid, path, cmd, owner, rid, sid, start_date,"", "", "","", "", "", "X", "", "",session_id,db);
      else
      {
        for (k=0;k<j;k++)
        {
          addProcesstoDB(process, pid, path, cmd, owner, rid, sid, start_date,
                                port_line[k].protocol, port_line[k].IP_src, port_line[k].Port_src,
                                port_line[k].IP_dst, port_line[k].Port_dst, port_line[k].state, "X", "", "",session_id,db);
        }
      }
    }
    CloseHandle(hProcess);
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_process(LPVOID lParam)
{
  //check if local or not :)
  if (!LOCAL_SCAN || WINE_OS)
  {
    h_thread_test[(unsigned int)lParam] = 0;
    check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
    return 0;
  }

  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
  HANDLE hCT = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS|TH32CS_SNAPTHREAD, 0);
  if (hCT==INVALID_HANDLE_VALUE)return 0;

  DWORD cbNeeded, k, j, nb_process=0;
  HANDLE hProcess, parent_hProcess;
  HMODULE hMod[MAX_PATH];
  FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime;
  LINE_PROC_ITEM port_line[MAX_PATH];
  char process[DEFAULT_TMP_SIZE],
       pid[DEFAULT_TMP_SIZE],
       path[MAX_PATH],
       cmd[MAX_PATH],
       owner[DEFAULT_TMP_SIZE],
       rid[DEFAULT_TMP_SIZE],
       sid[DEFAULT_TMP_SIZE],
       start_date[DATE_SIZE_MAX],
       parent_pid[DEFAULT_TMP_SIZE],
       parent_path[MAX_PATH];

  PROCESS_INFOS_ARGS process_infos[MAX_PATH];

  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  while(Process32Next(hCT, &pe) && start_scan)
  {
    //open process info
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,pe.th32ProcessID);
    if (hProcess == NULL)continue;

    //process
    process[0] = 0;
    strncpy(process,pe.szExeFile,DEFAULT_TMP_SIZE);

    //pid
    snprintf(pid,DEFAULT_TMP_SIZE,"%05lu",pe.th32ProcessID);

    //path
    path[0]=0;
    if (EnumProcessModules(hProcess,hMod, MAX_PATH,&cbNeeded))
    {
      if (GetModuleFileNameEx(hProcess,hMod[0],path,MAX_PATH) == 0)path[0] = 0;
    }

    //cmd
    cmd[0]=0;
    GetProcessArg(hProcess, cmd, MAX_PATH);

    //owner
    GetProcessOwner(pe.th32ProcessID, owner, rid, sid, DEFAULT_TMP_SIZE);

    //parent processID
    snprintf(parent_pid,DEFAULT_TMP_SIZE,"%05lu",pe.th32ParentProcessID);

    //parent name
    parent_path[0]=0;
    parent_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,pe.th32ParentProcessID);
    if (parent_hProcess != NULL)
    {
      if (EnumProcessModules(parent_hProcess,hMod, MAX_PATH,&cbNeeded))
      {
        if (GetModuleFileNameEx(parent_hProcess,hMod[0],parent_path,MAX_PATH) == 0)parent_path[0] = 0;
      }
      CloseHandle(parent_hProcess);
    }

    //start date process
    start_date[0] = 0;
    if (GetProcessTimes(hProcess, &lpCreationTime,&lpExitTime, &lpKernelTime, &lpUserTime))
    {
      //traitement de la date
      if (lpCreationTime.dwHighDateTime != 0 && lpCreationTime.dwLowDateTime != 0)
      {
       filetimeToString_GMT(lpCreationTime, start_date, DATE_SIZE_MAX);
      }
    }

    //ports !
    j=GetPortsFromPID(pe.th32ProcessID, port_line, MAX_PATH, SIZE_ITEMS_PORT_MAX);

    //update list of process
    if (nb_process<MAX_PATH)
    {
      process_infos[nb_process].pid = pe.th32ProcessID;
      snprintf(process_infos[nb_process].args,MAX_PATH,"%s",cmd);
      nb_process++;
    }
    convertStringToSQL(path, MAX_PATH);
    convertStringToSQL(cmd, MAX_PATH);

    //add items !
    if (j == 0)addProcesstoDB(process, pid, path, cmd, owner, rid, sid, start_date,"", "", "","", "", "",""  , parent_path, parent_pid,session_id,db);
    else
    {
      for (k=0;k<j;k++)
      {
        addProcesstoDB(process, pid, path, cmd, owner, rid, sid, start_date,
                              port_line[k].protocol, port_line[k].IP_src, port_line[k].Port_src,
                              port_line[k].IP_dst, port_line[k].Port_dst, port_line[k].state,"" , parent_path, parent_pid,session_id,db);
      }
    }
    CloseHandle(hProcess);
  }

  //verify shadow process !!!
  EnumProcessAndThread(nb_process, process_infos,session_id,db);

  CloseHandle(hCT);
  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

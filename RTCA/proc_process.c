//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http:\\omni.a.free.fr
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
void ReadProcessInfo(DWORD pid, HANDLE hlv)
{
  LINE_ITEM lv_line[4];
  HMODULE hMod[MAX_LINE_SIZE];
  DWORD cbNeeded = 0,j;

  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0, pid);
  if (hProcess != NULL)
  {
    //chargement de la liste des dll du processus
    if (EnumProcessModules(hProcess, hMod, MAX_LINE_SIZE,&cbNeeded))
    {
      strcpy(lv_line[0].c,"DLL dependency");
      lv_line[2].c[0]=0;
      lv_line[3].c[0]=0;

      for ( j = 1; j < (cbNeeded / sizeof(HMODULE)); j++)
      {
          //emplacement de la dll
          lv_line[1].c[0]=0;
          if (GetModuleFileNameEx(hProcess,hMod[j],lv_line[1].c,MAX_LINE_SIZE)>0)
          {
            AddToLV(hlv, lv_line, 4);
          }
      }
    }
    CloseHandle(hProcess);
  }




/*
   //test autre méthode d'affichage
    DWORD aProcesses[TAILLE_TMP_DB], cbNeeded, cProcesses;
    unsigned int i,j;
    char NomProcessus[MAX_PATH];
    char Emplacement[MAX_PATH];
    HANDLE hProcess;
    HMODULE hMod[TAILLE_TMP_DB];
    char tmp[TAILLE_TMP_DB];
    char TNom[TAILLE_TMP];
    char TEmplacement[TAILLE_TMP_DB];
    char TPID[16];
    int taille;
    unsigned short Err=0;

    //implémentation de chargement des fonctions
    HMODULE hDLL;
    typedef int (WINAPI *ENUMPROC)(DWORD*,DWORD,DWORD*);
    ENUMPROC EnumProcesses;
    typedef int (WINAPI *ENUMPROCMOD)(HANDLE,HMODULE*,DWORD,LPDWORD);
    ENUMPROCMOD EnumProcessModules;
    typedef DWORD (WINAPI *GETMODBN)(HANDLE,HMODULE,LPTSTR,DWORD);
    GETMODBN GetModuleBaseName;
    typedef DWORD (WINAPI *GETMODFNEX)(HANDLE,HMODULE,LPTSTR,DWORD);
    GETMODFNEX GetModuleFileNameEx;

    XML_Debut_Rubrique(xml,"Processus en cours (API)",rubrique);


    //chargement de la dll
    if ((hDLL = LoadLibrary( "PSAPI.DLL" ))!=NULL)
    {
        //chargement des fonctions (attention utilisation d eleurs noms ANSI !!!)
        EnumProcesses = (ENUMPROC) GetProcAddress(hDLL,"EnumProcesses");
        EnumProcessModules = (ENUMPROCMOD) GetProcAddress(hDLL,"EnumProcessModules");
        GetModuleBaseName = (GETMODBN) GetProcAddress(hDLL,"GetModuleBaseNameA");
        GetModuleFileNameEx = (GETMODFNEX) GetProcAddress(hDLL,"GetModuleFileNameExA");

        if((EnumProcesses)||(EnumProcessModules)||(GetModuleBaseName)||(GetModuleFileNameEx) )
        {
            //recupération de la liste des pid
            if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cProcesses ))
              Err=1;
            else
            {
                // calcul du nombre de processus retournés.
                // récupère le nom + emplacement + pid de chaque processus.
                for ( i = 0; i < cProcesses; i++ )
                {
                    // récupère le handle du processus.
                    if ((hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0, aProcesses[i] ))!=NULL)
                    {
                        if ( EnumProcessModules( hProcess, hMod, sizeof(hMod),&cbNeeded) )
                        {
                            XML_Donnee(xml,T_TBL,TNom,"",TEmplacement,"",TPID,"","DLL liées au processus:","","","","");
                            // permet la récupération des dll liés aux processus
                            for ( j = 1; j < (cbNeeded / sizeof(HMODULE)); j++)
                            {
                                //emplacement de la dll
                                if (GetModuleFileNameEx(hProcess,hMod[j],Emplacement,sizeof(Emplacement))>0)
                                {
                                  //transformation du résultat en chaine
                                  //et implémentation pour mise en page
                                  snprintf(TEmplacement,TAILLE_TMP_DB,"Emplacement: %s",Emplacement);
                                  XML_Donnee(xml,T_LST,"","",TEmplacement,"","","","","","","","");
                                }else
                                  XML_Donnee(xml,T_LST,"","","Erreur de chargement!!!",T_R,"","","","","","","");
                            }
                        }
                    }
                    CloseHandle( hProcess );
                }
            }

*/



}
//------------------------------------------------------------------------------
DWORD GetPortsFromPID(DWORD pid, LINE_ITEM *port_line, unsigned int nb_item_max,unsigned int taille_max_line)
{
  //chargement en live des fonctions
  //évite un dépendance DLL
  //load de la ddl
  HMODULE hLibrary = LoadLibrary( "IPHLPAPI.DLL" );
  if (hLibrary == NULL)return 0;

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
  } TCP_TABLE_CLASS, *PTCP_TABLE_CLASS;

  typedef enum  {
    UDP_TABLE_BASIC,
    UDP_TABLE_OWNER_PID,
    UDP_TABLE_OWNER_MODULE
  } UDP_TABLE_CLASS, *PUDP_TABLE_CLASS;

  typedef DWORD (WINAPI TypeGetExtendedTcpTable)(PVOID, PDWORD, BOOL, ULONG, TCP_TABLE_CLASS, ULONG);
  typedef DWORD (WINAPI TypeGetExtendedUdpTable)(PVOID, PDWORD, BOOL, ULONG, UDP_TABLE_CLASS, ULONG);

  //chargement des fonctions
  TypeGetExtendedTcpTable *MyGetExtendedTcpTable;
  TypeGetExtendedUdpTable *MyGetExtendedUdpTable;

  MyGetExtendedTcpTable = (TypeGetExtendedTcpTable *)GetProcAddress(hLibrary, "GetExtendedTcpTable");
  MyGetExtendedUdpTable = (TypeGetExtendedUdpTable *)GetProcAddress(hLibrary, "GetExtendedUdpTable");

  DWORD nb_ligne=0;
  DWORD ret = 0, size = 0, i;
  if (MyGetExtendedTcpTable != NULL)
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
    } MIB_TCPROW_OWNER_MODULE, *PMIB_TCPROW_OWNER_MODULE;


    typedef struct {
    DWORD                   dwNumEntries;
    MIB_TCPROW_OWNER_MODULE table[ANY_SIZE];
    } MIB_TCPTABLE_OWNER_MODULE, *PMIB_TCPTABLE_OWNER_MODULE;

    //récupération de la taille du buffer
    PVOID pTCPTable = NULL;
    ret = MyGetExtendedTcpTable(pTCPTable, &size, TRUE, AF_INET, TCP_TABLE_OWNER_MODULE_ALL, 0);
    if (ret == ERROR_INSUFFICIENT_BUFFER)
    {
      //allocation de la mémoire
      pTCPTable = malloc(size+1);
      if(pTCPTable != NULL)
      {
        if (MyGetExtendedTcpTable(pTCPTable, &size, TRUE, AF_INET, TCP_TABLE_OWNER_MODULE_ALL, 0) == NO_ERROR )
        {
          char tmp[MAX_PATH];
          for(i=0; i<((PMIB_TCPTABLE_OWNER_MODULE)pTCPTable)->dwNumEntries && nb_ligne<nb_item_max;i++)
          {
            MIB_TCPROW_OWNER_MODULE module = ((PMIB_TCPTABLE_OWNER_MODULE)pTCPTable)->table[i];

            if (pid == module.dwOwningPid)
            {
              snprintf(tmp,MAX_PATH,"TCP - %s:%d -> %s:%d "
                     ,inet_ntoa(*(struct in_addr *)(char *)&module.dwLocalAddr)
                     ,htons((u_short) module.dwLocalPort)
                     ,inet_ntoa(*(struct in_addr *)(char *)&module.dwRemoteAddr),htons((u_short) module.dwRemotePort));
              //traitement de l'état
              switch(module.dwState)
              {
                case 1:strncat(tmp ," (CLOSED)\0",MAX_PATH);break;
                case 2:strncat(tmp ," (LISTENING)\0",MAX_PATH);break;
                case 3:strncat(tmp ," (SYN_SENT)\0",MAX_PATH);break;
                case 4:strncat(tmp ," (SYN_RECIVED)\0",MAX_PATH);break;
                case 5:strncat(tmp ," (ESTABLISHED)\0",MAX_PATH);break;
                case 6:
                case 7:strncat(tmp ," (FIN_WAIT)\0",MAX_PATH);break;
                case 8:strncat(tmp ," (CLOSE_WAIT)\0",MAX_PATH);break;
                case 9:strncat(tmp ," (CLOSING)\0",MAX_PATH);break;
                case 10:strncat(tmp," (LAST_ACK)\0",MAX_PATH);break;
                case 11:strncat(tmp," (TIME_WAIT)\0",MAX_PATH);break;
                case 12:strncat(tmp," (DELETE_TCB)\0",MAX_PATH);break;
                default:strncat(tmp," (UNKNOW STATE)\0",MAX_PATH);break;
              }
              strncpy(port_line[nb_ligne++].c,tmp,taille_max_line);
            }
          }
        }
        free(pTCPTable);
      }
    }
  }

  //--------------------UDP
  if (MyGetExtendedUdpTable != NULL)
  {
    typedef struct _MIB_UDPROW_EX {
       DWORD dwLocalAddr;
       DWORD dwLocalPort;
       DWORD dwProcessId;
    } MIB_UDPROW_EX, *PMIB_UDPROW_EX;

    typedef struct _MIB_UDPTABLE_EX
    {
       DWORD dwNumEntries;
       MIB_UDPROW_EX table[1];
    } MIB_UDPTABLE_EX, *PMIB_UDPTABLE_EX;

    PVOID pUDPTable = NULL;
    size = 0;
    ret = MyGetExtendedUdpTable(pUDPTable, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0);
    if (ret == ERROR_INSUFFICIENT_BUFFER)
    {
      //allocation de la mémoire
      pUDPTable = malloc(size+1);
      if(pUDPTable != NULL)
      {
        if (MyGetExtendedUdpTable(pUDPTable, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0) == NO_ERROR )
        {
          char tmp[MAX_PATH];
          PMIB_UDPTABLE_EX UDP_table = (PMIB_UDPTABLE_EX)pUDPTable;
          for (i=0; i<UDP_table->dwNumEntries;i++)
          {
            if (pid == UDP_table->table[i].dwProcessId)
            {
              snprintf(tmp,MAX_PATH,"UDP - %s:%d -> *:*  (LISTENING)"
                     ,inet_ntoa(*(struct in_addr *)(char *)&UDP_table->table[i].dwLocalAddr)
                     ,htons((u_short) UDP_table->table[i].dwLocalPort));
              strncpy(port_line[nb_ligne++].c,tmp,taille_max_line);
            }
          }
        }
        free(pUDPTable);
      }
    }
  }


  FreeLibrary(hLibrary);
  return nb_ligne;
}
//------------------------------------------------------------------------------
void EnableTokenPrivilege()
{
	HANDLE ht	= 0;
	TOKEN_PRIVILEGES tkp = {0};

	//token du process
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &ht) != 0)
  {
    //add privilège
    if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid))
    {
      tkp.PrivilegeCount = 1;
      tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      AdjustTokenPrivileges(ht, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);
    }
  }
}
//------------------------------------------------------------------------------
void GetProcessArg(HANDLE hProcess, char* arg, unsigned int size)
{
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
  HMODULE hLibrary = GetModuleHandle("ntdll.dll");
  if (hLibrary != NULL)
  {
    lpfnNtQueryInformationProcess = (LPNTQUERYINFOPROCESS *)GetProcAddress(hLibrary, "ZwQueryInformationProcess");
    if (lpfnNtQueryInformationProcess != NULL)
    {
      DWORD dwSize;
      lpfnNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);

      //lecture de la mémoire
      if (ReadProcessMemory(hProcess, pbi.PebBaseAddress, &PEB, sizeof(PEB), &dwSize) != 0)
      {
        if (ReadProcessMemory(hProcess, (LPVOID)PEB.dwInfoBlockAddress, &Block, sizeof(Block), &dwSize) != 0)
        {
          char *cmd = NULL;
          cmd = (char*)malloc(Block.wMaxLength+1);
          if (cmd!=NULL)
          {
            if (ReadProcessMemory(hProcess, (LPVOID)Block.dwCmdLineAddress, cmd, Block.wMaxLength, &dwSize) != 0)
            {
              snprintf(arg,size,"%S",cmd);
            }
            free(cmd);
          }
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
void EnumProcess(HANDLE hlv, unsigned short nb_colonne)
{
  ListView_DeleteAllItems(hlv);
  //liste des processus
  DWORD j, k, cbNeeded;

  //enable privilège pour accéder aus process système
  EnableTokenPrivilege();

  PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
  HANDLE hCT = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hCT!=INVALID_HANDLE_VALUE)
  {
    if(Process32First(hCT, &pe))
    {
      HANDLE hProcess,hProcess2;
      HMODULE hMod[MAX_PATH];
      FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime ,LocalFileTime;
      SYSTEMTIME SysTime;

      LINE_ITEM lv_line[SIZE_UTIL_ITEM];
      LINE_ITEM port_line[MAX_PATH];

      char tmp[MAX_PATH];
      do
      {
        // récupère le handle du processus.
        if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,pe.th32ProcessID))!=NULL)
        {
          //PID
          sprintf(lv_line[0].c,"%04lu",pe.th32ProcessID);
          lv_line[1].c[0] = 0;
          lv_line[2].c[0] = 0;
          lv_line[3].c[0] = 0;
          lv_line[4].c[0] = 0;
          lv_line[5].c[0] = 0;
          lv_line[6].c[0] = 0;

          //name
          snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s",pe.szExeFile);

          //description
          if (EnumProcessModules(hProcess,hMod, MAX_PATH,&cbNeeded) )
          {
            //Path
            GetModuleFileNameEx(hProcess,hMod[0],lv_line[2].c,MAX_LINE_SIZE);
          }

          //lecture de la ligne de commande
          GetProcessArg(hProcess, lv_line[3].c, MAX_LINE_SIZE);

          //user
          if ((hProcess2 = OpenProcess(READ_CONTROL,0,pe.th32ProcessID))!=NULL)
          {
            PSECURITY_DESCRIPTOR psDesc = (PSECURITY_DESCRIPTOR)GlobalAlloc(GMEM_FIXED,MAX_PATH);
            DWORD dwSize = 0;
            SID_NAME_USE f_sid;
            PSID psid=0;
            BOOL proprio = FALSE;
            char c_name[TAILLE_TMP];
            DWORD d_name = TAILLE_TMP;
            char c_domaine[TAILLE_TMP];
            DWORD d_domaine = TAILLE_TMP;
            if (psDesc != NULL)
            {
              //info de l'objet
              if (GetKernelObjectSecurity(hProcess2,OWNER_SECURITY_INFORMATION,psDesc,1000,&dwSize))
              {
                //security descriptor
                if (GetSecurityDescriptorOwner(psDesc,&psid, &proprio))
                {
                  //SID to user + domaine
                  c_name[0]=0;
                  c_domaine[0]=0;
                  if(LookupAccountSid(NULL,psid,c_name,&d_name,c_domaine,&d_domaine,&f_sid))
                  {
                    snprintf(lv_line[4].c,MAX_LINE_SIZE,"%s\\%s",c_domaine,c_name);
                  }

                  //ajout du SID user :
                  PSID_IDENTIFIER_AUTHORITY t = GetSidIdentifierAuthority(psid);
                  snprintf(tmp,MAX_PATH," SID : S-%d-%u",((SID*)psid)->Revision,t->Value[5]+ (t->Value[4]<<8) + (t->Value[3]<<16) + (t->Value[2]<<24));
                  strncat(lv_line[4].c,tmp,MAX_LINE_SIZE);
                  PUCHAR pcSubAuth = GetSidSubAuthorityCount(psid);
                  unsigned char m,ucMax = *pcSubAuth;
                  DWORD *SidP;

                  for (m=0;m<ucMax;++m)
                  {
                    SidP=GetSidSubAuthority(psid,m);
                    snprintf(tmp,MAX_PATH,"-%lu",*SidP);
                    strncat(lv_line[4].c,tmp,MAX_LINE_SIZE);
                  }
                  strncat(lv_line[4].c,"\0",MAX_LINE_SIZE);
                }
              }
              GlobalFree(psDesc);
            }
            CloseHandle(hProcess2);
          }

          //date de début
          if (GetProcessTimes(hProcess, &lpCreationTime,&lpExitTime, &lpKernelTime, &lpUserTime))
          {
            //traitement de la date
            if (lpCreationTime.dwHighDateTime != 0 && lpCreationTime.dwLowDateTime != 0)
            {
              if(FileTimeToLocalFileTime(&lpCreationTime, &LocalFileTime))
              {
                if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
                  snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              }
            }
          }

          //port réseau ouvert
          j=GetPortsFromPID(pe.th32ProcessID, port_line, MAX_PATH, MAX_LINE_SIZE);
          if (j == 0)AddToLV(hlv,lv_line, nb_colonne);
          else
          {
            for (k=0;k<j;k++)
            {
              strcpy(lv_line[6].c, port_line[k].c);
              AddToLV(hlv,lv_line, nb_colonne);
            }
          }

          CloseHandle(hProcess);
        }
      }while(Process32Next(hCT, &pe));
    }
    CloseHandle(hCT);
  }
}

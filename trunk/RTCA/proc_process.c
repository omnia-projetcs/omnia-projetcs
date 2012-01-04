//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Comparaison de deux chaines en fonction de leur ordre alphabétique
int CALLBACK CompareStringTri(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
  sort_st *st = (sort_st *)lParam3;

  static char buf1[MAX_LINE_SIZE], buf2[MAX_LINE_SIZE];

  ListView_GetItemText(st->hlv, lParam1, st->col, buf1, MAX_LINE_SIZE);
  ListView_GetItemText(st->hlv, lParam2, st->col, buf2, MAX_LINE_SIZE);

  if (st->sort) return (strcmp(buf1, buf2));
  else return (strcmp(buf1, buf2)*-1);
}
//------------------------------------------------------------------------------
//dump de la mémoire d'un processus
DWORD WINAPI DumpProcessMemory(LPVOID lParam)
{
  DWORD pid = (DWORD)lParam;

  //Récupération des privilèges Debug
//  SetDebugPrivilege(TRUE);

  //ouvrir le process
  HANDLE hProc = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, pid);//PROCESS_ALL_ACCESS
  if (hProc!=NULL)
  {
    //hardward info : last memory process
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    //64bit : GetSystemInfo(&si);

    //choix de l'emplacement de sauvegarde ^^
    char path[MAX_PATH]="";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = Tabl[TABL_MAIN];
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter ="File RAW\0*.raw\0";
    ofn.nFilterIndex = 1;
    ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt =".raw\0";

    if (GetSaveFileName(&ofn)==TRUE)
    {
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        DWORD copiee;

        //écriture en raw (région/régions)
        MEMORY_BASIC_INFORMATION    mbi;
        LPVOID mem = 0;
        LPVOID buffer;
        while (mem < si.lpMaximumApplicationAddress)
        {
          //lecture des informations de la zone mémoire !
          if (!VirtualQueryEx(hProc,mem, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))break;

          //lecture écriture des datas !
          if (mbi.RegionSize > 0)
          {
            buffer = (char*)malloc(mbi.RegionSize);
            if (buffer!=NULL)
            {
              //lecture
              if (ReadProcessMemory(hProc, mbi.BaseAddress, buffer, mbi.RegionSize, NULL))
              {
                //sauvegarde
                WriteFile(MyhFile,buffer,mbi.RegionSize,&copiee,0);
              }
              free(buffer);
            }
          }else break;

          //région suivante :
          mem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);
        }
      }
      CloseHandle(MyhFile);
    }
    CloseHandle(hProc);

    MessageBox(0,path,"Memory Dump done",MB_OK|MB_TOPMOST);
  }

  //disable right
  //SetDebugPrivilege(FALSE);
  return 0;
}
//------------------------------------------------------------------------------
//Functions for DLL injection from : http://www.cppfrance.com//code.aspx?ID=49419
#define W_MAX_PATH MAX_PATH*sizeof(WCHAR)
BOOL WINAPI DLLInjecteurW(DWORD dwPid,PWSTR szDLLPath)
{
	LPTHREAD_START_ROUTINE lpthThreadFunction; /* Pointeur de fonction. */
	/* Recherche de l'adresse de LoadLibraryW dans kernel32. */
    lpthThreadFunction = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryW"); /* GetProcAdress renvoie l'adresse de la fonction LoadLibrary. */
    if(lpthThreadFunction == NULL){return FALSE;}

	HANDLE hProcess;
	/* Récupération du handle du processus cible. */
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,dwPid);
    if(hProcess == NULL)return FALSE;

	/* Allocation mémoire dans le processus cible. */
    PWSTR szDllVariable;
    szDllVariable = (PWSTR) VirtualAllocEx(hProcess, NULL, (lstrlenW(szDLLPath)+1)*sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);
    if(szDllVariable == NULL){ CloseHandle(hProcess); return FALSE; }

	/* Ecriture de l'adresse de la dll dans la mémoire du processus cible*/
    DWORD dwBytes;
    if(!WriteProcessMemory(hProcess, szDllVariable, szDLLPath,(lstrlenW(szDLLPath)+1)*sizeof(WCHAR), &dwBytes)){ CloseHandle(hProcess); return FALSE; }
    if(dwBytes != (lstrlenW(szDLLPath)+1)*sizeof(WCHAR))
	{
		VirtualFreeEx(hProcess,szDllVariable,0,MEM_RELEASE);
		return FALSE;
	}

	/* Création du thread dans le processus cible. */
    DWORD dwThreadID = 0;
    HANDLE hThread = NULL;
    hThread = CreateRemoteThread(hProcess, NULL, 0, lpthThreadFunction,szDllVariable, 0, &dwThreadID);
    if(hThread == NULL){
		VirtualFreeEx(hProcess,szDllVariable,0,MEM_RELEASE);
		return FALSE;
	}

    WaitForSingleObject(hThread, INFINITE); /* Attente de la fin du thread. */

    VirtualFreeEx(hProcess,szDllVariable,0,MEM_RELEASE);
    CloseHandle(hProcess);
    CloseHandle(hThread);

	return TRUE;
}

BOOL WINAPI DLLEjecteurW(DWORD dwPid,PWSTR szDLLPath)
{
	/* Recherche de l'adresse du module dans le processus cible. */
	MODULEENTRY32W meModule;
	meModule.dwSize = sizeof(meModule);
	HANDLE hSnapshot = NULL;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if(hSnapshot == NULL)return FALSE;

	/* Parcour de la liste des modules du processus cible. */
	Module32FirstW(hSnapshot, &meModule);
	do{
		if((lstrcmpiW(meModule.szModule,szDLLPath) == 0) || (lstrcmpiW(meModule.szExePath,szDLLPath) == 0))break;
	}while(Module32NextW(hSnapshot, &meModule));

	/* Récupération du handle du processus. */
	HANDLE hProcess;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,dwPid);
    if(hProcess == NULL){
		CloseHandle(hSnapshot);
		return FALSE;
	}

    LPTHREAD_START_ROUTINE lpthThreadFunction; /* Pointeur de fonction. */
    /* Récupération de l'adresse de FreeLibrary dans kernel32.dll . */
	lpthThreadFunction = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "FreeLibrary"); /* GetProcAdress renvoie l'adresse de la fonction LoadLibrary. */
    if(lpthThreadFunction == NULL){
		CloseHandle(hProcess);
		CloseHandle(hSnapshot);
		return FALSE;
	}
    /* Remarque : ici pas besoin de crée une variable contenant le path de la dll dans le processus cible
     * étant donné que la dll a été chargée, il suffit de récupérer l'adresse de celle-ci
     * dans les modules chargés par le processus cible. (fait ci-dessus grâce à CreateToolhelp32Snapshot, ... )
     */


    /* Création du thread dans le processus cible. */
    DWORD dwThreadID = 0;
    HANDLE hThread = NULL;
    hThread = CreateRemoteThread(hProcess, NULL, 0, lpthThreadFunction,meModule.modBaseAddr, 0, &dwThreadID);
    if(hThread == NULL){
		CloseHandle(hSnapshot);
		CloseHandle(hProcess);
		return FALSE;
	}

	WaitForSingleObject(hThread,INFINITE);

	CloseHandle(hProcess);
	CloseHandle(hThread);

	return TRUE;
}

/*Privilege DEBUG. */
void SetDebugPrivilege(BOOL enable)
{
    if (enable)nb_process_SE_DEBUG++;
    else
    {
      nb_process_SE_DEBUG--;
      if (nb_process_SE_DEBUG)return;
    }

    TOKEN_PRIVILEGES privilege;
    LUID Luid;
    HANDLE handle1;
    HANDLE handle2;
    handle1 = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    OpenProcessToken(handle1,TOKEN_ALL_ACCESS, &handle2);
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid);
    privilege.PrivilegeCount = 1;
    privilege.Privileges[0].Luid = Luid;
    privilege.Privileges[0].Attributes = enable?SE_PRIVILEGE_ENABLED:0x04/*SE_PRIVILEGE_REMOVED*/;
    AdjustTokenPrivileges(handle2, FALSE, &privilege, sizeof(privilege), NULL, NULL);
    CloseHandle(handle2);
    CloseHandle(handle1);
}
BOOL WINAPI DllInjecteurA(DWORD dwPid,char * szDLLPath){

	WCHAR wszDllPath[W_MAX_PATH];

	MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)szDLLPath,(int)-1,(LPWSTR)wszDllPath,(int)W_MAX_PATH);

	return (DLLInjecteurW(dwPid,wszDllPath));
}

BOOL WINAPI DllEjecteurA(DWORD dwPid,char * szDLLPath){

	WCHAR wszDllPath[W_MAX_PATH];

	MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)szDLLPath,(int)-1,(LPWSTR)wszDllPath,(int)W_MAX_PATH);

	return (DLLEjecteurW(dwPid,wszDllPath));
}
//------------------------------------------------------------------------------
void SupSItem(HANDLE hlv,unsigned int column,char *txt)
{
  DWORD NBLigne=SendMessage(hlv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    int j;
    char tmp[MAX_LINE_SIZE]="";
    for (j=NBLigne-1;j>-1;j--)//ligne par ligne
    {
      tmp[0]=0;
      ListView_GetItemText(hlv,j,column,tmp,MAX_LINE_SIZE);
      if (!strcmp(tmp,txt))
        SendMessage(hlv,LVM_DELETEITEM,(WPARAM)j,(LPARAM)NULL);
    }
  }
}
//------------------------------------------------------------------------------
void KilllvProcess(HANDLE hlv, DWORD id, unsigned int column)
{
  //récupération de l'id
  char tmp[MAX_LINE_SIZE]="";
  ListView_GetItemText(hlv,id,column,tmp,MAX_LINE_SIZE);

  //passage en mode privilèges
  //SetDebugPrivilege(TRUE);

  //kill
  HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, atoi(tmp));
  if (hProcess != NULL)
  {
    if (TerminateProcess(hProcess, 0))SupSItem(hlv ,column, tmp);
    CloseHandle(hProcess);
  }
  //SetDebugPrivilege(FALSE);
}
//------------------------------------------------------------------------------
int __stdcall GetIconProcess(HANDLE hlv,char * szProcessPath)
{
    SHFILEINFO sfiInfo;
    HIMAGELIST hilPicture = (HIMAGELIST) SHGetFileInfo(szProcessPath,0,&sfiInfo,sizeof(SHFILEINFO),SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
    ListView_SetImageList(hlv,hilPicture,LVSIL_SMALL);
    return sfiInfo.iIcon;
}
//------------------------------------------------------------------------------
BOOL GetFileSizeI(char *file, char *resultat, unsigned int max_size)
{
  //File property
  resultat[0]=0;
  BOOL ret = FALSE;
  WIN32_FIND_DATA data;
  HANDLE hfic = FindFirstFile(file, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    //taille
    if (data.nFileSizeLow>(1024*1024*1024))snprintf(resultat,max_size,"%luGo (%luo)",data.nFileSizeLow/(1024*1024*1024),data.nFileSizeLow);
    else if (data.nFileSizeLow>(1024*1024))snprintf(resultat,max_size,"%luMo (%luo)",data.nFileSizeLow/(1024*1024),data.nFileSizeLow);
    else if (data.nFileSizeLow>(1024))snprintf(resultat,max_size,"%luKo (%luo)",data.nFileSizeLow/(1024),data.nFileSizeLow);
    else snprintf(resultat,max_size,"%luo",data.nFileSizeLow);

    ret = TRUE;
  }
  FindClose(hfic);
  return ret;
}
//------------------------------------------------------------------------------
BOOL GetAppVersion( char *file, char *version, unsigned int max_size)
{
  typedef BOOL (WINAPI *GETFILEVERSIONINFOSIZEA)(char *file, LPDWORD handle);
  typedef BOOL (WINAPI *GETFILEVERSIONINFOA)(char *file, DWORD handle, DWORD len, LPVOID data);
  typedef BOOL (WINAPI *VERQUERYVALUEA)(LPCVOID *block, LPCSTR subblock, PVOID buffer, PUINT len);

  GETFILEVERSIONINFOSIZEA GetFileVersionInfoSize;
  GETFILEVERSIONINFOA GetFileVersionInfo;
  VERQUERYVALUEA VerQueryValue;

  HMODULE hDLL = LoadLibrary( "VERSION.dll");
  if (hDLL != NULL)
  {
    GetFileVersionInfoSize = (GETFILEVERSIONINFOSIZEA) GetProcAddress(hDLL,"GetFileVersionInfoSizeA");
    GetFileVersionInfo = (GETFILEVERSIONINFOA) GetProcAddress(hDLL,"GetFileVersionInfoA");
    VerQueryValue = (VERQUERYVALUEA) GetProcAddress(hDLL,"VerQueryValueA");

    if (GetFileVersionInfoSize && GetFileVersionInfo && VerQueryValue)
    {
      DWORD dwHandle, dwLen;
      UINT BufLen;
      LPTSTR  lpData;
      VS_FIXEDFILEINFO *pFileInfo;
      dwLen = GetFileVersionInfoSize(file, &dwHandle);
      if (!dwLen) return FALSE;

      lpData = (LPTSTR) malloc (dwLen);
      if (!lpData) return FALSE;

      if(!GetFileVersionInfo(file, dwHandle, dwLen, lpData))
      {
        free (lpData);
        return FALSE;
      }
      if(VerQueryValue(lpData, "\\", (LPVOID *) &pFileInfo, (PUINT)&BufLen ) )
      {
        snprintf(version,max_size,"%d.%d.%d.%d",
                 HIWORD(pFileInfo->dwFileVersionMS),
                 LOWORD(pFileInfo->dwFileVersionMS),
                 HIWORD(pFileInfo->dwFileVersionLS),
                 LOWORD(pFileInfo->dwFileVersionLS));
        free (lpData);
        return TRUE;
      }
     free (lpData);
    }
    FreeLibrary(hDLL);
  }
 return FALSE;
}
//----------------------------------------------------------------------
void FileInfoRead(char *file, char *ProductName, char *FileVersion, char *CompanyName, char *FileDescription)
{
  ProductName[0]=0;
  FileVersion[0]=0;
  CompanyName[0]=0;
  FileDescription[0]=0;

  HINSTANCE hDLL;
  if((hDLL = LoadLibrary("VERSION.DLL" ))!= NULL)
  {
    typedef BOOL (WINAPI * GETFILEVERSIONINFO)(LPCTSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
    typedef BOOL (WINAPI * VERQUERYVALUE)(LPCVOID pBlock, LPCTSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen);

    GETFILEVERSIONINFO GetFileVersionInfo      = (GETFILEVERSIONINFO)   GetProcAddress(hDLL , "GetFileVersionInfoA");
    VERQUERYVALUE VerQueryValue                = (VERQUERYVALUE)   GetProcAddress(hDLL , "VerQueryValueA");
    if (GetFileVersionInfo && VerQueryValue)
    {
      //chargement des infos
      char buffer[MAX_LINE_SIZE];
      if (GetFileVersionInfo(file, 0, MAX_LINE_SIZE, (LPVOID) buffer) > 0 )
      {
        //lecture des infos du buffer
         WORD             *d_buffer;
         char             *c;
         unsigned int     size;

         //vérification si bien une table + lecture de la langue
         if (VerQueryValue(buffer, "\\VarFileInfo\\Translation", (LPVOID *)&d_buffer, &size))
         {
           if (size>0 && d_buffer != NULL)
           {
            //génération du début de string :
            char v_string[MAX_PATH],t_string[MAX_PATH];
            /*char s_string[][]={"CompanyName"    , "FileDescription", "FileVersion",
                                 "InternalName"   , "LegalCopyright" , "OriginalFilename",
                                 "ProductName"    , "ProductVersion"};*/
            snprintf(v_string,MAX_PATH,"\\StringFileInfo\\%04x%04x\\", d_buffer[0], d_buffer[1]);

            //lecture de ProductName
            if (ProductName != NULL)
            {
              snprintf(t_string,MAX_PATH,"%sProductName",v_string);
              if (VerQueryValue(buffer, t_string, (LPVOID *)&c, &size))
              {
                if (size>0 && c!= NULL)strcpy(ProductName,c);
              }
            }

            //lecture de FileVersion
            if (FileVersion != NULL)
            {
              snprintf(t_string,MAX_PATH,"%sFileVersion",v_string);
              if (VerQueryValue(buffer, t_string, (LPVOID *)&c, &size))
              {
                if (size>0 && c!= NULL)strcpy(FileVersion,c);
              }
            }

            //lecture du CompanyName
            if (CompanyName != NULL)
            {
              snprintf(t_string,MAX_PATH,"%sCompanyName",v_string);
              if (VerQueryValue(buffer, t_string, (LPVOID *)&c, &size))
              {
                if (size>0 && c!= NULL)strcpy(CompanyName,c);
              }
            }

            //lecture du FileDescription
            if (FileDescription != NULL)
            {
              snprintf(t_string,MAX_PATH,"%sFileDescription",v_string);
              if (VerQueryValue(buffer, t_string, (LPVOID *)&c, &size))
              {
                if (size>0 && c!= NULL)strcpy(FileDescription,c);
              }
            }
           }
         }
      }
    }
    FreeLibrary(hDLL);
  }
}
//------------------------------------------------------------------------------
void ReadProcessInfo(HANDLE hlv_src, DWORD id)
{
  LINE_ITEM lv_line[8];
  HMODULE hMod[MAX_LINE_SIZE];
  DWORD cbNeeded = 0,j;
  char tmp[MAX_PATH]="";

  //init de la listeview
  HANDLE hlv = GetDlgItem(Tabl[TABL_INFO],LV_VIEW);
  ListView_DeleteAllItems(hlv);

  //File Name
  tmp[0]=0;
  lv_line[2].c[0]=0;
  lv_line[3].c[0]=0;

  ListView_GetItemText(hlv_src,id,2,tmp,MAX_PATH);
  if (tmp[0]!=0)
  {
    strcpy(lv_line[0].c,"File : path");
    if (tmp[0]=='\\' && tmp[1]=='?' && tmp[2]=='?' && tmp[3]=='\\')
    {
      strcpy(lv_line[1].c,tmp+4);
    }else if (tmp[0]=='\\' && tmp[1]=='S' && tmp[2]=='y' && tmp[3]=='s' && tmp[4]=='t' && tmp[5]=='e' && tmp[6]=='m' && tmp[7]=='R')
    {
      //variable système
      if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion","SystemRoot",lv_line[1].c))
      {
        //ajout du path lue
        strncat(lv_line[1].c,tmp+11,MAX_LINE_SIZE);
        strncat(lv_line[1].c,"\0",MAX_LINE_SIZE);
      }else strcpy(lv_line[1].c,tmp);
    }else strcpy(lv_line[1].c,tmp);
    strcpy(tmp,lv_line[1].c);

    //size
    GetFileSizeI(lv_line[1].c, lv_line[2].c, MAX_LINE_SIZE);

    //owner
    GetACLS(lv_line[1].c, NULL, 0, lv_line[3].c, MAX_LINE_SIZE);

    FileInfoRead(lv_line[1].c, lv_line[4].c, lv_line[5].c, lv_line[6].c, lv_line[7].c);
    AddToLV(hlv, lv_line,8);
    lv_line[2].c[0]=0;
    lv_line[3].c[0]=0;
  }


  //version du fichier
  if (GetAppVersion(tmp, lv_line[1].c, MAX_LINE_SIZE))
  {
    strcpy(lv_line[0].c,"File : version");
    AddToLV(hlv, lv_line,4);
  }

  //File property
  WIN32_FIND_DATA data;
  HANDLE hfic = FindFirstFile(tmp, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    //traitement des dates
    FILETIME LocalFileTime;
    SYSTEMTIME SysTimeCreation,SysTimeModification,SysTimeAcces;

    FileTimeToLocalFileTime(&(data.ftCreationTime), &LocalFileTime);
    if (FileTimeToSystemTime(&LocalFileTime, &SysTimeCreation))
    {
      strcpy(lv_line[0].c,"File : creation date");
      snprintf(lv_line[1].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeCreation.wYear,SysTimeCreation.wMonth,SysTimeCreation.wDay,SysTimeCreation.wHour,SysTimeCreation.wMinute,SysTimeCreation.wSecond);
      AddToLV(hlv, lv_line,4);
    }

    FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
    if (FileTimeToSystemTime(&LocalFileTime, &SysTimeModification))
    {
      strcpy(lv_line[0].c,"File : last time modification");
      snprintf(lv_line[1].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond);
      AddToLV(hlv, lv_line,4);
    }

    FileTimeToLocalFileTime(&(data.ftLastAccessTime), &LocalFileTime);
    if (FileTimeToSystemTime(&LocalFileTime, &SysTimeAcces))
    {
      strcpy(lv_line[0].c,"File : last access time");
      snprintf(lv_line[1].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeAcces.wYear,SysTimeAcces.wMonth,SysTimeAcces.wDay,SysTimeAcces.wHour,SysTimeAcces.wMinute,SysTimeAcces.wSecond);
      AddToLV(hlv, lv_line,4);
    }
  }
  FindClose(hfic);

  //ACL
  lv_line[1].c[0]=0;
  GetACLS(tmp, lv_line[1].c, MAX_LINE_SIZE, NULL, 0);
  if (lv_line[1].c[0]!=0)
  {
    strcpy(lv_line[0].c,"File : ACL");
    AddToLV(hlv, lv_line,4);
  }

  //DLL dependency
  tmp[0]=0;
  ListView_GetItemText(hlv_src,id,1,tmp,MAX_PATH);

  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0, atoi(tmp));
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
            //taille
            GetFileSizeI(lv_line[1].c, lv_line[2].c, MAX_LINE_SIZE);

            //proprio
            GetACLS(lv_line[1].c, NULL, 0, lv_line[3].c, MAX_LINE_SIZE);

            //infos
            FileInfoRead(lv_line[1].c, lv_line[4].c, lv_line[5].c, lv_line[6].c, lv_line[7].c);
            AddToLV(hlv, lv_line, 8);
          }
      }
    }
    CloseHandle(hProcess);
  }
}
//------------------------------------------------------------------------------
DWORD GetPortsFromPID(DWORD pid, LINE_PROC_ITEM *port_line, unsigned int nb_item_max,unsigned int taille_max_line)
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
          for(i=0; i<((PMIB_TCPTABLE_OWNER_MODULE)pTCPTable)->dwNumEntries && nb_ligne<nb_item_max;i++)
          {
            MIB_TCPROW_OWNER_MODULE module = ((PMIB_TCPTABLE_OWNER_MODULE)pTCPTable)->table[i];

            if (pid == module.dwOwningPid)
            {
              strcpy(port_line[nb_ligne].protocol,"TCP");
              snprintf(port_line[nb_ligne].IP_src,MAX_PROC_LINE_ITEM_SIZE,"%s",inet_ntoa(*(struct in_addr *)(char *)&module.dwLocalAddr));
              snprintf(port_line[nb_ligne].IP_dst,MAX_PROC_LINE_ITEM_SIZE,"%s",inet_ntoa(*(struct in_addr *)(char *)&module.dwRemoteAddr));
              snprintf(port_line[nb_ligne].Port_src,MAX_PROC_LINE_ITEM_SIZE,"%d",htons((u_short) module.dwLocalPort));
              snprintf(port_line[nb_ligne].Port_dst,MAX_PROC_LINE_ITEM_SIZE,"%d",htons((u_short) module.dwRemotePort));

              //traitement de l'état
              switch(module.dwState)
              {
                case 1:strcpy(port_line[nb_ligne].state,"CLOSED");break;
                case 2:strcpy(port_line[nb_ligne].state,"LISTENING");break;
                case 3:strcpy(port_line[nb_ligne].state,"SYN_SENT");break;
                case 4:strcpy(port_line[nb_ligne].state,"SYN_RECIVED");break;
                case 5:strcpy(port_line[nb_ligne].state,"ESTABLISHED");break;
                case 6:
                case 7:strcpy(port_line[nb_ligne].state,"FIN_WAIT");break;
                case 8:strcpy(port_line[nb_ligne].state,"CLOSE_WAIT");break;
                case 9:strcpy(port_line[nb_ligne].state,"CLOSING");break;
                case 10:strcpy(port_line[nb_ligne].state,"LAST_ACK");break;
                case 11:strcpy(port_line[nb_ligne].state,"TIME_WAIT");break;
                case 12:strcpy(port_line[nb_ligne].state,"DELETE_TCB");break;
                default:strcpy(port_line[nb_ligne].state,"UNKNOW STATE");break;
              }
              nb_ligne++;
            }else
            {
              port_line[nb_ligne].protocol[0]=0;
              port_line[nb_ligne].IP_src[0]=0;
              port_line[nb_ligne].Port_src[0]=0;
              port_line[nb_ligne].IP_dst[0]=0;
              port_line[nb_ligne].Port_dst[0]=0;
              port_line[nb_ligne].state[0]=0;
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
          PMIB_UDPTABLE_EX UDP_table = (PMIB_UDPTABLE_EX)pUDPTable;
          for (i=0; i<UDP_table->dwNumEntries;i++)
          {
            if (pid == UDP_table->table[i].dwProcessId)
            {
              strcpy(port_line[nb_ligne].protocol,"UDP");
              snprintf(port_line[nb_ligne].IP_src,MAX_PROC_LINE_ITEM_SIZE,"%s",inet_ntoa(*(struct in_addr *)(char *)&UDP_table->table[i].dwLocalAddr));
              strcpy(port_line[nb_ligne].IP_dst,"*.*");
              snprintf(port_line[nb_ligne].Port_src,MAX_PROC_LINE_ITEM_SIZE,"%d",htons((u_short) UDP_table->table[i].dwLocalPort));
              port_line[nb_ligne].Port_dst[0]=0;
              strcpy(port_line[nb_ligne].state,"LISTENING");
              nb_ligne++;
            }else
            {
              port_line[nb_ligne].protocol[0]=0;
              port_line[nb_ligne].IP_src[0]=0;
              port_line[nb_ligne].Port_src[0]=0;
              port_line[nb_ligne].IP_dst[0]=0;
              port_line[nb_ligne].Port_dst[0]=0;
              port_line[nb_ligne].state[0]=0;
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
              ret = TRUE;
            }
            free(cmd);
          }
        }
      }
    }
  }
  return ret;
}
//------------------------------------------------------------------------------
void EnumProcessAndThread(HANDLE hlv, unsigned short nb_colonne, unsigned int last_id, unsigned int end_id)
{
  //liste des processus
  DWORD i, j, k, cbNeeded;

  //enable privilège pour accéder aux process système

  HANDLE hProcess,hProcess2;
  HMODULE hMod[MAX_PATH];
  FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime ,LocalFileTime;
  SYSTEMTIME SysTime;

  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  LINE_PROC_ITEM port_line[MAX_PATH];

  unsigned int img = 0;
  BOOL continue_ok = TRUE;

  char tmp[MAX_LINE_SIZE];

  //afin de passer outre les rootkit on énumère manuellement les process par id
  for (i=last_id+1;i<end_id;i++) //32768 = 2gb/64k pour la gestion du 64bits
  {
    // récupère le handle du processus.
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,i);
    if (hProcess!=INVALID_HANDLE_VALUE && hProcess!=NULL)
    {
      lv_line[0].c[0] = 0;
      lv_line[1].c[0] = 0;
      lv_line[2].c[0] = 0;
      lv_line[3].c[0] = 0;
      lv_line[4].c[0] = 0;
      lv_line[5].c[0] = 0;
      lv_line[6].c[0] = 0;
      lv_line[7].c[0] = 0;
      lv_line[8].c[0] = 0;
      lv_line[9].c[0] = 0;
      lv_line[10].c[0] = 0;
      lv_line[11].c[0] = 0;

      //PID
      sprintf(lv_line[1].c,"%04lu",i);

      //lecture de la ligne de commande
      GetProcessArg(hProcess, lv_line[3].c, MAX_LINE_SIZE);

      //on vérifie si le path existe déja ou non
      j = ListView_GetItemCount(hlv);
      continue_ok = TRUE;
      for (k=0;k<j;k++)
      {
        ListView_GetItemText(hlv,k,3,tmp,MAX_LINE_SIZE);
        if (!strcmp(tmp,lv_line[3].c)){continue_ok = FALSE;break;}
      }
      if (!continue_ok)continue;

      //name
      //if (GetModuleBaseName(hProcess,NULL,lv_line[0].c,MAX_LINE_SIZE) == 0) continue;
      if (GetModuleBaseName(hProcess,NULL,tmp,MAX_LINE_SIZE) != 0)
      {
        snprintf(lv_line[0].c,MAX_LINE_SIZE,"[%s]",tmp);
      }else lv_line[0].c[0] = 0;

      //description
      if (EnumProcessModules(hProcess,hMod, MAX_PATH,&cbNeeded))
      {
        //Path
        if (GetModuleFileNameEx(hProcess,hMod[0],lv_line[2].c,MAX_LINE_SIZE) == 0)lv_line[2].c[0] = 0;
      }

      //user
      if ((hProcess2 = OpenProcess(READ_CONTROL,0,i))!=NULL)
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
              if (IsValidSid(psid))
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
              }else lv_line[4].c[0] = 0;
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

      //icon
      if (lv_line[2].c[0]=='\\' && lv_line[2].c[1]=='?' && lv_line[2].c[2]=='?' && lv_line[2].c[3]=='\\')img = GetIconProcess(hlv,lv_line[2].c+4);
      else img = GetIconProcess(hlv,lv_line[2].c);

      //port réseau ouvert
      j=GetPortsFromPID(i, port_line, MAX_PATH, MAX_LINE_SIZE);

      if (j == 0)AddToLVICON(hlv,lv_line, nb_colonne,img);
      else
      {
        for (k=0;k<j;k++)
        {
          strcpy(lv_line[6].c, port_line[k].protocol);
          strcpy(lv_line[7].c, port_line[k].IP_src);
          strcpy(lv_line[8].c, port_line[k].Port_src);
          strcpy(lv_line[9].c, port_line[k].IP_dst);
          strcpy(lv_line[10].c, port_line[k].Port_dst);
          strcpy(lv_line[11].c, port_line[k].state);
          AddToLVICON(hlv,lv_line, nb_colonne,img);
        }
      }
      CloseHandle(hProcess);
    }
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI EnumProcess(LPVOID lParam)
{
  if (enum_en_cours)return 0;
  enum_en_cours = TRUE;

  HANDLE hlv                = GetDlgItem(Tabl[TABL_PROCESS],LV_VIEW);
  unsigned short nb_colonne = NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL];

  ListView_DeleteAllItems(hlv);
  //liste des processus
  DWORD j, k, cbNeeded;

  //enable privilège pour accéder aux process système
  //SetDebugPrivilege(TRUE);

  PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
  HANDLE hCT = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS|TH32CS_SNAPTHREAD, 0);
  if (hCT!=INVALID_HANDLE_VALUE)
  {
    if(Process32First(hCT, &pe))
    {
      HANDLE hProcess,hProcess2;
      HMODULE hMod[MAX_PATH];
      FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime ,LocalFileTime;
      SYSTEMTIME SysTime;

      LINE_ITEM lv_line[SIZE_UTIL_ITEM];
      LINE_PROC_ITEM port_line[MAX_PATH];

      unsigned int img = 0;

      char tmp[MAX_PATH];
      do
      {
        // récupère le handle du processus.
        if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,pe.th32ProcessID))!=NULL)
        {
          lv_line[1].c[0] = 0;
          lv_line[2].c[0] = 0;
          lv_line[3].c[0] = 0;
          lv_line[4].c[0] = 0;
          lv_line[5].c[0] = 0;
          lv_line[6].c[0] = 0;
          lv_line[7].c[0] = 0;
          lv_line[8].c[0] = 0;
          lv_line[9].c[0] = 0;
          lv_line[10].c[0] = 0;
          lv_line[11].c[0] = 0;

          //PID
          sprintf(lv_line[1].c,"%04lu",pe.th32ProcessID);

          //name
          snprintf(lv_line[0].c,MAX_LINE_SIZE,"%s",pe.szExeFile);

          //description
          if (EnumProcessModules(hProcess,hMod, MAX_PATH,&cbNeeded) )
          {
            //Path
            if (GetModuleFileNameEx(hProcess,hMod[0],lv_line[2].c,MAX_LINE_SIZE) == 0)lv_line[2].c[0] = 0;
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

          //icon
          if (lv_line[2].c[0]=='\\' && lv_line[2].c[1]=='?' && lv_line[2].c[2]=='?' && lv_line[2].c[3]=='\\')img = GetIconProcess(hlv,lv_line[2].c+4);
          else img = GetIconProcess(hlv,lv_line[2].c);

          //port réseau ouvert
          j=GetPortsFromPID(pe.th32ProcessID, port_line, MAX_PATH, MAX_LINE_SIZE);

          if (j == 0)AddToLVICON(hlv,lv_line, nb_colonne,img);
          else
          {
            for (k=0;k<j;k++)
            {
              strcpy(lv_line[6].c, port_line[k].protocol);
              strcpy(lv_line[7].c, port_line[k].IP_src);
              strcpy(lv_line[8].c, port_line[k].Port_src);
              strcpy(lv_line[9].c, port_line[k].IP_dst);
              strcpy(lv_line[10].c, port_line[k].Port_dst);
              strcpy(lv_line[11].c, port_line[k].state);
              AddToLVICON(hlv,lv_line, nb_colonne,img);
            }
          }

          CloseHandle(hProcess);
        }
      }while(Process32Next(hCT, &pe));

      //ajout des processus non énumérés
      EnumProcessAndThread(hlv, nb_colonne, 1, 32768);
    }
    CloseHandle(hCT);
  }
  //SetDebugPrivilege(FALSE);
  enum_en_cours = FALSE;
  return 0;
}

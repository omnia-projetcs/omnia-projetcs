//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
BOOL CreateFileCopyFilefromPath(char *path_src, char *path_dst)
{
  HANDLE Hfic_src = CreateFile(path_src,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
  if (Hfic_src == INVALID_HANDLE_VALUE)return FALSE;

  HANDLE Hfic_dst = CreateFile(path_dst,GENERIC_WRITE,FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (Hfic_dst == INVALID_HANDLE_VALUE)
  {
    CloseHandle(Hfic_dst);
    return FALSE;
  }

  DWORD position = 0, increm = 0, copiee, taille_fic = GetFileSize(Hfic_src,NULL);
  if (taille_fic > 0)
  {
    char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
    if (!buffer)
    {
      CloseHandle(Hfic_src);
      CloseHandle(Hfic_dst);
      return FALSE;
    }

    if (taille_fic > DIXM)increm = DIXM;
    else increm = taille_fic;

    while (position<taille_fic && increm!=0)
    {
      copiee = 0;
      ReadFile(Hfic_src, buffer+position, increm, &copiee,0);
      position += copiee;
      if (taille_fic-position < increm)increm = taille_fic-position ;
    }

    WriteFile(Hfic_dst,buffer,taille_fic,&copiee,0);
    HeapFree(GetProcessHeap(), 0, buffer);

    CloseHandle(Hfic_src);
    CloseHandle(Hfic_dst);
    return TRUE;
  }
  return FALSE;
}
//------------------------------------------------------------------------------
/*void CopyFileInVSSMode(char *path_src, char *path_dst)
{
  //ok on lance la création d'un snap shoot + copy
  char path_src_VSS[MAX_PATH]="";
  char cmd_VSS[MAX_PATH]="create shadow /for=C:";
  cmd_VSS[19] = path_src[0];

  //create snapshot
  //vssadmin create shadow /for=C:
  ShellExecute(h_main, "vssadmin",cmd_VSS,"",NULL,SW_HIDE);

  //read last snapshot VolumeShadowCopy
  //\\?\GLOBALROOT\Device\HarddiskVolumeShadowCopyX
  unsigned int test = 400; //not good code but compatible with much systems
  do
  {
    snprintf(path_src_VSS,MAX_PATH,"\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy%d%s",test,&path_src[2]);
  }while (CopyFile(path_src_VSS,path_dst,FALSE) == 0 && test-->0);

  //copy
  CopyFile(path_src_VSS,path_dst,FALSE);
}*/
//------------------------------------------------------------------------------
/*void VSSFileCopyFilefromPath(char *path_src, char *path_dst)
{
  if (path_src[0] == '\\' || path_src[0] == 0)return;

  //get access of all services
  SC_HANDLE h_SCM = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT|SC_MANAGER_ENUMERATE_SERVICE|SERVICE_START|SERVICE_STOP);//SC_MANAGER_ALL_ACCESS
  if (h_SCM != ERROR_ACCESS_DENIED && h_SCM != ERROR_DATABASE_DOES_NOT_EXIST && h_SCM != NULL)
  {
    //get access to vss service
    SC_HANDLE h_SC_service = OpenService(h_SCM,"VSS",SERVICE_START|SERVICE_STOP);//SERVICE_ALL_ACCESS
    if (h_SC_service != NULL)
    {
      //get state
      SERVICE_STATUS_PROCESS ssStatus;
      DWORD dwBytesNeeded;
      if (QueryServiceStatusEx(h_SC_service,SC_STATUS_PROCESS_INFO,(LPBYTE)&ssStatus,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
      {
        //start VSS service if not
        if(ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
        {
          CopyFileInVSSMode(path_src, path_dst);
        }else //start service
        {
          if(StartService(h_SC_service, 0, NULL))
          {
            //wait until ok
            SERVICE_STATUS_PROCESS ssStatus2;
            do
            {
              Sleep(100);
              if (!QueryServiceStatusEx(h_SC_service,SC_STATUS_PROCESS_INFO,(LPBYTE)&ssStatus2,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))break;
            }while (ssStatus2.dwCurrentState == SERVICE_START_PENDING);

            CopyFileInVSSMode(path_src, path_dst);

            //end !
            ControlService(h_SC_service, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssStatus);
          }
        }
      }
      CloseServiceHandle(h_SC_service);
    }
    CloseServiceHandle(h_SCM);
  }
}*/

//------------------------------------------------------------------------------
BOOL VSSCopyFile(char *path_src, char *path_dst)
{
  //load ddl to verify if cuntion is enable
  HMODULE hDLL;
  if ((hDLL = LoadLibrary( "VssApi.dll"))!=NULL)
  {
    //verify if enable
    typedef HRESULT (WINAPI *ISVVOLUMESNAPSHOTTED)(char* pwszVolumeName, BOOL *pbSnapshotsPresent, LONG *plSnapshotCapability);
    ISVVOLUMESNAPSHOTTED IsVolumeSnapshotted = (ISVVOLUMESNAPSHOTTED) GetProcAddress(hDLL,"IsVolumeSnapshotted");
    if (IsVolumeSnapshotted != NULL)
    {
      //windows 2008 and after solution !
      //help from : http://www.petri.co.il/working-active-directory-snapshots-windows-server-2008.htm

      //--------------------------
      //create snapshot
      char cmd[MAX_PATH] ="\"%WINDIR%\\System32\\ntdsutil.exe\" snapshot \"Activate Instance NTDS\" create quit quit";
      ReplaceEnv("WINDIR",cmd,MAX_PATH);
      system(cmd);





      /*char cmd[MAX_PATH] ="\"%WINDIR%\\System32\\ntdsutil.exe\" snapshot \"activate instance ntds\" create quit quit";
      ReplaceEnv("WINDIR",cmd,MAX_PATH);

      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      ZeroMemory(&si, sizeof(si));
      ZeroMemory(&pi, sizeof(pi));
      si.cb = sizeof(si);

      if(!CreateProcess(0, cmd, 0, 0, 0, 0, 0, 0, &si, &pi))return FALSE;

      // Wait until child process exits.
      WaitForSingleObject(pi.hProcess, INFINITE);
      if (pi.hProcess!=INVALID_HANDLE_VALUE)CloseHandle(pi.hProcess);
      if (pi.hThread!=INVALID_HANDLE_VALUE)CloseHandle(pi.hThread);*/

      //load resultats
      /*
      Snapshot set {3a861a35-2f33-4d7a-8861-a10e47afdaba}
      */

      //--------------------------
      //Get file access
      //char GUID[MAX_PATH];








      //check if use of ntdisutil or vss




      //check if service exist start it if is not


      //check if snapshott enable
      /*BOOL SnapshotsPresent   = FALSE;
      LONG SnapshotCapability = 0;
      char path[MAX_PATH]     = "c:\\";
      path[0]                 = path_src[0];
      if (IsVolumeSnapshotted(path,&SnapshotsPresent,&SnapshotCapability) != S_OK)
      {
        if (!SnapshotsPresent) //no shadowcopy
        {
          //do a snapshot


        }
      }*/

      //get last snapshot

      //cp file

      //if service vsshave been started stop it !


    }
    FreeLibrary(hDLL);
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL CopyFilefromPath(char *path_src, char *path_dst, BOOL direct_shadow_copy)
{
  //if (direct_shadow_copy)VSSFileCopyFilefromPath(path_src, path_dst);//by (VSS) Shadow copy
  //else
  {
    if (!CreateFileCopyFilefromPath(path_src, path_dst))//by share read
    {
      if(CopyFile(path_src,path_dst,FALSE)) return TRUE;  //by sys copy
      else
      {
        //or by (VSS) Shadow copy
        //VSSFileCopyFilefromPath(path_src, path_dst);
        return FALSE;
      }
    }else return TRUE;
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//convert \Device\HarddiskVolume1\ to reel path
BOOL ConvertDosPathtoReel(char *path, unsigned int max_path)
{
  char tmp_path[MAX_PATH],tmp_pathfinal[MAX_PATH];
  if (path[0] == '\\' && path[1] != '\\')
  {
    charToLowChar(path);

    //for each system file
    char tmp[MAX_PATH], let[3]="C:";
    int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);
    //search
    for (i=0;i<nblecteurs;i+=4)
    {
      let[0]      = tmp[i]; //only 'C:' not 'C:\'
      tmp_path[0] = 0;
      if (QueryDosDevice(let,tmp_path,MAX_PATH) > 0)
      {
        if (Contient(path,charToLowChar(tmp_path)))//ok ^^
        {
          strncpy(tmp_pathfinal,path,MAX_PATH);
          snprintf(path,max_path,"%c:\%s",let[0],tmp_pathfinal+strlen(tmp_path));
          return TRUE;
        }
      }
    }
  }else return TRUE; //also OK
  return FALSE;
}
//------------------------------------------------------------------------------
DWORD WINAPI BackupRegFile(LPVOID lParam)
{
  //choix du répertoire de destination ^^
  BROWSEINFO browser;
  LPITEMIDLIST lip;
  char tmp[MAX_PATH]      = "";
  char cmd[MAX_PATH*2]    = "";
  browser.hwndOwner       = h_main;
  browser.pidlRoot        = 0;
  browser.lpfn            = 0;
  browser.iImage          = 0;
  browser.ulFlags         = BIF_NEWDIALOGSTYLE; //ytou can create new directories
  browser.lParam          = 0;
  browser.pszDisplayName  = tmp;
  browser.lpszTitle       = cps[TXT_SAVE_TO].c;
  lip                     = SHBrowseForFolder(&browser);
  if (lip != NULL)
  {
    SHGetPathFromIDList(lip,tmp);
    if (strlen(tmp)>0)
    {
      //save all registry
      //http://msdn.microsoft.com/en-us/library/windows/desktop/ms724877%28v=vs.85%29.aspx
      //Config hardware volatile keys
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\HARDWARE \"%s\\HARDWARE_volatile_key\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //SAM
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\SAM \"%s\\SAM\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //Security
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\SECURITY \"%s\\SECURITY\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //Software
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\SOFTWARE \"%s\\SOFTWARE\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //System
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\SYSTEM \"%s\\SYSTEM\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //Classes.dat
      snprintf(cmd,MAX_PATH*2,"SAVE HKCR \"%s\\CLASSES.dat\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //Default
      snprintf(cmd,MAX_PATH*2,"SAVE HKU\\.DEFAULT \"%s\\DEFAULT\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
      //Current user
      snprintf(cmd,MAX_PATH*2,"SAVE HKCU \"%s\\HKCU_NTUSER.DAT\"",tmp);
      ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);

      //all registry files in direct copy from registry access
      HKEY CleTmp;
      unsigned int state = 0;
      if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\hivelist",&CleTmp)==ERROR_SUCCESS)
      {
        DWORD nbValue = 0;
        if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValue,0,0,0,0)==ERROR_SUCCESS)
        {
          DWORD i, valueSize, dataSize;
          char value[MAX_PATH], data[MAX_PATH];
          for (i=0;i<nbValue;i++)
          {
            value[0]  = 0;
            data[0]   = 0;
            valueSize = MAX_PATH;
            dataSize  = MAX_PATH;
            if (RegEnumValue (CleTmp,i,(LPTSTR)value,(LPDWORD)&valueSize,0,NULL,(LPBYTE)data,(LPDWORD)&dataSize)==ERROR_SUCCESS)
            {
              if (data[0] != 0 && valueSize > 16)
              {
                //get SID from value, only for users files
                if (value[16] == '-')
                {
                  snprintf(cmd,MAX_PATH*2,"%s\\%s",tmp,&value[15]);
                  if (ConvertDosPathtoReel(data,MAX_PATH))
                  {
                    state += !CopyFilefromPath(data, cmd, FALSE);
                  }else state+=1;
                }
              }
            }
          }
        }else state = 1;
        RegCloseKey(CleTmp);
      }else state = 1;

      //registry extract
      if (state)
      {
        if (RegOpenKey(HKEY_USERS,"",&CleTmp)==ERROR_SUCCESS)
        {
          DWORD nbSubKey = 0;
          if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
          {
            DWORD i, subkey_size;
            char subkey[MAX_PATH];
            for (i=0;i<nbSubKey;i++)
            {
              subkey[0]   = 0;
              subkey_size = MAX_PATH;
              if (RegEnumKeyEx (CleTmp,i,subkey,&subkey_size,0,0,0,0)==ERROR_SUCCESS)
              {
                if (subkey[0] != 0 && subkey_size > 17) //bypass default users_keys
                {
                  snprintf(cmd,MAX_PATH*2,"SAVE HKU\\%s \"%s\\%s_NTUSER.DAT\"",subkey,tmp,subkey);
                  ShellExecute(h_main, "open","REG",cmd,NULL,SW_HIDE);
                }
              }
            }
          }
          RegCloseKey(CleTmp);
        }
      }
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
void CopyFileFromTo(char *from, char *to, unsigned int ext_size)
{
  //énumération des fichier
  WIN32_FIND_DATA data;
  char src[MAX_PATH], dst[MAX_PATH];
  HANDLE hfic = FindFirstFile(from, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      strcpy(src,from);
      src[strlen(src)-ext_size] =0;
      strncat(src,data.cFileName,MAX_PATH);

      snprintf(dst,MAX_PATH,"%s%s",to,data.cFileName);

      //copy
      CopyFilefromPath(src, dst, FALSE);
    }while(FindNextFile (hfic,&data));
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI BackupEvtFile(LPVOID lParam)
{
  //choix du répertoire de destination ^^
  BROWSEINFO browser;
  LPITEMIDLIST lip;
  char tmp[MAX_PATH+1]    = "";
  char path[MAX_PATH]     = "",
       path2[MAX_PATH]    = "";
  browser.hwndOwner       = h_main;
  browser.pidlRoot        = 0;
  browser.lpfn            = 0;
  browser.iImage          = 0;
  browser.ulFlags         = BIF_NEWDIALOGSTYLE; //permet la création d'un dossier
  browser.lParam          = 0;
  browser.pszDisplayName  = tmp;  //résultat ici
  browser.lpszTitle       = cps[TXT_SAVE_TO].c;
  lip                     = SHBrowseForFolder(&browser);
  if (lip != NULL)
  {
    SHGetPathFromIDList(lip,tmp);
    if (strlen(tmp)>0)
    {
      strncat(tmp,"\\\0",MAX_PATH);
      //récupération du chemin du système
      if(ReadValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "SystemRoot", path, MAX_LINE_SIZE))
      {
        if (path[0]!=0)
        {
          //on copie les fichier evt
          snprintf(path2,MAX_PATH,"%s\\system32\\config\\*.evt",path);
          CopyFileFromTo(path2, tmp, 5);

          //le fichiers evtx
          snprintf(path2,MAX_PATH,"%s\\system32\\winevt\\Logs\\*.evtx",path);
          CopyFileFromTo(path2, tmp, 6);
        }else //default path
        {
          CopyFileFromTo("c:\\system32\\config\\*.evt", tmp, 5);
          CopyFileFromTo("c:\\system32\\winevt\\Logs\\*.evtx", tmp, 6);
        }
      }else //default pathtmp,
      {
        CopyFileFromTo("c:\\system32\\config\\*.evt", tmp, 5);
        CopyFileFromTo("c:\\system32\\winevt\\Logs\\*.evtx", tmp, 6);
      }
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI BackupNTDIS(LPVOID lParam)
{
  //choix du répertoire de destination ^^
  BROWSEINFO browser;
  LPITEMIDLIST lip;
  char tmp[MAX_PATH+1]    = "";
  char path[MAX_PATH]     = "";
  browser.hwndOwner       = h_main;
  browser.pidlRoot        = 0;
  browser.lpfn            = 0;
  browser.iImage          = 0;
  browser.ulFlags         = BIF_NEWDIALOGSTYLE; //permet la création d'un dossier
  browser.lParam          = 0;
  browser.pszDisplayName  = tmp;  //résultat ici
  browser.lpszTitle       = cps[TXT_SAVE_TO].c;
  lip                     = SHBrowseForFolder(&browser);
  if (lip != NULL)
  {
    SHGetPathFromIDList(lip,tmp);
    if (strlen(tmp)>0)
    {
      strncat(tmp,"\\ntds.dit\0",MAX_PATH);
      //récupération du chemin du système
      if(ReadValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\NTDS\\Parameters", "DSA Database File", path, MAX_PATH))
      {
        if (path[0]!=0)
        {
          ReplaceEnv("WINDIR",path,MAX_PATH);
          CopyFilefromPath(path, tmp, FALSE);
        }else CopyFilefromPath("c:\\windows\\ntds\\ntds.dit",tmp, FALSE);
      }else CopyFilefromPath("c:\\windows\\ntds\\ntds.dit",tmp, FALSE);
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI BackupFile(LPVOID lParam)
{
  char file[MAX_PATH]     = "",
      file_p[MAX_PATH]    = "",
       path[MAX_PATH]     = "",
       path2[MAX_PATH]    = "";

  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = h_main;
  ofn.lpstrFile = file_p;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter = "*.* \0*.*\0";;
  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_EXPLORER|OFN_SHOWHELP;
  ofn.lpstrDefExt ="*.*";
  if (GetOpenFileName(&ofn)==TRUE)
  {
    //get path to save
    BROWSEINFO browser;
    LPITEMIDLIST lip;
    browser.hwndOwner       = h_main;
    browser.pidlRoot        = 0;
    browser.lpfn            = 0;
    browser.iImage          = 0;
    browser.ulFlags         = BIF_NEWDIALOGSTYLE; //permet la création d'un dossier
    browser.lParam          = 0;
    browser.pszDisplayName  = path;  //résultat ici
    browser.lpszTitle       = cps[TXT_SAVE_TO].c;
    lip                     = SHBrowseForFolder(&browser);
    if (lip != NULL)
    {
      if (SHGetPathFromIDList(lip,path))
      {
        snprintf(path2,MAX_PATH,"%s\\%s",path,extractFileFromPath(file_p, file, MAX_PATH));
        //save
        CopyFilefromPath(file,path, FALSE);
      }
    }
  }
  return 0;
}

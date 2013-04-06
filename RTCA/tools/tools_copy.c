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
BOOL VSSFileCopyFilefromPath(char *path_src, char *path_dst)
{
  BOOL Ok=FALSE;
  //load dll to verify if funtion exist
  HMODULE hDLL;
  if ((hDLL = LoadLibrary( "VssApi.dll"))!=NULL)
  {
    char cmd_vssadmin[MAX_PATH]="", path_src_VSS[MAX_PATH]="";
    //create a snapshot of lecteur 2003r2/2008/2012 only
    snprintf(cmd_vssadmin,MAX_PATH,"vssadmin create shadow /for=%c:",path_src[0]);
    system(cmd_vssadmin);

    //copy file
    unsigned int test = 400; //not good code but compatible with much systems
    do
    {
      snprintf(path_src_VSS,MAX_PATH,"\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy%d%s",test,&path_src[2]);

    }while (CopyFile(path_src_VSS,path_dst,FALSE) == 0 && test-->0);
    if (test > 0 )Ok = TRUE;

    //no delete snapshot
    //delete snapshot
    //snprintf(cmd_vssadmin,MAX_PATH,"vssadmin delete shadows /for=%c: /shadow=%s",path_src[0],snapshoot_id);
    //system(cmd_vssadmin);
    FreeLibrary(hDLL);
  }
  return Ok;
}

//------------------------------------------------------------------------------
BOOL CopyFilefromPath(char *path_src, char *path_dst, BOOL direct_shadow_copy)
{
  if (direct_shadow_copy)VSSFileCopyFilefromPath(path_src, path_dst);//by (VSS) Shadow copy
  else
  {
    if (!CreateFileCopyFilefromPath(path_src, path_dst))//by share read
    {
      if(CopyFile(path_src,path_dst,FALSE)) return TRUE;  //by sys copy
      else
      {
        //or by (VSS) Shadow copy
        VSSFileCopyFilefromPath(path_src, path_dst);
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
      SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Copy registry file's done !!!");
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

          //usb history
          snprintf(path2,MAX_PATH,"%s\\setupapi.log",path);
          CopyFileFromTo(path2, tmp, 5);
          snprintf(path2,MAX_PATH,"%s\\inf\\setupapi.log",path);
          CopyFileFromTo(path2, tmp, 5);
          snprintf(path2,MAX_PATH,"%s\\inf\\setupapi.dev.log",path);
          CopyFileFromTo(path2, tmp, 5);

          //firewall history
          snprintf(path2,MAX_PATH,"%s\\pfirewall.log",path);
          CopyFileFromTo(path2, tmp, 5);
          snprintf(path2,MAX_PATH,"%s\\system32\\logfiles\\firewall\\pfirewall.log",path);
          CopyFileFromTo(path2, tmp, 5);
        }else //default path
        {
          //system
          CopyFileFromTo("c:\\windows\\system32\\config\\*.evt", tmp, 5);
          CopyFileFromTo("c:\\windows\\system32\\winevt\\Logs\\*.evtx", tmp, 6);

          //usb
          CopyFileFromTo("c:\\windows\\setupapi.log", tmp, 5); //2000,xp,2003
          CopyFileFromTo("c:\\windows\\inf\\setupapi.log", tmp, 5);//after
          CopyFileFromTo("c:\\windows\\inf\\setupapi.dev.log", tmp, 5);

          //firewall history
          CopyFileFromTo("c:\\windows\\pfirewall.log", tmp, 5);//after
          CopyFileFromTo("c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log", tmp, 5);
        }
      }else //default pathtmp,
      {
        //system
        CopyFileFromTo("c:\\windows\\system32\\config\\*.evt", tmp, 5);
        CopyFileFromTo("c:\\windows\\system32\\winevt\\Logs\\*.evtx", tmp, 6);

        //usb
        CopyFileFromTo("c:\\windows\\setupapi.log", tmp, 5); //2000,xp,2003
        CopyFileFromTo("c:\\windows\\inf\\setupapi.log", tmp, 5);//after
        CopyFileFromTo("c:\\windows\\inf\\setupapi.dev.log", tmp, 5);

        //firewall history
        CopyFileFromTo("c:\\windows\\pfirewall.log", tmp, 5);//after
        CopyFileFromTo("c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log", tmp, 5);
      }
      SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Copy event file's done !!!");
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
      SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Copy NTDIS.DIT file done !!!");
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
        SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Copy file done !!!");
      }
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
void GetLocalPath(char *path, unsigned int SIZE_MAX)
{
  char *c = path+GetModuleFileName(0, path,SIZE_MAX);
  while(*c != '\\') c--;
  *c = 0;
}
//------------------------------------------------------------------------------
void CopyEvtToZIP(void *hz,char *path,char*current_path, char*computername,HANDLE MyhFile)
{
  char tmp_path[MAX_PATH],path2[MAX_PATH],path3[MAX_PATH],file[MAX_PATH];
  if (path == NULL)snprintf(tmp_path,MAX_PATH,"%s\\system32\\config\\*.evt",path);
  else snprintf(tmp_path,MAX_PATH,"c:\\Windows\\system32\\config\\*.evt");

  WIN32_FIND_DATA data;
  DWORD copiee;
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (path == NULL)snprintf(path2,MAX_PATH,"%s\\system32\\config\\%s",path,data.cFileName);
      else snprintf(path2,MAX_PATH,"c:\\Windows\\system32\\config\\%s",data.cFileName);

      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,path2,strlen(path2),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }

      snprintf(path3,MAX_PATH,"%s\\%s",current_path,data.cFileName);
      SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)path3);
      if(CopyFilefromPath(path2, path3, FALSE))
      {
        snprintf(file,MAX_PATH,"%s\\eventlog\\%s",computername,data.cFileName);
        addSrc((TZIP *)hz,(void *)file, (void *)path3,0, 2);
        DeleteFile(path3);
      }
    }while(FindNextFile (hfic,&data));
  }
}
//------------------------------------------------------------------------------
void CopyEvtxToZIP(void *hz,char *path,char*current_path, char*computername,HANDLE MyhFile)
{
  char tmp_path[MAX_PATH],path2[MAX_PATH],path3[MAX_PATH],file[MAX_PATH];
  if (path == NULL)snprintf(tmp_path,MAX_PATH,"%s\\system32\\winevt\\Logs\\*.evtx",path);
  else snprintf(tmp_path,MAX_PATH,"c:\\Windows\\system32\\winevt\\Logs\\*.evtx");

  WIN32_FIND_DATA data;
  DWORD copiee;
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (path == NULL)snprintf(path2,MAX_PATH,"%s\\system32\\winevt\\Logs\\%s",path,data.cFileName);
      else snprintf(path2,MAX_PATH,"c:\\Windows\\system32\\winevt\\Logs\\%s",data.cFileName);

      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,path2,strlen(path2),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }

      snprintf(path3,MAX_PATH,"%s\\%s",current_path,data.cFileName);
      SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)path3);
      if(CopyFilefromPath(path2, path3, FALSE))
      {
        snprintf(file,MAX_PATH,"%s\\eventlog\\%s",computername,data.cFileName);
        addSrc((TZIP *)hz,(void *)file, (void *)path3,0, 2);
        DeleteFile(path3);
      }
    }while(FindNextFile (hfic,&data));
  }
}
//------------------------------------------------------------------------------
void CopyPrefetchToZIP(void *hz,char *path, char*current_path, char*computername,HANDLE MyhFile)
{
  char tmp_path[MAX_PATH],path2[MAX_PATH],path3[MAX_PATH],file[MAX_PATH];
  if (path == NULL)snprintf(tmp_path,MAX_PATH,"%s\\Prefetch\\*.pf",path);
  else snprintf(tmp_path,MAX_PATH,"c:\\Windows\\Prefetch\\*.pf");

  WIN32_FIND_DATA data;
  DWORD copiee;
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (path == NULL)snprintf(path2,MAX_PATH,"%s\\Prefetch\\%s",path,data.cFileName);
      else snprintf(path2,MAX_PATH,"c:\\Windows\\Prefetch\\%s",data.cFileName);

      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,path2,strlen(path2),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }

      snprintf(path3,MAX_PATH,"%s\\%s",current_path,data.cFileName);
      SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)path3);
      if(CopyFilefromPath(path2, path3, FALSE))
      {
        snprintf(file,MAX_PATH,"%s\\Prefetch\\%s",computername,data.cFileName);
        addSrc((TZIP *)hz,(void *)file, (void *)path3,0, 2);
        DeleteFile(path3);
      }
    }while(FindNextFile (hfic,&data));
  }
}
//------------------------------------------------------------------------------
void CopyJobToZIP(void *hz,char *path, char*current_path, char*computername,HANDLE MyhFile)
{
  char tmp_path[MAX_PATH],path2[MAX_PATH],path3[MAX_PATH],file[MAX_PATH];
  if (path == NULL)snprintf(tmp_path,MAX_PATH,"%s\\Tasks\\*.job",path);
  else snprintf(tmp_path,MAX_PATH,"c:\\Windows\\Tasks\\*.job");

  WIN32_FIND_DATA data;
  DWORD copiee;
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (path == NULL)snprintf(path2,MAX_PATH,"%s\\Tasks\\%s",path,data.cFileName);
      else snprintf(path2,MAX_PATH,"c:\\Windows\\Tasks\\%s",data.cFileName);

      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,path2,strlen(path2),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }

      snprintf(path3,MAX_PATH,"%s\\%s",current_path,data.cFileName);
      SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)path3);
      if(CopyFilefromPath(path2, path3, FALSE))
      {
        snprintf(file,MAX_PATH,"%s\\Tasks\\%s",computername,data.cFileName);
        addSrc((TZIP *)hz,(void *)file, (void *)path3,0, 2);
        DeleteFile(path3);
      }
    }while(FindNextFile (hfic,&data));
  }
}
//------------------------------------------------------------------------------
void CopySetupApiToZIP(void *hz,char *path,char*computername,HANDLE MyhFile)
{
  char file[MAX_PATH];
  DWORD copiee;
  if (path == NULL)
  {
    snprintf(file,MAX_PATH,"%s\\logs\\setupapi.log",computername);
    addSrc((TZIP *)hz,  (void *)file, (void *)"c:\\windows\\setupapi.log",0, 2);
    addSrc((TZIP *)hz,  (void *)file, (void *)"c:\\windows\\inf\\setupapi.log",0, 2);
    snprintf(file,MAX_PATH,"%s\\logs\\setupapi.dev.log",computername);
    addSrc((TZIP *)hz,  (void *)"setupapi.dev.log", (void *)"c:\\windows\\inf\\setupapi.dev.log",0, 2);

    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      if(FileExist("c:\\windows\\setupapi.log"))
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"c:\\windows\\setupapi.log");
        WriteFile(MyhFile,"c:\\windows\\setupapi.log\r\n",strlen("c:\\windows\\setupapi.log\r\n"),&copiee,0);
      }

      if(FileExist("c:\\windows\\inf\\setupapi.log"))
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"c:\\windows\\inf\\setupapi.log");
        WriteFile(MyhFile,"c:\\windows\\inf\\setupapi.log\r\n",strlen("c:\\windows\\inf\\setupapi.log\r\n"),&copiee,0);
      }

      if(FileExist("c:\\windows\\inf\\setupapi.dev.log"))
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"c:\\windows\\inf\\setupapi.dev.log");
        WriteFile(MyhFile,"c:\\windows\\inf\\setupapi.dev.log\r\n",strlen("c:\\windows\\inf\\setupapi.dev.log\r\n"),&copiee,0);
      }
    }

  }else
  {
    char tmp_path[MAX_PATH];
    snprintf(file,MAX_PATH,"%s\\logs\\setupapi.log",computername);
    snprintf(tmp_path,MAX_PATH,"%s\\setupapi.log",path);
    if(FileExist(tmp_path))
    {
      addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
        WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }
    }

    snprintf(tmp_path,MAX_PATH,"%s\\inf\\setupapi.log",path);
    if(FileExist(tmp_path))
    {
      addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
        WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }
    }

    snprintf(file,MAX_PATH,"%s\\logs\\setupapi.dev.log",computername);
    snprintf(tmp_path,MAX_PATH,"%s\\inf\\setupapi.dev.log",path);
    if(FileExist(tmp_path))
    {
      addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
        WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }
    }
  }
}
//------------------------------------------------------------------------------
void CopyfirewallLogZIP(void *hz,char *path,char*computername,HANDLE MyhFile)
{
  char file[MAX_PATH];
  DWORD copiee;
  snprintf(file,MAX_PATH,"%s\\logs\\pfirewall.log",computername);
  if (path == NULL)
  {
    addSrc((TZIP *)hz,  (void *)file, (void *)"c:\\windows\\pfirewall.log",0, 2);
    addSrc((TZIP *)hz,  (void *)file, (void *)"c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log",0, 2);

    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      if(FileExist("c:\\windows\\pfirewall.log"))
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"c:\\windows\\pfirewall.log");
        WriteFile(MyhFile,"c:\\windows\\pfirewall.log\r\n",strlen("c:\\windows\\pfirewall.log\r\n"),&copiee,0);
      }

      if(FileExist("c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log"))
      {
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log");
        WriteFile(MyhFile,"c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log\r\n",strlen("c:\\windows\\system32\\logfiles\\firewall\\pfirewall.log\r\n"),&copiee,0);
      }
    }
  }else
  {
    char tmp_path[MAX_PATH];
    snprintf(tmp_path,MAX_PATH,"%s\\pfirewall.log",path);
    addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);

    if(FileExist(tmp_path))
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }

    snprintf(tmp_path,MAX_PATH,"%s\\system32\\logfiles\\firewall\\pfirewall.log",path);
    addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);

    if(FileExist(tmp_path))
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }
  }
}
//------------------------------------------------------------------------------
void CopyNTDISToZIP(void *hz,char *path, char*current_path,char*computername,HANDLE MyhFile)
{
  char local_path[MAX_PATH];
  snprintf(local_path,MAX_PATH,"%s\\NTDIS.DIT",current_path);
  char file[MAX_PATH];
  snprintf(file,MAX_PATH,"%s\\AD\\NTDIS.DIT",computername);
  DWORD copiee;

  if (path == NULL)
  {
    if(CopyFilefromPath("c:\\windows\\ntds\\ntds.dit", local_path, FALSE))
    {
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,"c:\\windows\\ntds\\ntds.dit\r\n",strlen("c:\\windows\\ntds\\ntds.dit\r\n"),&copiee,0);
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"c:\\windows\\ntds\\ntds.dit");
      }
      addSrc((TZIP *)hz, (void *)file,(void *)local_path, 0, 2);
      DeleteFile(local_path);
    }
  }else
  {
    if(CopyFilefromPath(path, local_path, FALSE))
    {
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        WriteFile(MyhFile,path,strlen(path),&copiee,0);
        SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)path);
        WriteFile(MyhFile,"\r\n",2,&copiee,0);
      }
      addSrc((TZIP *)hz, (void *)file,(void *)local_path, 0, 2);
      DeleteFile(local_path);
    }
  }
}
//------------------------------------------------------------------------------
void CopyRegistryToZIP(void *hz, char*local_path,char*computername,HANDLE MyhFile)
{
  char tmp_path[MAX_PATH];
  char file[MAX_PATH];
  DWORD copiee;

  snprintf(file,MAX_PATH,"%s\\Registry\\HARDWARE_volatile_key",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKLM\\HARDWARE \"%s\\HARDWARE_volatile_key\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\HARDWARE_volatile_key",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\SAM",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKLM\\SAM \"%s\\SAM\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\SAM",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\SECURITY",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKLM\\SECURITY \"%s\\SECURITY\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\SECURITY",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\SOFTWARE",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKLM\\SOFTWARE \"%s\\SOFTWARE\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\SOFTWARE",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\SYSTEM",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKLM\\SYSTEM \"%s\\SYSTEM\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\SYSTEM",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\CLASSES.dat",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKCR \"%s\\CLASSES.dat\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\CLASSES.dat",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\DEFAULT",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKU\\.DEFAULT \"%s\\DEFAULT\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\DEFAULT",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);

  snprintf(file,MAX_PATH,"%s\\Registry\\NTUSER.DAT",computername);
  snprintf(tmp_path,MAX_PATH,"REG SAVE HKCU \"%s\\NTUSER.DAT\"",local_path);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    WriteFile(MyhFile,tmp_path,strlen(tmp_path),&copiee,0);
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path);
    WriteFile(MyhFile,"\r\n",2,&copiee,0);
  }
  system(tmp_path);
  snprintf(tmp_path,MAX_PATH,"%s\\NTUSER.DAT",local_path);
  addSrc((TZIP *)hz,  (void *)file, (void *)tmp_path,0, 2);
  DeleteFile(tmp_path);
}
//------------------------------------------------------------------------------
void CopyRegistryUSERmToZIP(void *hz, char*local_path,char*computername,HANDLE MyhFile, char *tmp_path, char *path)
{
  char tmp_path2[MAX_PATH],tmp_path_dst[MAX_PATH];
  WIN32_FIND_DATA data;
  DWORD copiee;
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(data.cFileName[0] == '.' && (data.cFileName[1] == 0 || data.cFileName[1] == '.')){}
      else if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        //1
        snprintf(tmp_path2,MAX_PATH,"%s\\%s\\NTUSER.DAT",path,data.cFileName);
        snprintf(tmp_path_dst,MAX_PATH,"%s\\Registry\\%s_NTUSER.DAT",computername,data.cFileName);
        addSrc((TZIP *)hz,  (void *)tmp_path_dst, (void *)tmp_path2,0, 2);
        if (MyhFile != INVALID_HANDLE_VALUE)
        {
          WriteFile(MyhFile,tmp_path2,strlen(tmp_path2),&copiee,0);
          SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path2);
          WriteFile(MyhFile,"\r\n",2,&copiee,0);
        }

        //2
        snprintf(tmp_path2,MAX_PATH,"%s\\%s\\Local Settings\\Application Data\\Microsoft\\Windows\\UsrClass.dat",path,data.cFileName);
        snprintf(tmp_path_dst,MAX_PATH,"%s\\Registry\\%s_UsrClass.dat",computername,data.cFileName);
        addSrc((TZIP *)hz,  (void *)tmp_path_dst, (void *)tmp_path2,0, 2);
        if (MyhFile != INVALID_HANDLE_VALUE)
        {
          WriteFile(MyhFile,tmp_path2,strlen(tmp_path2),&copiee,0);
          SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path2);
          WriteFile(MyhFile,"\r\n",2,&copiee,0);
        }

        //3
        snprintf(tmp_path2,MAX_PATH,"%s\\%s\\AppData\\Local\\Microsoft\\Windows\\UsrClass.dat",path,data.cFileName);
        snprintf(tmp_path_dst,MAX_PATH,"%s\\Registry\\%s_UsrClass.dat",computername,data.cFileName);
        addSrc((TZIP *)hz,  (void *)tmp_path_dst, (void *)tmp_path2,0, 2);
        if (MyhFile != INVALID_HANDLE_VALUE)
        {
          WriteFile(MyhFile,tmp_path2,strlen(tmp_path2),&copiee,0);
          SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)tmp_path2);
          WriteFile(MyhFile,"\r\n",2,&copiee,0);
        }
      }
    }while(FindNextFile (hfic,&data));
  }
}

//------------------------------------------------------------------------------
void CopyRegistryUSERToZIP(void *hz, char*local_path,char*computername,HANDLE MyhFile)
{
  CopyRegistryUSERmToZIP(hz, local_path,computername,MyhFile, "C:\\Documents and Settings\\*.*", "C:\\Documents and Settings");
  CopyRegistryUSERmToZIP(hz, local_path,computername,MyhFile, "D:\\Documents and Settings\\*.*", "D:\\Documents and Settings");
  CopyRegistryUSERmToZIP(hz, local_path,computername,MyhFile, "E:\\Documents and Settings\\*.*", "E:\\Documents and Settings");

  CopyRegistryUSERmToZIP(hz, local_path,computername,MyhFile, "C:\\Users\\*.*", "C:\\Users");
  CopyRegistryUSERmToZIP(hz, local_path,computername,MyhFile, "D:\\Users\\*.*", "D:\\Users");
  CopyRegistryUSERmToZIP(hz, local_path,computername,MyhFile, "E:\\Users\\*.*", "E:\\Users");
}
//------------------------------------------------------------------------------
void SaveALL(char*filetosave, char *computername)
{
  void *hz;
  if (createZip(&hz, (void *)filetosave,0,2,"")==0)
  {
    SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Backup files :");

    //get local directory
    char local_path[MAX_PATH]="",file[MAX_PATH];
    GetLocalPath(local_path, MAX_PATH);

    snprintf(file,MAX_PATH,"%s\\log.txt",local_path);
    HANDLE MyhFile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);

    char tmp_path[MAX_PATH]="";
    char *s = tmp_path;

    if(!ReadValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "SystemRoot", tmp_path, MAX_PATH))
      s = NULL;

    if (tmp_path[0] == 0)s = NULL;

    //evt + evtx
    CopyEvtToZIP(hz,s,local_path,computername,MyhFile);
    CopyEvtxToZIP(hz,s,local_path,computername,MyhFile);

    //system logs
    CopySetupApiToZIP(hz,s,computername,MyhFile);
    CopyfirewallLogZIP(hz,s,computername,MyhFile);

    //prefetch
    CopyPrefetchToZIP(hz,s,local_path,computername,MyhFile);

    //job
    CopyJobToZIP(hz,s,local_path,computername,MyhFile);

    //NTDIS.DIT
    s = tmp_path;
    tmp_path[0] = 0;
    if(!ReadValue(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\NTDS\\Parameters", "DSA Database File", tmp_path, MAX_PATH))
      s = NULL;
    else ReplaceEnv("WINDIR",tmp_path,MAX_PATH);

    if (tmp_path[0] == 0)s = NULL;

    CopyNTDISToZIP(hz,s,local_path,computername,MyhFile);

    //registry
    CopyRegistryToZIP(hz,local_path,computername,MyhFile);
    CopyRegistryUSERToZIP(hz,local_path,computername,MyhFile);

    //list of files
    CloseHandle(MyhFile);

    snprintf(tmp_path,MAX_PATH,"%s\\log.txt",computername);
    addSrc((TZIP *)hz,  (void *)tmp_path, (void *)file,0, 2);
    DeleteFile(file);

    ZipClose(hz);

    SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Copy of all files done !");
    SendMessage(hstatus_bar,SB_SETTEXT,1, (LPARAM)"");
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI BackupAllFiles(LPVOID lParam)
{
  char file[MAX_PATH]="";
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = h_main;
  ofn.lpstrFile = file;
  ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter ="*.zip\0*.zip\0";
  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt =".zip\0";
  if (GetSaveFileName(&ofn)==TRUE)
  {
    //get computername
    DWORD taille = 256;
    char computername[256]="";
    GetComputerName(computername,&taille);
    SaveALL(file,computername);
    SendMessage(hstatus_bar,SB_SETTEXT,0, (LPARAM)"Backup all file done !!!");
  }
  return 0;
}

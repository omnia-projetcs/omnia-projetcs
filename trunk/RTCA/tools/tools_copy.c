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

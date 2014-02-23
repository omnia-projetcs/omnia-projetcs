//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
char path_to_save[MAX_PATH+1];
//----------------------------------------------------------------
//get list of files with size and last modifcation date
//take directory with '\\'
void ListAllfiles(char *path, char*results, unsigned int max_size, BOOL resursiv)
{
  char tmp_path[MAX_PATH], tmp[MAX_PATH];
  snprintf(tmp_path,MAX_PATH,"%s*.*",path);
  results[0] = 0;

  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeModification;
  LARGE_INTEGER filesize;
  WIN32_FIND_DATA data;
  HANDLE hfind = FindFirstFile(tmp_path, &data);
  if (hfind != INVALID_HANDLE_VALUE)
  {
    do
    {
      filesize.HighPart = data.nFileSizeHigh;
      filesize.LowPart  = data.nFileSizeLow;

      FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
      snprintf(tmp,MAX_PATH,"%s%s [Last_modification:%02d/%02d/%02d-%02d:%02d:%02d] %ld bytes\n"
                  ,path,data.cFileName
                   ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                   ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond
                   ,filesize.QuadPart);
      strncat(results+strlen(results),tmp,max_size-strlen(results));
      strncat(results+strlen(results),"\0",max_size-strlen(results));

      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
      {
        snprintf(tmp,MAX_PATH,"%s%s\\",path,data.cFileName);
        ListAllfiles(tmp, results+strlen(results), max_size-strlen(results), resursiv);
      }
    }while(FindNextFile(hfind, &data) != 0);
    CloseHandle(hfind);
  }
}
//----------------------------------------------------------------
//copy a directory or files in a directory with extensions
//take directory with '\\'
void Copyfiles(char *pathsrc, char*pathdst, char *ext, BOOL resursiv)
{
  char tmp_path[MAX_PATH], tmp[MAX_PATH],tmp2[MAX_PATH];
  WIN32_FIND_DATA data;

  if (ext != NULL)
  {
    //all files
    snprintf(tmp_path,MAX_PATH,"%s*.%s",pathsrc,ext);
    HANDLE hfind = FindFirstFile(tmp_path, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        snprintf(tmp,MAX_PATH,"%s%s",pathsrc,data.cFileName);
        snprintf(tmp2,MAX_PATH,"%s%s",pathdst,data.cFileName);
        CopyFile(tmp,tmp2,TRUE);
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
    //all directories
    snprintf(tmp_path,MAX_PATH,"%s*.*",pathsrc);
    hfind = FindFirstFile(tmp_path, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
        {
          snprintf(tmp,MAX_PATH,"%s%s\\",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s\\",pathdst,data.cFileName);
          CreateDirectory(tmp2,NULL);
          Copyfiles(tmp, tmp2, ext, resursiv);
        }
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
  }else
  {
    snprintf(tmp_path,MAX_PATH,"%s*.*",pathsrc);
    HANDLE hfind = FindFirstFile(tmp_path, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
        {
          snprintf(tmp,MAX_PATH,"%s%s\\",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s\\",pathdst,data.cFileName);
          CreateDirectory(tmp2,NULL);
          Copyfiles(tmp, tmp2, ext, resursiv);
        }else
        {
          snprintf(tmp,MAX_PATH,"%s%s",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s",pathdst,data.cFileName);
          CopyFile(tmp,tmp2,TRUE);
        }
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
  }
}
//----------------------------------------------------------------
char *filetimeToString(FILETIME FileTime, char *str, unsigned int string_size)
{
  str[0] = 0;
  SYSTEMTIME SysTime;
  if (FileTimeToSystemTime(&FileTime, &SysTime) != 0)//traitement de l'affichage de la date
    snprintf(str,string_size,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);

  return str;
}
//----------------------------------------------------------------
BOOL GetPathToSAve(char *path)
{
  BROWSEINFO browser;
  ITEMIDLIST *lip;

  path[0] = 0;
  browser.hwndOwner          = h_main;
  browser.pidlRoot           = NULL;
  browser.pszDisplayName     = path;
  browser.lpszTitle          = "Backup directory";
  browser.ulFlags            = BIF_NEWDIALOGSTYLE;
  browser.lpfn               = NULL;

  if((lip=SHBrowseForFolder(&browser))!=NULL)
  {
    if (SHGetPathFromIDList(lip,path))
    {
      if (path[0] != 0)
      {
        strncat(path,"\\\0",MAX_PATH);
        return TRUE;
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
BOOL BackupRegistryKey(HKEY hkey, char *key_path, char*fileToSave)
{
  HKEY hkey_tmp;
  BOOL ret = FALSE;
  if(RegOpenKey(hkey,"SOFTWARE",&hkey_tmp) == ERROR_SUCCESS)
  {
    if (RegSaveKey(hkey_tmp,fileToSave,NULL)) ret = TRUE;

    RegCloseKey(hkey_tmp);
  }
  return ret;
}
//----------------------------------------------------------------
BOOL BackupServiceList(HKEY hkey, char *ckey, char*fileToSave, char*ip)
{
  HKEY hkey_tmp;
  BOOL ret = FALSE;
  if (RegOpenKey(hkey,ckey,&hkey_tmp)==ERROR_SUCCESS)
  {
    DWORD i, nbSubKey = 0, copiee=0;
    if (RegQueryInfoKey (hkey_tmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      HANDLE hfile = CreateFile(fileToSave, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (hfile != INVALID_HANDLE_VALUE)
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<NS>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        FILETIME LastWriteTime;
        char key[MAX_PATH],key_path[MAX_PATH], tmp_line[MAX_LINE_SIZE];
        DWORD d_tmp;
        DWORD key_size;
        char lastupdate[MAX_PATH], name[MAX_PATH], path[MAX_PATH], state[MAX_PATH], type[MAX_PATH], description[MAX_PATH];

        for (i=0;i<nbSubKey && scan_start;i++)
        {
          key_size  = MAX_PATH;
          key[0]    = 0;
          if (RegEnumKeyEx (hkey_tmp,i,key,&key_size,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
          {
            ret = TRUE;
            name[0]       = 0;
            path[0]       = 0;
            description[0]= 0;
            state[0]      = 0;
            type[0]       = 0;
            lastupdate[0] = 0;

            //path
            snprintf(key_path,MAX_PATH,"%s%s",ckey,key);

            //last update
            filetimeToString(LastWriteTime, lastupdate, MAX_PATH);

            //name
            if (ReadValue(hkey,key_path,"DisplayName",name, MAX_PATH) == 0)
            {
              if (ReadValue(hkey,key_path,"Group",name, MAX_PATH) == 0)continue;

              strncpy(name,key,MAX_PATH);
            }

            //path : ImagePath
            ReadValue(hkey,key_path,"ImagePath",path, MAX_PATH);

            //description : Description
            if(ReadValue(hkey,key_path,"Description",description, MAX_PATH) == 0)
              ReadValue(hkey,key_path,"Group",description, MAX_PATH);

            d_tmp = 0;
            ReadValue(hkey,key_path,"Start",&d_tmp, sizeof(d_tmp));
            switch(d_tmp)
            {
              case 0: strcpy(state,"Kernel module");break;
              case 1: strcpy(state,"Start by system");break;
              case 2: strcpy(state,"Automatic start ");break;
              case 3: strcpy(state,"Manual start ");break;
              case 4: strcpy(state,"Disable");break;
              default:strcpy(state,"Unknow");break;
            }

            d_tmp = 0;
            ReadValue(hkey,key_path,"Type",&d_tmp, sizeof(d_tmp));
            if (d_tmp = 1)strcpy(type,"Service");
            else strcpy(type,"Driver");

            snprintf(tmp_line,MAX_LINE_SIZE,
                     " <Data>\r\n"
                     "  <Key><![CDATA[%s\\HKEY_LOCAL_MACHINE\\%s]]><Key/>\r\n"
                     "  <Name><![CDATA[%s]]><Name/>\r\n"
                     "  <Command><![CDATA[%s]]><Command/>\r\n"
                     "  <LastKeyUpdateUTC><![CDATA[%s]]><LastKeyUpdateUTC/>\r\n"
                     "  <State><![CDATA[%s]]><State/>\r\n"
                     "  <Type><![CDATA[%s]]><Type/>\r\n"
                     "  <Description><![CDATA[%s]]><Description/>\r\n"
                     " <Data/>\r\n"
                     ,key_path,ip,name,path,lastupdate,state,type,description);
            WriteFile(hfile,tmp_line,strlen(tmp_line),&copiee,0);
          }
        }
        WriteFile(hfile,"</NS>",5,&copiee,0);
        CloseHandle(hfile);
      }
    }
    RegCloseKey(hkey_tmp);
  }
  return ret;
}//----------------------------------------------------------------
BOOL BackupSoftwareList(HKEY hkey, char *ckey, char*fileToSave, char*ip)
{
  HKEY hkey_tmp;
  BOOL ret = FALSE;
  if (RegOpenKey(hkey,ckey,&hkey_tmp)==ERROR_SUCCESS)
  {
    DWORD i, nbSubKey = 0, copiee=0;
    if (RegQueryInfoKey (hkey_tmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      HANDLE hfile = CreateFile(fileToSave, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (hfile != INVALID_HANDLE_VALUE)
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<NS>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        FILETIME LastWriteTime;
        char key[MAX_PATH],key_path[MAX_PATH], tmp_line[MAX_LINE_SIZE];
        DWORD d_tmp;
        DWORD key_size;
        char lastupdate[MAX_PATH], name[MAX_PATH], path[MAX_PATH], location[MAX_PATH];

        for (i=0;i<nbSubKey && scan_start;i++)
        {
          key_size  = MAX_PATH;
          key[0]    = 0;
          if (RegEnumKeyEx (hkey_tmp,i,key,&key_size,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
          {
            ret = TRUE;
            name[0]       = 0;
            path[0]       = 0;
            location[0]   = 0;
            lastupdate[0] = 0;

            //path
            snprintf(key_path,MAX_PATH,"%s%s",ckey,key);

            //last update
            filetimeToString(LastWriteTime, lastupdate, MAX_PATH);

            //name
            if (ReadValue(hkey,key_path,"DisplayName",name, MAX_PATH) == 0)
            {
              strncpy(name,key,MAX_PATH);
            }

            //path : Publisher
            ReadValue(hkey,key_path,"Publisher",path, MAX_PATH);

            //installlocation
            if (ReadValue(hkey,key_path,"InstallLocation",location, MAX_PATH) == 0)
              if (ReadValue(hkey,key_path,"Inno Setup: App Path",location, MAX_PATH) == 0)
                ReadValue(hkey,key_path,"RegistryLocation",location, MAX_PATH);

            snprintf(tmp_line,MAX_LINE_SIZE,
                     " <Data>\r\n"
                     "  <Key><![CDATA[%s\\HKEY_LOCAL_MACHINE\\%s]]><Key/>\r\n"
                     "  <Name><![CDATA[%s]]><Name/>\r\n"
                     "  <Publisher><![CDATA[%s]]><Publisher/>\r\n"
                     "  <LastKeyUpdateUTC><![CDATA[%s]]><LastKeyUpdateUTC/>\r\n"
                     "  <Location><![CDATA[%s]]><Location/>\r\n"
                     " <Data/>\r\n"
                     ,key_path,ip,name,path,lastupdate,location);
            WriteFile(hfile,tmp_line,strlen(tmp_line),&copiee,0);
          }
        }
        WriteFile(hfile,"</NS>",5,&copiee,0);
        CloseHandle(hfile);
      }
    }
    RegCloseKey(hkey_tmp);
  }
  return ret;
}
//----------------------------------------------------------------
void RemoteRegistryExtract(DWORD iitem,char *name, char *ip, SCANNE_ST config, char*pathToSave)
{
  HANDLE connect      = 0;
  char tmp[MAX_PATH]  = "", remote_name[MAX_PATH]  = "", msg[LINE_SIZE], tmp_path[MAX_PATH];
  connect = NetConnexionAuthenticateTest(ip, remote_name,config, iitem, TRUE, NULL);

  //remote registry connexion
  HKEY hkey;
  snprintf(tmp,MAX_PATH,"\\\\%s",ip);
  LONG reg_access = RegConnectRegistry(tmp, HKEY_LOCAL_MACHINE, &hkey);
  BOOL start_remote_registry = FALSE;

  //remote start registry service
  if (reg_access!=ERROR_SUCCESS && connect != 0)
  {
    if (StartRemoteRegistryService(ip, TRUE))
    {
      //wait 10 secondes
      Sleep(10000);
      start_remote_registry = TRUE;
      reg_access = RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey);
    }
  }

  if (reg_access==ERROR_SUCCESS)
  {
    //extract all services + infos
    if (config.check_services)
    {
      snprintf(tmp_path,MAX_PATH,"%sSERVICES_%s.xml",pathToSave,ip);
      BackupServiceList(hkey, "SYSTEM\\CurrentControlSet\\Services\\", tmp_path, ip);
    }

    //extract all software + infos
    if (config.check_software)
    {
      snprintf(tmp_path,MAX_PATH,"%sSOFTWARE_%s.xml",pathToSave,ip);
      BackupSoftwareList(hkey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\", tmp_path, ip);
    }

    //extract all USB key + infos
    if (config.check_USB)
    {
      snprintf(tmp_path,MAX_PATH,"%sUSB_%s.xml",pathToSave,ip);
    }

    //backup all registry key
    if (config.check_registry)
    {
      snprintf(tmp_path,MAX_PATH,"%sHKLM_SOFTWARE_%s.reg",pathToSave,ip);
      if (BackupRegistryKey(hkey, "SOFTWARE", tmp_path))AddMsg(h_main,(char*)"REMOTE REGISTRY","Backup HKLM\\SOFTWARE",(char*)ip);

      snprintf(tmp_path,MAX_PATH,"%sHKLM_SYSTEM_%s.reg",pathToSave,ip);
      if (BackupRegistryKey(hkey, "SYSTEM", tmp_path))AddMsg(h_main,(char*)"REMOTE REGISTRY","Backup HKLM\\SYSTEM",(char*)ip);
    }

    if (start_remote_registry)
    {
      StartRemoteRegistryService(ip, FALSE);
    }
  }

  if(connect)
  {
    WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    CloseHandle(connect);
  }
}
//----------------------------------------------------------------
BOOL RemoteFilesAutenthicateForExtract(DWORD iitem, char *ip, char*remote_share, SCANNE_ST config, char*pathToSave)
{
  //check file
  char tmp_login[MAX_PATH], file[LINE_SIZE], tmp_path[LINE_SIZE];
  char remote_name[LINE_SIZE], msg[LINE_SIZE];
  snprintf(remote_name,LINE_SIZE,"\\\\%s\\%s",ip,remote_share);
  DWORD j=0, _nb_j = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  if (config.nb_accounts == 0)
  {
    NETRESOURCE NetRes  = {0};
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;

    tmp_login[0] = 0;
    if (config.domain[0] != 0)
    {
      snprintf(tmp_login,MAX_PATH,"%s\\%s",config.domain,config.login);
    }else
    {
      if (!config.local_account)snprintf(tmp_login,MAX_PATH,"%s\\%s",ip,config.login);
    }

    if (WNetAddConnection2(&NetRes,config.mdp,tmp_login,CONNECT_PROMPT)==NO_ERROR)
    {
      snprintf(msg,LINE_SIZE,"%s\\%s with %s account.",ip,remote_share,tmp_login);
      AddMsg(h_main,(char*)"LOGIN (Files:NET)",msg,(char*)"");

      snprintf(msg,LINE_SIZE,"Login NET %s\\%s with %s account",ip,remote_share,tmp_login);
      AddLSTVUpdateItem(msg, COL_CONFIG, iitem);

      for (;j<_nb_j && scan_start;j++)
      {
        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
        {
          snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
          Copyfiles(tmp_path, pathToSave, NULL, TRUE);
          AddMsg(h_main,(char*)"FILES COPY",tmp_path,(char*)"");
        }
      }

      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      return TRUE;
    }
  }else
  {
    unsigned int i;
    for (i=0; i<config.nb_accounts ;i++)
    {
      NETRESOURCE NetRes  = {0};
      NetRes.dwScope      = RESOURCE_GLOBALNET;
      NetRes.dwType	      = RESOURCETYPE_ANY;
      NetRes.lpLocalName  = (LPSTR)"";
      NetRes.lpProvider   = (LPSTR)"";
      NetRes.lpRemoteName	= remote_name;

      tmp_login[0] = 0;
      if (config.accounts[i].domain[0] != 0)
      {
        snprintf(tmp_login,MAX_PATH,"%s\\%s",config.accounts[i].domain,config.accounts[i].login);
      }else
      {
        snprintf(tmp_login,MAX_PATH,"%s\\%s",ip,config.accounts[i].login);
      }
      if (WNetAddConnection2(&NetRes,config.accounts[i].mdp,tmp_login,CONNECT_PROMPT)==NO_ERROR)
      {
        snprintf(msg,LINE_SIZE,"%s\\%s with %s (%02d) account.",ip,remote_share,tmp_login,i);
        AddMsg(h_main,(char*)"LOGIN (Files:NET)",msg,(char*)"");

        snprintf(msg,LINE_SIZE,"Login NET %s\\%s with %s (%02d) account",ip,remote_share,tmp_login,i);
        AddLSTVUpdateItem(msg, COL_CONFIG, iitem);

        for (;j<_nb_j && scan_start;j++)
        {
          if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

          if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
          {
            snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
            Copyfiles(tmp_path, pathToSave, NULL, TRUE);
            AddMsg(h_main,(char*)"FILES COPY",tmp_path,(char*)"");
          }
        }

        WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
        return TRUE;
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
void RemoteFilesExtract(DWORD iitem, char *name, char *ip, SCANNE_ST config, char*pathToSave)
{
  //backup all files and directories
  if(RemoteFilesAutenthicateForExtract(iitem, ip, (char*)"C$", config, pathToSave))
  {
    RemoteFilesAutenthicateForExtract(iitem, ip, (char*)"D$", config, pathToSave);
    RemoteFilesAutenthicateForExtract(iitem, ip, (char*)"E$", config, pathToSave);
  }else AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_FILES,iitem);
}
//----------------------------------------------------------------
DWORD WINAPI remote_extractIP(LPVOID lParam)
{
  DWORD index = (DWORD)lParam;
  long long int iitem = ID_ERROR;
  char ip[MAX_PATH]="", dsc[MAX_PATH]="", dns[MAX_PATH]="", ttl_s[MAX_PATH]="", test_title[MAX_PATH];
  int ttl = -1;
  BOOL exist  = FALSE;

  //get IP
  if (SendDlgItemMessage(h_main, CB_IP, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) > MAX_PATH)
  {
    ReleaseSemaphore(hs_threads,1,NULL);

    //tracking
    if (scan_start)
    {
      EnterCriticalSection(&Sync);
      snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,++nb_test_ip,nb_i);
      LeaveCriticalSection(&Sync);
      SetWindowText(h_main,test_title);
    }
    return 0;
  }
  SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)index,(LPARAM)ip);

  if (SendDlgItemMessage(h_main, CB_DSC, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) < MAX_PATH)
  {
    SendDlgItemMessage(h_main, CB_DSC, LB_GETTEXT, (WPARAM)index,(LPARAM)dsc);
  }

  if (ip[0]!=0)
  {
    //disco mode !
    if (config.disco_icmp||config.disco_dns)
    {
      WaitForSingleObject(hs_disco,INFINITE);
      if (ip[0]> '9' || ip[0]< '0' || ((ip[1]> '9' || ip[1]< '0') && ip[1] != '.'))
      {
        //resolution inverse
        strncpy(dns,ip,MAX_PATH);

        struct in_addr **a;
        struct hostent *host;

        if (host=gethostbyname(ip))
        {
          a = (struct in_addr **)host->h_addr_list;
          snprintf(ip,16,"%s",inet_ntoa(**a));
          if (auto_scan_config.DNS_DISCOVERY)
          {
            exist = TRUE;
            iitem = AddLSTVItem(ip, dsc, dns, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
          }
        }else
        {
          iitem = AddLSTVItem((char*)"[ERROR DNS]", ip, dsc, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (char*)"OK");
          ReleaseSemaphore(hs_disco,1,NULL);
          ReleaseSemaphore(hs_threads,1,NULL);

          //tracking
          EnterCriticalSection(&Sync);
          snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,++nb_test_ip,nb_i);
          LeaveCriticalSection(&Sync);
          SetWindowText(h_main,test_title);
          return 0;
        }
      }

      //ICMP
      if (config.disco_icmp && scan_start)
      {
        ttl = Ping(ip);
        if (ttl > -1)
        {
          snprintf(ttl_s,MAX_PATH,"TTL:%d",ttl);

          if (!exist)
          {
            if (ttl <= MACH_LINUX)iitem = AddLSTVItem(ip, dsc, NULL, ttl_s, (char*)"Linux",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            else if (ttl <= MACH_WINDOWS)iitem = AddLSTVItem(ip, dsc, NULL, ttl_s, (char*)"Windows",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            else if (ttl <= MACH_WINDOWS)iitem = AddLSTVItem(ip, dsc, NULL, ttl_s, (char*)"Router",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            exist = TRUE;
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_TTL,ttl_s);

            if (ttl <= MACH_LINUX)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"Linux")
            else if (ttl <= MACH_WINDOWS)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"Windows")
            else if (ttl <= MACH_WINDOWS)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"Router")
          }
        }
      }

      //DNS
      if (config.disco_dns && scan_start && dns[0] == 0 && (exist || (!exist && auto_scan_config.DNS_DISCOVERY)))
      {
        if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"DNS");
        if(ResDNS(ip, dns, MAX_PATH))
        {
          if (!exist)
          {
            if (auto_scan_config.DNS_DISCOVERY)
            {
              iitem = AddLSTVItem(ip, dsc, dns, NULL, (char*)"Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
              exist = TRUE;
            }
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
          }
        }
      }
      ReleaseSemaphore(hs_disco,1,NULL);
    }else
    {
      if (ip[0]> '9' || ip[0]< '0' || ((ip[1]> '9' || ip[1]< '0') && ip[1] != '.'))exist = FALSE;
      else
      {
        exist = TRUE;
        //on ajoute seulement si une IP
        iitem = AddLSTVItem(ip, dsc, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      }
    }

    if (exist)
    {
      //create the directory where to save all datas !!!
      char tmp_path[MAX_PATH];
      snprintf(tmp_path,MAX_PATH,"%s%s\\",path_to_save,ip);
      CreateDirectory(tmp_path, NULL);

      //tests !!!
      //Registry
      if (config.check_registry || config.check_services || config.check_software  || config.check_USB)
      {
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Registry");
        WaitForSingleObject(hs_registry,INFINITE);
        RemoteRegistryExtract(iitem, dns, ip, config, tmp_path);
        ReleaseSemaphore(hs_registry,1,NULL);
      }

      //files
      if( config.check_files )
      {
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files");
        WaitForSingleObject(hs_file,INFINITE);
        RemoteFilesExtract(iitem, dns, ip, config, tmp_path);
        ReleaseSemaphore(hs_file,1,NULL);
      }

      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"OK");
    }
  }

  ReleaseSemaphore(hs_threads,1,NULL);

  //tracking
  if (scan_start)
  {
    EnterCriticalSection(&Sync);
    snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,++nb_test_ip,nb_i);
    LeaveCriticalSection(&Sync);
    SetWindowText(h_main,test_title);
  }
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI remote_extract(LPVOID lParam)
{
  time_t exec_time_start, exec_time_end;
  time(&exec_time_start);

  //load IP
  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    //load IP interval
    SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);
    SendDlgItemMessage(h_main,CB_DSC,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

    DWORD LIp1,LIp2;
    BYTE L11,L12,L13,L14,L21,L22,L23,L24;
    SendDlgItemMessage(h_main,IP1, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp1);
    SendDlgItemMessage(h_main,IP2, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp2);

    L11 = LIp1 >> 24;
    L12 = (LIp1 >> 16) & 0xFF;
    L13 = (LIp1 >> 8) & 0xFF;
    L14 = LIp1 & 0xFF;

    L21 = LIp2 >> 24;
    L22 = (LIp2 >> 16) & 0xFF;
    L23 = (LIp2 >> 8) & 0xFF;
    L24 = LIp2 & 0xFF;

    if ((L21 | L22 | L23 | L24 | L11 | L12 | L13 | L14) == 0){}
    else if ((L21 | L22 | L23 | L24) == 0)
    {
      char sip1[IP_SIZE];
      snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      addIPTest(sip1,"");
    }else if ((L11 | L12 | L13 | L14) == 0)
    {
      char sip2[IP_SIZE];
      snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);
      addIPTest(sip2,"");
    }else
    {
      char sip1[IP_SIZE],sip2[IP_SIZE];
      snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);
      addIPInterval(sip1, sip2,"");
    }
  }

  char tmp[MAX_PATH];
  snprintf(tmp,LINE_SIZE,"Loaded %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
  AddMsg(h_main,(char*)"INFORMATION",tmp,(char*)"");

  //get configuration
  unsigned int ref  = 0;

  config.disco_icmp           = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_dns            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  ref+=3;
  config.check_files          = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_registry       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_services       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_software       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_USB            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);

  if (config.check_files)   config.check_files    = (BOOL)load_file_list(CB_T_FILES,     (char*)DEFAULT_LIST_FILES);

  //where save the datas ?
  if (GetPathToSAve(path_to_save))
  {
    hs_threads  = CreateSemaphore(NULL,NB_MAX_THREAD,NB_MAX_THREAD,NULL);
    hs_disco    = CreateSemaphore(NULL,NB_MAX_DISCO_THREADS,NB_MAX_DISCO_THREADS,NULL);
    hs_file     = CreateSemaphore(NULL,NB_MAX_FILE_THREADS,NB_MAX_FILE_THREADS,NULL);
    hs_registry = CreateSemaphore(NULL,NB_MAX_REGISTRY_THREADS,NB_MAX_REGISTRY_THREADS,NULL);

    //wsa init
    WSADATA WSAData;
    WSAStartup(0x02, &WSAData );
    nb_test_ip = 0;

    if (nb_i == 1)
    {
      char tmp_IP[MAX_PATH]="";
      SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)0,(LPARAM)tmp_IP);
      if (tmp_IP[0] == 0 || (tmp_IP[0] == '0' && tmp_IP[1] == '.'))
        nb_i = 0;
    }

    DWORD i;
    for (i=0;(i<nb_i) && scan_start;i++)
    {
      //ScanIp((LPVOID)i);
      WaitForSingleObject(hs_threads,INFINITE);
      CreateThread(NULL,0,remote_extractIP,(PVOID)i,0,0);
    }

    //wait
    AddMsg(h_main,(char*)"INFORMATION",(char*)"Start waiting threads.",(char*)"");

    if (!scan_start)
    {
      while (nb_test_ip < i && scan_start)Sleep(100);
    }else
    {
      for(i=0;i<NB_MAX_THREAD;i++)WaitForSingleObject(hs_threads,INFINITE);

      WaitForSingleObject(hs_file,INFINITE);
      WaitForSingleObject(hs_registry,INFINITE);
    }
    WSACleanup();
  }

  //calcul run time
  time(&exec_time_end);

  AddMsg(h_main,(char*)"INFORMATION",(char*)"End of remote extract!",(char*)"");
  snprintf(tmp,MAX_PATH,"Ip view:%lu/%lu in %d.%0d minutes",ListView_GetItemCount(GetDlgItem(h_main,LV_results)),nb_i,(exec_time_end - exec_time_start)/60,(exec_time_end - exec_time_start)%60);
  AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");

  CloseHandle(hs_threads);
  CloseHandle(hs_disco);
  CloseHandle(hs_file);
  CloseHandle(hs_registry);

  //---------------------------------------------
  //init
  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),TRUE);
  }

  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),TRUE);
    EnableWindow(GetDlgItem(h_main,IP1),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_IP_CP),TRUE);
    EnableWindow(GetDlgItem(h_main,IP2),TRUE);
  }

  EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  EnableWindow(GetDlgItem(h_main,CB_tests),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  scan_start = FALSE;

  SetWindowText(GetDlgItem(h_main,BT_RE),"Remote Extract");
  SetWindowText(h_main,TITLE);
  h_thread_scan = 0;
  return 0;
}

//----------------------------------------------------------------

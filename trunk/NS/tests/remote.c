//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
char path_to_save[MAX_PATH+1];
//----------------------------------------------------------------
BOOL cpfile(char *src, char*dst, BOOL replace, BOOL msg, DWORD iitem)
{
  BOOL ret = FALSE;
  HANDLE h_src = CreateFile(src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (h_src != INVALID_HANDLE_VALUE)
  {
    HANDLE h_dst = CreateFile(dst, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, replace?CREATE_ALWAYS:OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (h_dst != INVALID_HANDLE_VALUE)
    {
      DWORD read=0, write=0;
      DWORD tampon=FILE_BUFFER_SIZE, lue=0, size      = GetFileSize(h_src, NULL);
      char buffer[FILE_BUFFER_SIZE+1];
      BOOL ok = TRUE;

      while (ok && lue < size && tampon > 0)
      {
        ok = ReadFile(h_src, buffer, tampon, &read, 0);
        if (ok && read > 0)
        {
          WriteFile(h_dst,buffer,read,&write,0);
          lue+=read;

          read = 0;
        }
        if (size - lue < FILE_BUFFER_SIZE)tampon = size - lue;
      }
      if (lue >= size)
      {
        ret = TRUE;
        AddLSTVUpdateItem(src, COL_FILES, iitem);
      }else
      {
        if(msg)AddMsg(h_main,(char*)"ERROR COPY (File):bad file size",src,dst);
        DeleteFile(dst);
      }

      CloseHandle(h_dst);
    }else if(msg)AddMsg(h_main,(char*)"ERROR COPY (File):bad destination file",src,dst);
    CloseHandle(h_src);
  }else if(msg)AddMsg(h_main,(char*)"ERROR COPY (File):bad source file",src,dst);
  return ret;
}
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
void Copyfiles(char *pathsrc, char*pathdst, char *ext, BOOL resursiv, BOOL msg, DWORD iitem)
{
  char tmp_path[MAX_PATH], tmp[MAX_PATH],tmp2[MAX_PATH];
  WIN32_FIND_DATA data;
  BOOL ok;

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
        //ok = CopyFile(tmp,tmp2,TRUE);
        ok = cpfile(tmp,tmp2,TRUE,FALSE,iitem);
        if (ok && msg)
        {
          AddMsg(h_main,(char*)"INFORMATION (CopyFile)",(char*)tmp,(char*)tmp2);
        }
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
          Copyfiles(tmp, tmp2, ext, resursiv, TRUE, iitem);
        }
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
  }else
  {
    //snprintf(tmp_path,MAX_PATH,"%s*.*",pathsrc);
    //HANDLE hfind = FindFirstFile(tmp_path, &data);
    HANDLE hfind = FindFirstFile(pathsrc, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
        {
          snprintf(tmp,MAX_PATH,"%s%s\\",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s\\",pathdst,data.cFileName);
          CreateDirectory(tmp2,NULL);
          Copyfiles(tmp, tmp2, ext, resursiv, TRUE, iitem);
        }else
        {
          snprintf(tmp,MAX_PATH,"%s%s",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s",pathdst,data.cFileName);
          //ok = CopyFile(tmp,tmp2,TRUE);
          ok = cpfile(tmp,tmp2,TRUE,FALSE,iitem);
          if (ok && msg)
          {
            AddMsg(h_main,(char*)"INFORMATION (CopyFile)",(char*)tmp,(char*)tmp2);
          }
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
BOOL BackupRegistryKey(HKEY hkey, char *key_path, char*pathtosave, char *file, char*ip, DWORD iitem, SCANNE_ST config)
{
  HKEY hkey_tmp;
  BOOL ret = FALSE;
  char msg[LINE_SIZE];
  char localfile[MAX_PATH];

  if(RegOpenKey(hkey,key_path,&hkey_tmp) == ERROR_SUCCESS)
  {
    snprintf(localfile, MAX_PATH, "C:\\%s",file);
    if (RegSaveKey(hkey_tmp,localfile,NULL))
    {
      //move the file from the remote directory
      if (RemoteFilesCopy(iitem, ip, "C$", config, pathtosave, file))
      {
        snprintf(msg,LINE_SIZE,"Backup %s\\%s to %s%s",ip,key_path,pathtosave,file);
        AddLSTVUpdateItem(msg, COL_REG, iitem);

        ret = TRUE;
      }
    }

    RegCloseKey(hkey_tmp);
  }
  return ret;
}
//----------------------------------------------------------------
BOOL BackupServiceList(HKEY hkey, char *ckey, char*fileToSave, char*ip, DWORD iitem, BOOL lstv_msg)
{
  HKEY hkey_tmp;
  BOOL ret = FALSE;
  char msg[LINE_SIZE];

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
        SYSTEMTIME SysTime;
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
            snprintf(key_path,MAX_PATH,"%s%s\\",ckey,key);

            //last update
            if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
            {
                snprintf(lastupdate,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
            }

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
                     "  <LastKeyUpdate><![CDATA[%s]]><LastKeyUpdate/>\r\n"
                     "  <State><![CDATA[%s]]><State/>\r\n"
                     "  <Type><![CDATA[%s]]><Type/>\r\n"
                     "  <Description><![CDATA[%s]]><Description/>\r\n"
                     " <Data/>\r\n"
                     ,ip,key_path,name,path,lastupdate,state,type,description);
            WriteFile(hfile,tmp_line,strlen(tmp_line),&copiee,0);

            snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s",ip,key_path,path);
            AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
            AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
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
BOOL BackupSoftwareList(HKEY hkey, char *ckey, char*fileToSave, char*ip, DWORD iitem, BOOL lstv_msg)
{
  HKEY hkey_tmp;
  BOOL ret = FALSE;
  char msg[LINE_SIZE];

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
        SYSTEMTIME SysTime;
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
            snprintf(key_path,MAX_PATH,"%s%s\\",ckey,key);

            //last update
            if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
            {
                snprintf(lastupdate,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
            }

            //name
            if (ReadValue(hkey,key_path,"DisplayName",name, MAX_PATH) == 0)
            {
              strncpy(name,key,MAX_PATH);
            }

            //path : Publisher
            ReadValue(hkey,key_path,"Publisher",path, MAX_PATH);

            //installlocation
            if (ReadValue(hkey,key_path,"InstallLocation",location, MAX_PATH) != 0)
            {
              snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sInstallLocation=%s %s",ip,key_path,location,lastupdate);
              AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
              AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
            }else if (ReadValue(hkey,key_path,"Inno Setup: App Path",location, MAX_PATH) != 0)
            {
              snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sInno Setup: App Path=%s %s",ip,key_path,location,lastupdate);
              AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
              AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
            }else if(ReadValue(hkey,key_path,"RegistryLocation",location, MAX_PATH) != 0)
            {
              snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sRegistryLocation=%s %s",ip,key_path,location,lastupdate);
              AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
              AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
            }else if(ReadValue(hkey,key_path,"UninstallString",location, MAX_PATH) != 0)
            {
              snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sUninstallString=%s %s",ip,key_path,location,lastupdate);
              AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
              AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
            }

            snprintf(tmp_line,MAX_LINE_SIZE,
                     " <Data>\r\n"
                     "  <Key><![CDATA[%s\\HKEY_LOCAL_MACHINE\\%s]]><Key/>\r\n"
                     "  <Name><![CDATA[%s]]><Name/>\r\n"
                     "  <Publisher><![CDATA[%s]]><Publisher/>\r\n"
                     "  <LastKeyUpdate><![CDATA[%s]]><LastKeyUpdate/>\r\n"
                     "  <Location><![CDATA[%s]]><Location/>\r\n"
                     " <Data/>\r\n"
                     ,ip,key_path,name,path,lastupdate,location);
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
BOOL BackupUSBList(HKEY hkey, char *ckey, char*fileToSave, char*ip, DWORD iitem, BOOL lstv_msg)
{
  HKEY hkey_tmp, CleTmp2;
  BOOL ret = FALSE;
  char msg[LINE_SIZE];

  if (RegOpenKey(hkey,ckey,&hkey_tmp)==ERROR_SUCCESS)
  {
    DWORD i,j, nbSubKey = 0, nbSubKey2, copiee=0;
    if (RegQueryInfoKey (hkey_tmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      HANDLE hfile = CreateFile(fileToSave, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (hfile != INVALID_HANDLE_VALUE)
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<NS>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        FILETIME LastWriteTime;
        SYSTEMTIME SysTime;
        char key[MAX_PATH], key2[LINE_SIZE], key_tmp_path[MAX_PATH], key_path[MAX_PATH], tmp_line[MAX_LINE_SIZE];
        DWORD d_tmp;
        DWORD key_size, key_size2;

        for (i=0;i<nbSubKey && scan_start;i++)
        {
          key_size  = MAX_PATH;
          key[0]    = 0;
          if (RegEnumKeyEx (hkey_tmp,i,key,&key_size,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
          {
            ret = TRUE;
            snprintf(key_tmp_path,MAX_PATH,"%s%s\\",ckey,key);
            if (RegOpenKey(hkey,key_tmp_path,&CleTmp2)!=ERROR_SUCCESS)continue;

            nbSubKey2 = 0;
            if (RegQueryInfoKey (CleTmp2,0,0,0,&nbSubKey2,0,0,0,0,0,0,0)==ERROR_SUCCESS)
            {
              for (j=0;j<nbSubKey2 && scan_start;j++)
              {
                key_size2 = LINE_SIZE;
                key2[0]   = 0;
                if (RegEnumKeyEx (CleTmp2,j,key2,&key_size2,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
                {
                  //path
                  snprintf(key_path,MAX_PATH,"%s%s\\",key_tmp_path,key2);

                  if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",ip,key_path,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);

                    snprintf(tmp_line,MAX_LINE_SIZE,
                     " <Data>\r\n"
                     "  <Key><![CDATA[%s\\HKEY_LOCAL_MACHINE\\%s]]><Key/>\r\n"
                     "  <Name><![CDATA[%s]]><Name/>\r\n"
                     "  <LastKeyUpdateUTC><![CDATA[%02d/%02d/%02d-%02d:%02d:%02d]]><LastKeyUpdateUTC/>\r\n"
                     " <Data/>\r\n"
                     ,ip,key_path,key2,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                    WriteFile(hfile,tmp_line,strlen(tmp_line),&copiee,0);

                  }else
                  {
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                  }

                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key_path);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }
              }
            }
            RegCloseKey(CleTmp2);
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
void backupRegKey(HKEY hkey, char *chkey,char *ckey, char *filetosave, HANDLE hfile, BOOL createf)
{
  HKEY hkey_tmp;
  HANDLE hfile_tmp = hfile;
  char tmp[MAX_LINE_SIZE];
  DWORD copiee;
  if (RegOpenKey(hkey,ckey,&hkey_tmp)==ERROR_SUCCESS)
  {
    if (createf)
    {
      hfile_tmp = CreateFile(filetosave, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
      if (hfile_tmp == INVALID_HANDLE_VALUE)
      {
        RegCloseKey(hkey_tmp);
        return;
      }
      snprintf(tmp,MAX_LINE_SIZE,"\"HKEY\";\"PATH\";\"TYPE\";\"VALUE\";\"DATA\";\"Last Update\";\r\n");
      WriteFile(hfile_tmp,tmp,strlen(tmp),&copiee,0);
    }

    if (hfile_tmp == INVALID_HANDLE_VALUE)
    {
      RegCloseKey(hkey_tmp);
      return;
    }

    //enum keys
    DWORD nbValue=0,i,j, nbSubKey=0;
    DWORD key_size;

    FILETIME LastWriteTime;
    SYSTEMTIME SysTime;
    char ckey_tmp[MAX_PATH],value[MAX_LINE_SIZE],data[MAX_LINE_SIZE],data2[MAX_LINE_SIZE], date[MAX_PATH]="";
    DWORD value_size, data_size, type, data2_size;
    if (RegQueryInfoKey (hkey_tmp,0,0,0,&nbSubKey,0,0,&nbValue,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
    {
      //date
      if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
      {
        snprintf(date,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
      }

      //values
      for (i=0;i<nbValue && scan_start;i++)
      {
        value_size  = MAX_LINE_SIZE;
        value[0]    = 0;
        data_size   = MAX_LINE_SIZE;
        data[0]     = 0;
        type        = 0;

        if (RegEnumValue (hkey_tmp,i,value,&value_size,0,&type,(LPBYTE)data,&data_size)==ERROR_SUCCESS)
        {
          //snprintf(tmp,MAX_LINE_SIZE,"\"HKEY\";\"PATH\";\"TYPE\";\"VALUE\";\"DATA\";\"Last Update\";\r\n");
          if (value_size || data_size)
          {
            switch (type)
            {
              case REG_EXPAND_SZ:snprintf(tmp,MAX_LINE_SIZE,"\"%s\";\"%s\";\"REG_EXPAND_SZ\";\"%s\";\"%s\";\"%s\";\r\n",chkey,ckey,value,data,date);break;
              case REG_SZ:snprintf(tmp,MAX_LINE_SIZE,"\"%s\";\"%s\";\"REG_SZ\";\"%s\";\"%s\";\"%s\";\r\n",chkey,ckey,value,data,date);break;
              case REG_LINK:snprintf(tmp,MAX_LINE_SIZE,"\"%s\";\"%s\";\"REG_LINK\";\"%s\";\"%s\";\"%s\";\r\n",chkey,ckey,value,data,date);break;
              case REG_MULTI_SZ:
                for (j=0;j<data_size;j++)
                {
                  if (data[j] == 0)data[j]=';';
                }
                snprintf(tmp,MAX_LINE_SIZE,"\"%s\";\"%s\";\"REG_MULTI_SZ\";\"%s\";\"%s\";\"%s\";\r\n",chkey,ckey,value,data,date);
              break;
              case REG_DWORD:
                {
                  DWORD *d = (DWORD*)data;
                  snprintf(tmp,MAX_LINE_SIZE,"\"%s\";\"%s\";\"REG_DWORD\";\"%s\";\"%lu\";\"%s\";\r\n",chkey,ckey,value,(*d)&0xFFFFFFFF,date);
                }
              break;
              case REG_BINARY:
              default:
                data2[0] = 0;
                for (j=0;j<data_size && data2_size<MAX_LINE_SIZE;j++)
                {
                  data2_size = strlen(data2);
                  snprintf(data2+data2_size,MAX_LINE_SIZE-data2_size,"%02X",data[j]&0xFF);
                }
                snprintf(tmp,MAX_LINE_SIZE,"\"%s\";\"%s\";\"%02x\";\"%s\";\"0x%s\";\"%s\";\r\n",chkey,ckey,type,value,data2,date);
              break;
            }
            WriteFile(hfile_tmp,tmp,strlen(tmp),&copiee,0);
          }
        }
      }

      //subkeys
      for (i=0;i<nbSubKey && scan_start;i++)
      {
        key_size    = MAX_PATH;
        ckey_tmp[0] = 0;
        if (RegEnumKeyEx(hkey_tmp,i,ckey_tmp,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          if (ckey != NULL)
          {
            snprintf(tmp,MAX_LINE_SIZE,"%s\\%s",ckey,ckey_tmp);
            backupRegKey(hkey, chkey,tmp, filetosave, hfile_tmp, FALSE);
          }else
          {
            backupRegKey(hkey, chkey,ckey_tmp, filetosave, hfile_tmp, FALSE);
          }
        }
      }
    }
    RegCloseKey(hkey_tmp);
  }
  if (createf)
  {
    CloseHandle(hfile_tmp);
  }
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

  if (reg_access==ERROR_SUCCESS && scan_start)
  {
    //OS
    RegistryOS(iitem, hkey);

    //extract all services + infos
    if (config.check_services && scan_start)
    {
      snprintf(tmp_path,MAX_PATH,"%s%s_SERVICES.xml",pathToSave,ip);
      BackupServiceList(hkey, "SYSTEM\\CurrentControlSet\\Services\\", tmp_path, ip, iitem, TRUE);
    }

    //extract all software + infos
    if (config.check_software && scan_start)
    {
      snprintf(tmp_path,MAX_PATH,"%s%s_SOFTWARE.xml",pathToSave,ip);
      BackupSoftwareList(hkey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\", tmp_path, ip, iitem, TRUE);
    }

    //extract all USB key + infos
    if (config.check_USB && scan_start)
    {
      snprintf(tmp_path,MAX_PATH,"%s%s_USB.xml",pathToSave,ip);
      BackupUSBList(hkey, "SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\", tmp_path, ip, iitem, TRUE);
    }

    //backup all registry key
    if (config.check_registry && scan_start)
    {
      /*snprintf(tmp_path,MAX_PATH,"%s_HKLM_SOFTWARE.RAW",ip);
      if (BackupRegistryKey(hkey, "SOFTWARE", pathToSave, tmp_path, ip, iitem, config)){}//AddMsg(h_main,(char*)"INFORMATION (Remote registry)","Backup HKLM\\SOFTWARE from ",(char*)ip);
      else if (scan_start)
      {
        char chkey[MAX_PATH];
        snprintf(chkey,MAX_PATH,"%s\\HKEY_LOCAL_MACHINE",tmp);
        snprintf(tmp_path,MAX_PATH,"%s%s_HKLM_SOFTWARE.csv",pathToSave,ip);
        backupRegKey(hkey, chkey,"SOFTWARE", tmp_path, NULL, TRUE);
      }

      if (scan_start)
      {
        snprintf(tmp_path,MAX_PATH,"%s_HKLM_SYSTEM.RAW",ip);
        if (BackupRegistryKey(hkey, "SYSTEM", pathToSave, tmp_path, ip, iitem, config)){}//AddMsg(h_main,(char*)"INFORMATION (Remote registry)","Backup HKLM\\SYSTEM from ",(char*)ip);
        else if (scan_start)
        {
          char chkey[MAX_PATH];
          snprintf(chkey,MAX_PATH,"%s\\HKEY_LOCAL_MACHINE",tmp);
          snprintf(tmp_path,MAX_PATH,"%s%s_HKLM_SYSTEM.csv",pathToSave,ip);
          backupRegKey(hkey, chkey,"SYSTEM", tmp_path, NULL, TRUE);
        }
      }*/

      char chkey[MAX_PATH];
      snprintf(chkey,MAX_PATH,"%s\\HKEY_LOCAL_MACHINE",tmp);
      snprintf(tmp_path,MAX_PATH,"%s%s_HKLM_SOFTWARE.csv",pathToSave,ip);
      backupRegKey(hkey, chkey,"SOFTWARE", tmp_path, NULL, TRUE);
      if (scan_start)
      {
        snprintf(tmp_path,MAX_PATH,"%s%s_HKLM_SYSTEM.csv",pathToSave,ip);
        backupRegKey(hkey, chkey,"SYSTEM", tmp_path, NULL, TRUE);
      }

      //HKEY_USERS :
      if (scan_start)
      {
        RegCloseKey(hkey);
        if (RegConnectRegistry(tmp, HKEY_USERS, &hkey)==ERROR_SUCCESS)
        {
          snprintf(chkey,MAX_PATH,"%s\\HKEY_USERS",tmp);
          snprintf(tmp_path,MAX_PATH,"%s%s_HKU.csv",pathToSave,ip);
          backupRegKey(hkey, chkey,NULL, tmp_path, NULL, TRUE);
        }
      }
    }

    RegCloseKey(hkey);
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
BOOL RemoteFilesCopy(DWORD iitem, char *ip, char*remote_share, SCANNE_ST config, char*pathToSave, char*file)
{
  //check file
  char tmp_login[MAX_PATH], tmp_path[LINE_SIZE];
  char remote_name[LINE_SIZE], file_src[LINE_SIZE], file_dst[LINE_SIZE];
  snprintf(remote_name,LINE_SIZE,"\\\\%s\\%s",ip,remote_share);
  DWORD j=0, _nb_j = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  BOOL ret = FALSE;

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
      snprintf(file_src,LINE_SIZE,"%s\\%s",remote_name,file);
      snprintf(file_dst,LINE_SIZE,"%s%s",pathToSave,file);

      //if (CopyFile(file_src,file_dst,TRUE)) ret = TRUE;
      if (cpfile(file_src,file_dst,TRUE,FALSE,iitem)) ret = TRUE;
      DeleteFile(file_src);

      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      return ret;
    }
  }else
  {
    unsigned int i;
    for (i=0; i<config.nb_accounts && scan_start ;i++)
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
        snprintf(file_src,LINE_SIZE,"%s\\%s",remote_name,file);
        snprintf(file_dst,LINE_SIZE,"%s%s",pathToSave,file);

        //if (CopyFile(file_src,file_dst,TRUE)) ret = TRUE;
        if (cpfile(file_src,file_dst,TRUE,FALSE,iitem)) ret = TRUE;
        DeleteFile(file_src);

        WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
        return ret;
      }
    }
  }
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteFilesAutenthicateForExtract(DWORD iitem, char *ip, char*remote_share, SCANNE_ST config, char*pathToSave)
{
  //check file
  char tmp_login[MAX_PATH], file[LINE_SIZE], tmp_path[LINE_SIZE], tmp_path2[LINE_SIZE], filename[MAX_PATH];
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
          filename[0] = 0;
          snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
          snprintf(tmp_path2,LINE_SIZE,"%s%s_%s",pathToSave,ip,extractFileFromPath(file, filename, MAX_PATH));

          //Copyfiles(tmp_path, tmp_path2, NULL, TRUE, TRUE, iitem);
          if (cpfile(tmp_path, tmp_path2, TRUE, FALSE, iitem)) AddMsg(h_main,(char*)"COPY (File)",tmp_path,tmp_path2);
          //else AddMsg(h_main,(char*)"ERROR COPY (File)",tmp_path,tmp_path2);
        }
      }

      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      return TRUE;
    }
  }else
  {
    unsigned int i;
    for (i=0; i<config.nb_accounts && scan_start;i++)
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
            filename[0] = 0;
            snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
            snprintf(tmp_path2,LINE_SIZE,"%s%s_%s",pathToSave,ip,extractFileFromPath(file, filename, MAX_PATH));

            //Copyfiles(tmp_path, tmp_path2, NULL, TRUE, TRUE, iitem);
            if (cpfile(tmp_path, tmp_path2, TRUE, FALSE, iitem)) AddMsg(h_main,(char*)"COPY (File)",tmp_path,tmp_path2);
            //else AddMsg(h_main,(char*)"ERROR COPY (File)",tmp_path,tmp_path2);
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
void RemoteSSHExtract(DWORD iitem,char *name, char *ip, SCANNE_ST config, char*pathToSave)
{
  //OS
  char tmp_os[MAX_MSG_SIZE]="";
  BOOL authent_ok = FALSE;
  if (config.nb_accounts == 0)
  {
    if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.mdp, -1,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,TRUE,TRUE) == SSH_ERROR_OK)
    {
      if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "head -n 1 /etc/issue"))
      {
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
        authent_ok = TRUE;
      }else  if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.mdp, -1,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE,FALSE) == SSH_ERROR_OK)
      {
        if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
        {
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
          authent_ok = TRUE;
        }
      }
    }

    //tests
    if (authent_ok)
    {
      if (ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.login, config.mdp) == SSH_ERROR_OK)
      {
        //backup
        char tmp[MAX_PATH]="", buffer[MAX_MSG_SIZE]="";
        HANDLE hfile;
        DWORD copiee;

        snprintf(tmp, MAX_PATH, "%s%s_SSH.txt",pathToSave,ip);
        hfile = CreateFile(tmp, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (hfile != INVALID_HANDLE_VALUE)
        {
          ListView_GetItemText(GetDlgItem(h_main,LV_results), iitem, COL_SSH, buffer, MAX_MSG_SIZE);
          WriteFile(hfile,buffer,strlen(buffer),&copiee,0);
          CloseHandle(hfile);
        }
      }
    }
  }else
  {
    DWORD j = 0;
    int account_OK = -1;
    BOOL first_msg = TRUE;
    BOOL msg_auth  = TRUE;
    char msg[MAX_LINE_SIZE];
    for (j=0;j<config.nb_accounts && scan_start;j++)
    {
      //OS rescue
      tmp_os[0] = 0;
      if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].mdp, j,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,first_msg,msg_auth) == SSH_ERROR_OK)
      {
        if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "head -n 1 /etc/issue"))
        {
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
          account_OK = j;
          authent_ok = TRUE;
          break;
        }else if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].mdp, j,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE, msg_auth) == SSH_ERROR_OK)
        {
          if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
            account_OK = j;
            authent_ok = TRUE;
            break;
          }
        }
      }
      first_msg = FALSE;
    }

        //tests
    if (authent_ok && account_OK != -1)
    {
      if (ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[account_OK].login, config.accounts[account_OK].mdp) == SSH_ERROR_OK)
      {
        //backup
        char tmp[MAX_PATH]="", buffer[MAX_MSG_SIZE]="";
        HANDLE hfile;
        DWORD copiee;

        snprintf(tmp, MAX_PATH, "%s%s_SSH.txt",pathToSave,ip);
        hfile = CreateFile(tmp, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (hfile != INVALID_HANDLE_VALUE)
        {
          ListView_GetItemText(GetDlgItem(h_main,LV_results), iitem, COL_SSH, buffer, MAX_MSG_SIZE);
          WriteFile(hfile,buffer,strlen(buffer),&copiee,0);
          CloseHandle(hfile);
        }
      }
    }
  }
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

  if (ip[0]!=0 && scan_start)
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

    if (exist && scan_start)
    {
      //tests !!!
      //Registry
      if ((config.check_registry || config.check_services || config.check_software  || config.check_USB) && scan_start)
      {
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Registry");
        WaitForSingleObject(hs_registry,INFINITE);
        RemoteRegistryExtract(iitem, dns, ip, config, path_to_save);
        ReleaseSemaphore(hs_registry,1,NULL);
      }

      //files
      if(config.check_files && scan_start)
      {
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files");
        WaitForSingleObject(hs_file,INFINITE);
        RemoteFilesExtract(iitem, dns, ip, config, path_to_save);
        ReleaseSemaphore(hs_file,1,NULL);
      }

      if (config.check_USB && scan_start)
      {
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"SSH");
        if (TCP_port_open(iitem, ip, SSH_DEFAULT_PORT, FALSE))
        {
          WaitForSingleObject(hs_ssh,INFINITE);
          RemoteSSHExtract(iitem, dns, ip, config, path_to_save);
          ReleaseSemaphore(hs_ssh,1,NULL);
        }
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
  config.check_ssh            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);

  if (config.check_files)   config.check_files    = (BOOL)load_file_list(CB_T_FILES,(char*)DEFAULT_LIST_FILES);
  if (config.check_ssh)     config.check_ssh      = (BOOL)load_file_list(CB_T_SSH,   (char*)DEFAULT_LIST_SSH);

  //where save the datas ?
  if (GetPathToSAve(path_to_save))
  {
    AddMsg(h_main,(char*)"INFORMATION","Save directory:",(char*)path_to_save);

    hs_threads  = CreateSemaphore(NULL,NB_MAX_THREAD,NB_MAX_THREAD,NULL);
    hs_disco    = CreateSemaphore(NULL,NB_MAX_DISCO_THREADS,NB_MAX_DISCO_THREADS,NULL);
    hs_file     = CreateSemaphore(NULL,NB_MAX_FILE_THREADS,NB_MAX_FILE_THREADS,NULL);
    hs_registry = CreateSemaphore(NULL,NB_MAX_REGISTRY_THREADS,NB_MAX_REGISTRY_THREADS,NULL);
    hs_tcp      = CreateSemaphore(NULL,NB_MAX_TCP_TEST_THREADS,NB_MAX_TCP_TEST_THREADS,NULL);
    hs_ssh      = CreateSemaphore(NULL,NB_MAX_SSH_THREADS,NB_MAX_SSH_THREADS,NULL);

    //wsa init
    WSADATA WSAData;
    WSAStartup(0x02, &WSAData );
    nb_test_ip = 0;

    nb_i = SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

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
      WaitForSingleObject(hs_tcp,INFINITE);
      WaitForSingleObject(hs_ssh,INFINITE);
    }
    WSACleanup();
  }

  //calcul run time
  time(&exec_time_end);

  AddMsg(h_main,(char*)"INFORMATION",(char*)"End of remote extract!",(char*)"");
  snprintf(tmp,MAX_PATH,"Ip view:%lu/%lu in %d.%0d minutes",ListView_GetItemCount(GetDlgItem(h_main,LV_results)),nb_i,(exec_time_end - exec_time_start)/60,(exec_time_end - exec_time_start)%60);
  AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");

  //autosave
  if (ListView_GetItemCount(GetDlgItem(h_main,LV_results)) >0)
  {
    //save
    char file2[LINE_SIZE];
    time_t dateEtHMs;
    time(&dateEtHMs);
    struct tm *today = localtime(&dateEtHMs);

    char date[DATE_SIZE];
    strftime(date, DATE_SIZE,"%Y.%m.%d-%H.%M.%S",today);

    char cpath[LINE_SIZE]="";
    GetLocalPath(cpath, LINE_SIZE);

    snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.csv",cpath,date);
    if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_CSV, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
    else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

    snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.xml",cpath,date);
    if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_XML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
    else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

    save_done = TRUE;
  }


  CloseHandle(hs_threads);
  CloseHandle(hs_disco);
  CloseHandle(hs_file);
  CloseHandle(hs_registry);
  CloseHandle(hs_tcp);
  CloseHandle(hs_ssh);

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
  EnableWindow(GetDlgItem(h_main,BT_RE),TRUE);
  scan_start = FALSE;

  SetWindowText(GetDlgItem(h_main,BT_RE),"Remote Extract");
  SetWindowText(h_main,TITLE);
  h_thread_scan = 0;
  return 0;
}

//----------------------------------------------------------------

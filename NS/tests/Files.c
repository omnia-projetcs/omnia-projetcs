//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
void FileToMd5(HANDLE Hfic, char *md5)
{
  //ouverture du fichier en lecture partagé
  md5[0]=0;
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
  {
    unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
    if (buffer == NULL)return;

    //lecture du fichier
    DWORD copiee, position = 0, increm = 0;
    if (taille_fic > DIXM)increm = DIXM;
    else increm = taille_fic;
    BOOL read = TRUE;
    while (position<taille_fic && increm!=0 && read)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
    {
      copiee = 0;
      read =ReadFile(Hfic, buffer+position, increm,&copiee,0);
      position +=copiee;
      if (taille_fic-position < increm)increm = taille_fic-position ;
    }

    //traitement en MD5
    md5_state_t     state;
    md5_byte_t      digest[16];
    md5_init(&state);
    md5_append(&state,(const md5_byte_t  *)buffer,taille_fic);
    md5_finish(&state,digest);

    //génération du md5 en chaine
    unsigned short i;
    for(i=0;i<16;i++)snprintf(md5+i*2,3,"%02x",digest[i]);
    md5[32]=0;
    HeapFree(GetProcessHeap(), 0,buffer);
  }
}
//----------------------------------------------------------------
void FileToSHA1(HANDLE Hfic, char *csha1)
{
  //ouverture du fichier en lecture partagé
  csha1[0]=0;
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
  {
    unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
    if (buffer == NULL)return;

    //lecture du fichier
    DWORD copiee, position = 0, increm = 0;
    if (taille_fic > DIXM)increm = DIXM;
    else increm = taille_fic;

    BOOL read = TRUE;
    while (position<taille_fic && increm!=0 && read)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
    {
      copiee = 0;
      read = ReadFile(Hfic, buffer+position, increm,&copiee,0);
      position +=copiee;
      if (taille_fic-position < increm)increm = taille_fic-position ;
    }

    //traitement en SHA256
    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha, buffer, taille_fic);
    if (SHA1Result(&sha))
    {
      snprintf(csha1,SHA1_SIZE,"%08X%08X%08X%08X%08X",
              sha.Message_Digest[0],
              sha.Message_Digest[1],
              sha.Message_Digest[2],
              sha.Message_Digest[3],
              sha.Message_Digest[4]);
    }
    HeapFree(GetProcessHeap(), 0,buffer);
  }
}
//----------------------------------------------------------------
void FileToSHA256(HANDLE Hfic, char *csha256)
{
  if (SHA1_enable)
  {
    FileToSHA1(Hfic, csha256);
    return;
  }

  //ouverture du fichier en lecture partagé
  csha256[0]=0;
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
  {
    unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
    if (buffer == NULL)return;

    //lecture du fichier
    DWORD copiee, position = 0, increm = 0;
    if (taille_fic > DIXM)increm = DIXM;
    else increm = taille_fic;

    BOOL read = TRUE;
    while (position<taille_fic && increm!=0 && read)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
    {
      copiee = 0;
      read = ReadFile(Hfic, buffer+position, increm,&copiee,0);
      position +=copiee;
      if (taille_fic-position < increm)increm = taille_fic-position ;
    }

    //traitement en SHA256
    sha256_ctx    m_sha256;
    unsigned char digest[32];

    sha256_begin(&m_sha256);
    sha256_hash(buffer, taille_fic, &m_sha256);
    sha256_end(digest, &m_sha256);

    //génération du SHA256 en chaine
    unsigned short i;
    for(i=0;i<32;i++)snprintf(csha256+i*2,3,"%02x",digest[i]&0xFF);
    csha256[64]=0;
    HeapFree(GetProcessHeap(), 0,buffer);
  }
}
//----------------------------------------------------------------
/*void CheckFullFile(DWORD iitem, char *file, WIN32_FIND_DATA *data, char*s_md5, char*s_sha, DWORD file_sz)
{
  char s_sha[SHA256_SIZE]="",s_md5[MAX_PATH], date[MAX_PATH]="\0\0\0";
  HANDLE hfile;
  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeModification;

  //last modify
  if (data == NULL)
  {
    WIN32_FIND_DATA d0;
    HANDLE hfind = FindFirstFile(file, &d0);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      FileTimeToLocalFileTime(&(d0.ftLastWriteTime), &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
      snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d]"
                   ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                   ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond);
      FindClose(hfind);
    }
  }else if (data->ftLastWriteTime.dwHighDateTime != 0 || data->ftLastWriteTime.dwLowDateTime != 0)
  {
    FileTimeToLocalFileTime(&(data->ftLastWriteTime), &LocalFileTime);
    FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
    snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d]"
                 ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                 ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond);
  }

  //MD5
  hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    FileToMd5(hfile, s_md5);
    CloseHandle(hfile);

    //SHA256
    hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
    if (hfile != INVALID_HANDLE_VALUE)
    {
      FileToSHA256(hfile, s_sha);
      CloseHandle(hfile);
    }
  }

  if (s_sha[0] != 0 && s_md5[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;MD5;%s;%s;%s",file,date,s_md5,SHA1_enable?"SHA1":"SHA256",s_sha);
  }else if (s_md5[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;MD5;%s;;",file,date,s_md5);
  }else if (s_sha[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;;;%s;%s",file,date,SHA1_enable?"SHA1":"SHA256",s_sha);
  }else snprintf(file,LINE_SIZE,"%s %s;;;;",file,date);

  AddMsg(h_main,(char*)"FOUND (File)",file,(char*)"");

  AddLSTVUpdateItem(file, COL_FILES, iitem);
}*/
//----------------------------------------------------------------
void CheckFile(DWORD iitem, char *file, WIN32_FIND_DATA *data)
{
  char s_sha[SHA256_SIZE]="",s_md5[MAX_PATH], date[MAX_PATH]="\0\0\0";
  HANDLE hfile;
  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeModification;

  //last modify
  if (data == NULL)
  {
    WIN32_FIND_DATA d0;
    HANDLE hfind = FindFirstFile(file, &d0);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      FileTimeToLocalFileTime(&(d0.ftLastWriteTime), &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
      snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d]"
                   ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                   ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond);
      FindClose(hfind);
    }
  }else if (data->ftLastWriteTime.dwHighDateTime != 0 || data->ftLastWriteTime.dwLowDateTime != 0)
  {
    FileTimeToLocalFileTime(&(data->ftLastWriteTime), &LocalFileTime);
    FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
    snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d]"
                 ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                 ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond);
  }

  //MD5
  hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    FileToMd5(hfile, s_md5);
    CloseHandle(hfile);

    //SHA256
    hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
    if (hfile != INVALID_HANDLE_VALUE)
    {
      FileToSHA256(hfile, s_sha);
      CloseHandle(hfile);
    }
  }

  snprintf(file,LINE_SIZE,"%s %s;MD5;%s;%s;%s",file,date,s_md5[0]==0?"":s_md5,SHA1_enable?"SHA1":"SHA256",s_sha[0]==0?"":s_sha);
/*
  if (s_sha[0] != 0 && s_md5[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;MD5;%s;%s;%s",file,date,s_md5,SHA1_enable?"SHA1":"SHA256",s_sha);
  }else if (s_md5[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;MD5;%s;;",file,date,s_md5);
  }else if (s_sha[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;;;%s;%s",file,date,SHA1_enable?"SHA1":"SHA256",s_sha);
  }else snprintf(file,LINE_SIZE,"%s %s;;;;",file,date);*/

  AddMsg(h_main,(char*)"FOUND (File)",file,(char*)"");
  AddLSTVUpdateItem(file, COL_FILES, iitem);
}
//----------------------------------------------------------------
void CheckRecursivFiles(DWORD iitem, char *remote_name, char *file, BOOL recursif)
{
  //if the file exist
  WIN32_FIND_DATA data, d0;
  HANDLE hfind;
  char tmp_path[LINE_SIZE]="",tmp_remote_name[LINE_SIZE];

  if (file != NULL)
  {
    snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
    if (GetFileAttributes(tmp_path) != INVALID_FILE_ATTRIBUTES && scan_start)
    {
      //file exist + date
      hfind = FindFirstFile(tmp_path, &d0);
      if (hfind != INVALID_HANDLE_VALUE)
      {
        CheckFile(iitem, tmp_path, &d0);
        FindClose(hfind);
      }
    }

    if (tmp_path[strlen(tmp_path)-1] == '\\' || tmp_path[strlen(tmp_path)-1] == '/')
    {
      //if directory !
      tmp_path[strlen(tmp_path)-1] = 0; // remove the ending /
      CheckRecursivFiles(iitem, tmp_path, NULL, recursif);
    }
  }

  //next
  snprintf(tmp_path,LINE_SIZE,"%s\\*.*",remote_name);
  hfind = FindFirstFile(tmp_path, &data);
  if (hfind != INVALID_HANDLE_VALUE && scan_start)
  {
    do
    {
      if (data.cFileName[0] == '.' && (data.cFileName[1] == 0 || (data.cFileName[2] == 0 && data.cFileName[1] == '.'))){}
      else
      {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if (recursif)
          {
            snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,data.cFileName);
            CheckRecursivFiles(iitem, tmp_remote_name, file, recursif);
          }
        }else
        {
          if (file == NULL)
          {
            snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,data.cFileName);
            CheckFile(iitem, tmp_remote_name, &data);
          }
        }
      }
    }while(FindNextFile(hfind, &data) != 0 && scan_start);
    CloseHandle(hfind);
  }
}
//----------------------------------------------------------------
void CheckRecursivFilesFromSizeAndEM(DWORD iitem, char *remote_name, DWORD size, char *MD5, char *SHA256, BOOL recursif)
{
  WIN32_FIND_DATA data;
  char tmp_path[LINE_SIZE]="", tmp_remote_name[LINE_SIZE]="", date[MAX_PATH]="\0\0\0";

  //search
  HANDLE hfile, hfind;
  LARGE_INTEGER filesize;
  char s_sha[SHA256_SIZE]="",s_md5[MAX_PATH];
  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeModification;

  snprintf(tmp_path,LINE_SIZE,"%s\\*.*",remote_name);
  hfind = FindFirstFile(tmp_path, &data);
  if (hfind != INVALID_HANDLE_VALUE && scan_start)
  {
    do
    {
      if (data.cFileName[0] == '.' && (data.cFileName[1] == 0 || (data.cFileName[2] == 0 && data.cFileName[1] == '.')))continue;

      #ifdef DEBUG_MODE_FILES
      AddMsg(h_main,(char*)"DEBUG S(CheckRecursivFilesFromSizeAndEM)",remote_name,(char*)data.cFileName);
      #endif

      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && recursif)
      {
        snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,data.cFileName);

        #ifdef DEBUG_MODE_FILES
        AddMsg(h_main,(char*)"DEBUG D(CheckRecursivFilesFromSizeAndEM)",tmp_remote_name,(char*)"");
        #endif

        CheckRecursivFilesFromSizeAndEM(iitem, tmp_remote_name, size, MD5, SHA256, recursif);
        continue;
      }

      filesize.HighPart = data.nFileSizeHigh;
      filesize.LowPart  = data.nFileSizeLow;

      if (filesize.QuadPart == size)
      {
        snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,data.cFileName);

        //check MD5 or SHA256
        s_md5[0] = 0;
        s_sha[0] = 0;
        if (MD5 != NULL)
        {
          hfile = CreateFile(tmp_remote_name,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
          if (hfile != INVALID_HANDLE_VALUE)
          {
            FileToMd5(hfile, s_md5);
            CloseHandle(hfile);
          }
        }else if (SHA256 != NULL)
        {
          hfile = CreateFile(tmp_remote_name,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
          if (hfile != INVALID_HANDLE_VALUE)
          {
            FileToSHA256(hfile, s_sha);
            CloseHandle(hfile);
          }
        }

        //CheckFile(iitem, tmp_remote_name, &data);
        if (!MD5 && !SHA256)
        {
          date[0] = 0;
          FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
          FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
          snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d,Size:%do]"
                       ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                       ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond,size);

          snprintf(tmp_remote_name,LINE_SIZE,"%s %s;;;;",tmp_remote_name,date);
          AddMsg(h_main,(char*)"FOUND (File)",tmp_remote_name,(char*)"");
          AddLSTVUpdateItem(tmp_remote_name, COL_FILES, iitem);

        }else if (MD5 != NULL && compare_nocas(MD5,s_md5) && strlen(s_md5) > 0)
        {
          date[0] = 0;
          FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
          FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
          snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d,Size:%do]"
                       ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                       ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond,size);

          if (s_sha[0] != 0)
          {
            snprintf(tmp_remote_name,LINE_SIZE,"%s %s;MD5;%s;%s;%s",tmp_remote_name,date,s_md5,SHA1_enable?"SHA1":"SHA256",s_sha);
          }else
          {
            snprintf(tmp_remote_name,LINE_SIZE,"%s %s;MD5;%s;;",tmp_remote_name,date,s_md5);
          }
          AddMsg(h_main,(char*)"FOUND (File)",tmp_remote_name,(char*)"");
          AddLSTVUpdateItem(tmp_remote_name, COL_FILES, iitem);

        }else if (SHA256 != NULL && compare_nocas(SHA256,s_sha) && strlen(s_sha) > 0)
        {
          date[0] = 0;
          FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
          FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
          snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d,Size:%do]"
                       ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                       ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond,size);

          snprintf(tmp_remote_name,LINE_SIZE,"%s %s;;;%s;%s",tmp_remote_name,date,SHA1_enable?"SHA1":"SHA256",s_sha);
          AddMsg(h_main,(char*)"FOUND (File)",tmp_remote_name,(char*)"");
          AddLSTVUpdateItem(tmp_remote_name, COL_FILES, iitem);
        }
      }
    }while(FindNextFile(hfind, &data) != 0 && scan_start);
    FindClose(hfind);
  }
}
//----------------------------------------------------------------
void CheckFiles(DWORD iitem, char *remote_name, char *file)
{
  char tmp_path[LINE_SIZE]="";
  WIN32_FIND_DATA data;

  if (file[0] == ':')
  {
    #ifdef DEBUG_MODE_FILES
    AddMsg(h_main,(char*)"DEBUG (CheckFiles) FORMAT \":\"",file,(char*)remote_name);
    #endif

    char s_sha[SHA256_SIZE]="",s_md5[MAX_PATH]="", s_size[MAX_PATH]="";
    DWORD size = 0;

    //format= :size on octets:MD5 hash: SHA256 hash:
    char *c = file+1; //pass ':'
    char *d = s_size;
    if (*c == ':')return;
    while (*c && *c != ':')
    {
      *d++ = *c++;
    }
    *d = 0;
    size = atol(s_size);
    if (size < 1 )return;

    //MD5
    d = s_md5;
    c++;//pass ':'
    if (*c != ':')
    {
      while (*c && *c != ':')
      {
        *d++ = *c++;
      }
      *d = 0;
    }

    //SHA
    d = s_sha;
    c++;//pass ':'
    while (*c && *c != ':')
    {
      *d++ = *c++;
    }
    *d = 0;

    #ifdef DEBUG_MODE_FILES
    AddMsg(h_main,(char*)"DEBUG (CheckFiles)",remote_name,(char*)"");
    #endif

    CheckRecursivFilesFromSizeAndEM(iitem, remote_name, size, s_md5, s_sha, TRUE);
    return;
  }

  //check if file have % or ending with /
  long int position = Contient(file, "%");
  if (position > -1)
  {
    #ifdef DEBUG_MODE_FILES
    AddMsg(h_main,(char*)"DEBUG (CheckFiles) FORMAT \"%\"",file,(char*)remote_name);
    #endif

    //format : Windows\system32\%\host
    //loop on the same function
    char /*tmp_remote_name[LINE_SIZE],*/tmp_file[LINE_SIZE],tmp[LINE_SIZE];
    snprintf(tmp_file,LINE_SIZE,"%s",&file[position+1]);

    if (file[position] == '\\' || file[position] == '/')position--;
    if (file[position] == '%')position--;
    //if (file[position] == '\\' || file[position] == '/')position--;

    strncpy(tmp,file,LINE_SIZE);
    tmp[position] = 0;
    //snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,tmp);
    //snprintf(tmp_remote_name,LINE_SIZE,"%s\\",remote_name);

    //get all directory with recursivité
    #ifdef DEBUG_MODE_FILES
    AddMsg(h_main,(char*)"DEBUG (CheckFiles)",remote_name,(char*)tmp_file);
    #endif
    CheckRecursivFiles(iitem, remote_name, tmp_file, TRUE);
  }else
  {
    if (file[strlen(file)-1] == '\\' || file[strlen(file)-1] == '/') //directory
    {
      #ifdef DEBUG_MODE_FILES
      AddMsg(h_main,(char*)"DEBUG (CheckFiles) FORMAT \"Directory\"",file,(char*)remote_name);
      #endif
      //enumerate all file in the directory and sub directory
      char tmp_file[LINE_SIZE];
      strncpy(tmp_file,file,LINE_SIZE);
      tmp_file[strlen(tmp_file)-1] = 0;
      snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,tmp_file);
      #ifdef DEBUG_MODE_FILES
      AddMsg(h_main,(char*)"DEBUG (CheckFiles)",tmp_path,(char*)"");
      #endif
      CheckRecursivFiles(iitem, tmp_path, NULL, TRUE);
    }else
    {
      #ifdef DEBUG_MODE_FILES
      AddMsg(h_main,(char*)"DEBUG (CheckFiles) FORMAT \"File\"",file,(char*)remote_name);
      #endif
      //default
      snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
      #ifdef DEBUG_MODE_FILES
      AddMsg(h_main,(char*)"DEBUG (CheckFiles) GetFileAttributes \"File\"",tmp_path,(char*)"");
      #endif
      if (GetFileAttributes(tmp_path) != INVALID_FILE_ATTRIBUTES)
      {
        //file exist + date
        HANDLE hfind = FindFirstFile(tmp_path, &data);
        if (hfind != INVALID_HANDLE_VALUE)
        {
          #ifdef DEBUG_MODE_FILES
          AddMsg(h_main,(char*)"DEBUG (CheckFiles)",tmp_path,(char*)"");
          #endif
          CheckFile(iitem, tmp_path, &data);
          FindClose(hfind);
        }
      }
    }
  }
}

//----------------------------------------------------------------
BOOL RemoteAuthenticationFilesScan(DWORD iitem, char *ip, DWORD ip_id, char *remote_share, PSCANNE_ST config, long int *id_ok, DWORD id_cb, BOOL multi)
{
  //check file
  char remote_name[LINE_SIZE], msg[LINE_SIZE];
  snprintf(remote_name,LINE_SIZE,"\\\\%s\\%s",ip,remote_share);

  if (config->nb_accounts == 0)
  {
    NETRESOURCE NetRes  = {0};
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;

    char tmp_login[MAX_PATH]="";
    if (config->domain[0] != 0)
    {
      snprintf(tmp_login,MAX_PATH,"%s\\%s",config->domain,config->login);
    }else
    {
      if (!config->local_account)snprintf(tmp_login,MAX_PATH,"%s\\%s",ip,config->login);
    }

    if (WNetAddConnection2(&NetRes,config->password,tmp_login,CONNECT_PROMPT)==NO_ERROR)
    {
      snprintf(msg,LINE_SIZE,"%s\\%s with %s account.",ip,remote_share,tmp_login);
      AddMsg(h_main,(char*)"LOGIN (Files:NET)",msg,(char*)"");

      snprintf(msg,LINE_SIZE,"Login NET %s\\%s with %s account",ip,remote_share,tmp_login);
      AddLSTVUpdateItem(msg, COL_CONFIG, iitem);

      if (multi)CheckRecursivFilesList(iitem, remote_name, id_cb);
      else
      {
        //check file
        char file[LINE_SIZE];
        DWORD j=0, _nb_i = SendDlgItemMessage(h_main,id_cb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

        if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;

        for (;j<_nb_i && scan_start;j++)
        {
          if (SendDlgItemMessage(h_main,id_cb,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

          file[0] = 0;
          if (SendDlgItemMessage(h_main,id_cb,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
          {
            if (file)CheckFiles(iitem, remote_name, file);
          }
        }
      }
      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      return TRUE;
    }
  }else if (config->global_ip_file)
  {
    NETRESOURCE NetRes  = {0};
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;

    char tmp_login[MAX_PATH];
    if (config->accounts[ip_id].domain[0] != 0)
    {
      snprintf(tmp_login,MAX_PATH,"%s\\%s",config->accounts[ip_id].domain,config->accounts[ip_id].login);
    }else
    {
      snprintf(tmp_login,MAX_PATH,"%s\\%s",ip,config->accounts[ip_id].login);
    }
    if (WNetAddConnection2(&NetRes,config->accounts[ip_id].password,tmp_login,CONNECT_PROMPT)==NO_ERROR)
    {
      snprintf(msg,LINE_SIZE,"%s\\%s with %s (%02d) account.",ip,remote_share,tmp_login,ip_id);
      AddMsg(h_main,(char*)"LOGIN (Files:NET)",msg,(char*)"");

      snprintf(msg,LINE_SIZE,"Login NET %s\\%s with %s (%02d) account",ip,remote_share,tmp_login,ip_id);
      AddLSTVUpdateItem(msg, COL_CONFIG, iitem);

      if (multi)CheckRecursivFilesList(iitem, remote_name, id_cb);
      else
      {
        //check file
        char file[LINE_SIZE];
        DWORD j, _nb_i = SendDlgItemMessage(h_main,id_cb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

        if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;

        for (j=0;j<_nb_i && scan_start;j++)
        {
          if (SendDlgItemMessage(h_main,id_cb,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

          file[0] = 0;
          if (SendDlgItemMessage(h_main,id_cb,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
          {
            if (file)CheckFiles(iitem, remote_name, file);
          }
        }
      }
      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      return TRUE;
    }
  }else
  {
    unsigned int i;
    for (i=0; i<config->nb_accounts && scan_start ;i++)
    {
      NETRESOURCE NetRes  = {0};
      NetRes.dwScope      = RESOURCE_GLOBALNET;
      NetRes.dwType	      = RESOURCETYPE_ANY;
      NetRes.lpLocalName  = (LPSTR)"";
      NetRes.lpProvider   = (LPSTR)"";
      NetRes.lpRemoteName	= remote_name;

      char tmp_login[MAX_PATH];
      if (config->accounts[i].domain[0] != 0)
      {
        snprintf(tmp_login,MAX_PATH,"%s\\%s",config->accounts[i].domain,config->accounts[i].login);
      }else
      {
        snprintf(tmp_login,MAX_PATH,"%s\\%s",ip,config->accounts[i].login);
      }
      if (WNetAddConnection2(&NetRes,config->accounts[i].password,tmp_login,CONNECT_PROMPT)==NO_ERROR)
      {
        snprintf(msg,LINE_SIZE,"%s\\%s with %s (%02d) account.",ip,remote_share,tmp_login,i);
        AddMsg(h_main,(char*)"LOGIN (Files:NET)",msg,(char*)"");

        snprintf(msg,LINE_SIZE,"Login NET %s\\%s with %s (%02d) account",ip,remote_share,tmp_login,i);
        AddLSTVUpdateItem(msg, COL_CONFIG, iitem);

        if (multi)CheckRecursivFilesList(iitem, remote_name, id_cb);
        else
        {
          //check file
          char file[LINE_SIZE];
          DWORD j, _nb_i = SendDlgItemMessage(h_main,id_cb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

          if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;

          for (j=0;j<_nb_i && scan_start;j++)
          {
            if (SendDlgItemMessage(h_main,id_cb,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

            file[0] = 0;
            if (SendDlgItemMessage(h_main,id_cb,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
            {
              if (file)CheckFiles(iitem, remote_name, file);
            }
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
BOOL LocalFilesScan(DWORD iitem, PSCANNE_ST config, long int *id_ok, DWORD cb_id)
{
  char tmp[MAX_PATH], letter[3]=" :\0";
  int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);

  for (i=0;i<nblecteurs && scan_start;i+=4)
  {
    switch(GetDriveType(&tmp[i]))
    {
      /*case DRIVE_UNKNOWN:     break;
      case DRIVE_NO_ROOT_DIR: break;
      case DRIVE_CDROM:break;*/

      case DRIVE_FIXED:
      //case DRIVE_REMOTE:
      //case DRIVE_RAMDISK:
      //case DRIVE_REMOVABLE:
        {
          //check file
          char file[LINE_SIZE];
          DWORD j, _nb_i = SendDlgItemMessage(h_main,cb_id,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
          letter[0] = tmp[i];

          if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;
          #ifdef DEBUG_MODE_FILES
          AddMsg(h_main,(char*)"INFORMATION","START LOCAL FILE SEARCH",(char*)letter);
          #endif
          for (j=0;j<_nb_i && scan_start;j++)
          {
            if (SendDlgItemMessage(h_main,cb_id,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

            file[0] = 0;
            if (SendDlgItemMessage(h_main,cb_id,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
            {
              #ifdef DEBUG_MODE_FILES
              AddMsg(h_main,(char*)"DEBUG (LocalFilesScan)",file,(char*)letter);
              #endif
              if (file)CheckFiles(iitem, letter, file);
            }
          }
          #ifdef DEBUG_MODE_FILES
          AddMsg(h_main,(char*)"INFORMATION","END LOCAL FILE SEARCH",(char*)letter);
          #endif
        }
      break;
    }
  }
  return TRUE;
}
//----------------------------------------------------------------
void CheckListFile(DWORD iitem, char *file_path, char*filename, DWORD filesize, WIN32_FIND_DATA *data, DWORD cb_id)
{
  DWORD j, _nb_i = SendDlgItemMessage(h_main,cb_id,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  char file[LINE_SIZE], tmp[LINE_SIZE], date[MAX_PATH];

  HANDLE hfile;
  char s_sha[SHA256_SIZE]="",s_md5[MAX_PATH];
  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeModification;
  BOOL exist = FALSE;

  char ss_sha[SHA256_SIZE]="",ss_md5[MAX_PATH]="", s_size[MAX_PATH]="";
  DWORD d_size = 0;

  for (j=0;j<_nb_i && scan_start;j++)
  {
    if (SendDlgItemMessage(h_main,cb_id,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;
    if (SendDlgItemMessage(h_main,cb_id,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
    {
      exist = FALSE;
      //check if the file exist
      if (file[0] == ':') // hash type
      {
        ss_sha[0] = 0;
        ss_md5[0] = 0;
        s_size[0] = 0;
        d_size    = 0;

        //format= :size on octets:MD5 hash: SHA256 hash:
        char *c = file+1; //pass ':'
        char *d = s_size;
        if (*c == ':')continue;
        while (*c && *c != ':')
        {
          *d++ = *c++;
        }
        *d = 0;
        d_size = atol(s_size);
        if (d_size < 1 )continue;

        //MD5
        d = ss_md5;
        c++;//pass ':'
        if (*c != ':')
        {
          while (*c && *c != ':')
          {
            *d++ = *c++;
          }
          *d = 0;
        }

        //SHA
        d = ss_sha;
        c++;//pass ':'
        while (*c && *c != ':')
        {
          *d++ = *c++;
        }
        *d = 0;

        //check
        if (d_size == filesize)
        {
          //check all other datas
          s_md5[0] = 0;
          s_sha[0] = 0;
          if (ss_md5[0] != 0)
          {
            hfile = CreateFile(file_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              FileToMd5(hfile, s_md5);
              CloseHandle(hfile);
            }

            if (compare_nocas(s_md5,ss_md5))exist = TRUE;
          }

          if ((!exist && ss_md5[0] == 0) && (ss_sha[0] != 0))
          {
            hfile = CreateFile(file_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              FileToSHA256(hfile, s_sha);
              CloseHandle(hfile);
            }

            if (compare_nocas(s_sha,ss_sha))exist = TRUE;
          }

          if (!exist)
          {
            if (ss_md5[0] == 0 && ss_sha[0] == 0)exist = TRUE;
          }
        }
      }else //file type
      {
        if (compare_nocas(filename,file))
        {
          exist = TRUE;

          s_md5[0] = 0;
          s_sha[0] = 0;

          hfile = CreateFile(file_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
          if (hfile != INVALID_HANDLE_VALUE)
          {
            FileToMd5(hfile, s_md5);
            CloseHandle(hfile);

            hfile = CreateFile(file_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              FileToSHA256(hfile, s_sha);
              CloseHandle(hfile);
            }
          }
        }
      }

      if (exist)
      {
        date[0] = 0;
        FileTimeToLocalFileTime(&(data->ftLastWriteTime), &LocalFileTime);
        FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
        snprintf(date,MAX_PATH,"[Last_modification:%02d/%02d/%02d-%02d:%02d:%02d,Size:%do]"
                     ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                     ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond,filesize);

        snprintf(tmp,LINE_SIZE,"%s %s;MD5;%s;%s;%s",file_path,date,s_md5[0]==0?"":s_md5,SHA1_enable?"SHA1":"SHA256",s_sha[0]==0?"":s_sha);
        AddMsg(h_main,(char*)"FOUND (File)",tmp,(char*)"");
        AddLSTVUpdateItem(tmp, COL_FILES, iitem);
      }
    }
  }
}
//----------------------------------------------------------------
void CheckRecursivFilesList(DWORD iitem, char *remote_name, DWORD cb_id)
{
  WIN32_FIND_DATA data;
  LARGE_INTEGER filesize;
  char tmp_path[LINE_SIZE]="", tmp_remote_name[LINE_SIZE]="";

  snprintf(tmp_path,LINE_SIZE,"%s\\*.*",remote_name);
  HANDLE hfind = FindFirstFile(tmp_path, &data);

  if (hfind != INVALID_HANDLE_VALUE && scan_start)
  {
    do
    {
      if (data.cFileName[0] == '.' && (data.cFileName[1] == 0 || (data.cFileName[2] == 0 && data.cFileName[1] == '.')))continue;

      //directory
      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,data.cFileName);
        CheckRecursivFilesList(iitem, tmp_remote_name, cb_id);
        continue;
      }

      //file
      filesize.HighPart = data.nFileSizeHigh;
      filesize.LowPart  = data.nFileSizeLow;

      snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,data.cFileName);
      CheckListFile(iitem,tmp_remote_name,data.cFileName,filesize.QuadPart,&data,cb_id);

    }while(FindNextFile(hfind, &data) != 0 && scan_start);
    FindClose(hfind);
  }
}
//----------------------------------------------------------------
BOOL LocalFilesScanList(DWORD iitem, PSCANNE_ST config, DWORD cb_id)
{
  char tmp[MAX_PATH], letter[3]=" :\0";
  int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);

  for (i=0;i<nblecteurs && scan_start;i+=4)
  {
    switch(GetDriveType(&tmp[i]))
    {
      /*case DRIVE_UNKNOWN:     break;
      case DRIVE_NO_ROOT_DIR: break;
      case DRIVE_CDROM:break;*/

      case DRIVE_FIXED:
      //case DRIVE_REMOTE:
      //case DRIVE_RAMDISK:
      //case DRIVE_REMOVABLE:
        {
          letter[0] = tmp[i];
          CheckRecursivFilesList(iitem, letter, cb_id);
        }
      break;
    }
  }
  return TRUE;
}
//----------------------------------------------------------------
BOOL RemoteConnexionFilesScan(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, long int *id_ok)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","files:RemoteConnexionFilesScan",ip);
  #endif

  if (ipIsLoclahost(ip))
  {
    if (!LocalFilesScan(iitem, config, id_ok, CB_T_FILES))
      AddLSTVUpdateItem((char*)"LOCAL FILE SCAN FAIL!",COL_FILES,iitem);

    //localFileScanList!
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files (List)");
    if (!LocalFilesScanList(iitem, config, CB_T_MULFILES))
      AddLSTVUpdateItem((char*)"LOCAL FILE SCAN FAIL!",COL_FILES,iitem);

    return TRUE;
  }else
  {
    if(RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"C$", config, id_ok, CB_T_FILES, FALSE))
    {
      nb_files++;
      RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"D$", config, id_ok, CB_T_FILES, FALSE);
      RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"E$", config, id_ok, CB_T_FILES, FALSE);

      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files (List)");
      RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"C$", config, id_ok, CB_T_MULFILES, TRUE);
      RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"D$", config, id_ok, CB_T_MULFILES, TRUE);
      RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"E$", config, id_ok, CB_T_MULFILES, TRUE);

      return TRUE;
    }else AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_FILES,iitem);
  }
  return FALSE;
}

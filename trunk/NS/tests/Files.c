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
    for(i=0;i<16;i++)snprintf(md5+i*2,3,"%02X",digest[i]);
    md5[32]=0;
    HeapFree(GetProcessHeap(), 0,buffer);
  }
}
//----------------------------------------------------------------
void FileToSHA256(HANDLE Hfic, char *csha256)
{
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

  if (s_sha[0] != 0 && s_md5[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;MD5;%s;SHA256;%s",file,date,s_md5,s_sha);
  }else if (s_md5[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;MD5;%s;;",file,date,s_md5);
  }else if (s_sha[0] != 0)
  {
    snprintf(file,LINE_SIZE,"%s %s;;;SHA256;%s",file,date,s_sha);
  }else snprintf(file,LINE_SIZE,"%s %s;;;;",file,date);

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
void CheckFiles(DWORD iitem, char *remote_name, char *file)
{
  char tmp_path[LINE_SIZE]="";
  WIN32_FIND_DATA data;

  //check if file have % or ending with /
  long int position = Contient(file, "%");
  if (position > -1)
  {
    //format : Windows\system32\%\host
    //loop on the same function
    char tmp_remote_name[LINE_SIZE],tmp_file[LINE_SIZE],tmp[LINE_SIZE];
    snprintf(tmp_file,LINE_SIZE,"%s",&file[position+1]);

    if (file[position] == '\\' || file[position] == '/')position--;
    if (file[position] == '%')position--;
    //if (file[position] == '\\' || file[position] == '/')position--;

    strncpy(tmp,file,LINE_SIZE);
    tmp[position] = 0;
    snprintf(tmp_remote_name,LINE_SIZE,"%s\\%s",remote_name,tmp);

    //get all directory with recursivité
    CheckRecursivFiles(iitem, tmp_remote_name, tmp_file, TRUE);
  }else
  {
    if (file[strlen(file)-1] == '\\' || file[strlen(file)-1] == '/') //directory
    {
      //enumerate all file in the directory and sub directory
      char tmp_file[LINE_SIZE];
      strncpy(tmp_file,file,LINE_SIZE);
      tmp_file[strlen(tmp_file)-1] = 0;
      snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,tmp_file);
      CheckRecursivFiles(iitem, tmp_path, NULL, TRUE);
    }else
    {
      //default
      snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
      if (GetFileAttributes(tmp_path) != INVALID_FILE_ATTRIBUTES)
      {
        //file exist + date
        HANDLE hfind = FindFirstFile(tmp_path, &data);
        if (hfind != INVALID_HANDLE_VALUE)
        {
          CheckFile(iitem, tmp_path, &data);
          FindClose(hfind);
        }
      }
    }
  }
}

//----------------------------------------------------------------
BOOL RemoteAuthenticationFilesScan(DWORD iitem, char *ip, DWORD ip_id, char *remote_share, PSCANNE_ST config, long int *id_ok)
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

      //check file
      char file[LINE_SIZE];
      DWORD j=0, _nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

      if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;

      for (;j<_nb_i && scan_start;j++)
      {
        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
        {
          CheckFiles(iitem, remote_name, file);
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

      //check file
      char file[LINE_SIZE];
      DWORD j, _nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

      if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;

      for (j=0;j<_nb_i && scan_start;j++)
      {
        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
        {
          CheckFiles(iitem, remote_name, file);
        }
      }
      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      return TRUE;
    }
  }else
  {
    unsigned int i;
    for (i=0; i<config->nb_accounts ;i++)
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

        //check file
        char file[LINE_SIZE];
        DWORD j, _nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

        if (id_ok != NULL && *id_ok > ID_ERROR) j = *id_ok;

        for (j=0;j<_nb_i && scan_start;j++)
        {
          if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXTLEN,(WPARAM)j,(LPARAM)NULL) > LINE_SIZE)continue;

          if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)j,(LPARAM)file))
          {
            CheckFiles(iitem, remote_name, file);
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
BOOL RemoteConnexionFilesScan(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, long int *id_ok)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","files:RemoteConnexionFilesScan",ip);
  #endif

  if(RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"C$", config, id_ok))
  {
    nb_files++;
    RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"D$", config, id_ok);
    RemoteAuthenticationFilesScan(iitem, ip, ip_id, (char*)"E$", config, id_ok);

    return TRUE;
  }else AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_FILES,iitem);

  return FALSE;
}

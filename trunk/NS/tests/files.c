//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
#include "../crypt/sha2.h"
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
//----------------------------------------------------------------            //MD5
BOOL RemoteAuthenticationFilesScan(DWORD iitem, char *ip, char *remote_share, SCANNE_ST config)
{
  //check file
  char remote_name[LINE_SIZE], msg[LINE_SIZE];
  snprintf(remote_name,LINE_SIZE,"\\\\%s\\%s",ip,remote_share);

  NETRESOURCE NetRes  = {0};
  NetRes.dwScope      = RESOURCE_GLOBALNET;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = "";
  NetRes.lpProvider   = "";
  NetRes.lpRemoteName	= remote_name;

  if (WNetAddConnection2(&NetRes,config.mdp,config.mdp,CONNECT_PROMPT)==NO_ERROR)
  //if (WNetAddConnection2(&NetRes,config.mdp,config.mdp,CONNECT_TEMPORARY)==NO_ERROR)
  {
    snprintf(msg,LINE_SIZE,"Login (NET) in %s IP with %s account.",ip,config.login);
    AddMsg(h_main,"INFORMATION",msg,"");

    //check file
    char tmp_path[LINE_SIZE], file[LINE_SIZE], s_sha[SHA256_SIZE]="",s_md5[MAX_PATH];
    DWORD i, nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
    for (i=0;i<nb_i && scan_start;i++)
    {
      if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)i,(LPARAM)file))
      {
        snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","File:check",tmp_path);
        #endif
        HANDLE hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
        if (hfile != INVALID_HANDLE_VALUE)
        {
            //MD5
            AddMsg(h_main,"FOUND (File)",tmp_path,"CHECK MD5");
            FileToMd5(hfile, s_md5);
            CloseHandle(hfile);

            //SHA256
            hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              AddMsg(h_main,"FOUND (File)",tmp_path,"CHECK SHA256");
              FileToSHA256(hfile, s_sha);
              CloseHandle(hfile);
            }

            if (s_sha[0] != 0 && s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;SHA256;%s",remote_name,file,s_md5,s_sha);
            }else if (s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;;",remote_name,file,s_md5);
            }else if (s_sha[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;;;SHA256;%s",remote_name,file,s_sha);
            }

            AddMsg(h_main,"FOUND (File)",tmp_path,"");
            AddLSTVUpdateItem(tmp_path, COL_FILES, iitem);
        }
      }
    }
    WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    return TRUE;
  }else
  {
    HANDLE htoken = UserConnect(ip,config);
    if (htoken != NULL)
    {
      snprintf(msg,LINE_SIZE,"Login (LogonUser) in %s IP with %s\\%s account.",ip,config.domain,config.login);
      AddMsg(h_main,"INFORMATION",msg,"");

      //check file
      char tmp_path[LINE_SIZE], file[LINE_SIZE], s_sha[SHA256_SIZE]="",s_md5[MAX_PATH];
      DWORD i, nb_i = SendDlgItemMessage(h_main,CB_T_FILES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
      for (i=0;i<nb_i && scan_start;i++)
      {
        if (SendDlgItemMessage(h_main,CB_T_FILES,LB_GETTEXT,(WPARAM)i,(LPARAM)file))
        {
          snprintf(tmp_path,LINE_SIZE,"%s\\%s",remote_name,file);
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","File:check",tmp_path);
          #endif
          HANDLE hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
          if (hfile != INVALID_HANDLE_VALUE)
          {
            //MD5
            AddMsg(h_main,"FOUND (File)",tmp_path,"CHECK MD5");
            FileToMd5(hfile, s_md5);
            CloseHandle(hfile);

            //SHA256
            hfile = CreateFile(tmp_path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
            if (hfile != INVALID_HANDLE_VALUE)
            {
              AddMsg(h_main,"FOUND (File)",tmp_path,"CHECK SHA256");
              FileToSHA256(hfile, s_sha);
              CloseHandle(hfile);
            }

            if (s_sha[0] != 0 && s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;SHA256;%s",remote_name,file,s_md5,s_sha);
            }else if (s_md5[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;MD5;%s;;",remote_name,file,s_md5);
            }else if (s_sha[0] != 0)
            {
              snprintf(tmp_path,LINE_SIZE,"%s\\%s;;;SHA256;%s",remote_name,file,s_sha);
            }

            AddMsg(h_main,"FOUND (File)",tmp_path,"");
            AddLSTVUpdateItem(tmp_path, COL_FILES, iitem);
          }
        }
      }
      UserDisConnect(htoken);
    }else
    {
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","files:RemoteAuthenticationFilesScan:UserConnect=FAIL",ip);
      #endif
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
BOOL RemoteConnexionFilesScan(DWORD iitem,char *name, char *ip, SCANNE_ST config)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","files:RemoteConnexionFilesScan",ip);
  #endif
  BOOL ret = FALSE;

  if(RemoteAuthenticationFilesScan(iitem, ip, "C$", config))
  {
    RemoteAuthenticationFilesScan(iitem, ip, "D$", config);
    return TRUE;
  }
  return FALSE;
}

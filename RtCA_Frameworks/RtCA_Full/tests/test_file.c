//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addFiletoDB(char *path, char *file, char *extension,
                  char *Create_time, char *Modify_time, char *Access_Time,char *Size,
                  char *Owner, char *RID, char *SID, char *ACL,
                  char *Hidden, char *System, char *Archive, char *Encrypted, char *Tempory,
                  char *ADS, char *SAH256, char *VirusTotal, char *Description, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_file "
           "(path,file,extension,Create_time,Modify_time,Access_Time,Size,Owner,RID,SID,ACL,"
           "Hidden,System,Archive,Encrypted,Tempory,ADS,SAH256,VirusTotal,Description,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
            path,file,extension,Create_time,Modify_time,Access_Time,Size,Owner,RID,SID,ACL,
            Hidden,System,Archive,Encrypted,Tempory,ADS,SAH256,VirusTotal,Description,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
//from : http://rootkitanalytics.com/userland/Exploring-Alternate-Data-Streams.php
DWORD EnumADS(char *file, char *resultat, DWORD size)
{
  PVOID streamContext = 0;
  DWORD dwReadBytes, seek_high;
  WIN32_STREAM_ID streamHeader;
  WCHAR strStreamName[MAX_PATH];
  char strBuffer[MAX_PATH];
  char tmp_buffer[MAX_PATH];
  BOOL first = TRUE;
  DWORD nb_ads = 0;

  resultat[0]=0;

  HANDLE Hfic = CreateFile(file, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if( Hfic == INVALID_HANDLE_VALUE )return 0;

  while(1)
  {
    //check if we have reached the end of file....
    if ( FALSE == BackupRead(Hfic, (LPBYTE)&streamHeader, (LPBYTE)&streamHeader.cStreamName-(LPBYTE)&streamHeader, &dwReadBytes, FALSE, FALSE, &streamContext))break;

    //check if we have read the stream header properly...
    if( (long)dwReadBytes != (LPBYTE)&streamHeader.cStreamName-(LPBYTE)&streamHeader )
    break;

    //we are interested only in alternate data streams....
    if(streamHeader.dwStreamId == BACKUP_ALTERNATE_DATA)
    {
      if (streamHeader.dwStreamNameSize != 0 )
      {
        if( BackupRead(Hfic, (LPBYTE)strStreamName, streamHeader.dwStreamNameSize, &dwReadBytes, FALSE, FALSE, &streamContext))
        {
          strStreamName[streamHeader.dwStreamNameSize/2]=L'\0';

          //Reformat the stream file name ... :stream.txt:$DATA
          snprintf(strBuffer, MAX_PATH, "%S", &strStreamName[1]);
          char *ptr = strchr(strBuffer, ':');
          if( ptr != NULL )
          *ptr = '\0';

          if (first)
          {
            first = FALSE;
            snprintf(resultat,size,":%s",strBuffer);
          }else
          {
            strncpy(tmp_buffer,resultat,MAX_PATH);
            snprintf(resultat,size,"%s | :%s",tmp_buffer,strBuffer);
          }
          nb_ads++;
        }
      }
    }

    // jump to the next stream header
    if (BackupSeek(Hfic, ~0, ~0, &dwReadBytes, &seek_high, &streamContext) == FALSE)
    {
      //for any errors other than seek break out of loop
      if (GetLastError() != ERROR_SEEK)
      {
        // terminate BackupRead() loop
        BackupRead(Hfic, 0, 0, &dwReadBytes, TRUE, FALSE, &streamContext);
        break;
      }

      streamHeader.Size.QuadPart -= dwReadBytes;
      streamHeader.Size.HighPart -= seek_high;
      BYTE buffer[MAX_PATH];

      while(streamHeader.Size.QuadPart > 0)
      {
        if (dwReadBytes!=sizeof(buffer) || !BackupRead(Hfic, buffer, sizeof(buffer), &dwReadBytes, FALSE, FALSE, &streamContext))break;
        streamHeader.Size.QuadPart -= dwReadBytes;
      }
    }
  } //main while loop

  //Finally clean up the buffers use for seeking
  if (streamContext) BackupRead(Hfic, 0, 0, &dwReadBytes, TRUE, FALSE, &streamContext);

  CloseHandle(Hfic);
  return nb_ads;
}
//------------------------------------------------------------------------------
#include "../crypt/sha2.h"
void FileToSHA256(char *path, char *sha256)
{
  //ouverture du fichier en lecture partagé
  sha256[0]=0;
  HANDLE Hfic = CreateFile(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic != INVALID_HANDLE_VALUE)
  {
    DWORD taille_fic = GetFileSize(Hfic,NULL);
    if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
    {
      unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
      if (buffer == NULL)
      {
        CloseHandle(Hfic);
        return;
      }

      //lecture du fichier
      DWORD copiee, position = 0, increm = 0;
      if (taille_fic > DIXM)increm = DIXM;
      else increm = taille_fic;

      while (position<taille_fic && increm!=0)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
      {
        copiee = 0;
        ReadFile(Hfic, buffer+position, increm,&copiee,0);
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
      for(i=0;i<32;i++)snprintf(sha256+i*2,3,"%02x",digest[i]&0xFF);
      sha256[64]=0;
      HeapFree(GetProcessHeap(), 0,buffer);
    }
    CloseHandle(Hfic);
  }
}
//-----------------------------------------------------------------------------
void SidtoUser(PSID psid, char *user, char *rid, char *sid, unsigned int max_size)
{
  if (IsValidSid(psid))
  {
    DWORD saccount = 0;
    char* account = NULL;
    DWORD sdomain = 0;
    char* domain = NULL;
    SID_NAME_USE snu;

    //tentative de récupération du nom + domaine
    LookupAccountSid(NULL, psid, account, &saccount, domain, &sdomain, &snu);
    if (!saccount || !sdomain) return;
    account = (LPSTR)HeapAlloc(GetProcessHeap(), 0, saccount);
    domain = (LPSTR)HeapAlloc(GetProcessHeap(), 0, sdomain);

    if (!account || !domain) return;
    account[0]=0;domain[0]=0;

    if(LookupAccountSid(NULL, psid, account, &saccount, domain, &sdomain, &snu))
    {
      //user
      if (user!=NULL && domain[0]!=0 && account[0]!=0 && saccount && sdomain)snprintf(user,max_size,"%s\\%s ",domain,account);
      else user[0]=0;

      //rid
      PSID_IDENTIFIER_AUTHORITY t = GetSidIdentifierAuthority(psid);
      PUCHAR pcSubAuth = GetSidSubAuthorityCount(psid);
      DWORD *SidP;

      unsigned char i,ucMax = *pcSubAuth;
      if (ucMax>2)
      {
        SidP=GetSidSubAuthority(psid,ucMax-1);
        snprintf(rid,max_size,"%05lu",*SidP);
      }else rid[0]=0;

      //sid
      char tmp[MAX_PATH];
      snprintf(sid,MAX_PATH,"S-%d-%u",((SID*)psid)->Revision,t->Value[5]+ (t->Value[4]<<8) + (t->Value[3]<<16) + (t->Value[2]<<24));

      //sid
      for (i=0;i<ucMax;++i)
      {
        SidP=GetSidSubAuthority(psid,i);
        strncpy(tmp,sid,MAX_PATH);
        snprintf(sid,max_size,"%s-%lu",tmp,*SidP);
      }
    }

    HeapFree(GetProcessHeap(), 0, account);
    HeapFree(GetProcessHeap(), 0, domain);
  }
}
//-----------------------------------------------------------------------------
void GetOwner(char *file, char* owner,char *rid, char *sid, unsigned int size_max)
{
  owner[0]=0;
  rid[0]=0;
  sid[0]=0;

  DWORD ssd = 0;
  GetFileSecurity(file, OWNER_SECURITY_INFORMATION, NULL, 0, &ssd);
  if (ssd != 0)
  {
    PSECURITY_DESCRIPTOR psd = HeapAlloc(GetProcessHeap(), 0, ssd);
    if (!psd)return;

    if(GetFileSecurity(file, OWNER_SECURITY_INFORMATION, psd, ssd, &ssd))
    {
      PSID psid = NULL;
      BOOL pFlag = FALSE;
      if (GetSecurityDescriptorOwner(psd, &psid, &pFlag))
      {
        SidtoUser(psid, owner, rid, sid, size_max);
      }
    }
    HeapFree(GetProcessHeap(), 0, psd);
  }
}

//-----------------------------------------------------------------------------
void GetACLS(char *file, char *acls, char* owner,char *rid, char *sid, unsigned int size_max)
//void GetACLS(char *file, char *acls, char* owner, unsigned int res_taille_max, unsigned int prop_taille_max)
{
  //droits sur le fichier
  SECURITY_DESCRIPTOR *sd;
  unsigned long size_sd = 0;
  //récupération du descripteur sécurité
  GetFileSecurity(file, DACL_SECURITY_INFORMATION, 0, 0, &size_sd);
  if (acls && size_sd>0)
  {
    sd = (SECURITY_DESCRIPTOR *) HeapAlloc(GetProcessHeap(), 0, size_sd);
    if (sd != NULL)
    {
      if (GetFileSecurity(file, DACL_SECURITY_INFORMATION, sd, size_sd, &size_sd))
      {
        ACL *acl;
        int defaulted, present;
        //récupération des ACLS du descripteur
        if (GetSecurityDescriptorDacl(sd, &present, &acl, &defaulted))
        {
          //Information sur l'ACL
          ACL_SIZE_INFORMATION acl_size_info;
          if (acl != NULL)
          {
            if (GetAclInformation(acl, (void *) &acl_size_info, sizeof(acl_size_info), AclSizeInformation))
            {
              //traitement de l'affichage des ACLS
              unsigned int i;
              void *ace;
              SID *psid = NULL;
              int mask;

              for (i=0;i<acl_size_info.AceCount; i++)
              {
                //affichage d'une ACE :
                if (GetAce(acl, i, &ace))
                {
                  if (ace != NULL)
                  {
                    mask = 0;

                    //récupération des droits authorisés
                    if (((ACCESS_ALLOWED_ACE *)ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE){
                        mask = ((ACCESS_ALLOWED_ACE *)ace)->Mask;
                        psid  = (SID *) &((ACCESS_ALLOWED_ACE *)ace)->SidStart;
                    //récupération des droits refusés
                    }else if(((ACCESS_DENIED_ACE *)ace)->Header.AceType == ACCESS_DENIED_ACE_TYPE){
                        mask = ((ACCESS_DENIED_ACE *)ace)->Mask;
                        psid  = (SID *) &((ACCESS_DENIED_ACE *)ace)->SidStart;
                    }

                    //traitement pour affichage des droits
                    if (mask & FILE_GENERIC_READ) strncat(acls,"r",size_max); else strncat(acls,"-",size_max);
                    if (mask & FILE_GENERIC_WRITE) strncat(acls,"w",size_max); else strncat(acls,"-",size_max);
                    if (mask & FILE_GENERIC_EXECUTE) strncat(acls,"x",size_max); else strncat(acls,"-",size_max);

                    //traitement des droits étendus ^^
                    /*if (mask & FILE_EXECUTE)ad_r->b_FILE_EXECUTE = TRUE;
                    if (mask & FILE_READ_DATA)ad_r->b_FILE_READ_DATA = TRUE;
                    if (mask & FILE_READ_EA)ad_r->b_FILE_READ_EA = TRUE;
                    if (mask & FILE_WRITE_DATA)ad_r->b_FILE_WRITE_DATA = TRUE;
                    if (mask & FILE_WRITE_EA)ad_r->b_FILE_WRITE_EA = TRUE;
                    if (mask & FILE_READ_ATTRIBUTES)ad_r->b_FILE_READ_ATTRIBUTES = TRUE;
                    if (mask & FILE_WRITE_ATTRIBUTES)ad_r->b_FILE_WRITE_ATTRIBUTES = TRUE;
                    if (mask & FILE_APPEND_DATA)ad_r->b_FILE_APPEND_DATA = TRUE;
                    if (mask & STANDARD_RIGHTS_EXECUTE)ad_r->b_STANDARD_RIGHTS_EXECUTE = TRUE;
                    if (mask & STANDARD_RIGHTS_READ)ad_r->b_STANDARD_RIGHTS_READ = TRUE;
                    if (mask & STANDARD_RIGHTS_WRITE)ad_r->b_STANDARD_RIGHTS_WRITE = TRUE;
                    if (mask & SYNCHRONIZE)ad_r->b_SYNCHRONIZE = TRUE;*/

                    //compte associé au droit
                    char cuser[MAX_PATH]="", csid[MAX_PATH]="", crid[MAX_PATH]="";
                    SidtoUser(psid, cuser, crid, csid, MAX_PATH);

                    strncat(acls," ",size_max);
                    if (strlen(cuser)>0) strncat(acls,cuser,size_max);
                    if (strlen(csid)>0) strncat(acls,csid,size_max);
                    strncat(acls,"\r\n\0",size_max);
                  }
                }
              }
            }
          }
          HeapFree(GetProcessHeap(), 0, acl);
        }
      }
      HeapFree(GetProcessHeap(), 0, sd);
    }
  }

  //récupération du propriétaire du fichier
  DWORD ssd = 0;
  GetFileSecurity(file, OWNER_SECURITY_INFORMATION, NULL, 0, &ssd);
  if (ssd != 0)
  {
    PSECURITY_DESCRIPTOR psd = NULL;
    psd = HeapAlloc(GetProcessHeap(), 0, ssd);
    if (!psd)return;

    if(owner && GetFileSecurity(file, OWNER_SECURITY_INFORMATION, psd, ssd, &ssd))
    {
      PSID psid = NULL;
      BOOL pFlag = FALSE;
      GetSecurityDescriptorOwner(psd, &psid, &pFlag);

      char cuser[MAX_PATH]="",csid[MAX_PATH]="", crid[MAX_PATH]="";
      SidtoUser(psid, cuser, crid, csid, MAX_PATH);

      strncpy(owner,cuser,size_max);
      strncpy(sid,csid,size_max);
      strncpy(rid,crid,size_max);
    }
    HeapFree(GetProcessHeap(), 0, psd);
  }
}
/*
//new version : bug
//-----------------------------------------------------------------------------
void GetACLS(char *file, char *acls, char* owner,char *rid, char *sid, unsigned int size_max)
{
  //droits sur le fichier
  SECURITY_DESCRIPTOR *sd;
  unsigned long size_sd = 0;
  char buffer_tmp[MAX_PATH];

  char s_user[MAX_PATH],s_rid[MAX_PATH],s_sid[MAX_PATH];

  acls[0]=0;
  //récupération du descripteur sécurité
  GetFileSecurity(file, DACL_SECURITY_INFORMATION, 0, 0, &size_sd);
  if (acls && size_sd>0)
  {
    sd = (SECURITY_DESCRIPTOR *) HeapAlloc(GetProcessHeap(), 0, size_sd);
    if (sd != NULL)
    {
      if (GetFileSecurity(file, DACL_SECURITY_INFORMATION, sd, size_sd, &size_sd))
      {
        ACL *acl;
        int defaulted, present;
        //récupération des ACLS du descripteur
        if (GetSecurityDescriptorDacl(sd, &present, &acl, &defaulted))
        {
          //Information sur l'ACL
          ACL_SIZE_INFORMATION acl_size_info;
          if (acl != NULL)
          {
            if (GetAclInformation(acl, (void *) &acl_size_info, sizeof(acl_size_info), AclSizeInformation))
            {
              //traitement de l'affichage des ACLS
              unsigned int i;
              void *ace;
              SID *psid = NULL;
              int mask;

              for (i=0;i<acl_size_info.AceCount; i++)
              {
                //affichage d'une ACE :
                if (GetAce(acl, i, &ace))
                {
                  if (ace != NULL)
                  {
                    mask = 0;

                    //récupération des droits authorisés
                    if (((ACCESS_ALLOWED_ACE *)ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE){
                        mask = ((ACCESS_ALLOWED_ACE *)ace)->Mask;
                        psid  = (SID *) &((ACCESS_ALLOWED_ACE *)ace)->SidStart;
                    //récupération des droits refusés
                    }else if(((ACCESS_DENIED_ACE *)ace)->Header.AceType == ACCESS_DENIED_ACE_TYPE){
                        mask = ((ACCESS_DENIED_ACE *)ace)->Mask;
                        psid  = (SID *) &((ACCESS_DENIED_ACE *)ace)->SidStart;
                    }

                    //owner
                    s_user[0]=0;
                    s_rid[0]=0;
                    s_sid[0]=0;
                    SidtoUser(psid, s_user, s_rid, s_sid, MAX_PATH);

                    //traitement pour affichage des droits
                    snprintf(buffer_tmp,MAX_PATH,"%s%c%c%c%s%s\r\n",
                             acls,
                             mask & FILE_GENERIC_READ?'r':'-',
                             mask & FILE_GENERIC_WRITE?'w':'-',
                             mask & FILE_GENERIC_EXECUTE?'x':'-',
                             s_user,s_sid);
                  }
                }
              }
            }
          }
          HeapFree(GetProcessHeap(), 0, acl);
        }
      }
      HeapFree(GetProcessHeap(), 0, sd);
    }
  }

  GetOwner(file, owner, rid, sid, size_max);
}*/
//------------------------------------------------------------------------------
void scan_file_ex(char *path, BOOL acl, BOOL ads, BOOL sha, unsigned int session_id, sqlite3 *db)
{
  WIN32_FIND_DATA data;
  char tmp_path[MAX_PATH], path_ex[MAX_PATH], file[MAX_PATH];

  char CreationTime[DATE_SIZE_MAX],LastWriteTime[DATE_SIZE_MAX],LastAccessTime[DATE_SIZE_MAX];
  char size[DEFAULT_TMP_SIZE];
  char s_ads[MAX_PATH]="";
  char s_sha[65]="";
  char s_acl[MAX_PATH]="",owner[MAX_PATH]="",rid[MAX_PATH]="",sid[MAX_PATH]="";
  char ext[MAX_PATH]="";

  snprintf(tmp_path,MAX_PATH,"%s*.*",path);
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic == INVALID_HANDLE_VALUE)return;
  do
  {
    // return
    if(data.cFileName[0] == '.' && (data.cFileName[1] == 0 || data.cFileName[1] == '.')){}
    else
    {
        //dates
        filetimeToString_GMT(data.ftCreationTime, CreationTime, DATE_SIZE_MAX);
        filetimeToString_GMT(data.ftLastWriteTime, LastWriteTime, DATE_SIZE_MAX);
        filetimeToString_GMT(data.ftLastAccessTime, LastAccessTime, DATE_SIZE_MAX);

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          //directory
          snprintf(path_ex,MAX_PATH,"%s%s\\",path,data.cFileName);

          //ads
          if(ads)EnumADS(path_ex, s_ads, MAX_PATH);

          //acls
          if(acl)GetACLS(path_ex, s_acl, owner, rid, sid, MAX_PATH);

          //ad to bdd
          addFiletoDB(path_ex, "", "",
                      CreationTime, LastWriteTime, LastAccessTime,"",
                      owner, rid, sid, s_acl,
                      data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN?"H":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM?"S":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE?"A":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED?"E":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY?"T":"",
                      s_ads, "", "", "",session_id,db);

          scan_file_ex(path_ex, acl, ads, sha, session_id,db);
        }else
        {
          //file
          snprintf(file,MAX_PATH,"%s%s",path,data.cFileName);

          //size
          if ((data.nFileSizeLow+data.nFileSizeHigh) > 1099511627776)snprintf(size,DEFAULT_TMP_SIZE,"%u%uo (%uTo)",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)>>32),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)&0xFFFFFFFF),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1099511627776));
          else if (data.nFileSizeLow+data.nFileSizeHigh > 1073741824)snprintf(size,DEFAULT_TMP_SIZE,"%u%uo (%uGo)",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)>>32),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)&0xFFFFFFFF),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1073741824));
          else if (data.nFileSizeLow+data.nFileSizeHigh > 1048576)snprintf(size,DEFAULT_TMP_SIZE,"%uo (%uMo)",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1048576));
          else if (data.nFileSizeLow+data.nFileSizeHigh  > 1024)snprintf(size,DEFAULT_TMP_SIZE,"%uo (%uKo)",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1024));
          else snprintf(size,DEFAULT_TMP_SIZE,"%uo",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh));

          //ads
          if(ads)EnumADS(file, s_ads, MAX_PATH);

          //sha256
          if(sha)FileToSHA256(file, s_sha);

          //acl
          if(acl)GetACLS(file, s_acl, owner, rid, sid, MAX_PATH);

          //extension
          strncpy(file,data.cFileName,MAX_PATH);
          extractExtFromFile(charToLowChar(file), ext, MAX_PATH);

          //ad to bdd
          addFiletoDB(path, data.cFileName, ext,
                      CreationTime, LastWriteTime, LastAccessTime, size,
                      owner, rid, sid, s_acl,
                      data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN?"H":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM?"S":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE?"A":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED?"E":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY?"T":"",
                      s_ads, s_sha, "", "",session_id,db);
        }
    }
  }while(FindNextFile (hfic,&data) && start_scan);
}
//------------------------------------------------------------------------------
void scan_file_uniq(char *path, BOOL acl, BOOL ads, BOOL sha, unsigned int session_id, sqlite3 *db)
{
  WIN32_FIND_DATA data;

  char CreationTime[DATE_SIZE_MAX],LastWriteTime[DATE_SIZE_MAX],LastAccessTime[DATE_SIZE_MAX];
  char size[DEFAULT_TMP_SIZE];
  char s_ads[MAX_PATH]="";
  char s_sha[65]="";
  char s_acl[MAX_PATH]="",owner[MAX_PATH]="",rid[MAX_PATH]="",sid[MAX_PATH]="";
  char ext[MAX_PATH]="";

  HANDLE hfic = FindFirstFile(path, &data);
  if (hfic == INVALID_HANDLE_VALUE)return;
  do
  {
    // return
    if(data.cFileName[0] == '.' && (data.cFileName[1] == 0 || data.cFileName[1] == '.')){}
    else
    {
        //dates
        filetimeToString_GMT(data.ftCreationTime, CreationTime, DATE_SIZE_MAX);
        filetimeToString_GMT(data.ftLastWriteTime, LastWriteTime, DATE_SIZE_MAX);
        filetimeToString_GMT(data.ftLastAccessTime, LastAccessTime, DATE_SIZE_MAX);

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          //ads
          if(ads)EnumADS(path, s_ads, MAX_PATH);

          //acls
          if(acl)GetACLS(path, s_acl, owner, rid, sid, MAX_PATH);

          //ad to bdd
          addFiletoDB(path, "", "",
                      CreationTime, LastWriteTime, LastAccessTime,"",
                      owner, rid, sid, s_acl,
                      data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN?"H":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM?"S":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE?"A":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED?"E":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY?"T":"",
                      s_ads, "", "", "",session_id,db);
        }else
        {
          //size
          if ((data.nFileSizeLow+data.nFileSizeHigh) > 1099511627776)snprintf(size,DEFAULT_TMP_SIZE,"%u%uo (%uTo)",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)>>32),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)&0xFFFFFFFF),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1099511627776));
          else if (data.nFileSizeLow+data.nFileSizeHigh > 1073741824)snprintf(size,DEFAULT_TMP_SIZE,"%u%uo (%uGo)",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)>>32),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)&0xFFFFFFFF),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1073741824));
          else if (data.nFileSizeLow+data.nFileSizeHigh > 1048576)snprintf(size,DEFAULT_TMP_SIZE,"%uo (%uMo)",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1048576));
          else if (data.nFileSizeLow+data.nFileSizeHigh  > 1024)snprintf(size,DEFAULT_TMP_SIZE,"%uo (%uKo)",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh),(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1024));
          else snprintf(size,DEFAULT_TMP_SIZE,"%uo",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh));

          //ads
          if(ads)EnumADS(path, s_ads, MAX_PATH);

          //sha256
          if(sha)FileToSHA256(path, s_sha);

          //acl
          if(acl)GetACLS(path, s_acl, owner, rid, sid, MAX_PATH);

          //extension
          extractExtFromFile(charToLowChar(path), ext, MAX_PATH);

          //ad to bdd
          addFiletoDB(path, data.cFileName, ext,
                      CreationTime, LastWriteTime, LastAccessTime, size,
                      owner, rid, sid, s_acl,
                      data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN?"H":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM?"S":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE?"A":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED?"E":"",
                      data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY?"T":"",
                      s_ads, s_sha, "", "",session_id,db);
        }
    }
  }while(FindNextFile (hfic,&data) && start_scan);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_files(LPVOID lParam)
{
  //read list of disk
  char tmp[MAX_PATH];
  int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);
  unsigned int session_id = current_session_id;

  //db
  sqlite3 *db = (sqlite3 *)db_scan;

  //get local path !
  //get child
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
  if (!LOCAL_SCAN)
  {
    if (hitem == NULL)//get all files infos !!!
    {
      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      while(hitem!=NULL)
      {
        //get item txt
        GetTextFromTrv(hitem, tmp, MAX_PATH);
        scan_file_uniq(tmp, FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);

        hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
      }

      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
      while(hitem!=NULL)
      {
        //get item txt
        GetTextFromTrv(hitem, tmp, MAX_PATH);
        scan_file_uniq(tmp, FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);

        hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
      }

      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
      while(hitem!=NULL)
      {
        //get item txt
        GetTextFromTrv(hitem, tmp, MAX_PATH);
        scan_file_uniq(tmp, FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);

        hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
      }
    }else
    {
      while(hitem!=NULL)
      {
        //get item txt
        GetTextFromTrv(hitem, tmp, MAX_PATH);
        scan_file_ex(tmp, FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);

        hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
      }
    }
  }else
  {
    //search
    for (i=0;i<nblecteurs;i+=4)
    {
      switch(GetDriveType(&tmp[i]))
      {
        case DRIVE_FIXED:
        case DRIVE_REMOTE:
        case DRIVE_RAMDISK:
        case DRIVE_REMOVABLE:
          //for each scan it
          scan_file_ex(&tmp[i], FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);
        break;
      }
    }
  }
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

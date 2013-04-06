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
                  char *Owner, char *RID, char *sid, char *ACL,
                  char *Hidden, char *System, char *Archive, char *Encrypted, char *Tempory,
                  char *ADS, char *SAH256, char *VirusTotal, char *Description, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_file "
           "(path,file,extension,Create_time,Modify_time,Access_Time,Size,Owner,RID,SId,ACL,"
           "Hidden,System,Archive,Encrypted,Tempory,ADS,SAH256,VirusTotal,Description,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
            path,file,extension,Create_time,Modify_time,Access_Time,Size,Owner,RID,sid,ACL,
            Hidden,System,Archive,Encrypted,Tempory,ADS,SAH256,VirusTotal,Description,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
//src : http://code.google.com/p/liblnk/
void ReadLNKInfos(char *file, char*create_time,char*last_access_time,char*last_modification_time,char*local_path,char*to)
{
  typedef struct
  {
    unsigned int header_size;         // default : 0x0000004C (76)
    unsigned char lnk_class_id[16];   // default : 00021401-0000-0000-00c0-000000000046
    unsigned int data_flag;
    unsigned int file_attribute_flag;
    FILETIME create_time;
    FILETIME last_access_time;
    FILETIME last_modification_time;
    unsigned int file_size;
    unsigned int icon_index;
    unsigned int show_window_value;
    unsigned short hot_key;
    unsigned char reserved[10];
  }LNK_STRUCT, *PLNK_STRUCT;

  if (create_time != NULL)create_time[0]                        = 0;
  if (last_access_time != NULL)last_access_time[0]              = 0;
  if (last_modification_time != NULL)last_modification_time[0]  = 0;
  if (local_path != NULL)local_path[0]                          = 0;
  if (to != NULL)to[0]                                          = 0;


  HANDLE Hfic = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic != INVALID_HANDLE_VALUE)
  {
    DWORD taille_fic = GetFileSize(Hfic,NULL);
    if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
    {
      unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
      if (buffer != NULL)
      {
        DWORD copiee = 0;
        ReadFile(Hfic, buffer, taille_fic,&copiee,0);
        if (copiee>0)
        {
          //header
          PLNK_STRUCT p = buffer;

          //get times
          if (create_time != NULL)filetimeToString_GMT(p->create_time, create_time, DATE_SIZE_MAX);
          if (last_access_time != NULL)filetimeToString_GMT(p->last_access_time, last_access_time, DATE_SIZE_MAX);
          if (last_modification_time != NULL)filetimeToString_GMT(p->last_modification_time, last_modification_time, DATE_SIZE_MAX);

          //get path !!!
          char *c = buffer+taille_fic;

          //search 0x00002500 => start of local path
          while (c-4 != buffer && *c != 0x00 && *(c-1)!=0x00 && *(c-2)!='%' && *(c-3)!=0x00 && *(c-4)!=0x00)c--;
          char mlocal_path[MAX_PATH]="";
          snprintf(mlocal_path,MAX_PATH,"%s",c+1);
          if (local_path != NULL)strcpy(local_path,mlocal_path);

          //to
          if (to != NULL)snprintf(to,MAX_PATH,"%s",c+2+strlen(local_path));
        }
        HeapFree(GetProcessHeap(), 0,buffer);
      }
    }
  }
  CloseHandle(Hfic);
}

//------------------------------------------------------------------------------
#define MAGIC_NUMBER_SIZE 16
void ReadMagicNumber(char *file, char *magicnumber, unsigned short magicnumber_size_max)
{
  if (magicnumber != NULL)
  {
    magicnumber[0] = 0;
    char datas[MAGIC_NUMBER_SIZE+1] = "";

    //read 32first octets if possible
    HANDLE Hfic = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
    if (Hfic != INVALID_HANDLE_VALUE)
    {
      DWORD taille_fic = GetFileSize(Hfic,NULL);
      if (taille_fic > 0)
      {
        unsigned short toread = MAGIC_NUMBER_SIZE;
        if (taille_fic<toread)toread = taille_fic-1;
        {
          //read datas
          DWORD copiee = 0;
          ReadFile(Hfic, datas, toread,&copiee,0);

          if (copiee>0)
          {
            //search mime type !!!
            //http://www.garykessler.net/library/file_sigs.html
            struct
            {
              union
              {
                unsigned char uc;             //1
                unsigned short us;            //2
                unsigned int ui;              //4
                unsigned long int uli;        //8
                unsigned long long int ulli;  //16
              };
            }*s_datas = datas;

            //movies
            if (s_datas->uli == 0x7079746614000000) strcpy(magicnumber,"Movie (MOV/MP4)");
            else if (datas[0] == 0x00 && datas[1] == 0x00 && datas[2] == 0x01 && datas[3]&0xB0 == 0xB0) strcpy(magicnumber,"Movie (MPG, MPEG, VOB/DVD)");
            else if (s_datas->ui == 0x464D522E) strcpy(magicnumber,"Movie (RMVB/RA)");
            else if (s_datas->ui == 0x01564C46) strcpy(magicnumber,"Movie (FLV)");
            //else if (s_datas->ui == 0x46464952) strcpy(magicnumber,"Movie (AVI)");
            else if (s_datas->ui == 0x766F6F6D) strcpy(magicnumber,"Movie (MOV)");
            else if (s_datas->ulli == 0x616B736F7274616D88824293A3DF451A) strcpy(magicnumber,"Movie (MKV)");

            //audio
            else if (s_datas->uli == 0x11CF668E75B22630) strcpy(magicnumber,"Audio (ASF, WMA, WMV)");
            else if (datas[0] == 0x49 && datas[1] == 0x44 && datas[2] == 0x33) strcpy(magicnumber,"Audio (MP3)");
            else if (s_datas->ui == 0x6468544D) strcpy(magicnumber,"Audio (MID/MIDI)");
            else if (s_datas->uli == 0x000002005367674F) strcpy(magicnumber,"Audio (OGG)");
            else if (s_datas->ui == 0x46464952) strcpy(magicnumber,"Audio/Movie (WAV/AVI)");

            //image
            else if (s_datas->us == 0x4D42) strcpy(magicnumber,"Image (BMP)");
            else if (s_datas->ui == 0x38464947) strcpy(magicnumber,"Image (GIF)");
            else if (s_datas->ui == 0x474E5089) strcpy(magicnumber,"Image (PNG)");
            else if (s_datas->ui == 0x00010000) strcpy(magicnumber,"Image (ICO)");
            else if (s_datas->ui == 0xE1FFD8FF || s_datas->ui == 0xE0FFD8FF) strcpy(magicnumber,"Image (JPG)");

            //office
            else if (s_datas->uli == 0x000560000008190) strcpy(magicnumber,"Office MS (XLS)");
            else if (s_datas->ui == 0x434F440D || s_datas->ui == 0x002DA5DB || s_datas->ui == 0x00C1A5EC || s_datas->uli == 0x00E11AB1A1E011CF) strcpy(magicnumber,"Office MS (DOC)");
            else if (s_datas->ui == 0x03E8000F ||s_datas->ui == 0x0F1D46A0) strcpy(magicnumber,"Office MS (PPT)");
            else if (s_datas->ui == 0x46445025) strcpy(magicnumber,"Office (PDF)");
            else if (s_datas->uli == 0x0006001404034B50) strcpy(magicnumber,"Office (DOCX, PPTX, XLSX)");
            else if (s_datas->uli == 0xE11AB1A1E011CFD0) strcpy(magicnumber,"Office (DOC, PPT, XLS)");

            //compressé
            else if (s_datas->us == 0x8B1F) strcpy(magicnumber,"Archive (GZ, TGZ, GZIP)");
            else if (s_datas->us == 0x9D1F) strcpy(magicnumber,"Archive (TAR.Z)");
            else if (s_datas->us == 0xA01F) strcpy(magicnumber,"Archive (TAR.Z)");
            else if (s_datas->us == 0x6C2D) strcpy(magicnumber,"Archive (LHA, LZH)");
            else if (s_datas->ui == 0xAFBC7A37) strcpy(magicnumber,"Archive (7zip)");
            else if (s_datas->us == 0x5A42) strcpy(magicnumber,"Archive (BZ2, TAR.BZ2, TBZ2, TB2)");
            else if (s_datas->ui == 0x30304443) strcpy(magicnumber,"Archive (ISO)");
            else if (s_datas->ui == 0x4F534943) strcpy(magicnumber,"Archive (Compresed ISO : CSO)");
            else if (s_datas->ui == 0x28635349) strcpy(magicnumber,"Archive (CAB, HDR)");
            else if (s_datas->ui == 0x04034B50 || s_datas->ui == 0x5A6E6957 || s_datas->us == 0x4B50) strcpy(magicnumber,"Archive (ZIP)");
            else if (s_datas->ui == 0x21726152) strcpy(magicnumber,"Archive (RAR/ZIP)");
            else if (s_datas->us == 0xEA60) strcpy(magicnumber,"Archive (JAR)");
            else if (s_datas->ui == 0x61747375) strcpy(magicnumber,"Archive (TAR)");
            else if (s_datas->ui == 0x4643534D) strcpy(magicnumber,"Archive (CAB)");

            //crypt
            else if (datas[0] == 0x41 && datas[1] == 0x45 &&  datas[2] == 0x53) strcpy(magicnumber,"Crypt (AES)");
            else if (s_datas->uc == 0x99) strcpy(magicnumber,"Crypt (GPG)");

            //special
            else if (s_datas->uli == 0x4143435300000011) strcpy(magicnumber,"System Windows prefetch (PF)");
            else if (s_datas->uli == 0x654C664C00000030) strcpy(magicnumber,"System Windows log (EVT)");
            else if (s_datas->ui == 0x46666C45) strcpy(magicnumber,"System Windows log (EVTX)");
            else if (s_datas->ui == 0x00035F3F || s_datas->ui == 0x00024E4C) strcpy(magicnumber,"System Windows help (HLP)");
            else if (s_datas->ui == 0x46535449) strcpy(magicnumber,"System Windows help (CHM)");
            else if (s_datas->ui == 0x65696C43) strcpy(magicnumber,"Navigateur IE history (DAT)");
            else if (s_datas->ui == 0x45474552) strcpy(magicnumber,"System Windows Registre (REG)");
            else if (s_datas->ui == 0x66676572) strcpy(magicnumber,"System Windows Registre (DAT)");
            else if (s_datas->ui == 0xFFFFFFFD) strcpy(magicnumber,"System Windows (Thumbs.db)");
            else if (s_datas->ui == 0x6974227B) strcpy(magicnumber,"Navigateur bookmark Firefox (JSON)");
            else if (s_datas->ui == 0x0000004C) strcpy(magicnumber,"System Windows link (LNK)");
            else if (s_datas->ui == 0x68532E5B) strcpy(magicnumber,"System Windows config file (INI)");
            else if (s_datas->ui == 0x000DFEFF) strcpy(magicnumber,"System Windows config file (INF)");

            //bdd
            else if (s_datas->ui == 0x694C5153) strcpy(magicnumber,"Bdd (SQLITE)");
            else if (s_datas->ui == 0xF905D5D9) strcpy(magicnumber,"Bdd (sqlite-journal)");

            //Executable
            else if (s_datas->ui == 0x4352414A) strcpy(magicnumber,"Executable (JAR)");
            else if (s_datas->uli == 0x0008001404034B50) strcpy(magicnumber,"Executable (JAR)");
            else if (s_datas->us == 0x5A4D || s_datas->ui == 0x54202124) strcpy(magicnumber,"Executable (COM, DLL, DRV, EXE, PIF, QTS, QTX, SYS)"); //32bits

            //code
            //else if (s_datas->ui == 0x6D783F3C) strcpy(magicnumber,"Web page (XML)");
            else if (s_datas->ui == 0x4F44213C || s_datas->ui == 0x44213C0A) strcpy(magicnumber,"Web page (HTML)");

            //or
            //else snprintf(magicnumber,magicnumber_size_max,"MagicNumber: %02X%02X%02X%02X",datas[0]&0xff,datas[1]&0xff,datas[2]&0xff,datas[3]&0xff);
            //test
            else snprintf(magicnumber,magicnumber_size_max,"Unknow MagicNumber: %02X%02X%02X%02X",datas[3]&0xff,datas[2]&0xff,datas[1]&0xff,datas[0]&0xff);
          }
        }
      }
    }
    CloseHandle(Hfic);
  }
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

  HANDLE Hfic = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
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
void FileToSHA256(char *path, char *csha256)
{
  //ouverture du fichier en lecture partagé
  csha256[0]=0;
  HANDLE Hfic = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
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
      for(i=0;i<32;i++)snprintf(csha256+i*2,3,"%02x",digest[i]&0xFF);
      csha256[64]=0;
      HeapFree(GetProcessHeap(), 0,buffer);
    }
    CloseHandle(Hfic);
  }
}
//-----------------------------------------------------------------------------
void SidtoUser(PSID psid, char *user, char *rid, char *sid, unsigned int max_size)
{
  if (psid == NULL) return;
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
        if (*SidP<65535) snprintf(rid,max_size,"%05lu",*SidP);
        else rid[0]=0;
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
{
  //droits sur le fichier
  SECURITY_DESCRIPTOR *sd;
  DWORD size_sd = 0, size_sd2 = 0;
  char cuser[MAX_PATH]="", csid[MAX_PATH]="", crid[MAX_PATH]="";

  GetFileSecurity(file, DACL_SECURITY_INFORMATION, 0, 0, &size_sd);
  if (acls && size_sd)
  {
    sd = HeapAlloc(GetProcessHeap(), 0, size_sd+1);
    if (sd != NULL)
    {
      if (GetFileSecurity(file, DACL_SECURITY_INFORMATION, sd, size_sd, &size_sd2))
      {
        if (size_sd2)
        {
          ACL *acl;
          BOOL present   = FALSE,
               defaulted = FALSE;
          //ACL descriptor
          if (GetSecurityDescriptorDacl(sd, &present, &acl, &defaulted))
          {
            //Infos
            ACL_SIZE_INFORMATION acl_size_info;
            if (acl != NULL)
            {
              if (GetAclInformation(acl, (void *) &acl_size_info, sizeof(acl_size_info), AclSizeInformation))
              {
                unsigned int i;
                void *ace;
                SID *psid = NULL;
                int mask;

                for (i=0;i<acl_size_info.AceCount; i++)
                {
                  //view ACE :
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

                      cuser[0] = 0;
                      csid[0]  = 0;
                      crid[0]  = 0;
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
          }
        }
      }
      HeapFree(GetProcessHeap(), 0, sd);
    }
  }

  //Owner infos
  if(owner == NULL) return;
  owner[0] = 0;
  rid[0]   = 0;
  sid[0]   = 0;

  DWORD ssd = 0, ssd2 = 0;
  GetFileSecurity(file, OWNER_SECURITY_INFORMATION, 0, 0, &ssd);
  if (ssd != 0)
  {
    PSECURITY_DESCRIPTOR psd = NULL;
    psd = HeapAlloc(GetProcessHeap(), 0, ssd+1);
    if (psd == NULL)return;

    if(GetFileSecurity(file, OWNER_SECURITY_INFORMATION, psd, ssd, &ssd2))
    {
      if (ssd2)
      {
        PSID psid  = NULL;
        BOOL pFlag = FALSE;
        if(GetSecurityDescriptorOwner(psd, &psid, &pFlag))
        {
          SidtoUser(psid, cuser, crid, csid, MAX_PATH);
          strncpy(owner,cuser,size_max);
          strncpy(rid,crid,size_max);
          strncpy(sid,csid,size_max);
        }
      }
    }
    HeapFree(GetProcessHeap(), 0, psd);
  }
}
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
  char magi_number[DEFAULT_TMP_SIZE]="";

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

        s_ads[0] = 0;
        s_acl[0] = 0;
        s_sha[0] = 0;

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          //directory
          snprintf(path_ex,MAX_PATH,"%s%s\\",path,data.cFileName);

          if (!sha && !CONSOL_ONLY)SendMessage(GetDlgItem(h_conf,DLG_CONF_SB),SB_SETTEXT,0, (LPARAM)path_ex);

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
          if(sha)
          {
            if(!CONSOL_ONLY)SendMessage(GetDlgItem(h_conf,DLG_CONF_SB),SB_SETTEXT,0, (LPARAM)file);
            FileToSHA256(file, s_sha);
          }

          //acl
          if(acl)GetACLS(file, s_acl, owner, rid, sid, MAX_PATH);

          //file magic number
          if(enable_magic)ReadMagicNumber(file, magi_number, DEFAULT_TMP_SIZE);

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
                      s_ads, s_sha, "", magi_number,session_id,db);
        }
    }
  }while(FindNextFile (hfic,&data) && start_scan);
}
//------------------------------------------------------------------------------
void scan_file_exF(char *path, BOOL acl, BOOL ads, BOOL sha, unsigned int session_id, sqlite3 *db)
{
  WIN32_FIND_DATA data;
  char tmp_path[MAX_PATH], path_ex[MAX_PATH], file[MAX_PATH];

  char CreationTime[DATE_SIZE_MAX],LastWriteTime[DATE_SIZE_MAX],LastAccessTime[DATE_SIZE_MAX];
  char size[DEFAULT_TMP_SIZE];
  char s_ads[MAX_PATH]="";
  char s_sha[65]="";
  char s_acl[MAX_PATH]="",owner[MAX_PATH]="",rid[MAX_PATH]="",sid[MAX_PATH]="";
  char ext[MAX_PATH]="";
  char magi_number[DEFAULT_TMP_SIZE]="";

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

        s_ads[0] = 0;
        s_acl[0] = 0;
        s_sha[0] = 0;

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
          //directory
          snprintf(path_ex,MAX_PATH,"%s%s\\",path,data.cFileName);

          if (!sha && !CONSOL_ONLY)SendMessage(GetDlgItem(h_conf,DLG_CONF_SB),SB_SETTEXT,0, (LPARAM)path_ex);

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
          if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
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
          if(sha)
          {
            if(!CONSOL_ONLY)SendMessage(GetDlgItem(h_conf,DLG_CONF_SB),SB_SETTEXT,0, (LPARAM)file);
            FileToSHA256(file, s_sha);
          }


          //acl
          if(acl)GetACLS(file, s_acl, owner, rid, sid, MAX_PATH);

          //file magic number
          if(enable_magic)ReadMagicNumber(file, magi_number, DEFAULT_TMP_SIZE);

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
                      s_ads, s_sha, "", magi_number,session_id,db);
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
  char magi_number[DEFAULT_TMP_SIZE]="";

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

          //file magic number
          if(enable_magic)ReadMagicNumber(path, magi_number, DEFAULT_TMP_SIZE);

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
                      s_ads, s_sha, "", magi_number,session_id,db);
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
  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  if (!LOCAL_SCAN)
  {
    HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
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
        scan_file_exF(tmp, FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);

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
          scan_file_exF(&tmp[i], FILE_ACL, FILE_ADS, FILE_SHA, session_id,db);
        break;
      }
    }
  }

  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  if (!CONSOL_ONLY)check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}

//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//-----------------------------------------------------------------------------
void SidtoUser(PSID psid, char *user, char *sid, unsigned int taille_user,unsigned int taille_sid, BOOL onlySID)
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
      //generation du SID :
      PSID_IDENTIFIER_AUTHORITY t = GetSidIdentifierAuthority(psid);
      char tmp[MAX_PATH],tmp1[MAX_PATH];
      snprintf(tmp,MAX_PATH,"S-%d-%u",((SID*)psid)->Revision,t->Value[5]+ (t->Value[4]<<8) + (t->Value[3]<<16) + (t->Value[2]<<24));

      PUCHAR pcSubAuth = GetSidSubAuthorityCount(psid);
      unsigned char i,ucMax = *pcSubAuth;
      DWORD *SidP;

      for (i=0;i<ucMax;++i)
      {
        SidP=GetSidSubAuthority(psid,i);
        snprintf(tmp1,MAX_PATH,"-%lu",*SidP);
        strncat(tmp,tmp1,MAX_PATH);
      }
      strncat(tmp,"\0",MAX_PATH);

      if (domain[0]!=0 && account[0]!=0 && saccount && sdomain)snprintf(user,taille_user,"%s\\%s ",domain,account);
      else user[0]=0;
      if (sid!=NULL)
      {
        if (onlySID)
         snprintf(sid,taille_sid,"%s",tmp);
        else
         snprintf(sid,taille_sid,"SID : %s",tmp);
      }
    }

    HeapFree(GetProcessHeap(), 0, account);
    HeapFree(GetProcessHeap(), 0, domain);
  }
}
//-----------------------------------------------------------------------------
void GetACLS(char *file, char *acls, unsigned int res_taille_max, char* proprio, unsigned int prop_taille_max)
{
  //récupération du propriétaire du fichier
  DWORD ssd = 0;
  GetFileSecurity(file, OWNER_SECURITY_INFORMATION, NULL, 0, &ssd);
  if (ssd != 0)
  {
    PSECURITY_DESCRIPTOR psd = NULL;
    psd = HeapAlloc(GetProcessHeap(), 0, ssd);
    if (!psd)return;

    if(GetFileSecurity(file, OWNER_SECURITY_INFORMATION, psd, ssd, &ssd))
    {
      PSID psid = NULL;
      BOOL pFlag = FALSE;
      GetSecurityDescriptorOwner(psd, &psid, &pFlag);

      char cuser[MAX_PATH]="",csid[MAX_PATH]="";
      SidtoUser(psid, cuser, csid, MAX_PATH,MAX_PATH,FALSE);

      strncpy(proprio,cuser,prop_taille_max);
      strncat(proprio,csid,prop_taille_max);
      strncat(proprio,"\0",prop_taille_max);
    }
    HeapFree(GetProcessHeap(), 0, psd);
  }

  //droits sur le fichier
  SECURITY_DESCRIPTOR *sd;
  unsigned long size_sd = 0;
  //récupération du descripteur sécurité
  GetFileSecurity(file, DACL_SECURITY_INFORMATION, 0, 0, &size_sd);
  if (size_sd>0)
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
                    if (mask & FILE_GENERIC_READ) strncat(acls,"r",res_taille_max); else strncat(acls,"-",res_taille_max);
                    if (mask & FILE_GENERIC_WRITE) strncat(acls,"w",res_taille_max); else strncat(acls,"-",res_taille_max);
                    if (mask & FILE_GENERIC_EXECUTE) strncat(acls,"x",res_taille_max); else strncat(acls,"-",res_taille_max);

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
                    char cuser[MAX_PATH]="",csid[MAX_PATH]="";
                    SidtoUser(psid, cuser, csid, MAX_PATH,MAX_PATH,FALSE);

                    strncat(acls," ",res_taille_max);
                    if (strlen(cuser)>0) strncat(acls,cuser,res_taille_max);
                    if (strlen(csid)>0) strncat(acls,csid,res_taille_max);
                    strncat(acls,"\r\n\0",res_taille_max);
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
}
//------------------------------------------------------------------------------
void TypeFile(char *cFileName,char *result)
{
  unsigned int i,s= strlen(cFileName);
  char myext[6]="\0\0\0\0\0";
  result[0]=0;
  if (s<5) return;
  else if (cFileName[s-2]=='.'){sprintf(myext,"%s",&cFileName[s-1]);s=1;}
  else if (cFileName[s-3]=='.'){sprintf(myext,"%s",&cFileName[s-2]);s=2;}
  else if (cFileName[s-4]=='.'){sprintf(myext,"%s",&cFileName[s-3]);s=3;}
  else if (cFileName[s-5]=='.'){sprintf(myext,"%s",&cFileName[s-4]);s=4;}
  else if (cFileName[s-6]=='.'){sprintf(myext,"%s",&cFileName[s-5]);s=5;}
  else if (cFileName[s-7]=='.'){sprintf(myext,"%s",&cFileName[s-6]);s=6;}
  else if (cFileName[s-8]=='.'){sprintf(myext,"%s",&cFileName[s-7]);s=7;}
  else if (cFileName[s-9]=='.'){sprintf(myext,"%s",&cFileName[s-8]);s=8;}
  else return;

  //PASSAGE TOUT EN MINUSCULE
  for (i=0;i<s;i++)
  {
    if (myext[i]<91 && myext[i]>64) myext[i] +=32;
  }

  //analyse du format
  for (i=0;i<nb_ext_doc;i++){if (!strcmp(ext_doc[i].ext,myext)){strcpy(result,"Office\0");return;}}
  for (i=0;i<nb_ext_med;i++){if (!strcmp(ext_med[i].ext,myext)){strcpy(result,"Media\0");return;}}
  for (i=0;i<nb_ext_img;i++){if (!strcmp(ext_img[i].ext,myext)){strcpy(result,"Picture\0");return;}}
  for (i=0;i<nb_ext_exe;i++){if (!strcmp(ext_exe[i].ext,myext)){strcpy(result,"Software\0");return;}}
  for (i=0;i<nb_ext_crypt;i++){if (!strcmp(ext_crypt[i].ext,myext)){strcpy(result,"Crypt\0");return;}}
  for (i=0;i<nb_ext_mail;i++){if (!strcmp(ext_mail[i].ext,myext)){strcpy(result,"Mail\0");return;}}
  for (i=0;i<nb_ext_other;i++){if (!strcmp(ext_mail[i].ext,myext)){strcpy(result,"Other\0");return;}}

  for (i=0;i<nb_ext_source_code;i++){if (!strcmp(ext_source_code[i].ext,myext)){strcpy(result,"Source code\0");return;}}
  for (i=0;i<nb_ext_lnk;i++){if (!strcmp(ext_link[i].ext,myext)){strcpy(result,"Link\0");return;}}
  for (i=0;i<nb_ext_compresse;i++){if (!strcmp(ext_compresse[i].ext,myext)){strcpy(result,"Compresse\0");return;}}
  for (i=0;i<nb_ext_web;i++){if (!strcmp(ext_web[i].ext,myext)){strcpy(result,"Web\0");return;}}
  for (i=0;i<nb_ext_configuration;i++){if (!strcmp(ext_configurationr[i].ext,myext)){strcpy(result,"Configuration\0");return;}}
  for (i=0;i<nb_ext_bdd;i++){if (!strcmp(ext_bdd[i].ext,myext)){strcpy(result,"BDD\0");return;}}
  for (i=0;i<nb_ext_audit;i++){if (!strcmp(ext_audit[i].ext,myext)){strcpy(result,"Audit\0");return;}}
}
//-----------------------------------------------------------------------------
void Scan_files_Rep(char *path, HANDLE hlv, HTREEITEM hparent, BOOL fat, char *filesys)
{
  WIN32_FIND_DATA data;
  HANDLE hfic = FindFirstFile(path, &data);
  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeCreation,SysTimeModification,SysTimeAcces;
  char Rep [MAX_PATH];
  DWORD FileAttributes;

  LINE_ITEM lv_line[NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]];

  if (hfic != INVALID_HANDLE_VALUE)
  {
    do
    {
      if(data.cFileName[0] == '.' && (data.cFileName[1] == 0 || data.cFileName[1] == '.')){} //si bien un path valide
      else
      {
        //traitement des dates
        FileTimeToLocalFileTime(&(data.ftCreationTime), &LocalFileTime);
        if (FileTimeToSystemTime(&LocalFileTime, &SysTimeCreation))
          snprintf(lv_line[4].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeCreation.wYear,SysTimeCreation.wMonth,SysTimeCreation.wDay,SysTimeCreation.wHour,SysTimeCreation.wMinute,SysTimeCreation.wSecond);
        else lv_line[4].c[0]=0;

        FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
        if (FileTimeToSystemTime(&LocalFileTime, &SysTimeModification))
          snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond);
        else lv_line[5].c[0]=0;

        FileTimeToLocalFileTime(&(data.ftLastAccessTime), &LocalFileTime);
        if (FileTimeToSystemTime(&LocalFileTime, &SysTimeAcces))
          snprintf(lv_line[6].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeAcces.wYear,SysTimeAcces.wMonth,SysTimeAcces.wDay,SysTimeAcces.wHour,SysTimeAcces.wMinute,SysTimeAcces.wSecond);
        else lv_line[6].c[0]=0;

        //mise en forme du chemin pour traitement
        strcpy(Rep,path);
        Rep[strlen(path)-3]=0;

        //un répertoire
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          strncat(Rep,data.cFileName,MAX_PATH);
          strncat(Rep,"\\\0",MAX_PATH);

          HTREEITEM hitem = AjouterItemTreeViewFile(Tabl[TABL_FILES], TV_VIEW,data.cFileName,hparent,0);

          strcpy(lv_line[2].c,"Directory");

          //type d'attribut
          FileAttributes = GetFileAttributes(Rep);
          if ((FileAttributes&0x2) == 0x2)strcpy(lv_line[8].c,"H"); else lv_line[8].c[0] = 0;//fichier caché
          if ((FileAttributes&0x4)== 0x4)strcpy(lv_line[9].c,"S"); else lv_line[9].c[0] = 0;//fichier système

          //copie du nom du répertoire et du fichier
          strcpy(lv_line[0].c,Rep);
          lv_line[1].c[0] = 0;

          //ACLS
          lv_line[3].c[0] = 0;
          if (!fat)strcpy(lv_line[7].c,filesys);
          else
          {
            //lecture des ACLS
            lv_line[7].c[0] = 0;
            if (ACL_Enable)GetACLS(Rep, lv_line[7].c, MAX_LINE_SIZE, lv_line[3].c, MAX_LINE_SIZE);
          }
          //ajout à la listeview
          AddToLV_File(hlv, lv_line, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]);

          //on traite les autres répertoires
          strncat(Rep,"*.*\0",MAX_PATH);
          Scan_files_Rep(Rep, hlv, hitem, fat,filesys);
          SendDlgItemMessage(Tabl[TABL_FILES],TV_VIEW, TVM_SORTCHILDREN,(WPARAM)TRUE, (LPARAM)hitem);
        }else // un fichier
        {
          //copie du nom du répertoire et du fichier
          strcpy(lv_line[0].c,Rep);
          strcpy(lv_line[1].c,data.cFileName);

          AjouterItemTreeViewFile(Tabl[TABL_FILES], TV_VIEW,data.cFileName,hparent,ICON_FILE);

          //file type
          if (Type_Enable)TypeFile(data.cFileName,lv_line[2].c);
          else lv_line[2].c[0]=0;

          //type d'attribut
          FileAttributes = GetFileAttributes(Rep);
          if ((FileAttributes&0x2) == 0x2)strcpy(lv_line[8].c,"H"); else lv_line[8].c[0] = 0;//fichier caché
          if ((FileAttributes&0x4) == 0x4)strcpy(lv_line[9].c,"S"); else lv_line[9].c[0] = 0;//fichier système

          //ACLS
          lv_line[3].c[0] = 0;

          //lecture des ACLS
          lv_line[7].c[0] = 0;
          if (ACL_Enable)GetACLS(Rep, lv_line[7].c, MAX_LINE_SIZE, lv_line[3].c, MAX_LINE_SIZE);
          //ajout à la listeview
          AddToLV_File(hlv, lv_line, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]);
        }
      }
    }while(FindNextFile (hfic,&data)); //récupération des fichiers 1 par 1
  }
  FindClose(hfic);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_files(LPVOID lParam)
{
  //on vide les listeview + treeview
  HANDLE hlv        = GetDlgItem(Tabl[TABL_FILES],LV_FILES_VIEW);
  HANDLE htv        = GetDlgItem(Tabl[TABL_FILES],TV_VIEW);
  ListView_DeleteAllItems(hlv);
  SendMessage(htv,TVM_DELETEITEM,(WPARAM)0, (LPARAM)TVI_ROOT);

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_FILES,TRUE);

  char lettre[4]="";
  unsigned long FileMaxLen=10,FileFlags;
  char FileSysName[11];
  BOOL bACL = TRUE;
  HTREEITEM hitem;

  if ((BOOL)lParam)
  {
    char tmp[MAX_PATH], tmp_path[8];
    int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);
    if (nblecteurs>0)
    {
      for (i=0;i<nblecteurs;i+=4)
      {
        switch(GetDriveType(&tmp[i]))
        {
          case DRIVE_FIXED :
          case DRIVE_REMOTE :
          case DRIVE_RAMDISK :
          case DRIVE_REMOVABLE :
            //type de système de fichier
            FileMaxLen      = 10;
            FileFlags       = 0;
            strcpy(lettre,"_:\\\0");
            lettre[0]       = tmp[i];
            FileSysName[0]  = 0;

            if (GetVolumeInformation(lettre,0,10,0,&FileMaxLen, &FileFlags,FileSysName,10)==0)bACL = FALSE; //non formaté
            else if (!strcmp(FileSysName,"FAT"))bACL = FALSE;//FAT
            else bACL = TRUE;

            //traitement de recherche
            strcpy(tmp_path,"_:\\*.*\0");
            tmp_path[0]=tmp[i];
            hitem = AjouterItemTreeViewFile(Tabl[TABL_FILES], TV_VIEW,lettre,TVI_ROOT,ICON_FILE_DOSSIER);
            Scan_files_Rep(tmp_path, hlv, hitem, bACL, FileSysName);
            SendDlgItemMessage(Tabl[TABL_FILES],TV_VIEW, TVM_SORTCHILDREN,(WPARAM)TRUE, (LPARAM)hitem);
          break;
        }
      }
    }
  }else
  {
    //récupération du 1er fils dans le treeview
    HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_FILES]);

    //on énumère tous les items fils du treeview
    char tmp[MAX_PATH];
    do
    {
      //récupération du texte de l'item
      TVITEM tvitem;
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        //le test est fait si différent du précedent
        if (tmp[0] != lettre[0])
        {
          //type de système de fichier
          FileMaxLen      = 10;
          FileFlags       = 0;
          strcpy(lettre,"_:\\\0");
          lettre[0]       = tmp[0];
          FileSysName[0]  = 0;

          if (GetVolumeInformation(lettre,0,10,0,&FileMaxLen, &FileFlags,FileSysName,10)==0)bACL = FALSE; //non formaté
          else if (!strcmp(FileSysName,"FAT"))bACL = FALSE;//FAT
          else bACL = TRUE;
        }

        //traitement
        strncat(tmp,"*.*\0",MAX_PATH);

        hitem = AjouterItemTreeViewFile(Tabl[TABL_FILES], TV_VIEW,lettre,TVI_ROOT,ICON_FILE_DOSSIER);
        Scan_files_Rep(tmp, hlv, hitem, bACL, FileSysName);
        SendDlgItemMessage(Tabl[TABL_FILES],TV_VIEW, TVM_SORTCHILDREN,(WPARAM)TRUE, (LPARAM)hitem);
      }
    }while((hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)) && ScanStart);
  }

  h_scan_files = NULL;
  if (!h_scan_logs && !h_scan_files && !h_scan_registry)
  {
    ScanStart = FALSE;
    SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_START),"Start");
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_INFO, (LPARAM)"");
  }
  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_FILES,FALSE);
  SendDlgItemMessage(Tabl[TABL_FILES],TV_VIEW, TVM_SORTCHILDREN,(WPARAM)TRUE, (LPARAM)TVI_ROOT);
//  SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_FILES, (LPARAM)"");
  return 0;
}

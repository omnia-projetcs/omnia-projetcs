//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/bb309022%28v=vs.85%29.aspx
void TraiterEventlogFileEvt(char * eventfile, HANDLE hlv)
{
  //ouverture du fichier en lecture partagé
  HANDLE Hlog = CreateFile(eventfile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hlog != INVALID_HANDLE_VALUE)
  {
    //test de la taille
    BOOL critical = FALSE;
    DWORD taille_fic = GetFileSize(Hlog,NULL);
    if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
    {
      unsigned char *b, *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
      if (buffer == NULL)
      {
        CloseHandle(Hlog);
        SB_add_T(TABL_CONF-1, "LOG : Out of memory");
        return;
      }

      //lecture du fichier
      DWORD copiee, position = 0, increm = 0;
      unsigned int taille_tmp;
      if (taille_fic > DIXM)increm = DIXM;
      else increm = taille_fic;

      while (position<taille_fic && increm!=0)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
      {
        copiee = 0;
        ReadFile(Hlog, buffer+position, increm,&copiee,0);
        position +=copiee;
        if (taille_fic-position < increm)increm = taille_fic-position ;
      }

        //traitement par parssage
        //information sur l'entête
        typedef struct _EVENTLOGHEADER {
          ULONG HeaderSize;
          ULONG Signature;
          ULONG MajorVersion;
          ULONG MinorVersion;
          ULONG StartOffset;
          ULONG EndOffset;
          ULONG CurrentRecordNumber;
          ULONG OldestRecordNumber;
          ULONG MaxSize;
          ULONG Flags;
          ULONG Retention;
          ULONG EndHeaderSize;
        } EVENTLOGHEADER, *PEVENTLOGHEADER;
        b = buffer;

        //validité du fichier
        if (((PEVENTLOGHEADER)b)->HeaderSize == 0x30 && ((PEVENTLOGHEADER)b)->MajorVersion == 1&& ((PEVENTLOGHEADER)b)->MinorVersion == 1)
        {
          unsigned long int i=0,c=0,k=0,nb_events = ((PEVENTLOGHEADER)b)->CurrentRecordNumber;

          //debut du 1er enregistrement
          EVENTLOGRECORD *pevlr = (EVENTLOGRECORD *) b;
          BOOL inconnu;
          FILETIME FileTime, LocalFileTime;
          SYSTEMTIME SysTime;
          LONGLONG lgTemp;
          PSID lpSid;
          char szName[MAX_PATH], szDomain[MAX_PATH];
          DWORD cbName = MAX_PATH, cbDomain = MAX_PATH;
          SID_NAME_USE snu;

          pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr+48); //48 = 0x30
          c+=48;

          //vérification de la validité des informations du header
          if (pevlr->Reserved != 1699505740)
          {
            //recherche du prochain header
            i = 0x30;
            do
            {
              pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr+1);
              i++;c++;
            }while (i<taille_fic && pevlr->Reserved != 1699505740);

            if (pevlr->Reserved == 1699505740)i=0;
            else i = nb_events;
          }

          //traitement des différents journaux
          LINE_ITEM lv_line[NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]];
          unsigned long int r=0;
          char tmp_t[MAX_LINE_SIZE];
          char * pStrings;

          strcpy(lv_line[0].c,eventfile); //journal

          for (;i<nb_events && c<taille_fic ;i++)
          {
            //init
            snprintf(lv_line[1].c,MAX_LINE_SIZE,"%05lu",r++/*pevlr->RecordNumber*/);

            //Type
            inconnu = FALSE;
            switch(pevlr->EventType)
            {
              case 0x01 : strcpy(lv_line[6].c,"ERROR"); break;
              case 0x02 : strcpy(lv_line[6].c,"WARNING"); break;
              case 0x04 : strcpy(lv_line[6].c,"INFORMATION"); break;
              case 0x08 : strcpy(lv_line[6].c,"AUDIT_SUCCESS"); break;
              case 0x10 : strcpy(lv_line[6].c,"AUDIT_FAILURE"); break;
              default :
                inconnu = TRUE;
                snprintf(lv_line[6].c,MAX_LINE_SIZE,"UNKNOW (%d)",pevlr->EventType);
              break;
            }

            if (!inconnu)
            {
              //traitement de la date
              lgTemp = Int32x32To64(pevlr->TimeGenerated,10000000) + 116444736000000000;
              FileTime.dwLowDateTime = (DWORD) lgTemp;
              FileTime.dwHighDateTime = (DWORD)(lgTemp >> 32);
              if(FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
              {
                if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
                  snprintf(lv_line[3].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                else lv_line[3].c[0]=0;
              }else lv_line[3].c[0]=0;

              //source
              lv_line[4].c[0]=0;
              if (sizeof(EVENTLOGRECORD) < pevlr->Length)
                snprintf(lv_line[4].c,MAX_LINE_SIZE,"%S",(char *)pevlr+sizeof(EVENTLOGRECORD));

              //ID
              if(EventIdtoDscr(pevlr->EventID& 0xFFFF, lv_line[4].c, lv_line[2].c, MAX_LINE_SIZE)){strcpy(lv_line[8].c,"X");critical=TRUE;}
              else {lv_line[8].c[0]=0;critical=FALSE;}

              //Description + infos
              //NumStrings, StringOffset
              memset(lv_line[5].c, 0, MAX_LINE_SIZE);
              taille_tmp = (pevlr->Length) * sizeof(char);
              if (taille_tmp > 56)
              {
                pStrings = (char*)HeapAlloc(GetProcessHeap(), 0, taille_tmp+1);
                if (pStrings != NULL)
                {
                  memset(pStrings, 0, taille_tmp);
                  memcpy(pStrings, (LPBYTE)pevlr+56, (pevlr->Length-56));

                  //on remplace tous les 0 :
                  replace_to_char(pStrings, pevlr->Length-58,'/');
                  if (strlen(pStrings)>0)
                  {
                    snprintf(lv_line[5].c,MAX_LINE_SIZE,"%S ",(wchar_t*)pStrings);
                  }
                  HeapFree(GetProcessHeap(), 0,pStrings);
                }
              }
              taille_tmp = pevlr->DataOffset-pevlr->StringOffset * sizeof(char);
              if (taille_tmp > 0)
              {
                pStrings = (char*)HeapAlloc(GetProcessHeap(), 0, taille_tmp+1);
                if (pStrings != NULL)
                {
                  memset(pStrings, 0, taille_tmp);
                  memcpy(pStrings, (LPBYTE)pevlr + pevlr->StringOffset, (pevlr->DataOffset-pevlr->StringOffset));
                  if (strlen(pStrings)>0)
                  {
                    snprintf(tmp_t,MAX_LINE_SIZE,"%S ",(wchar_t*)pStrings);
                    strncat(lv_line[5].c,tmp_t,MAX_LINE_SIZE);
                  }
                  HeapFree(GetProcessHeap(), 0,pStrings);
                }
              }
              taille_tmp = pevlr->DataLength * sizeof(char);
              if (taille_tmp>0 && pevlr->DataOffset>0)
              {
                pStrings = (char*)HeapAlloc(GetProcessHeap(), 0, taille_tmp+1);
                if (pStrings != NULL)
                {
                  memset(pStrings, 0, taille_tmp);
                  memcpy(pStrings, (LPBYTE)pevlr+pevlr->DataOffset, (pevlr->DataLength));
                  if (strlen(pStrings)>0)
                  {
                    strncat(lv_line[5].c,pStrings,MAX_LINE_SIZE);
                  }
                  HeapFree(GetProcessHeap(), 0,pStrings);
                }
              }
              strncat(lv_line[5].c,"\0",MAX_LINE_SIZE);

              //Utilisateur + SID
              //récupération du nom d'utilisateur  associés a l'évenement
              lpSid = (PSID)((LPBYTE) pevlr + pevlr->UserSidOffset);
              szName[0]=0;
              szDomain[0]=0;
              cbName = MAX_PATH;
              cbDomain = MAX_PATH;
              if (LookupAccountSid(0, lpSid, szName, &cbName, szDomain, &cbDomain, &snu))
              {
                if (cbName>0)
                {
                  strncpy(lv_line[7].c,szDomain,MAX_LINE_SIZE);
                  strncat(lv_line[7].c,"\\",MAX_LINE_SIZE);
                  strncat(lv_line[7].c,szName,MAX_LINE_SIZE);
                }else lv_line[7].c[0]=0;
              }else lv_line[7].c[0]=0;

              //ajout du SID
              if (IsValidSid(lpSid))
              {
                PUCHAR pcSubAuth = GetSidSubAuthorityCount(lpSid);
                unsigned char ucMax = *pcSubAuth;
                DWORD *SidP;
                strncat(lv_line[7].c,"SID:S-1-5\0",MAX_LINE_SIZE);

                //récupération des éléments un par un
                szName[0] = 0;
                for (k=0;k<ucMax;++k)
                {
                 SidP=GetSidSubAuthority(lpSid,k);
                 snprintf(szName,TAILLE_TMP,"-%d",(int)*SidP);
                 strncat(lv_line[7].c,szName,MAX_LINE_SIZE);
                }
              }
              strncat(lv_line[7].c,"\0",MAX_LINE_SIZE);

              AddToLV_log(hlv, lv_line, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL],critical);
            }
            c+= pevlr->Length;
            pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr + pevlr->Length);
          }
        }
        HeapFree(GetProcessHeap(), 0, buffer);
    }
  }
  CloseHandle(Hlog);
}

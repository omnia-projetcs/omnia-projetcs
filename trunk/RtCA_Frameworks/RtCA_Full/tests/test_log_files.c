//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void TraiterEventlogFileEvt(char * eventfile, sqlite3 *db, unsigned int session_id)
{
  HANDLE Hlog = CreateFile(eventfile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hlog != INVALID_HANDLE_VALUE)
  {
    char indx[DEFAULT_TMP_SIZE], log_id[DEFAULT_TMP_SIZE],
    send_date[DATE_SIZE_MAX], write_date[DATE_SIZE_MAX],
    source[MAX_PATH], description[MAX_LINE_SIZE],
    user[DEFAULT_TMP_SIZE], rid[DEFAULT_TMP_SIZE], sid[DEFAULT_TMP_SIZE],
    state[DEFAULT_TMP_SIZE], critical[DEFAULT_TMP_SIZE];

    DWORD taille_fic = GetFileSize(Hlog,NULL);
    if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
    {
      unsigned char *b, *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
      if (buffer == NULL)
      {
        CloseHandle(Hlog);
        return;
      }

      //read file datas
      DWORD copiee, position = 0, increm = 0,pos=0;
      if (taille_fic > DIXM)increm = DIXM;
      else increm = taille_fic;

      while (position<taille_fic && increm!=0)
      {
        copiee = 0;
        ReadFile(Hlog, buffer+position, increm,&copiee,0);
        position +=copiee;
        if (taille_fic-position < increm)increm = taille_fic-position ;
      }

      //header infos
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
        unsigned long int i=0,c=0,nb_events = ((PEVENTLOGHEADER)b)->CurrentRecordNumber;

        //first record
        EVENTLOGRECORD *pevlr = (EVENTLOGRECORD *) b;
        unsigned long int size_max, uSize, x, uStringOffset, uStepOfString;
        char* szExpandedString;

        pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr+48); //48 = 0x30
        c+=48;

        //good header or not ?
        if (pevlr->Reserved != 1699505740)
        {
          //next header
          i = 0x30;
          do
          {
            pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr+1);
            i++;c++;
          }while (i<taille_fic && pevlr->Reserved != 1699505740);

          if (pevlr->Reserved == 1699505740)i=0;
          else i = nb_events;
        }

        //datas
        char * pStrings;

        for (;i<nb_events && c<taille_fic  && start_scan;i++)
        {
          //init
          snprintf(indx,DEFAULT_TMP_SIZE,"%08lu",pevlr->RecordNumber);

          //Type
          switch(pevlr->EventType)
          {
            case 0x01 : strcpy(state,"ERROR"); break;
            case 0x02 : strcpy(state,"WARNING"); break;
            case 0x04 : strcpy(state,"INFORMATION"); break;
            case 0x08 : strcpy(state,"AUDIT_SUCCESS"); break;
            case 0x10 : strcpy(state,"AUDIT_FAILURE"); break;
            default :state[0]=0;break;
          }

          if (state[0]!=0)
          {
            //date : send_date
            timeToString(pevlr->TimeGenerated, send_date, DATE_SIZE_MAX);

            //date : write_date
            timeToString(pevlr->TimeWritten, write_date, DATE_SIZE_MAX);

            //source
            source[0]=0;
            if (sizeof(EVENTLOGRECORD) < pevlr->Length)
              snprintf(source,DEFAULT_TMP_SIZE,"%S",(char *)pevlr+sizeof(EVENTLOGRECORD));

            //ID
            snprintf(log_id,DEFAULT_TMP_SIZE,"%08lu",pevlr->EventID& 0xFFFF);

            //user+rid+sid
            user[0] = 0;
            rid[0]  = 0;
            sid[0]  = 0;
            SidtoUser((PSID)((LPBYTE) pevlr + pevlr->UserSidOffset), user, rid, sid, DEFAULT_TMP_SIZE);

            //descriptions strings !!!
            //init
            pos = 0;
            memset(description, 0, MAX_LINE_SIZE);

            //first wave
            uSize         = pevlr->DataOffset - pevlr->StringOffset;
            uStringOffset = pevlr->StringOffset;
            if (uSize>0 && uStringOffset>0 && pos<MAX_LINE_SIZE)
            {
              pStrings = (char*)GlobalAlloc(GPTR, uSize * sizeof(BYTE));
              if (pStrings != 0)
              {
                 memset(pStrings, 0, uSize * sizeof(BYTE));
                 memcpy(pStrings, (char*)pevlr + uStringOffset, uSize);
                 uStepOfString   = 0;

                 szExpandedString = (char*)GlobalAlloc(GPTR, (uSize + MAX_LINE_SIZE+1) * sizeof(BYTE));
                 size_max = uSize + MAX_LINE_SIZE;
                 if (szExpandedString > 0)
                 {
                   for(x = 0; x < pevlr->NumStrings-1 && MAX_LINE_SIZE>pos && uStepOfString < uSize; x++)
                   {
                      snprintf((char*)szExpandedString,size_max,"%S,",pStrings + uStepOfString);
                      if (strlen(szExpandedString)>1)
                      {
                        strncpy(description+pos,(char*)szExpandedString,MAX_LINE_SIZE-pos);
                        pos = pos + strlen(description);
                      }
                      uStepOfString = strlen((char*)szExpandedString)*2+1;
                   }
                   GlobalFree(szExpandedString);
                 }
                GlobalFree(pStrings);
              }
            }

            //next wave
            uSize         = pevlr->DataLength * sizeof(char);
            if (pevlr->DataLength > 0 && pevlr->DataOffset >0 && pos<MAX_LINE_SIZE)
            {
              pStrings = (char*)GlobalAlloc(GPTR, uSize * sizeof(BYTE)+1);
              if (pStrings != 0)
              {
                memset(pStrings, 0, uSize * sizeof(BYTE));
                memcpy(pStrings, (LPBYTE)pevlr+pevlr->DataOffset, (pevlr->DataLength));
                pStrings[pevlr->DataLength] = 0;
                if (strlen(pStrings)>0)
                {
                  snprintf(description+pos,MAX_LINE_SIZE-pos,"%s",(char*)pStrings);
                  pos = pos + strlen(description);
                }
                GlobalFree(pStrings);
              }
            }

            if (strcmp(send_date,write_date) != 0)strncpy(critical,"X",DEFAULT_TMP_SIZE);
            else critical[0]=0;

            convertStringToSQL(source, MAX_PATH);
            convertStringToSQL(description, MAX_LINE_SIZE);
            addLogtoDB(eventfile, indx, log_id,
                   send_date, write_date, source, description,
                   user, rid, sid, state, critical, session_id, db);
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
//------------------------------------------------------------------------------
void TraiterEventlogFileLog(char * eventfile, sqlite3 *db, unsigned int session_id)
{
  //ouverture du fichier
  HANDLE Hlog = CreateFile(eventfile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hlog != INVALID_HANDLE_VALUE)
  {
    //lecture du contenu
    long i,j=0,k,taille_fic = GetFileSize(Hlog,NULL);
    if (taille_fic > 17 && taille_fic!=INVALID_FILE_SIZE) // 17 = taile d'une ligne minimum
    {
      unsigned char *b, *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
      b = buffer;
      if (buffer != NULL)
      {
        //read file datas
        DWORD copiee, position = 0, increm = 0;
        if (taille_fic > DIXM)increm = DIXM;
        else increm = taille_fic;

        while (position<taille_fic && increm!=0)
        {
          copiee = 0;
          ReadFile(Hlog, buffer+position, increm,&copiee,0);
          position +=copiee;
          if (taille_fic-position < increm)increm = taille_fic-position ;
        }

        char indx[DEFAULT_TMP_SIZE], send_date[DATE_SIZE_MAX],source[MAX_PATH], description[MAX_LINE_SIZE];

        //traitement des données
        char ligne[MAX_LINE_SIZE];
        char *l, *lv;

        //gestion suivant le type de format de fichier ^^
        BOOL date_normal_format;
        if (buffer[9] == buffer[12] && buffer[12] == ':')date_normal_format = FALSE;
        else if(buffer[13] == buffer[16] && buffer[16] == ':')date_normal_format = TRUE;
        else
        {
          //bad file format
          HeapFree(GetProcessHeap(), 0, buffer);
          CloseHandle(Hlog);
          return;
        }

        while(*b)
        {
          //copie des données ligne par ligne
          i=0;
          l = ligne;
          while(*b && i++<MAX_LINE_SIZE && *b !='\n')*l++ = *b++;
          *l=0;
          b++;

          //traitement de la ligne !
          if (strlen(ligne)>17)
          {
            //Date
            send_date[0] = 0;
            if (date_normal_format)
            {
              //2011-08-05 18:44:23 - > 2011/08/15-10:34:57
              send_date[0] =ligne[0]; //Année
              send_date[1] =ligne[1];
              send_date[2] =ligne[2];
              send_date[3] =ligne[3];
              send_date[4] ='/';
              send_date[5] =ligne[5]; //Mois
              send_date[6] =ligne[6];
              send_date[7] ='/';
              send_date[8] =ligne[8]; //Jours
              send_date[9] =ligne[9];
              send_date[10]='-';
              send_date[11]=ligne[11]; //Heures
              send_date[12]=ligne[12];
              send_date[13]=':';
              send_date[14]=ligne[14]; //Minutes
              send_date[15]=ligne[15];
              send_date[16]=':';
              send_date[17]=ligne[17]; //Secondes
              send_date[18]=ligne[18];
              send_date[19]=0;

              k=20;
              l = ligne+ 20;//on passe la dat
            }else
            {
              //Aug 14 14:09:37 - > 2011/08/15-10:34:57
              send_date[0] ='?'; //Année
              send_date[1] ='?';
              send_date[2] ='?';
              send_date[3] ='?';
              send_date[4] ='/';
              switch(ligne[0])      //mois
              {
                case 'J':
                  if (ligne[1] == 'a'){send_date[5] = '0';send_date[6] = '1';}        //January
                  else if (ligne[1] == 'u')
                  {
                    if (ligne[2] == 'n'){send_date[5] = '0';send_date[6] = '6';}      //June
                    else if (ligne[2] == 'l'){send_date[5] = '0';send_date[6] = '7';} //July
                    else {send_date[5] = '_';send_date[6] = '_';}
                  }else {send_date[5] = '_';send_date[6] = '_';}
                break;
                case 'F': send_date[5] = '0';send_date[6] = '2';break;                //February
                case 'M':
                  if (ligne[2] == 'r'){send_date[5] = '0';send_date[6] = '3';}        //March
                  else if (ligne[2] == 'y'){send_date[5] = '0';send_date[6] = '5';}   //May
                  else {send_date[5] = '_';send_date[6] = '_';}
                break;
                case 'A':
                  if (ligne[1] == 'p'){send_date[5] = '0';send_date[6] = '4';}        //April
                  else if (ligne[1] == 'u'){send_date[5] = '0';send_date[6] = '8';}   //August
                  else {send_date[5] = '_';send_date[6] = '_';}
                break;
                case 'S': send_date[5] = '0';send_date[6] = '9';break;                //September
                case 'O': send_date[5] = '1';send_date[6] = '0';break;                //October
                case 'N': send_date[5] = '1';send_date[6] = '1';break;                //November
                case 'D': send_date[5] = '1';send_date[6] = '2';break;                //December
                default : send_date[5] = '_';send_date[6] = '_';break;
              }
              send_date[7] ='/';
              send_date[8] =ligne[4]; // jour
              send_date[9] =ligne[5];
              send_date[10]='-';
              send_date[11]=ligne[7]; //Heures
              send_date[12]=ligne[8];
              send_date[13]=ligne[9];
              send_date[14]=ligne[10]; //Minutes
              send_date[15]=ligne[11];
              send_date[16]=ligne[12];
              send_date[17]=ligne[13]; //Secondes
              send_date[18]=ligne[14];
              send_date[19]=0;
              k=16;
              l = ligne+ 16;//on passe la date
            }

            //Source
            source[0] = 0;
            lv = source;
            while (k<MAX_PATH && k<strlen(ligne) && *l && *l !=' ')
            {
              *lv++ = *l++;
              k++;
            }
            *lv = 0;

            //Description
            l++; // on passe l'espace
            description[0] = 0;
            lv = description;
            while (k<MAX_LINE_SIZE && k<strlen(ligne) && *l)
            {
              *lv++ = *l++;
              k++;
            }
            *lv = 0;
            snprintf(indx,DEFAULT_TMP_SIZE,"%08lu",j++);

            //add
            addLogtoDB(eventfile, indx, "", send_date, send_date, source, description, "", "", "", "", "", session_id, db);
          }
        }
        HeapFree(GetProcessHeap(), 0, buffer);
      }
    }
  }
  CloseHandle(Hlog);
}

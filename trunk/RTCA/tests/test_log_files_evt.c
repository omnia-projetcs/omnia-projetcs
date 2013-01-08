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
  HANDLE Hlog = CreateFile(eventfile,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hlog != INVALID_HANDLE_VALUE)
  {
    char indx[DEFAULT_TMP_SIZE], log_id[DEFAULT_TMP_SIZE],
    send_date[DATE_SIZE_MAX], write_date[DATE_SIZE_MAX],
    source[MAX_PATH], description[MAX_LINE_SIZE],
    user[DEFAULT_TMP_SIZE], rid[DEFAULT_TMP_SIZE], sid[DEFAULT_TMP_SIZE],
    state[DEFAULT_TMP_SIZE], critical[DEFAULT_TMP_SIZE];

    char eventname[MAX_PATH];
    extractFileFromPath(eventfile, eventname, MAX_PATH);
    if (strlen(eventname)-4 > 0)
    {
      //remove .evt
      eventname[strlen(eventname)-4] = 0;
    }else eventname[0] = 0;

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
        for (;i<nb_events && c<taille_fic  && start_scan;i++)
        {
          //init
          snprintf(indx,DEFAULT_TMP_SIZE,"%08lu",pevlr->RecordNumber & 0xFFFF);

          //Type
          switch(pevlr->EventType)
          {
            case EVENTLOG_ERROR_TYPE      /*0x01*/ : strcpy(state,"ERROR"); break;
            case EVENTLOG_WARNING_TYPE    /*0x02*/ : strcpy(state,"WARNING"); break;
            case EVENTLOG_INFORMATION_TYPE/*0x04*/ : strcpy(state,"INFORMATION"); break;
            case EVENTLOG_AUDIT_SUCCESS   /*0x08*/ : strcpy(state,"AUDIT_SUCCESS"); break;
            case EVENTLOG_AUDIT_FAILURE   /*0x10*/ : strcpy(state,"AUDIT_FAILURE"); break;
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
            memset(description, 0, MAX_LINE_SIZE);

            if ((pevlr->StringOffset+ pevlr->DataLength) < taille_fic)
            {
              if (readMessageDatas(pevlr, eventname, source, description, MAX_LINE_SIZE) == FALSE)
              {
                //get string in raw mode !
                description[0] = 0;
                int z;
                char*c = ((LPBYTE) pevlr + pevlr->StringOffset);
                for (z = 0; z < pevlr->NumStrings; z++)
                {
                  snprintf(description+strlen(description),MAX_LINE_SIZE-strlen(description),"%S,",c);
                  c += strlen((char *) c) + 2;
                }
              }
            }

            //first wave
            /*
            DWORD pos=0;
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
            }*/

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

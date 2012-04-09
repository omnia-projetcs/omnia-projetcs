//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
#define NB_ST_ITEM        100
typedef struct _STR_T
{
  DWORD pos;
  unsigned short id;
  char s[TAILLE_TMP];
}STR_T;

typedef struct _STRING_TABLE
{
  STR_T item[NB_ST_ITEM];
  short Qualifiers;  //ID
  short Computer;    //User-SID
  short UserID;      //User-SID
  short Provider;    //Source
  short Level;       //Type
}STRING_TABLE;

//début de données qui fait 8 octets
#define STH     8
typedef struct _STRING_TABLE_HEADER
{
  unsigned short unknow1;
  unsigned short unknow2;
  unsigned short unknow3;
  unsigned short size;
}STRING_TABLE_HEADER;

//------------------------------------------------------------------------------
unsigned int ReadStringTable(char *buffer, DWORD size, DWORD pos_real,STRING_TABLE *my_s_table, unsigned int nb_item_ex_table)
{
  //les données sont stockées en unsigned short à la suite ^^
  typedef struct s_i
  {
    unsigned int off;
  }S_I;
  S_I *o;
  char *b = buffer;
  unsigned int pos = 0;
  unsigned int nb = 0, id=0;

  my_s_table->Qualifiers  = -1;
  my_s_table->Computer    = -1;
  my_s_table->UserID      = -1;
  my_s_table->Provider    = -1;
  my_s_table->Level       = -1;


  do
  {
    o = (S_I *)b;
    if (o->off > 0 && o->off<size)
    {
      //on commence a off+0x80 et l'entête de data est de 8 octets
      snprintf(my_s_table->item[nb].s,TAILLE_TMP,"%S (%d)",buffer-0x80+STH+o->off,id);

      if (strlen(my_s_table->item[nb].s)>1)
      {
        if (!strcmp("Qualifiers"    ,my_s_table->item[nb].s))my_s_table->Qualifiers = nb;
        else if (!strcmp("Computer" ,my_s_table->item[nb].s))my_s_table->Computer   = nb;
        else if (!strcmp("UserID"   ,my_s_table->item[nb].s))my_s_table->UserID     = nb;
        else if (!strcmp("Provider" ,my_s_table->item[nb].s))my_s_table->Provider   = nb;
        else if (!strcmp("Level"    ,my_s_table->item[nb].s))my_s_table->Level      = nb;
        my_s_table->item[nb].pos = pos_real+STH+o->off;
        my_s_table->item[nb++].id = id;
      }
    }
    id++;
    b+=4;
    pos+=4;
  }while(pos < 0x100 && nb<nb_item_ex_table);

  return nb;
}
//------------------------------------------------------------------------------
void TraiterDescription (char *buffer,DWORD size_buffer,char *description,DWORD size_description)
{
  char *s = buffer;
  char *d = description;
  DWORD j =0, i =0,k;

  while (j < size_buffer-1 && i<size_description)
  {
    while (*s == 0x00 && j<size_buffer ){s++;j++;};

    //copie
    k=0;
    while (j < size_buffer-1 && i<size_description && *s != 0x00)
    {
      if (*s < 32 || *s > 126)s++;
      else
      {
        *d++=*s++;
        i++;
        s++;
        j+=2;
        k++;
      }
    }

    //traitement des chaines incorrectes
    if (k<4)
    {
      while (k>1){d--;i--;k--;}
    }else
    {
      *d++=' ';
      i++;
    }
  }
  *d = 0;
}
//------------------------------------------------------------------------------
unsigned int ReadRecord(char *buffer, DWORD size, STRING_TABLE *my_s_table, LINE_ITEM *lv_line, HANDLE hlv)
{
  #define EVENTLOG_H_EVTX_SIZE                    4+4+8+8+6
  typedef struct _EVENTLOG_H_EVTX {
    unsigned char MagicString[4];                 //4 ** + 0x00 + 0x00
    unsigned int RecordSize;                      //4
    unsigned long long int RecordNumber;          //8   //RecordNumber
    unsigned long long int TimeCreated;           //8
    unsigned char padding_star_xml[6];            //6 : 0f01 0100 0c01
  }EVENTLOG_H_EVTX;
  EVENTLOG_H_EVTX *h_dheader = (EVENTLOG_H_EVTX *)buffer;

  DWORD id = 0;

  //test de validité
  if (h_dheader->RecordSize <= size
   && h_dheader->MagicString[0] == '*' && h_dheader->MagicString[1] == '*' && h_dheader->MagicString[2] == 0 && h_dheader->MagicString[3] == 0)
  {
    //lecture des informations du header
    //id
    snprintf(lv_line[1].c,MAX_LINE_SIZE,"%05lu",(long unsigned int)(h_dheader->RecordNumber));

    FILETIME FileTime, LocalFileTime;
    SYSTEMTIME SysTime;
    BOOL critical = FALSE;

    //traitement de la date (pas besoin de traitement depuis le 1/1/1970)
    FileTime.dwLowDateTime = (DWORD) h_dheader->TimeCreated;
    FileTime.dwHighDateTime = (DWORD)(h_dheader->TimeCreated >> 32);
    if(FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
    {
      if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
        snprintf(lv_line[3].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
      else lv_line[3].c[0]=0;
    }else lv_line[3].c[0]=0;

    //traitement des données :
    lv_line[2].c[0]=0;        //ID
    lv_line[4].c[0]=0;        //Source
    lv_line[5].c[0]=0;        //Description
    lv_line[6].c[0]=0;        //Type
    lv_line[7].c[0]=0;        //COMPUTER/SID-USER

    //afin de passer les enregistrelent avec header on passe tant qu'on à pas accès au heder attendu ^^
    //permet de traiter les cas de données corrompues
    //0x00 0000 0100 0400 0100 0400
    char *c = buffer+EVENTLOG_H_EVTX_SIZE;
    DWORD pos=EVENTLOG_H_EVTX_SIZE;
    BOOL ok =FALSE;
    do
    {
      if (*c == 0x00 && *(c+1) == 0x00 && *(c+2) == 0x00 &&
      *(c+3) == 0x01 && *(c+4) == 0x00 && *(c+5) == 0x04 && *(c+6) == 0x00 &&
      *(c+7) == 0x01 && *(c+8) == 0x00 && *(c+9) == 0x04 &&*(c+10) == 0x00)
      {
        ok = TRUE;
        break;
      }

      c++;
      pos++;
    }while(pos<h_dheader->RecordSize-12);

    //traitement de la suite ^^
    if (ok)
    {
      //recherche de l'id ^^ dans une zone de moins de 100octets ^^
      //0x2100 0400 0000 XXXX = ID
      typedef struct _ID
      {
        unsigned short param;   // 0x0021
        unsigned char end;
        unsigned char type;     // level
        unsigned short padding; // 0x0000
        unsigned short id;
      }M_ID;
      M_ID * mid;

      while(pos<h_dheader->RecordSize-10)
      {
        if (*c == 0x21 && *(c+1) == 0x00)
        {
          //id identifié ^^
          mid = (M_ID *)c;
          snprintf(lv_line[2].c,MAX_LINE_SIZE,"%05d",mid->id);
          id = mid->id;

          //type
          switch(mid->type)
          {
            case 0x00 : strcpy(lv_line[6].c,"INFORMATION"); break;
            case 0x01 : strcpy(lv_line[6].c,"WARNING"); break;
            case 0x02 : strcpy(lv_line[6].c,"ERROR"); break;
            case 0x04 : strcpy(lv_line[6].c,"CRITICAL"); break;
            case 0x08 : strcpy(lv_line[6].c,"AUDIT_SUCCESS"); break;
            case 0x10 : strcpy(lv_line[6].c,"AUDIT_FAILURE"); break;
            default : snprintf(lv_line[6].c,MAX_LINE_SIZE,"UNKNOW (%d)",mid->type);break;
          }
          break;
        }
        c++;
        pos++;
      }
      c+=8; //on passe l'id + 6 octets de padding

      //Source
      char *desc = c;

      while(pos+4<h_dheader->RecordSize-10)
      {//0x05- 0x12 = S-1-5-18, revoir la phase de détection pour prendre en compte le SID !!!!
        if (*c != 0x00 && *(c+1) == 0x00) //unicode
          if (*(c+2) != 0x00 && *(c+3)==0x00)
          //if (*c == 0x05)
          {
            snprintf(lv_line[4].c,MAX_LINE_SIZE,"%S",c);
            if(EventIdtoDscr(id, lv_line[4].c, lv_line[2].c, MAX_LINE_SIZE)){strcpy(lv_line[8].c,"X");critical=TRUE;}
            else {lv_line[8].c[0]=0;critical=FALSE;}

            break;

            /*if ((*(c+1) == 0x12 || *(c+1) == 0x13 || *(c+1) == 0x14) && *(c+2) == 0x00)
            {
              c+=5;
              snprintf(lv_line[4].c,MAX_LINE_SIZE,"%S",c);
            if(EventIdtoDscr(id, lv_line[4].c, lv_line[2].c, MAX_LINE_SIZE))strcpy(lv_line[8].c,"X");
            else lv_line[8].c[0]=0;
              break;
            }else if (*(c+1) == 0x15 && *(c+2) == 0x00)
            {
              c+=21;
              if (*c == 0x00)c++;
              snprintf(lv_line[4].c,MAX_LINE_SIZE,"%S",c);
            if(EventIdtoDscr(id, lv_line[4].c, lv_line[2].c, MAX_LINE_SIZE))strcpy(lv_line[8].c,"X");
            else lv_line[8].c[0]=0;
              break;
            }*/
          }
        c++;
        pos++;
      }

      //si aucune source n'est définie on ajoute le nom du fichier sans evtx ^^
      if (lv_line[4].c[0]< 32 || lv_line[4].c[0]> 126 ||
          lv_line[4].c[1]< 32 || lv_line[4].c[1]> 126 ||
          lv_line[4].c[2]< 32 || lv_line[4].c[2]> 126 ||
          lv_line[4].c[3]< 32 || lv_line[4].c[3]> 126)
      {
        char *n = lv_line[0].c;
        while (*n)n++;
        while (*n != '\\')n--;
        n++;
        sprintf(lv_line[4].c,"%s",n);
        lv_line[4].c[strlen(lv_line[4].c)-5]=0;

        //on vérifie que pas de % dans le nom sinon on met 0 à sa place ^^
        n = lv_line[4].c;
        while (*n)
        {
          if (*n == '%'){*n=0; break;}
          n++;
        }
        if(EventIdtoDscr(id, lv_line[4].c, lv_line[2].c, MAX_LINE_SIZE)){strcpy(lv_line[8].c,"X");critical=TRUE;}
        else {lv_line[8].c[0]=0;critical=FALSE;}
      }

      //traitement de la description !!
      if(!B_SAFE_MODE)TraiterDescription (desc,h_dheader->RecordSize-pos,lv_line[5].c,MAX_LINE_SIZE);
    }

    /*
    lv_line[5].c[0]=0;        //Description
    lv_line[7].c[0]=0;        //COMPUTER/SID-USER
*/

    //add
    AddToLV_log(hlv, lv_line, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL],critical);
  }
  return h_dheader->RecordSize;
}
//------------------------------------------------------------------------------
void TraiterEventlogFileEvtx(char *eventfile, HANDLE hlv)
{
  //ouverture du fichier en lecture partagé
  HANDLE Hlog = CreateFile(eventfile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hlog != INVALID_HANDLE_VALUE)
  {
    //test de la taille
    DWORD taille_fic = GetFileSize(Hlog,NULL);
    if (taille_fic>0 && taille_fic!=INVALID_FILE_SIZE)
    {
      unsigned char *buffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(unsigned char*)*taille_fic+1);
      if (buffer == NULL)
      {
        CloseHandle(Hlog);
        SB_add_T(TABL_CONF-1, "LOG : Out of memory");
        return;
      }

      //lecture du fichier
      DWORD copiee, position = 0, increm = 0;
      if (taille_fic > DIXM)increm = DIXM;
      else increm = taille_fic;

      while (position<taille_fic && increm!=0)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
      {
        copiee = 0;
        ReadFile(Hlog, buffer+position, increm,&copiee,0);
        position +=copiee;
        if (taille_fic-position < increm)increm = taille_fic-position ;
      }

      //traitement de l'entête
      typedef struct _EVENTLOGHEADER_EVTX {
        unsigned char MagicString[8];                 //8 ElfFile + 0x00
        unsigned long long int FirstChunk;            //8
        unsigned long long int LastChunk;             //8
        unsigned long long int NextRecord;            //8
        unsigned int HeaderSize;                      //4 toujours 0x80 = 128
        unsigned short MinorVersion;                  //2 toujours 1
        unsigned short MajorVersion;                  //2 toujours 3
        unsigned long long int CurrentSize;           //8
        unsigned char Reserved[72];                   //72
        unsigned int flag;                            //4
        unsigned int checksum;                        //4 checksum
      }EVENTLOGHEADER_EVTX;

      //on test la validité du fichier ^^
      EVENTLOGHEADER_EVTX* h_evtx = (EVENTLOGHEADER_EVTX*)buffer;
      if (!strcmp(h_evtx->MagicString,"ElfFile") && h_evtx->HeaderSize == 0x80 && h_evtx->MinorVersion>0 && h_evtx->MajorVersion>0)
      {
        //header d'une zone d'enregistrement
        typedef struct _EVENTLOGCHUNKHEADER_EVTX {
          unsigned char MagicString[8];                 //8 ElfChnk + 0x00
          unsigned long long int FirstRecord;           //8
          unsigned long long int LastRecord;            //8
          unsigned long long int FirstLogicRecord;      //8
          unsigned long long int LastLogicRecord;       //8
          unsigned int HeaderSize;                      //4 toujours 0x80 = 128
          unsigned int LastRecord_Offset;               //4
          unsigned int NextRecord_Offset;               //4
          unsigned int checksum_object;                 //4
          unsigned char Reserved[64];                   //64
          unsigned int flag;                            //4
          unsigned int checksum;                        //4 checksum
        }EVENTLOGCHUNKHEADER_EVTX;
        EVENTLOGCHUNKHEADER_EVTX *h_cheader;

        unsigned int nb;

        //traitement des enregistrement
        //commence à 0x1000 et un nouveau tous les 0x10000
        STRING_TABLE my_s_table;
        char *pos_buffer = buffer + 0x1000;
        DWORD p,tp=0,pos = 0x1000;
        DWORD i,nb_enrg = 0;

        LINE_ITEM lv_line[SIZE_UTIL_ITEM];
        strcpy(lv_line[0].c,eventfile);

        do
        {
          //vérification
          h_cheader = (EVENTLOGCHUNKHEADER_EVTX *)pos_buffer;
          if (strcmp(h_cheader->MagicString,"ElfChnk")!=0)break;

          //lecture du nombre d'enregistrement de se chunk
          nb_enrg = h_cheader->LastRecord - h_cheader->FirstRecord + 1;

          //lecture de la string table
          nb = ReadStringTable(pos_buffer+0x80, taille_fic-0x80-pos,pos, &my_s_table, NB_ST_ITEM);
          if (nb>0)
          {
            //traitement des enregistrements ^^
            //le 1er record commence toujours à 0x1200
            pos_buffer+=0x200;
            i=0;
            p=0;
            do
            {
              pos_buffer+=p;
              p = ReadRecord(pos_buffer, taille_fic-tp,&my_s_table,lv_line, hlv);
              tp+=p;
            }while ( p!= 0 && i<nb_enrg && tp < taille_fic);
          }

          pos+=0x10000;
          tp = pos;
          pos_buffer= buffer + pos;
        }while (pos<(taille_fic));
      }
      HeapFree(GetProcessHeap(), 0,buffer);
    }
  }
  CloseHandle(Hlog);
}

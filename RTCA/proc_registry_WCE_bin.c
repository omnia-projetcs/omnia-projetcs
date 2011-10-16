//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http:\\omni.a.free.fr
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//traitement des fichiers de base de registre Windows CE
void AnalyseFichierRegWCEBin(char *fic)
{
  //ouverture du fichier
  HANDLE Hfic = CreateFile(fic,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic == INVALID_HANDLE_VALUE)
  {
    SB_add_T(TABL_CONF-1, "REG : File access error");
    return;
  }
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic<1 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    SB_add_T(TABL_CONF-1, "REG : File error");
    return;
  }

  //allocation
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    SB_add_T(TABL_CONF-1, "REG : Out of memory");
    return;
  }

  //chargement du fichier en mémoire par morceau de 10mo
  //(pour éviter les pertes de synchro sur certaines machines)
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
  CloseHandle(Hfic);

  //traitement du buffer
  if (taille_fic>0x5000)
  {
    typedef struct ekim_header
    {
      char padding[8];        //hexa : 00 04 00 00 00 00 00 00
      int id;                 //0x4D494B45 = EKIM
      char unknow[36];
      //FILETIME timestamp;     //Date de dernière modification ??
    }EKIM_HEADER;

    #define EKIM_CH_HEADER_SIZE  26
    typedef struct ekim_chaine
    {
      char id[6];             //id hexa : 00 C0 00 00 00 00
      char unknow[16];
      unsigned short size;    //taille de la chaine parfois (attention calcul en unicode)
      unsigned short type;    //type parfois
      char chaine[1];
    }EKIM_CHAINE;
/*
    if (((EKIM_HEADER*)buffer)->id == 0x4D494B45) //Fichier WINDOWS CE standard
    {
      position = 0x5000;
      char *c = &buffer[0x5000]; //début de données  ???
      EKIM_CHAINE *ec;

      char tmp[MAX_PATH];

      while (position<taille_fic-4)
      {
        while (position<taille_fic-4 && *c != 0x00 && *(c+1) != 0xC0)
        {
          c++;
          position++;
        }

        snprintf(tmp,MAX_PATH,"%S",*(c+EKIM_CH_HEADER_SIZE));
        MessageBox(0,tmp,"TEST!",MB_OK|MB_TOPMOST);

        position+=strlen(tmp)*2 + EKIM_CH_HEADER_SIZE;
        c+=strlen(tmp)*2 + EKIM_CH_HEADER_SIZE;


        //snprintf(tmp,MAX_PATH,"%02X%02X",ec->id[0]&0xFF,ec->id[1]&0xFF);
        //MessageBox(0,tmp,"TEST0!",MB_OK|MB_TOPMOST);






        //MessageBox(0,"OOO","TEST!",MB_OK|MB_TOPMOST);


       /* if (ec->id[0] == 0x00 && ec->id[1] == 0xC0 && ec->id[2] == 0x00 && ec->id[3] == 0x00 && ec->id[4] == 0x00 && ec->id[5] == 0x00)
        {
          snprintf(tmp,MAX_PATH,"%S",ec->chaine);
          MessageBox(0,tmp,"TEST!",MB_OK|MB_TOPMOST);

          position+=strlen(tmp)*2 + EKIM_CH_HEADER_SIZE;
          c+=strlen(tmp)*2 + EKIM_CH_HEADER_SIZE;
        }else
        {
          c++;
          position++;
        }*//*
      }






      //0x5000 => début de données  ???


      //Texte en unicode, données = taille (2 octets) + type (2 octets)
      // la taille est exprimé en hexa mais en fait en décimal 62(hexa) = 62(decimal)
      // les données étant en unicode il faut multiplier la taille par Deux

      // dans certains cas la taille est juste avant la zone de texte ^^
      // le caractères A avec accent semble être le séparateur

      //début des données : 6044

      //format d'un enregistrement :
      //000C  // id = A avec accent
      //24 octets ???
      //Chaîne unicode








      //lecture de la date pour vérification ^^
      /*char tmp_t[MAX_PATH];
      FILETIME LocalFileTime;
      SYSTEMTIME SysTimeCreation;

      FileTimeToLocalFileTime(&(((EKIM_HEADER*)buffer)->timestamp), &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SysTimeCreation);

      snprintf(tmp_t,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeCreation.wYear,SysTimeCreation.wMonth,SysTimeCreation.wDay,SysTimeCreation.wHour,SysTimeCreation.wMinute,SysTimeCreation.wSecond);
      MessageBox(0,tmp_t,"TEST!",MB_OK|MB_TOPMOST);*/

/*
char tmp_t[MAX_PATH];
sprintf(tmp_t,"pevlr->Length: %d",pevlr->Length);
MessageBox(0,tmp_t,"TEST!",MB_OK|MB_TOPMOST);

  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeCreation,SysTimeModification,SysTimeAcces;
        //traitement des dates
        FileTimeToLocalFileTime(&(data.ftCreationTime), &LocalFileTime);
        if (FileTimeToSystemTime(&LocalFileTime, &SysTimeCreation))
          snprintf(lv_line[4].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTimeCreation.wYear,SysTimeCreation.wMonth,SysTimeCreation.wDay,SysTimeCreation.wHour,SysTimeCreation.wMinute,SysTimeCreation.wSecond);
        else lv_line[4].c[0]=0;
*/



    }



    /*


    if (((REGF_HEADER*)buffer)->id == 0x66676572) //Fichier REG standard
    {
      taille_fic = position;
      position = 0x1000; //début du 1er hbin
      HANDLE hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_VIEW);
      HANDLE htv = GetDlgItem(Tabl[TABL_REGISTRY],TV_REGISTRY_VIEW);

      //pointeurs pour les différents types de structure
      HBIN_HEADER *hb_h;
      DWORD pos_fhbin = 0;

      //recherche du 1er hbin !! (en cas de bug)
     /* while(position<taille_fic-4)
      {
        hb_h = (HBIN_HEADER *)&buffer[position];
        if (hb_h->id == 0x6E696268 )  //hbin
        {
          if (pos_fhbin == 0)pos_fhbin = position;
          position=position+HBIN_HEADER_SIZE;//entête hbin
          break;
        }else position++;
      }*/
      //identification de la syskey
      /*SyskeyExtract(buffer, taille_fic, pos_fhbin+HBIN_HEADER_SIZE, position,fic);

      //lecture de la première clée + bouclage
      Readnk(fic,buffer,taille_fic, pos_fhbin+HBIN_HEADER_SIZE, "", position,hlv,TVI_ROOT,htv);
    }*/
  //}

  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}

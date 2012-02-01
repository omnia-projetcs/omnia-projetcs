//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
void ReadDATFile(char *file, HANDLE hlv, char *description)
{
  //ouverte et mise en mémoire du fichier !!!
  //ouverture du fichier
  HANDLE Hfic = CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0/*FILE_FLAG_SEQUENTIAL_SCAN*/,0);
  if (Hfic == INVALID_HANDLE_VALUE)
  {
    SB_add_T(SB_ONGLET_CONF-1, "CONF : File access error");
    return;
  }
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  //0x5068 = 0x5000 = start + 0x68 = 104 = taille min d'un enregistrement
  if (taille_fic<0x5068 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    return;
  }

  //allocation
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    SB_add_T(SB_ONGLET_CONF-1, "CONF : Out of memory");
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
  if (position>0 && taille_fic > 0x5068)
  {
    //on test si bien un fichier dat d'historique
    if (buffer[0]!='C' || buffer[7]!='U' || buffer[10]!='C')
    {
      HeapFree(GetProcessHeap(), 0, buffer);
      return;
    }

    //structure du format des enregistrements
    typedef struct //URL + LEAK format
    {
      char type[4];     //URL, REDR, LEAK
      char size;        //size*128 = taille total des données
      char padding[3];
      FILETIME time_create;
      FILETIME time_last_access;
      char unknow1[60];
      unsigned short count;
      char unknow2[18];
      char url[1];  //URL +2caract = fichier + 2 caract =header
    }URL_INDEX_DAT;
    URL_INDEX_DAT *url_dat;

    LINE_ITEM lv_line[SIZE_UTIL_ITEM];
    position = 0x5000;
    char *c = buffer+position;

    SYSTEMTIME SysTime;

    strncpy(lv_line[0].c,file,MAX_LINE_SIZE);
    strncpy(lv_line[3].c,"Internet Explorer",MAX_LINE_SIZE);
    strncpy(lv_line[4].c,"History",MAX_LINE_SIZE);

    do
    {
      //MessageBox(0,"T","TEST!",MB_OK|MB_TOPMOST);
      url_dat = (URL_INDEX_DAT*)c;

      if ((url_dat->type[0] == 'U' && url_dat->type[1] == 'R' && url_dat->type[2] == 'L') ||
          (url_dat->type[0] == 'L' && url_dat->type[1] == 'E' && url_dat->type[2] == 'A' && url_dat->type[3] == 'K'))
      {
        lv_line[1].c[0]=0;
        lv_line[2].c[0]=0;
        lv_line[6].c[0]=0;
        lv_line[7].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s",url_dat->type);  //Parameter
        snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",url_dat->url);   //Data
        lv_line[1].c[4]=0;

        //count + description
        if (url_dat->type[0] == 'U')snprintf(lv_line[5].c,MAX_LINE_SIZE,"(use count %u) %s",url_dat->count,description);
        else strncpy(lv_line[5].c,description,MAX_LINE_SIZE);

        if (url_dat->time_create.dwLowDateTime != 0 || url_dat->time_create.dwHighDateTime != 0)
        {
          if (FileTimeToSystemTime(&(url_dat->time_create), &SysTime) != 0)//traitement de l'affichage de la date
            if (SysTime.wYear > 1601 && SysTime.wYear<3000 && SysTime.wMonth<13 && SysTime.wDay < 32 && SysTime.wHour<24 && SysTime.wMinute<60) //contrôle de la validité de la date
              snprintf(lv_line[6].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
        }

        if (url_dat->time_last_access.dwLowDateTime != 0 || url_dat->time_last_access.dwHighDateTime != 0)
        {
          if (FileTimeToSystemTime(&(url_dat->time_last_access), &SysTime) != 0)//traitement de l'affichage de la date
            if (SysTime.wYear > 1601 && SysTime.wYear<3000 && SysTime.wMonth<13 && SysTime.wDay < 32 && SysTime.wHour<24 && SysTime.wMinute<60) //contrôle de la validité de la date
              snprintf(lv_line[7].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
        }

        AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);

        position+= (url_dat->size * 128);
        c = c + (url_dat->size * 128);
      }else if (url_dat->type[0] == 'R' && url_dat->type[1] == 'E' && url_dat->type[2] == 'D' && url_dat->type[3] == 'R')
      {
        lv_line[1].c[0]=0;
        lv_line[2].c[0]=0;
        lv_line[6].c[0]=0;
        lv_line[7].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s",url_dat->type);  //Parameter
        snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",c+16);           //Data
        lv_line[1].c[4]=0;

        //description
        strncpy(lv_line[5].c,description,MAX_LINE_SIZE);;

        AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);

        position+= (url_dat->size * 128);
        c = c + (url_dat->size * 128);
      }else {c++;position++;}
    }while(position<taille_fic);
  }

  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}
//------------------------------------------------------------------------------
void Enum_IE_conf(HANDLE hlv)
{
//général
  //version IE
  LireValueRegToConf(HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE",
                     "SOFTWARE\\Microsoft\\Internet Explorer\\Version Vector\\",
                     "IE", "Internet Explorer", "Settings", "Version", hlv);
  //chemin de téléchargement par défaut
  LireValueRegToConf(HKEY_CURRENT_USER, "HKEY_CURRENT_USER",
                     "SOFTWARE\\Microsoft\\Internet Explorer\\",
                     "Download Directory", "Internet Explorer", "Settings", "Download Directory", hlv);
  //page de démarrage
  LireValueRegToConf(HKEY_CURRENT_USER, "HKEY_CURRENT_USER",
                     "SOFTWARE\\Microsoft\\Internet Explorer\\Main\\",
                     "Start Page", "Internet Explorer", "Settings", "Start page", hlv);
  //search page
  LireValueRegToConf(HKEY_CURRENT_USER, "HKEY_CURRENT_USER",
                     "SOFTWARE\\Microsoft\\Internet Explorer\\Main\\",
                     "Search Page", "Internet Explorer", "Settings", "Search page", hlv);

//chemins
  //chemins des fichiers importants
  EnumRegToConfValues(HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE",
                      "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\SafeSites\\",
                      "Internet Explorer", "Settings", "Safe Sites", hlv);
  EnumRegToConfValues(HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE",
                      "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ActiveX Cache\\",
                      "Internet Explorer", "Settings", "ActiveX Cache", hlv);
}

//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
void LireValeurGuidesExt(LINE_ITEM *lv_line, HANDLE hlv)
{
  //on découpe la clé pour récupérer les informations !
  char chemin[MAX_LINE_SIZE];
  HKEY CleTmp=0;

  switch(lv_line[1].c[5])
  {
    case 'C':
      switch(lv_line[1].c[13])
      {
        case 'R'://HKEY_CLASSES_ROOT
          strcpy(chemin,lv_line[1].c+18);
          if (RegOpenKey(HKEY_CLASSES_ROOT,chemin,&CleTmp)!=ERROR_SUCCESS)return;
        break;
        case 'U'://HKEY_CURRENT_USER
          strcpy(chemin,lv_line[1].c+18);
          if (RegOpenKey(HKEY_CURRENT_USER,chemin,&CleTmp)!=ERROR_SUCCESS)return;
        break;
        case 'C'://HKEY_CURRENT_CONFIG
          strcpy(chemin,lv_line[1].c+20);
          if (RegOpenKey(HKEY_CURRENT_CONFIG,chemin,&CleTmp)!=ERROR_SUCCESS)return;
        break;
      }
    break;
    case 'L'://HKEY_LOCAL_MACHINE
      strcpy(chemin,lv_line[1].c+19);
      if (RegOpenKey(HKEY_LOCAL_MACHINE,chemin,&CleTmp)!=ERROR_SUCCESS)return;
    break;
    case 'D'://HKEY_DYN_DATA
      strcpy(chemin,lv_line[1].c+14);
      if (RegOpenKey(HKEY_DYN_DATA,chemin,&CleTmp)!=ERROR_SUCCESS)return;
    break;
    case 'P'://HKEY_PERFORMANCE_DATA
      strcpy(chemin,lv_line[1].c+22);
      if (RegOpenKey(HKEY_PERFORMANCE_DATA,chemin,&CleTmp)!=ERROR_SUCCESS)return;
    break;
    case 'U'://HKEY_USERS
      strcpy(chemin,lv_line[1].c+11);
      if (RegOpenKey(HKEY_USERS,chemin,&CleTmp)!=ERROR_SUCCESS)return;
    break;
  }

  //lecture
  DWORD tailleCle=MAX_LINE_SIZE;
  lv_line[6].c[0]=0;
  if (RegQueryValueEx(CleTmp, lv_line[2].c, 0, 0, (LPBYTE)lv_line[6].c, &tailleCle)!=ERROR_SUCCESS)
  {
     RegCloseKey(CleTmp);
     tailleCle = 0;
  }

  if (tailleCle == 0)
  {
    if (lv_line[3].c[0]==0) snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);
    else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NO VALUE] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);
    lv_line[3].c[0]=0;
  }else
  {
    //type
    switch(lv_line[4].c[0])
    {
      case 'S': //string
        if (!strcmp(lv_line[3].c,lv_line[6].c))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);
        else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);

        snprintf(lv_line[3].c,MAX_LINE_SIZE,"%s",lv_line[6].c);
      break;
      case 'N': //dword
        sprintf(lv_line[7].c,"%02x%02x%02x%02x",lv_line[6].c[3]&0xff,lv_line[6].c[2]&0xff,lv_line[6].c[1]&0xff,lv_line[6].c[0]&0xff);

        if (!strcmp(lv_line[3].c,lv_line[7].c))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);
        else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);

        strcpy(lv_line[3].c,lv_line[7].c);
      break;
      case 'B': //hex
      {
        DWORD i;
        lv_line[7].c[0]=0;
        for (i=0;i<tailleCle && i*2<MAX_LINE_SIZE;i++)
        {
          sprintf(lv_line[7].c+i*2,"%02X",lv_line[6].c[i]&0xff);
        }
        lv_line[7].c[tailleCle*2]=0;

        if (!strcmp(lv_line[3].c,lv_line[7].c))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);
        else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);

        strcpy(lv_line[3].c,lv_line[7].c);
      }
      break;
      default : //binaire par défaut
      {
        DWORD i;
        lv_line[7].c[0]=0;
        for (i=0;i<tailleCle && i*2<MAX_LINE_SIZE;i++)
        {
          sprintf(lv_line[7].c+i*2,"%02X",lv_line[6].c[i]&0xff);
        }
        lv_line[7].c[tailleCle*2]=0;

        if (!strcmp(lv_line[3].c,lv_line[7].c))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK][UNKNOW] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);
        else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK][UNKNOW] Good value : %s ; %s",lv_line[3].c,lv_line[5].c);

        strcpy(lv_line[3].c,lv_line[7].c);
      }
      break;
    }
  }

  //additem
  lv_line[6].c[0]=0;
  AddToLV(hlv, lv_line, 5);

  //On ferme la cle
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
void TestRegistrGuide(char *fic)
{
  //ouverture du fichier
  HANDLE Hfic = CreateFile(fic,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic == INVALID_HANDLE_VALUE)return;

  //taille du fichier
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic<1 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    return;
  }

  //allocation
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, sizeof(char*)*taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    SB_add_T(TABL_CONF-1, "REG : Out of memory");
    return;
  }

  //lecture du fichier
  DWORD copiee;
  if (ReadFile(Hfic, buffer, taille_fic,&copiee,0))
  {
      taille_fic = copiee;
      //test si format Wildstring: on passe les 2 premier caractère et on format le tout
      if(buffer[0]==-1 && buffer[1]==-2)
      {
        char *buffer2 = (char *)HeapAlloc(GetProcessHeap(), 0, sizeof(char*)*taille_fic+1);
        if (buffer2 != NULL)
        {
          DWORD i, j=0;
          for (i=2;i<taille_fic;i+=2)
          {
            buffer2[j++]=buffer[i];
          }
          buffer2[j] = 0;
          taille_fic = j-1;
          strcpy(buffer,buffer2);
          HeapFree(GetProcessHeap(), 0,buffer2);
        }
      }

      //traitement pour ajout à la listeview
      DWORD i,j;
      char buffer_ligne[MAX_LINE_SIZE+1];
      char tmp[MAX_LINE_SIZE+1];
      BOOL continues = FALSE;
      char *p;

      LINE_ITEM lv_line[SIZE_UTIL_ITEM];
      lv_line[5].c[0]=0;
      lv_line[6].c[0]=0;
      strncpy(lv_line[0].c,fic,MAX_LINE_SIZE);

      HANDLE hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF);

      for (i=0;i<taille_fic;i++)
      {
        //copie d'une ligne
        j=0;
        while (buffer[i] && buffer[i]!='\n' && i<taille_fic && j<MAX_LINE_SIZE)buffer_ligne[j++] = buffer[i++];

        //si la ligne fait plus de 1 caractère on continue !!
        if (j>2)
        {
          //traitement du fin de chaîne
          if (buffer_ligne[j-1] == '\r')buffer_ligne[j-1]=0;
          else buffer_ligne[j]=0;

          if (continues)
          {
            if (buffer_ligne[0]==' ' && buffer_ligne[1]==' ')
              strcpy(tmp,buffer_ligne+2); //passage des 2 espaces de début
            else strcpy(tmp,buffer_ligne);

            if (tmp[strlen(tmp)-1]=='\\')//seulement l'hexa
            {
              tmp[strlen(tmp)-1]=0;
              SupChar(tmp,lv_line[3].c+strlen(lv_line[3].c),',',MAX_LINE_SIZE-strlen(lv_line[3].c));
            }else //si fin de la zone de donnée
            {
              SupChar(tmp,lv_line[3].c+strlen(lv_line[3].c),',',MAX_LINE_SIZE-strlen(lv_line[3].c));

              continues = FALSE;
              lv_line[5].c[0]=0;
              lv_line[6].c[0]=0;
              strncpy(lv_line[0].c,fic,MAX_LINE_SIZE);
              LireValeurGuidesExt(lv_line, hlv);
            }
          }else if (buffer_ligne[0] == ';') //description
          {
            strncpy(lv_line[5].c,buffer_ligne+1,MAX_LINE_SIZE);
          }else if (buffer_ligne[0] == '[') //début de chaîne
          {
            strncpy(lv_line[1].c,buffer_ligne+1,MAX_LINE_SIZE);
            //on supprime le ']' de fin
            lv_line[1].c[strlen(lv_line[1].c)-1]=0;
          }else if (buffer_ligne[0] == '@')
          {
            if (buffer_ligne[2]=='"') // des séparateurs
            {
              lv_line[2].c[0]=0;
              lv_line[3].c[0]=0;

              //data
              strcpy(lv_line[3].c,buffer_ligne+3);
              //on supprime le '"' de fin
              lv_line[3].c[strlen(lv_line[3].c)-1]=0;

              //on test si un type précis
              if ((lv_line[3].c[0]=='d' || lv_line[3].c[0]=='D')&&(lv_line[3].c[1]=='w' || lv_line[3].c[1]=='W')&&
                  (lv_line[3].c[2]=='o' || lv_line[3].c[2]=='O')&&(lv_line[3].c[3]=='r' || lv_line[3].c[3]=='R')&&
                  (lv_line[3].c[4]=='d' || lv_line[3].c[4]=='D')&& lv_line[3].c[5]==':'){strcpy(lv_line[4].c,"NUMBER");strcpy(tmp,lv_line[3].c+6);strcpy(lv_line[3].c,tmp);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&&lv_line[3].c[3]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+4);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[5]==')' &&
                       lv_line[3].c[6]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+7);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[6]==')' &&
                       lv_line[3].c[7]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+8);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[7]==')' &&
                       lv_line[3].c[8]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+9);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else strcpy(lv_line[4].c,"STRING");
              lv_line[5].c[0]=0;
              lv_line[6].c[0]=0;
              strncpy(lv_line[0].c,fic,MAX_LINE_SIZE);
              LireValeurGuidesExt(lv_line, hlv);
            }else
            {
              lv_line[3].c[0]=0;
              strcpy(lv_line[3].c,buffer_ligne+2);

                            //on test si un type précis
              if ((lv_line[3].c[0]=='d' || lv_line[3].c[0]=='D')&&(lv_line[3].c[1]=='w' || lv_line[3].c[1]=='W')&&
                  (lv_line[3].c[2]=='o' || lv_line[3].c[2]=='O')&&(lv_line[3].c[3]=='r' || lv_line[3].c[3]=='R')&&
                  (lv_line[3].c[4]=='d' || lv_line[3].c[4]=='D')&& lv_line[3].c[5]==':'){strcpy(lv_line[4].c,"NUMBER");strcpy(tmp,lv_line[3].c+6);strcpy(lv_line[3].c,tmp);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&&lv_line[3].c[3]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+4);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[5]==')' &&
                       lv_line[3].c[6]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+7);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[6]==')' &&
                       lv_line[3].c[7]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+8);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[7]==')' &&
                       lv_line[3].c[8]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+9);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else strcpy(lv_line[4].c,"STRING");

              //test si une chaine non finie avec un '\'
              if (lv_line[3].c[strlen(lv_line[3].c)-1]=='\\')
              {
                lv_line[3].c[strlen(lv_line[3].c)-1]=0;
                continues = TRUE;
              }else
              {
                lv_line[5].c[0]=0;
                lv_line[6].c[0]=0;
                strncpy(lv_line[0].c,fic,MAX_LINE_SIZE);
                LireValeurGuidesExt(lv_line, hlv);
              }
            }
          }else if (buffer_ligne[0] == '"')
          {
            //valeur
            strcpy(lv_line[2].c,buffer_ligne+1);
            p = lv_line[2].c;
            while (*p && !(*p=='"' && *(p+1)== '='))p++;
            *p=0;

            //données
            if (buffer_ligne[strlen(lv_line[2].c)+3]=='"')
            {
              lv_line[3].c[0]=0;
              strcpy(lv_line[3].c,buffer_ligne+strlen(lv_line[2].c)+4);
              lv_line[3].c[strlen(lv_line[3].c)-1]=0;//on supprime le '"' de fin

              //on test si un type précis
              if ((lv_line[3].c[0]=='d' || lv_line[3].c[0]=='D')&&(lv_line[3].c[1]=='w' || lv_line[3].c[1]=='W')&&
                  (lv_line[3].c[2]=='o' || lv_line[3].c[2]=='O')&&(lv_line[3].c[3]=='r' || lv_line[3].c[3]=='R')&&
                  (lv_line[3].c[4]=='d' || lv_line[3].c[4]=='D')&& lv_line[3].c[5]==':'){strcpy(lv_line[4].c,"NUMBER");strcpy(tmp,lv_line[3].c+6);strcpy(lv_line[3].c,tmp);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&&lv_line[3].c[3]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+4);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[5]==')' &&
                       lv_line[3].c[6]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+7);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[6]==')' &&
                       lv_line[3].c[7]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+8);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[7]==')' &&
                       lv_line[3].c[8]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+9);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else strcpy(lv_line[4].c,"STRING");

              lv_line[5].c[0]=0;
              lv_line[6].c[0]=0;
              strncpy(lv_line[0].c,fic,MAX_LINE_SIZE);
              LireValeurGuidesExt(lv_line, hlv);
            }
            else
            {
              lv_line[3].c[0]=0;
              strcpy(lv_line[3].c,buffer_ligne+strlen(lv_line[2].c)+3);

                            //on test si un type précis
              if ((lv_line[3].c[0]=='d' || lv_line[3].c[0]=='D')&&(lv_line[3].c[1]=='w' || lv_line[3].c[1]=='W')&&
                  (lv_line[3].c[2]=='o' || lv_line[3].c[2]=='O')&&(lv_line[3].c[3]=='r' || lv_line[3].c[3]=='R')&&
                  (lv_line[3].c[4]=='d' || lv_line[3].c[4]=='D')&& lv_line[3].c[5]==':'){strcpy(lv_line[4].c,"NUMBER");strcpy(tmp,lv_line[3].c+6);strcpy(lv_line[3].c,tmp);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&&lv_line[3].c[3]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+4);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[5]==')' &&
                       lv_line[3].c[6]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+7);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[6]==')' &&
                       lv_line[3].c[7]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+8);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else if ((lv_line[3].c[0]=='h' || lv_line[3].c[0]=='H')&&(lv_line[3].c[1]=='e' || lv_line[3].c[1]=='E')&&
                       (lv_line[3].c[2]=='x' || lv_line[3].c[2]=='X')&& lv_line[3].c[3]=='(' && lv_line[3].c[7]==')' &&
                       lv_line[3].c[8]==':'){strcpy(lv_line[4].c,"BINARY");strcpy(tmp,lv_line[3].c+9);SupChar(tmp,lv_line[3].c,',',MAX_LINE_SIZE);}
              else strcpy(lv_line[4].c,"STRING");

              //test si une chaine non finie avec un '\'
              if (lv_line[3].c[strlen(lv_line[3].c)-1]=='\\')
              {
                lv_line[3].c[strlen(lv_line[3].c)-1]=0;
                continues = TRUE;
              }else
              {
                if (lv_line[4].c[0] != 'H' && lv_line[4].c[0] != 'S' && lv_line[4].c[0] != 'N')strcpy(lv_line[4].c,"BINARY");

                lv_line[5].c[0]=0;
                lv_line[6].c[0]=0;
                strncpy(lv_line[0].c,fic,MAX_LINE_SIZE);
                LireValeurGuidesExt(lv_line, hlv);
              }
            }
          }
        }
      }
  }
  CloseHandle(Hfic);
  HeapFree(GetProcessHeap(), 0, buffer);
}
//------------------------------------------------------------------------------
void CheckRegistryFile()
{
  //chargement d'un fichier REG externe !!!
  OPENFILENAME ofn;
  char tmp[MAX_PATH]="";
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = Tabl[TABL_MAIN];
  ofn.lpstrFile = tmp;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter ="All files (*.*)\0*.*\0Registry (*.reg)\0*.reg\0";
  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST| OFN_EXPLORER;
  ofn.lpstrDefExt ="*.*\0";

  if (GetOpenFileName(&ofn)==TRUE)
  {
    //test par rapport à la configuration actuelle
    TestRegistrGuide(tmp);

    //ouverture de l'onglet configuration registre
    ViewTabl(TABL_REGISTRY);

    TV_REGISTRY_VISIBLE = FALSE;
    TABL_ID_REG_VISIBLE = 1;
    ShowWindow(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_VIEW), SW_HIDE);
    ShowWindow(GetDlgItem(Tabl[TABL_REGISTRY],TV_VIEW), SW_HIDE);
    ShowWindow(GetDlgItem(Tabl[TABL_REGISTRY],BT_TREE_VIEW), SW_HIDE);
    ShowWindow(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), SW_SHOW);

    //sélection de l'item n°2 de la liste !
    SendDlgItemMessage(Tabl[TABL_REGISTRY],CB_REGISTRY_VIEW, CB_SETCURSEL,(WPARAM)1, (LPARAM)0);
  }
}

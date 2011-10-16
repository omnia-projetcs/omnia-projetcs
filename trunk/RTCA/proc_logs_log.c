//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//traitement des formats de journaux de type linux/Unix
void TraiterlogFile(char *path, HANDLE hlv)
{
  //ouverture du fichier
  HANDLE Hlog = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
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
        DWORD copiee=0;
        ReadFile(Hlog, buffer, taille_fic,&copiee,0);
        taille_fic = copiee;

        //traitement des données
        LINE_ITEM lv_line[NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]];
        char ligne[MAX_LINE_SIZE];
        char *l, *lv;

        //init des variables
        strcpy(lv_line[0].c,path); //journal

        lv_line[2].c[0]=0;         //ID
        lv_line[6].c[0]=0;         //Type
        lv_line[7].c[0]=0;         //USER

        //gestion suivant le type de format de fichier ^^
        BOOL date_normal_format;
        if (buffer[9] == buffer[12] && buffer[12] == ':')date_normal_format = FALSE;
        else if(buffer[13] == buffer[16] && buffer[16] == ':')date_normal_format = TRUE;
        else
        {
          //format non pris en charge !!!
          HeapFree(GetProcessHeap(), 0, buffer);
          CloseHandle(Hlog);
          //MessageBox(0,path,"File format is not supported !",MB_OK|MB_TOPMOST|MB_ICONERROR);
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
            if (date_normal_format)
            {
              //2011-08-05 18:44:23 - > 2011/08/15-10:34:57
              lv_line[3].c[0] =ligne[0]; //Année
              lv_line[3].c[1] =ligne[1];
              lv_line[3].c[2] =ligne[2];
              lv_line[3].c[3] =ligne[3];
              lv_line[3].c[4] ='/';
              lv_line[3].c[5] =ligne[5]; //Mois
              lv_line[3].c[6] =ligne[6];
              lv_line[3].c[7] ='/';
              lv_line[3].c[8] =ligne[8]; //Jours
              lv_line[3].c[9] =ligne[9];
              lv_line[3].c[10]='-';
              lv_line[3].c[11]=ligne[11]; //Heures
              lv_line[3].c[12]=ligne[12];
              lv_line[3].c[13]=':';
              lv_line[3].c[14]=ligne[14]; //Minutes
              lv_line[3].c[15]=ligne[15];
              lv_line[3].c[16]=':';
              lv_line[3].c[17]=ligne[17]; //Secondes
              lv_line[3].c[18]=ligne[18];
              lv_line[3].c[19]=0;

              k=20;
              l = ligne+ 20;//on passe la dat
            }else
            {
              //Aug 14 14:09:37 - > 2011/08/15-10:34:57
              lv_line[3].c[0] ='?'; //Année
              lv_line[3].c[1] ='?';
              lv_line[3].c[2] ='?';
              lv_line[3].c[3] ='?';
              lv_line[3].c[4] ='/';
              switch(ligne[0])      //mois
              {
                case 'J':
                  if (ligne[1] == 'a'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '1';}        //January
                  else if (ligne[1] == 'u')
                  {
                    if (ligne[2] == 'n'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '6';}      //June
                    else if (ligne[2] == 'l'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '7';} //July
                    else {lv_line[3].c[5] = '_';lv_line[3].c[6] = '_';}
                  }else {lv_line[3].c[5] = '_';lv_line[3].c[6] = '_';}
                break;
                case 'F': lv_line[3].c[5] = '0';lv_line[3].c[6] = '2';break;                //February
                case 'M':
                  if (ligne[2] == 'r'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '3';}        //March
                  else if (ligne[2] == 'y'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '5';}   //May
                  else {lv_line[3].c[5] = '_';lv_line[3].c[6] = '_';}
                break;
                case 'A':
                  if (ligne[1] == 'p'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '4';}        //April
                  else if (ligne[1] == 'u'){lv_line[3].c[5] = '0';lv_line[3].c[6] = '8';}   //August
                  else {lv_line[3].c[5] = '_';lv_line[3].c[6] = '_';}
                break;
                case 'S': lv_line[3].c[5] = '0';lv_line[3].c[6] = '9';break;                //September
                case 'O': lv_line[3].c[5] = '1';lv_line[3].c[6] = '0';break;                //October
                case 'N': lv_line[3].c[5] = '1';lv_line[3].c[6] = '1';break;                //November
                case 'D': lv_line[3].c[5] = '1';lv_line[3].c[6] = '2';break;                //December
                default : lv_line[3].c[5] = '_';lv_line[3].c[6] = '_';break;
              }
              lv_line[3].c[7] ='/';
              lv_line[3].c[8] =ligne[4]; // jour
              lv_line[3].c[9] =ligne[5];
              lv_line[3].c[10]='-';
              lv_line[3].c[11]=ligne[7]; //Heures
              lv_line[3].c[12]=ligne[8];
              lv_line[3].c[13]=ligne[9];
              lv_line[3].c[14]=ligne[10]; //Minutes
              lv_line[3].c[15]=ligne[11];
              lv_line[3].c[16]=ligne[12];
              lv_line[3].c[17]=ligne[13]; //Secondes
              lv_line[3].c[18]=ligne[14];
              lv_line[3].c[19]=0;
              k=16;
              l = ligne+ 16;//on passe la date
            }

            //Source
            lv = lv_line[4].c;
            while (k<MAX_LINE_SIZE && k<strlen(ligne) && *l && *l !=' ')
            {
              *lv++ = *l++;
              k++;
            }
            *lv = 0;

            //Description
            l++; // on passe l'espace
            lv = lv_line[5].c;
            while (k<MAX_LINE_SIZE && k<strlen(ligne) && *l)
            {
              *lv++ = *l++;
              k++;
            }
            *lv = 0;
            snprintf(lv_line[1].c,MAX_LINE_SIZE,"%lu",j++);

            //ajout
            AddToLV_log(hlv, lv_line, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]);
          }
        }
        HeapFree(GetProcessHeap(), 0, buffer);
      }else SB_add_T(TABL_CONF-1, "LOG : Out of memory");
    }
  }
  CloseHandle(Hlog);
}

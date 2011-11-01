//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
int LireValeurGuidesExt(HKEY ENTETE,char *file, char *chemin,char *nom,unsigned int tailleMax,unsigned int type,char *Valeur, char *resultat_good, char *description, HANDLE hlv)
{
    //variables
    DWORD tailleCle=0;
    HKEY CleTmp=0;

    // on ouvre la cle
    if (RegOpenKey(ENTETE,chemin,&CleTmp)!=ERROR_SUCCESS)
       return FALSE;

    //On recupere la taille de la cle qui va être lue
    if (RegQueryValueEx(CleTmp, nom, 0, 0, 0, &tailleCle)!=ERROR_SUCCESS)
    {
       RegCloseKey(CleTmp);
       return FALSE;
    }

    //permet de gérer les chaines énormes
    if (Valeur == 0)
    {
      RegCloseKey(CleTmp);
      return tailleCle;
    }else if (tailleMax<tailleCle)
    {
      RegCloseKey(CleTmp);
      return -1;
    }else Valeur[0]=0;

    if (RegQueryValueEx(CleTmp, nom, 0, 0, Valeur, &tailleCle)!=ERROR_SUCCESS)
    {
       RegCloseKey(CleTmp);
       tailleCle = 0;
    }

    //traitement
    LINE_ITEM lv_line[SIZE_UTIL_ITEM];

    snprintf(lv_line[0].c,MAX_LINE_SIZE,"%s",file);
    lv_line[5].c[0]=0;

    //Key
    if (ENTETE == HKEY_LOCAL_MACHINE)           snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_LOCAL_MACHINE\\%s",chemin);
    else if (ENTETE == HKEY_USERS)              snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_USERS\\%s",chemin);
    else if (ENTETE == HKEY_PERFORMANCE_DATA)   snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_PERFORMANCE_DATA\\%s",chemin);
    else if (ENTETE == HKEY_DYN_DATA)           snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_DYN_DATA\\%s",chemin);
    else if (ENTETE == HKEY_CLASSES_ROOT)       snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_CLASSES_ROOT\\%s",chemin);
    else if (ENTETE == HKEY_CURRENT_USER)       snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_CURRENT_USER\\%s",chemin);
    else if (ENTETE == HKEY_CURRENT_CONFIG)     snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_CURRENT_CONFIG\\%s",chemin);

    if (tailleCle == 0)
    {
          lv_line[3].c[0]=0;
          if (resultat_good[0]==0) snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",resultat_good,description);
          else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NO VALUE] Good value : %s ; %s",resultat_good,description);
    }else
    {
      //type
      switch(type)
      {
        case 0: //string
          snprintf(lv_line[3].c,MAX_LINE_SIZE,"%s",Valeur,description);

          if (!strcmp(Valeur,resultat_good))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",resultat_good,description);
          else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK] Good value : %s ; %s",resultat_good,description);
        break;
        case 1: //dword
          sprintf(lv_line[3].c,"%02x%02x%02x%02x",Valeur[0]&0xff,Valeur[1]&0xff,Valeur[2]&0xff,Valeur[3]&0xff);

          if (!strcmp(lv_line[3].c,resultat_good))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",resultat_good,description);
          else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK] Good value : %s ; %s",resultat_good,description);
        break;
        case 2: //hex
        {
          DWORD i;
          snprintf(lv_line[2].c,MAX_LINE_SIZE,"%02X",Valeur[0]&0xff);
          strcpy(lv_line[3].c,lv_line[2].c);

          for (i=1;i<tailleCle;i++)
          {
            snprintf(lv_line[2].c+(i*3-1),MAX_LINE_SIZE-i*3,",%02X",Valeur[i]&0xff);
            snprintf(lv_line[3].c+(i*2),MAX_LINE_SIZE-i*2,"%02X",Valeur[i]&0xff);
          }
          strncat(lv_line[2].c,"\0",MAX_LINE_SIZE);
          strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

          if (!strcmp(lv_line[2].c,resultat_good))snprintf(lv_line[4].c,MAX_LINE_SIZE,"[OK] Good value : %s ; %s",resultat_good,description);
          else snprintf(lv_line[4].c,MAX_LINE_SIZE,"[NOK] Good value : %s ; %s",resultat_good,description);
        }
        break;
        default :
          lv_line[3].c[0]=0;
          snprintf(lv_line[4].c,MAX_LINE_SIZE,"[UNKNOW] Good value : %s ; %s",resultat_good,description);
        break;
      }
    }

    //Value
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",nom);

    //additem
    AddToLV(hlv, lv_line, 5);

    //On ferme la cle
    RegCloseKey(CleTmp);
    return tailleCle;
}
//---------------------------------------------------------------------------------------------------------------
void TestRegistrGuide(char *str_guide)
{
  char *buffer;
  DWORD taille_fichier, copiee;
  int ret;
  HANDLE hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF);

  //lecture de guide
  //puis on ouvre le fichier de guide
  HANDLE Hsrc = CreateFile(str_guide,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hsrc != INVALID_HANDLE_VALUE)
  {
    taille_fichier = GetFileSize(Hsrc,NULL);
    if (taille_fichier)
    {
      buffer = (char*)LocalAlloc(LMEM_FIXED, sizeof(char)*taille_fichier+1);
      if (buffer!=0)
      {
        if (ReadFile(Hsrc, buffer, taille_fichier,&copiee,0))
        {
          char buffer_ligne[MAX_PATH*4+1];
          char commentaire[MAX_PATH*4+1];
          char chemin[MAX_PATH*4+1];
          char valeur[MAX_PATH+1];
          char resultat[MAX_PATH+1];
          char resultat_lu[MAX_PATH+1];
          unsigned int type; // 0=string, 1=dword, 2=hex/binaire
          HKEY hkClasse;

          unsigned long int i;
          unsigned int j,k,l;

          for (i=0;i<taille_fichier;i++)
          {
            //on copie ligne par ligne
            j=0;
            while(buffer[i] != '\n' && i<taille_fichier && j<MAX_PATH*4 )
            {
              buffer_ligne[j++] = buffer[i++];
              if (buffer[i-1] == buffer[i] && buffer[i] == '\\')i++;
            }
            buffer_ligne[j]=0;

            if (strlen(buffer_ligne))
            {
              //traitement de la ligne
              switch(buffer_ligne[0])
              {
                //commentaire
                case ';':
                     strcpy(commentaire,buffer_ligne+1);
                     commentaire[strlen(commentaire)-1]=0; //suppression du \r\n de fin
                break;
                //chemin
                case '[':
                     //on décortique suivant le type
                     //et on récupère le chemin
                     if (buffer_ligne[6]=='L' || buffer_ligne[6]=='l')
                     {
                       hkClasse = HKEY_LOCAL_MACHINE;
                       strcpy(chemin,buffer_ligne+20);
                     }else if (buffer_ligne[6]=='U' || buffer_ligne[6]=='u')
                     {
                       hkClasse = HKEY_USERS;
                       strcpy(chemin,buffer_ligne+12);
                     }else if (buffer_ligne[6]=='P' || buffer_ligne[6]=='p')
                     {
                       hkClasse = HKEY_PERFORMANCE_DATA;
                       strcpy(chemin,buffer_ligne+23);
                     }else if (buffer_ligne[6]=='D' || buffer_ligne[6]=='d')
                     {
                       hkClasse = HKEY_DYN_DATA;
                       strcpy(chemin,buffer_ligne+15);
                     }else if (buffer_ligne[6]=='C' || buffer_ligne[6]=='c')
                     {
                       if (buffer_ligne[7]=='L' || buffer_ligne[7]=='l')
                       {
                         hkClasse = HKEY_CLASSES_ROOT;
                         strcpy(chemin,buffer_ligne+19);
                       }else if (buffer_ligne[14]=='U' || buffer_ligne[14]=='u')
                       {
                         hkClasse = HKEY_CURRENT_USER;
                         strcpy(chemin,buffer_ligne+19);
                       }else if (buffer_ligne[14]=='C' || buffer_ligne[14]=='c')
                       {
                         hkClasse = HKEY_CURRENT_CONFIG;
                         strcpy(chemin,buffer_ligne+21);
                       }
                     }
                     //suppression de la fin : ']\r'
                     chemin[strlen(chemin)-2]=0;
                break;
                //gestion du nom de la valeur, de son type et de son contenu
                case '"':
                     k=1,l=0;
                     while(k<j && buffer_ligne[k]!='"')valeur[l++]=buffer_ligne[k++];
                     valeur[l]=0;

                     k+=2; // on passe : "=

                     if (buffer_ligne[k] == 'h' || buffer_ligne[k] == 'H')//hexa
                     {
                       //on vérifie si des () après le hex
                       if (buffer_ligne[k+3] == '(')
                       {
                         if (buffer_ligne[k+5] == ')')strcpy(resultat,buffer_ligne+(k+7));
                         else if (buffer_ligne[k+6] == ')')strcpy(resultat,buffer_ligne+(k+8));
                         else if (buffer_ligne[k+7] == ')')strcpy(resultat,buffer_ligne+(k+9));
                         else if (buffer_ligne[k+8] == ')')strcpy(resultat,buffer_ligne+(k+10));
                         else strcpy(resultat,buffer_ligne+(k+11));
                       }else
                         strcpy(resultat,buffer_ligne+(k+4));

                       //traitement du cas multiples-ligne
                       if (resultat[strlen(resultat)-2]=='\\')
                       {
                        //pas traité pour le moment mise à part la suppression du caractères + ,
                        resultat[strlen(resultat)-2]=0;

                        //dans le cas de lignes multiples on a 2 espaces "  "
                       }

                       resultat[strlen(resultat)-1]=0; // suppression du '\r' de fin de ligne
                       type = 2;
                     //dword
                     }else if (buffer_ligne[k] == 'd' || buffer_ligne[k] == 'D')
                     {
                       strcpy(resultat,buffer_ligne+(k+6));
                       type = 1;
                       resultat[8]=0;
                     }else //string
                     {
                       strcpy(resultat,buffer_ligne+(k+1));
                       resultat[strlen(resultat)-2]=0; // suppression du '"' de fin de ligne
                       type = 0;
                     }

                     //maintenant on effectue le test de la valeur
                     ret = LireValeurGuidesExt(hkClasse,str_guide,chemin,valeur,MAX_PATH,type,resultat_lu,resultat,commentaire,hlv);

                     if (ret == -1)
                     {
                       int taille = LireValeurGuidesExt(hkClasse,str_guide,chemin,valeur,MAX_PATH,type,resultat_lu,resultat,commentaire,hlv);
                       char *buff_tmp;
                       buff_tmp = malloc(sizeof(char)*taille+1);
                       ret = LireValeurGuidesExt(hkClasse,str_guide,chemin,valeur,taille,buff_tmp,type,resultat,commentaire,hlv);
                       if (ret>0)
                       {
                         strncpy(resultat_lu,buff_tmp,MAX_PATH);
                         free(buff_tmp);
                       }
                     }
                break;
                /*case '@': //nom vide
                    valeur[0]=0;
                    k=2; // on passe @=

                     if (buffer_ligne[k] == 'h' || buffer_ligne[k] == 'H')//hexa
                     {
                       //on vérifie si des () après le hex
                       if (buffer_ligne[k+3] == '(')
                       {
                         if (buffer_ligne[k+5] == ')')strcpy(resultat,buffer_ligne+(k+7));
                         else if (buffer_ligne[k+6] == ')')strcpy(resultat,buffer_ligne+(k+8));
                         else if (buffer_ligne[k+7] == ')')strcpy(resultat,buffer_ligne+(k+9));
                         else if (buffer_ligne[k+8] == ')')strcpy(resultat,buffer_ligne+(k+10));
                         else strcpy(resultat,buffer_ligne+(k+11));
                       }else
                         strcpy(resultat,buffer_ligne+(k+4));

                       //traitement du cas multiples-ligne
                       if (resultat[strlen(resultat)-2]=='\\')
                       {
                        //pas traité pour le moment mise à part la suppression du caractères + ,
                        resultat[strlen(resultat)-2]=0;

                        //dans le cas de lignes multiples on a 2 espaces "  "
                       }

                       resultat[strlen(resultat)-1]=0; // suppression du '\r' de fin de ligne
                       type = 2;
                     //dword
                     }else if (buffer_ligne[k] == 'd' || buffer_ligne[k] == 'D')
                     {
                       strcpy(resultat,buffer_ligne+(k+6));
                       resultat[8]=0;

                       type = 1;
                     }else //string
                     {
                       strcpy(resultat,buffer_ligne+(k+1));
                       resultat[strlen(resultat)-2]=0; // suppression du '"' de fin de ligne

                       type = 0;
                     }

                     //maintenant on effectue le test de la valeur
                     ret = LireValeurGuidesExt(hkClasse,str_guide,chemin,valeur,MAX_PATH,type,resultat_lu,resultat,commentaire,hlv);

                     if (ret == -1)
                     {
                       int taille = LireValeurGuidesExt(hkClasse,str_guide,chemin,valeur,MAX_PATH,type,resultat_lu,resultat,commentaire,hlv);
                       char *buff_tmp;
                       buff_tmp = malloc(sizeof(char)*taille+1);
                       ret = LireValeurGuidesExt(hkClasse,str_guide,chemin,valeur,taille,buff_tmp,type,resultat,commentaire,hlv);
                       if (ret>0)
                       {
                         strncpy(resultat_lu,buff_tmp,MAX_PATH);
                         free(buff_tmp);
                       }
                     }
                break;*/
              }
            }
          }
        }
        LocalFree(buffer);
      }
    }
  }
  CloseHandle(Hsrc);
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
  }
}

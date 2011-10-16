//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http:\\omni.a.free.fr
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
/*
FORMAT fichier reg :
  regf_header
  hbin_header
  DATA : (hbin_cell_pre_header + format spécifiques)
    -
*/
//------------------------------------------------------------------------------
void Readnk(char* file, char *buffer, DWORD taille_fic, DWORD position, char *parent, DWORD pos_fhbin, HANDLE hlv, HTREEITEM hitem, HANDLE htv)
{
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E)
  {
    LINE_ITEM lv_line[SIZE_UTIL_ITEM];
    strcpy(lv_line[0].c,file);           //fichier d'origine
    lv_line[6].c[0]=0;

    HTREEITEM new_hitem = 0;

    //nom
    if (parent[0]==0)
    {
      strncpy(lv_line[1].c,(char *)(buffer+position+HBIN_CELL_NK_SIZE),MAX_LINE_SIZE);
      if (nk_h->key_name_size>MAX_LINE_SIZE)lv_line[1].c[MAX_LINE_SIZE-1]=0;
      else lv_line[1].c[nk_h->key_name_size]=0;
      new_hitem = AjouterItemTreeViewRep(Tabl[TABL_REGISTRY], TV_VIEW,lv_line[1].c,hitem);
    }else
    {
      strcpy(lv_line[1].c,parent);
      strncat(lv_line[1].c,"\\",MAX_LINE_SIZE);

      strncpy(lv_line[2].c,(char *)(buffer+position+HBIN_CELL_NK_SIZE),MAX_LINE_SIZE);
      if (nk_h->key_name_size>MAX_LINE_SIZE)lv_line[2].c[MAX_LINE_SIZE-1]=0;
      else lv_line[2].c[nk_h->key_name_size]=0;

      strncat(lv_line[1].c,lv_line[2].c,MAX_LINE_SIZE);
      strncat(lv_line[1].c,"\0",MAX_LINE_SIZE);
      new_hitem = AjouterItemTreeViewRep(Tabl[TABL_REGISTRY], TV_VIEW,lv_line[2].c,hitem);
      lv_line[2].c[0]=0;
    }

    //traitement des valeures liées
    DWORD i;
    if (nk_h->nb_values>0&& nk_h->nb_values<taille_fic && ((pos_fhbin+nk_h->val_ls_offset-HBIN_HEADER_SIZE )<taille_fic))
    {
      S_ITEM_LS *item_ls;// = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+pos_fhbin-HBIN_HEADER_SIZE+4];
      HBIN_CELL_VK_HEADER *vk_h;
      SYSTEMTIME SysTime;
      DWORD k;
      char tmp[MAX_LINE_SIZE];
      char *c;

      for (i=0;i<nk_h->nb_values;i++)
      {
        //pour le 1er il faut passer les 4 premier octets représentant une taille ?
        item_ls = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+pos_fhbin+i*HBIN_CELL_ITEM_LS-HBIN_HEADER_SIZE+4];
        //lecture de l'offset de la valeure :
        if (item_ls->val_of>0 && (item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic)
        {
          //traitement des valeures !!!
          vk_h = (HBIN_CELL_VK_HEADER *)&buffer[item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE];

          //récupération du nom de la valeur :
          strncpy(lv_line[2].c,vk_h->value,MAX_LINE_SIZE);
          if (vk_h->name_size>=MAX_LINE_SIZE)lv_line[2].c[MAX_LINE_SIZE-1]=0;
          else lv_line[2].c[vk_h->name_size]=0;

          nb_items++;
          if (nb_items%1000 == 0)
          {
            snprintf(tmp,MAX_LINE_SIZE,"loading %lu values",nb_items);
            SB_add_T(SB_ONGLET_REGISTRY, tmp);
          }

          switch(vk_h->data_type)
          {
            //Chaines
            case 0x00000001 : //REG_SZ, chaine ASCII et Unicodes
              lv_line[3].c[0] = 0;
              if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
              {
                if (vk_h->data_size < 5)
                {
                  snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",vk_h->cdata_offset);
                  lv_line[3].c[vk_h->data_size/2]=0;
                }else snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);

                snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
              }else
              {
                if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=",lv_line[2].c);
                }else
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                }
              }
              AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_STR);
              strcpy(lv_line[4].c,"REG_SZ");

              //date de création
              if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lv_line[5].c[0]=0;
              AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
            break;
            case 0x00000002 : //REG_EXPAND_SZ, chaine ASCII et Unicodes, contient des path type %path%
              lv_line[3].c[0] = 0;
              if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
              {
                if (vk_h->data_size < 5)
                {
                  strcpy(lv_line[3].c,vk_h->cdata_offset);
                  lv_line[3].c[4]=0;
                }else snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);

                snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
              }else
              {
                if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=",lv_line[2].c);
                }else
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                }
              }
              AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_STR);
              strcpy(lv_line[4].c,"REG_EXPAND_SZ");

              //date de création
              if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lv_line[5].c[0]=0;
              AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
            break;
            case 0x00000006 : //REG_LINK, chaine ASCII et Unicodes, lien lien
              lv_line[3].c[0] = 0;
              if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
              {
                if (vk_h->data_size < 5)
                {
                  strcpy(lv_line[3].c,vk_h->cdata_offset);
                  lv_line[3].c[4]=0;
                }else snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);

                snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
              }else
              {
                if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=",lv_line[2].c);
                }else
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                }
              }
              AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_STR);
              strcpy(lv_line[4].c,"REG_LINK");

              //date de création
              if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lv_line[5].c[0]=0;
              AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
            break;
            case 0x00000007 : //REG_MULTI_SZ, multiples chaine ASCII et Unicodes, lien
                lv_line[3].c[0] = 0;

                if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
                {
                  if (vk_h->data_size < 5)
                  {
                    strcpy(lv_line[3].c,vk_h->cdata_offset);
                    lv_line[3].c[4]=0;
                  }else
                  {
                    //traitement des chaines multiples :
                    do
                    {
                      snprintf(tmp,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+2*strlen(lv_line[3].c)]);
                      strncat (lv_line[3].c,tmp,MAX_LINE_SIZE);
                      strncat (lv_line[3].c," \0",MAX_LINE_SIZE);
                    }while (strlen(lv_line[3].c)*2 < (vk_h->data_size));
                  }

                  snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
                }else
                {
                  if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=",lv_line[2].c);
                  }else
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                  }
                }
                AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_STR);
                strcpy(lv_line[4].c,"REG_MULTI_SZ");

                //date de création
                if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                  snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                else lv_line[5].c[0]=0;
                AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
              break;
              case 0x00000003 : //REG_BINARY, données binaires
                lv_line[3].c[0] = 0;
                if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
                {
                  if (vk_h->data_size < 5)
                  {
                    for (k=0;k<vk_h->data_size && k/2<MAX_LINE_SIZE;k++)
                    {
                      snprintf(tmp,10,"%02X",vk_h->cdata_offset[k]&0xff);
                      strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                    }
                    strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
                  }else
                  {
                    for (k=0;k<vk_h->data_size && k/2<MAX_LINE_SIZE;k++)
                    {
                      snprintf(tmp,10,"%02X",buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+k]&0xff);
                      strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                    }
                    strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
                  }
                  snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
                }else
                {
                  if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=",lv_line[2].c);
                  }else
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                  }
                }
                AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_BIN);
                strcpy(lv_line[4].c,"REG_BINARY");

                //date de création
                if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                  snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                else lv_line[5].c[0]=0;
                AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
              break;
              case 0x00000004 : //REG_DWORD, données numériques 32bitschar
              case 0x00000005 : //REG_DWORD, données numériques 32bits signées
                lv_line[3].c[0] = 0;
                if (vk_h->data_size > 0)
                {
                  snprintf(lv_line[3].c,MAX_LINE_SIZE,"%08X",(unsigned int)(vk_h->data_offset&0xFFFFFFFF)); // en dword offset = valeur
                  snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
                }else
                {
                  snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(data:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                }
                AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_NUM);
                strcpy(lv_line[4].c,"REG_DWORD");

                //date de création
                if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                  snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                else lv_line[5].c[0]=0;
                AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
              break;
              case 0x0000000b : //REG_QWORD, données numériques 64bits signées
                lv_line[3].c[0] = 0;
                if (vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
                {
                  c = &buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE];
                  snprintf(lv_line[3].c,MAX_LINE_SIZE,"0x%02X%02X%02X%02X%02X%02X%02X%02X",c[7]&0xff,c[6]&0xff,c[5]&0xff,c[4]&0xff,c[3]&0xff,c[2]&0xff,c[1]&0xff,c[0]&0xff);

                  snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
                }else
                {
                  if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=",lv_line[2].c);
                  }else
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,vk_h->data_size,vk_h->data_offset);
                  }
                }
                AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_NUM);
                strcpy(lv_line[4].c,"REG_QWORD");

                //date de création
                if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                  snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                else lv_line[5].c[0]=0;
                AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
              break;
              default :
                //on ajoute pas au liste view les éléments inconnus non valides
                lv_line[3].c[0] = 0;
                if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
                {
                  for (k=0;k<vk_h->data_size && k/2<MAX_LINE_SIZE;k++)
                  {
                    snprintf(tmp,10,"%02X",buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+k]&0xff);
                    strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                  }
                  strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

                  if (vk_h->data_type == 0x00000000)strcpy(lv_line[4].c,"REG_NONE");
                  else
                  {
                    snprintf(lv_line[4].c,MAX_LINE_SIZE,"0x%08X",(unsigned int)(vk_h->data_type & 0xFFFFFFFF));
                  }

                  //date de création
                  if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
                    snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  else lv_line[5].c[0]=0;
                  AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

                  snprintf(tmp,MAX_LINE_SIZE,"%s=(type:0x%08X)%s",lv_line[2].c,(unsigned int)(vk_h->data_type & 0xFFFFFFFF),lv_line[3].c);
                }else
                {
                  if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(type:0x%08X)",lv_line[2].c,(unsigned int)(vk_h->data_type & 0xFFFFFFFF));
                  }else
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(type:0x%08X)(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,(unsigned int)(vk_h->data_type & 0xFFFFFFFF),vk_h->data_size,vk_h->data_offset);
                  }
                }
                AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,new_hitem,ICON_FILE_UNK);
              break;
          }
        }
      }
    }

    //traitement des sous clées nk ^^
    if (nk_h->nb_subkeys>0&& nk_h->nb_subkeys<taille_fic && ((pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE)<taille_fic))
    {
      HBIN_CELL_LF_HEADER *lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE];
      for (i=0;i<nk_h->nb_subkeys;i++)
      {
        if ((lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic && (lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE) > pos_fhbin)
          Readnk(file, buffer, taille_fic,lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, lv_line[1].c, pos_fhbin, hlv,new_hitem, htv);
      }
    }
    SendDlgItemMessage(Tabl[TABL_CONF],TV_VIEW, TVM_SORTCHILDREN,(WPARAM)TRUE, (LPARAM)new_hitem);//tri par orde alphabetique
    //SendDlgItemMessage(Tabl[TABL_REGISTRY],TV_VIEW,TVM_EXPAND, TVE_EXPAND,(LPARAM)new_hitem);
  }
}
//------------------------------------------------------------------------------
// pour fichier : .sav, NTUSER.DAT, SAM, REGF, SYSTEM
// Sources : différents fichiers : http://msdn.microsoft.com/en-us/library/ms724877%28v=VS.85%29.aspx
// Source de format : http://sourceforge.net/projects/libregf/
void AnalyseFichierRegBin(char *fic)
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
  if (position>0)
  {
    if (((REGF_HEADER*)buffer)->id == 0x66676572) //Fichier REG standard
    {
      taille_fic = position;
      position = 0x1000; //début du 1er hbin
      HANDLE hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_VIEW);
      HANDLE htv = GetDlgItem(Tabl[TABL_REGISTRY],TV_VIEW);

      //pointeurs pour les différents types de structure
      HBIN_HEADER *hb_h;
      DWORD pos_fhbin = 0;

      //recherche du 1er hbin !! (en cas de bug)
      while(position<taille_fic-4)
      {
        hb_h = (HBIN_HEADER *)&buffer[position];
        if (hb_h->id == 0x6E696268 )  //hbin
        {
          if (pos_fhbin == 0)pos_fhbin = position;
          position=position+HBIN_HEADER_SIZE;//entête hbin
          break;
        }else position++;
      }
      //identification de la syskey
      SyskeyExtract(buffer, taille_fic, pos_fhbin+HBIN_HEADER_SIZE, position,fic);

      //lecture de la première clée + bouclage
      Readnk(fic,buffer,taille_fic, pos_fhbin+HBIN_HEADER_SIZE, "", position,hlv,TVI_ROOT,htv);
    }
  }

  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}

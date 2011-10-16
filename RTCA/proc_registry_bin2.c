//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http:\\omni.a.free.fr
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
#define HBIN_CELL_DB_SIZE  16 // 12 + 4 de taille
typedef struct hbin_cell_db_header{
  DWORD size;

  short type;                 //0x6264 : debut standard de la zone de DATA
  short nb_segments;
  DWORD db_segment_list_offset;
  long padding;

}HBIN_CELL_DB_HEADER;
//------------------------------------------------------------------------------
void ReadPath(char *buffer, DWORD taille_fic, DWORD position, char *path)
{
  path[0]=0;
  char tmp[MAX_LINE_SIZE];
  DWORD pos =position;

  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+pos);

  while (0x1000+nk_h->parent_key < taille_fic && nk_h->parent_key > 0)
  {
    //lecture du nom
    if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && (pos-HBIN_CELL_NK_SIZE)&& nk_h->type == 0x6B6E)
    {
      tmp[0]=0;
      strncpy(tmp,(char *)(buffer+pos+HBIN_CELL_NK_SIZE),MAX_LINE_SIZE);
      if (nk_h->key_name_size>MAX_LINE_SIZE)tmp[MAX_LINE_SIZE-1]=0;
      else tmp[nk_h->key_name_size]=0;

      strncat(tmp,"\\",MAX_LINE_SIZE);
      strncat(tmp,path,MAX_LINE_SIZE);
      strncat(tmp,"\0",MAX_LINE_SIZE);
      strcpy(path,tmp);
    }
    pos = 0x1000+nk_h->parent_key;
    nk_h = (HBIN_CELL_NK_HEADER *)(buffer+pos);
  }

  //suppression de l'éventuel \\ de fin ^^
  pos = strlen(path);
  if (pos>0)
  {
    if (path[pos-1]=='\\')path[pos-1]=0;
  }
}
//------------------------------------------------------------------------------
//traitement des clés de type nk
DWORD Traiter_RegBin_nk(char *fic, DWORD position, DWORD taille_fic, char *buffer, HANDLE hlv,HANDLE htv)
{
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  //valide ?
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && (position-HBIN_CELL_NK_SIZE)&& nk_h->type == 0x6B6E)
  {
    if (nk_h->nb_values > 0)
    {
      LINE_ITEM lv_line[SIZE_UTIL_ITEM];
      strcpy(lv_line[0].c,fic);

      //lecture du path complet ^^
      ReadPath(buffer, taille_fic, position, lv_line[1].c);

      //lecture de l'update
      SYSTEMTIME SysTime;

      //lecture des valeures ^^
      DWORD i, k;
      S_ITEM_LS *item_ls;
      HBIN_CELL_VK_HEADER *vk_h;
      char tmp[MAX_LINE_SIZE];
      char *c;

      for (i=0;i<nk_h->nb_values;i++)
      {
        //pour le 1er il faut passer les 4 premier octets représentant une taille ?
        item_ls = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+0x1000+i*HBIN_CELL_ITEM_LS+4];
        //lecture de l'offset de la valeure :
        if (item_ls->val_of>0 && (item_ls->val_of+0x1000)<taille_fic)
        {
          //traitement des valeures !!!
          vk_h = (HBIN_CELL_VK_HEADER *)&buffer[item_ls->val_of+0x1000];

          //récupération du nom de la valeur :
          strncpy(lv_line[2].c,vk_h->value,MAX_LINE_SIZE);
          if (vk_h->name_size>=MAX_LINE_SIZE)lv_line[2].c[MAX_LINE_SIZE-1]=0;
          else lv_line[2].c[vk_h->name_size]=0;

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
                }else snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);

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
              strcpy(lv_line[4].c,"REG_SZ");
            break;
            case 0x00000002 : //REG_EXPAND_SZ, chaine ASCII et Unicodes, contient des path type %path%
              lv_line[3].c[0] = 0;
              if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
              {
                if (vk_h->data_size < 5)
                {
                  strcpy(lv_line[3].c,vk_h->cdata_offset);
                  lv_line[3].c[4]=0;
                }else snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);

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
              strcpy(lv_line[4].c,"REG_EXPAND_SZ");
            break;
            case 0x00000006 : //REG_LINK, chaine ASCII et Unicodes, lien lien
              lv_line[3].c[0] = 0;
              if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
              {
                if (vk_h->data_size < 5)
                {
                  strcpy(lv_line[3].c,vk_h->cdata_offset);
                  lv_line[3].c[4]=0;
                }else snprintf(lv_line[3].c,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);

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
              strcpy(lv_line[4].c,"REG_LINK");
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
                      snprintf(tmp,MAX_LINE_SIZE,"%S",(wchar_t *)&buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+2*strlen(lv_line[3].c)]);
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
                strcpy(lv_line[4].c,"REG_MULTI_SZ");
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
                      snprintf(tmp,10,"%02X",buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+k]&0xff);
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
                strcpy(lv_line[4].c,"REG_BINARY");
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
                strcpy(lv_line[4].c,"REG_DWORD");
              break;
              case 0x0000000b : //REG_QWORD, données numériques 64bits signées
                lv_line[3].c[0] = 0;
                if (vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
                {
                  c = &buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE];
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
                strcpy(lv_line[4].c,"REG_QWORD");
              break;
              default :
                //on ajoute pas au liste view les éléments inconnus non valides
                lv_line[3].c[0] = 0;
                if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
                {
                  for (k=0;k<vk_h->data_size && k/2<MAX_LINE_SIZE;k++)
                  {
                    snprintf(tmp,10,"%02X",buffer[0x1000+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+k]&0xff);
                    strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                  }
                  strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

                  if (vk_h->data_type == 0x00000000)strcpy(lv_line[4].c,"REG_NONE");
                  else
                  {
                    snprintf(lv_line[4].c,MAX_LINE_SIZE,"0x%08X",(unsigned int)(vk_h->data_type & 0xFFFFFFFF));
                  }

                  snprintf(tmp,MAX_LINE_SIZE,"%s=(type:0x%08X)%s",lv_line[2].c,(unsigned int)(vk_h->data_type & 0xFFFFFFFF),lv_line[3].c);
                }else
                {
                  if (vk_h->data_type == 0x00000000)strcpy(lv_line[4].c,"REG_NONE");
                  else
                  {
                    snprintf(lv_line[4].c,MAX_LINE_SIZE,"0x%08X",(unsigned int)(vk_h->data_type & 0xFFFFFFFF));
                  }

                  if(vk_h->data_offset == 0 && vk_h->data_size ==0)
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(type:0x%08X)",lv_line[2].c,(unsigned int)(vk_h->data_type & 0xFFFFFFFF));
                  }else
                  {
                    snprintf(tmp,MAX_LINE_SIZE,"%s=(type:0x%08X)(len:%d)(off:%lu)<DATA ERROR>",lv_line[2].c,(unsigned int)(vk_h->data_type & 0xFFFFFFFF),vk_h->data_size,vk_h->data_offset);
                  }
                }
              break;
          }

          if (FileTimeToSystemTime(&(nk_h->last_write), &SysTime) != 0)//traitement de l'affichage de la date
            snprintf(lv_line[5].c,MAX_LINE_SIZE,"%02d/%02d/%02d %02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
          else lv_line[5].c[0]=0;

          //ajout de l'item
          nb_items++;
          if (nb_items%1000 == 0)
          {
            snprintf(tmp,MAX_LINE_SIZE,"loading %lu values",nb_items);
            SB_add_T(SB_ONGLET_REGISTRY, tmp);
          }
          AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

          nb_items++;
          if (nb_items%1000 == 0)
          {
            snprintf(tmp,MAX_LINE_SIZE,"loading %lu values",nb_items);
            SB_add_T(SB_ONGLET_REGISTRY, tmp);
          }
        }
      }
    }
    return HBIN_CELL_NK_SIZE;
  }
  return 1;
}
//------------------------------------------------------------------------------
// pour fichier : .sav, NTUSER.DAT, SAM, REGF, SYSTEM
// Sources : différents fichiers : http://msdn.microsoft.com/en-us/library/ms724877%28v=VS.85%29.aspx
// Source de format : http://sourceforge.net/projects/libregf/
void AnalyseFichierRegBin2(char *fic)
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

      //traitement des données
      HBIN_CELL_PRE_HEADER *hb_ph;
      while(position<taille_fic-4)
      {
        //on ne traite que les clés nk (name key = directory)
        hb_ph = (HBIN_CELL_PRE_HEADER *)&buffer[position];
        if (((hb_ph->size[1]&0xFF) == 0xFF) && ((hb_ph->size[2]&0xFF) == 0xFF) && ((hb_ph->size[3]&0xFF) == 0xFF))
        {
          switch(hb_ph->type)
          {
            case 0x6B6E:  position = position + Traiter_RegBin_nk(fic,position, taille_fic, buffer,hlv,htv);break; //nk
            case 0x6B73 : position = position + HBIN_CELL_SK_SIZE;break;//sk
            case 0x6B76 : position = position + HBIN_CELL_VK_SIZE;break;//vk
            case 0x666C : position = position + HBIN_CELL_LF_SIZE;break;//lf
            case 0x686C : position = position + HBIN_CELL_LH_SIZE;break;//lh
            case 0x696C : position = position + HBIN_CELL_LI_SIZE;break;//li
            case 0x6972 : position = position + HBIN_CELL_RI_SIZE;break;//ri
            case 0x6264 : position = position + HBIN_CELL_DB_SIZE;break;//db
            //inconnus mais standards
            /*case 0xA0F8 :
            case 0xA4D8 :
            case 0xA9B8 :
            case 0x65E0 :
            case 0x0000 :
            case 0x0388 ://sans : 57054, 830454
            case 0x0518 ://avec : 56455, 492508*/
            //position+=0xF;break;
            default : position++; break;
          }
        }else position++;
      }
    }
  }
  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}

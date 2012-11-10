//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void AddToLVRegBin(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne)
{
  //ajout de la ligne
  LVITEM lvi;
  lvi.mask = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem = 0;
  lvi.lParam = LVM_SORTITEMS;
  lvi.pszText="";
  lvi.iItem = ListView_GetItemCount(hlv);
  long int itemPos = ListView_InsertItem(hlv, &lvi);

  //ajout des items
  unsigned short i=0;
  for (;i<nb_colonne;i++){if (item[i].c[0]!=0)ListView_SetItemText(hlv,itemPos,i,item[i].c);}
}
//------------------------------------------------------------------------------
/*void AddNextValueToTreeView(HANDLE htv, HTREEITEM hparent, char *value, DWORD value_type)
{
  switch(value_type)
  {
    case 0x00000001:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_TXT_REG);break;
    case 0x00000002:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_TXT_REG);break;
    case 0x00000003:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_BIN_REG);break;
    case 0x00000004:
    case 0x00000005:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_DWORD_REG);break;
    case 0x00000006:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_BIN_REG);break;
    case 0x00000007:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_TXT_REG);break;
    case 0x0000000b:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_DWORD_REG);break;
    default:AddItemTreeViewImg(htv,value, hparent,ICON_FILE_UNKNOW_REG);break;
  }
}
//------------------------------------------------------------------------------
HTREEITEM AddFirstValueToTreeView(HANDLE htv, HTREEITEM hparent, char *path, char *value, DWORD value_type)
{
  unsigned int i, nb = ExtractTextFromPathNb(path);
  HTREEITEM hitem = hparent, last_hitem;
  TVITEM tvitem;
  char tmp_item[MAX_PATH],tmp_read_item[MAX_PATH];
  for(i=1;i<nb;i++)
  {
    last_hitem = hitem;
    hitem = (HTREEITEM)SendMessage(htv, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)hitem);
    if (hitem == 0) //not exist
    {
      //add directory
      for(;i<nb;i++)
      {
        tmp_item[0] = 0;
        ExtractTextFromPath(path, tmp_item, MAX_PATH, i);
        last_hitem = AddItemTreeViewImg(htv, tmp_item, last_hitem, ICON_DIRECTORY_REG);
      }

      //add items
      AddNextValueToTreeView(htv, last_hitem, value, value_type);
      return last_hitem;
    }else //have child
    {
      //search if exist
      last_hitem = hitem;

      //get string
      tmp_item[0] = 0;
      ExtractTextFromPath(path, tmp_item, MAX_PATH, i);
      charToLowChar(tmp_item);
      do
      {
        //if exist, replace hitem
        tmp_read_item[0] = 0;
        tvitem.hItem = hitem;
        tvitem.mask = TVIF_TEXT;//|TVIF_IMAGE;
        tvitem.pszText = tmp_read_item;
        tvitem.cchTextMax = MAX_PATH;
        if (SendMessage(htv, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
        {
          //check if a directory
          if(tvitem.iImage != ICON_DIRECTORY_REG) continue;
          else if (strcmp(tmp_item,charToLowChar(tmp_read_item)) == 0)break;
        }else return 0;
      }while((hitem = (HTREEITEM)SendMessage(htv, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));

      if (hitem == 0) //not exist
      {
        //add directory
        for(;i<nb;i++)
        {
          tmp_item[0] = 0;
          ExtractTextFromPath(path, tmp_item, MAX_PATH, i);
          last_hitem = AddItemTreeViewImg(htv, tmp_item, last_hitem, ICON_DIRECTORY_REG);
        }

        //add items
        AddNextValueToTreeView(htv, last_hitem, value, value_type);
        return last_hitem;
      }
    }
  }

  if (i == nb && hitem != 0)
  {
    AddNextValueToTreeView(htv, hitem, value, value_type);
    return hitem;
  }
  return 0;
}*/
//------------------------------------------------------------------------------
BOOL ReadPath(char *buffer, DWORD taille_fic, DWORD position, char *path, unsigned int path_size_max, char *parent, char *sid, unsigned int sid_size_max)
{
  path[0]=0;
  char tmp[MAX_LINE_SIZE];
  DWORD pos =position;
  BOOL deleteted = FALSE;
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+pos);

  //Get SID
  sid[0] = 0;
  /*Readnk_Infos(buffer,taille_fic, 0x1000+HBIN_HEADER_SIZE, position,
               NULL, nk_h, NULL, NULL, NULL, 0,sid, sid_size_max);*/

  //Get path
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
      strncpy(path,tmp,path_size_max);
    }

    pos = 0x1000+nk_h->parent_key;
    nk_h = (HBIN_CELL_NK_HEADER *)(buffer+pos);
  }

  //suppression de l'éventuel \\ de fin ^^
  pos = strlen(path);
  if (pos>0 && pos-1 < path_size_max)
  {
    if (path[pos-1]=='\\')
    {
      path[pos-1]=0;
    }
  }

  //add parent in first case
  if (parent != NULL)
  {
    strncpy(tmp,path,MAX_LINE_SIZE);
    char *c = tmp;
    while (*c && *c != '\\')c++;
    if (*c == '\\')
    {
      c++;
      //if(GetRegistryNK(buffer, taille_fic, position, 0x1000, c) == NULL) deleteted = TRUE;
      snprintf(path,path_size_max,"%s%s",parent,c);
    }
  }

  return deleteted;
}
//------------------------------------------------------------------------------
//nk keys ( == directory)
DWORD Traiter_RegBin_nk(char *fic, HTREEITEM hparent, char *parent, DWORD position, DWORD taille_fic, char *buffer, HANDLE hlv,HANDLE htv)
{
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  //valide ?
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && (position-HBIN_CELL_NK_SIZE)&& nk_h->type == 0x6B6E)
  {
    if (nk_h->nb_values > 0)
    {
      LINE_ITEM lv_line[DLG_REG_LV_NB_COLUMN];
      strcpy(lv_line[0].c,fic);

      //lecture du path complet + SID ^^
      if(ReadPath(buffer, taille_fic, position, lv_line[1].c,MAX_LINE_SIZE,parent,lv_line[6].c,MAX_LINE_SIZE))
        strcpy(lv_line[7].c,"X");
      else lv_line[7].c[0]=0;

      //lecture des valeures ^^
      DWORD i, k;
      S_ITEM_LS *item_ls;
      HBIN_CELL_VK_HEADER *vk_h;
      char tmp[MAX_LINE_SIZE];
      char *c;

      //last update
      lv_line[5].c[0]=0;
      filetimeToString_GMT(nk_h->last_write, lv_line[5].c, MAX_LINE_SIZE);

      HTREEITEM htparent = 0;
      for (i=0;i<nk_h->nb_values;i++)
      {
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

          //ajout de l'item
          //if (i == 0 || htparent == 0) htparent = AddFirstValueToTreeView(htv, hparent, lv_line[1].c, tmp, vk_h->data_type);
          //else AddNextValueToTreeView(htv, htparent, tmp, vk_h->data_type);

          AddToLVRegBin(hlv, lv_line, DLG_REG_LV_NB_COLUMN);
        }
      }

      //no value : only directory
      if (nk_h->nb_values < 1 && nk_h->nb_subkeys < 1)
      {
        lv_line[2].c[0] = 0;
        lv_line[3].c[0] = 0;
        lv_line[4].c[0] = 0;
        AddToLVRegBin(hlv, lv_line, DLG_REG_LV_NB_COLUMN);
      }

      DWORD nb = ListView_GetItemCount(hlv);
      if (nb % 1000 == 0)
      {
        char tmp[MAX_PATH];
        snprintf(tmp,MAX_PATH,"Loading... %lu keys",nb);
        SendMessage(GetDlgItem(h_reg,STB),SB_SETTEXT,0, (LPARAM)tmp);
      }
    }
    return HBIN_CELL_NK_SIZE;
  }
  return 1;
}
//------------------------------------------------------------------------------
void GetRecoveryRegFile(char *reg_file, HTREEITEM hparent, char *parent, HANDLE hlv, HANDLE htv)
{
  //open file
  HANDLE Hfic = CreateFile(reg_file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic == INVALID_HANDLE_VALUE)return;

  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic<1 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    return;
  }

  //alloc memory
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    return;
  }

  //load file
  DWORD copiee, position = 0, increm = 0;
  if (taille_fic > DIXM)increm = DIXM;
  else increm = taille_fic;

  while (position<taille_fic && increm!=0)
  {
    copiee = 0;
    ReadFile(Hfic, buffer+position, increm,&copiee,0);
    position +=copiee;
    if (taille_fic-position < increm)increm = taille_fic-position ;
  }
  CloseHandle(Hfic);

  //working
  if (position>0)
  {
    if (((REGF_HEADER*)buffer)->id == 0x66676572) //Fichier REG standard
    {
      taille_fic = position;
      position = 0x1000; //first hbin struct

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

      HBIN_CELL_PRE_HEADER *hb_ph;
      while(position<taille_fic-4)
      {
        //on ne traite que les clés nk (name key = directory)
        hb_ph = (HBIN_CELL_PRE_HEADER *)&buffer[position];
        if (((hb_ph->size[1]&0xFF) == 0xFF) && ((hb_ph->size[2]&0xFF) == 0xFF) && ((hb_ph->size[3]&0xFF) == 0xFF))
        {
          switch(hb_ph->type)
          {
            case 0x6B6E:  position = position + Traiter_RegBin_nk(reg_file,hparent, parent, position, taille_fic, buffer,hlv,htv);break; //nk
            case 0x6B73 : position = position + HBIN_CELL_SK_SIZE;break;//sk
            case 0x6B76 : position = position + HBIN_CELL_VK_SIZE;break;//vk
            case 0x666C : position = position + HBIN_CELL_LF_SIZE;break;//lf
            case 0x686C : position = position + HBIN_CELL_LH_SIZE;break;//lh
            case 0x696C : position = position + HBIN_CELL_LI_SIZE;break;//li
            case 0x6972 : position = position + HBIN_CELL_RI_SIZE;break;//ri
            case 0x6264 : position = position + HBIN_CELL_DB_SIZE;break;//db
            default : position++; break;
          }
        }else position++;
      }
    }
  }
  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}

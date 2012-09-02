//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
unsigned int ExtractTextFromPathNb(char *path)
{
  char *c = path;
  unsigned int nb =0;

  while (*c)
  {
    if (*c == '\\')nb++;
    c++;
  }
  if (*(c-1) != '\\')nb++;
  return nb;
}
//------------------------------------------------------------------------------
char *ExtractTextFromPath(char *path, char *txt, unsigned int txt_size_max, unsigned int index)
{
  char *c = path;
  unsigned int nb =0;

  txt[0]=0;

  while (*c)
  {
    if (nb == index)
    {
      snprintf(txt,txt_size_max,"%s",c);
      char *s = txt;
      while (*s !='\\' && *s)s++;
      *s=0;
      return txt;
    }

    if (*c == '\\')nb++;
    c++;
  }
  return txt;
}
//------------------------------------------------------------------------------
//return only binary datas
DWORD GetBinaryValueData(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin,
                   unsigned int index, char *value, unsigned int value_size, char *data, DWORD *data_size)
{
  if (value!=NULL)value[0] = 0;
  if (data!=NULL)data[0] = 0;
  if (nk_h == NULL)return FALSE;

  if (nk_h->nb_values>0&& nk_h->nb_values<taille_fic && ((pos_fhbin+nk_h->val_ls_offset-HBIN_HEADER_SIZE)<taille_fic))
  {
    if (index == 0 && value == 0 && value_size == 0 && data == 0 && data_size == 0)return nk_h->nb_values;
    if (nk_h->nb_values > index)
    {
      S_ITEM_LS *item_ls = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+pos_fhbin+index*HBIN_CELL_ITEM_LS-HBIN_HEADER_SIZE+4];
      HBIN_CELL_VK_HEADER *vk_h = (HBIN_CELL_VK_HEADER *)&buffer[item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE];

      //get value
      if (value!=NULL && value_size!=0)
      {
        strncpy(value,vk_h->value,value_size);
        if (vk_h->name_size>=value_size)value[value_size-1]=0;
        else value[vk_h->name_size]=0;
      }

      //get data
      if (data!=NULL && data_size!=0)
      {
        DWORD type = GetBinaryRegistryData(vk_h, taille_fic, buffer, pos_fhbin, data, data_size);
        if (data[0] != 0)return type;
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//get value of index or nb value
DWORD GetValueData(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin,
                   unsigned int index, char *value, unsigned int value_size, char *data, unsigned int data_size)
{
  if (value!=NULL)value[0] = 0;
  if (data!=NULL)data[0] = 0;
  if (nk_h == NULL)return FALSE;

  if (nk_h->nb_values>0&& nk_h->nb_values<taille_fic && ((pos_fhbin+nk_h->val_ls_offset-HBIN_HEADER_SIZE)<taille_fic))
  {
    if (index == 0 && value == 0 && value_size == 0 && data == 0 && data_size == 0)return nk_h->nb_values;
    if (nk_h->nb_values > index)
    {
      S_ITEM_LS *item_ls = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+pos_fhbin+index*HBIN_CELL_ITEM_LS-HBIN_HEADER_SIZE+4];
      HBIN_CELL_VK_HEADER *vk_h = (HBIN_CELL_VK_HEADER *)&buffer[item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE];

      //get value
      if (value!=NULL && value_size!=0)
      {
        strncpy(value,vk_h->value,value_size);
        if (vk_h->name_size>=value_size)value[value_size-1]=0;
        else value[vk_h->name_size]=0;
      }

      //get data
      if (data!=NULL && data_size!=0)
      {
        DWORD type = GetRegistryData(vk_h, taille_fic, buffer, pos_fhbin, data, data_size);
        if (data[0] != 0)return type;
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//get subkey of index or nb subkey
DWORD GetSubNK(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin, unsigned int index, char *subkey, unsigned int subkey_size)
{
  if (subkey!=NULL)subkey[0] = 0;
  if (nk_h == NULL)return FALSE;

  //get subkey
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E && pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE < taille_fic)
  {
    //get list of nk child
    HBIN_CELL_LF_HEADER *lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE];
    if (index == 0 && subkey == 0 && subkey_size == 0)return nk_h->nb_subkeys;
    if (index<nk_h->nb_subkeys)
    {
      //get index subkey
      DWORD new_position = lf_h->hb_c[index].nk_of+pos_fhbin-HBIN_HEADER_SIZE;
      if (new_position < taille_fic)
      {
        HBIN_CELL_NK_HEADER *nk_h_tmp2 = (HBIN_CELL_NK_HEADER *)(buffer+new_position);

        //get name
        if (nk_h_tmp2->key_name_size >0 && nk_h_tmp2->key_name_size<taille_fic && nk_h_tmp2->size>0 && nk_h_tmp2->type == 0x6B6E)
        {
          strncpy(subkey,(char *)(buffer+new_position+HBIN_CELL_NK_SIZE),subkey_size);
          if (nk_h_tmp2->key_name_size>subkey_size)subkey[subkey_size-1]=0;
          else subkey[nk_h_tmp2->key_name_size]=0;
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//get nk or subkey of index or nb subkey
HBIN_CELL_NK_HEADER * GetSubNKtonk(char *buffer, DWORD taille_fic, HBIN_CELL_NK_HEADER *nk_h, DWORD pos_fhbin, unsigned int index)
{
  if (nk_h == NULL)return NULL;

  //get subkey
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E && pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE < taille_fic)
  {
    //get list of nk child
    HBIN_CELL_LF_HEADER *lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE];
    if (index<nk_h->nb_subkeys)
    {
      //get index subkey
      DWORD new_position = lf_h->hb_c[index].nk_of+pos_fhbin-HBIN_HEADER_SIZE;
      if (new_position < taille_fic)
      {
        HBIN_CELL_NK_HEADER *nk_h_tmp2 = (HBIN_CELL_NK_HEADER *)(buffer+new_position);

        //verify
        if (nk_h_tmp2->key_name_size >0 && nk_h_tmp2->key_name_size<taille_fic && nk_h_tmp2->size>0 && nk_h_tmp2->type == 0x6B6E)
        {
          return nk_h_tmp2;
        }
      }
    }
  }
  return NULL;
}
//------------------------------------------------------------------------------
//search key path from root
HBIN_CELL_NK_HEADER *GetRegistryNK(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path)
{
  BOOL ok = FALSE;
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  HBIN_CELL_NK_HEADER *nk_h_tmp = nk_h;

  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E)
  {
    //Search
    char tmp_reg_path[MAX_PATH],tmp[MAX_PATH],tmpkey[MAX_PATH];
    strncpy(tmp_reg_path,reg_path,MAX_PATH);
    charToLowChar(tmp_reg_path);

    //bypass first item = ruche name or
    DWORD new_position = position;
    unsigned int i,j, istring = ExtractTextFromPathNb(tmp_reg_path);

    unsigned int nb_ok = 0;
    HBIN_CELL_LF_HEADER *lf_h = 0;
    HBIN_CELL_NK_HEADER *nk_h_tmp2;

    for (i=0;i<istring;i++)
    {
      //read string to search
      tmp[0] = 0;
      ExtractTextFromPath(tmp_reg_path, tmp, MAX_PATH, i);
      if (tmp[0] != 0)
      {
        ok = FALSE;
      }else break;

      //get current and child string
      if (nk_h_tmp->key_name_size >0 && nk_h_tmp->key_name_size<taille_fic && nk_h_tmp->size>0 && nk_h_tmp->type == 0x6B6E)
      {
        //get nk child
        if (nk_h_tmp->nb_subkeys>0&& nk_h_tmp->nb_subkeys<taille_fic && ((pos_fhbin+nk_h_tmp->lf_offset-HBIN_HEADER_SIZE)<taille_fic))
        {
          //get list of nk child
          lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+nk_h_tmp->lf_offset-HBIN_HEADER_SIZE];
          for (j=0;j<nk_h_tmp->nb_subkeys;j++)
          {
            //get name
            if ((lf_h->hb_c[j].nk_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic && (lf_h->hb_c[j].nk_of+pos_fhbin-HBIN_HEADER_SIZE) > pos_fhbin)
            {
              new_position = lf_h->hb_c[j].nk_of+pos_fhbin-HBIN_HEADER_SIZE;
              nk_h_tmp2 = (HBIN_CELL_NK_HEADER *)(buffer+new_position);
              if (nk_h_tmp2->key_name_size >0 && nk_h_tmp2->key_name_size<taille_fic && nk_h_tmp2->size>0 && nk_h_tmp2->type == 0x6B6E)
              {
                //get name
                strncpy(tmpkey,(char *)(buffer+new_position+HBIN_CELL_NK_SIZE),MAX_PATH);
                if (nk_h_tmp2->key_name_size>MAX_PATH)tmpkey[MAX_PATH-1]=0;
                else tmpkey[nk_h_tmp2->key_name_size]=0;

                //equal or not ?
                if (strcmp(tmp,charToLowChar(tmpkey)) == 0)
                {
                  nk_h_tmp = nk_h_tmp2;
                  if (++nb_ok == istring)ok = TRUE;
                  break;
                }
              }
            }
          }
        }else break;
      }else break;
    }
  }

  if (ok) return nk_h_tmp;
  else return 0;
}
//------------------------------------------------------------------------------
//read data value from vk struct with data memory to write of min size to 5
DWORD GetBinaryRegistryData(HBIN_CELL_VK_HEADER *vk_h, DWORD taille_fic, char *buffer, DWORD pos_fhbin, char *data, DWORD *data_size)
{
  if (data!=NULL)data[0] = 0;
  if (*data_size<5)return FALSE;

  if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
  {
    if (vk_h->data_size < 5)
    {
      if (*data_size<vk_h->data_size)
      {
        memcpy(data,vk_h->cdata_offset,*data_size);
      }else
      {
        memcpy(data,vk_h->cdata_offset,vk_h->data_size);
        *data_size = vk_h->data_size;
      }
    }else
    {
      if (*data_size<vk_h->data_size)
      {
        memcpy(data,&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE],*data_size);
      }else
      {
        memcpy(data,&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE],vk_h->data_size);
        *data_size = vk_h->data_size;
      }
    }
    return vk_h->data_type;
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//read data value from vk struct with data memory to write of min size to 5
DWORD GetRegistryData(HBIN_CELL_VK_HEADER *vk_h, DWORD taille_fic, char *buffer, DWORD pos_fhbin, char *data, DWORD data_size)
{
  if (data!=NULL)data[0] = 0;
  if (data_size<5)return FALSE;
  switch(vk_h->data_type)
  {
    //Chaines
    case 0x00000001 : //REG_SZ, chaine ASCII et Unicodes
      if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
      {
        if (vk_h->data_size < 5)
        {
          snprintf(data,data_size,"%S",vk_h->cdata_offset);
          data[vk_h->data_size/2]=0;
        }else snprintf(data,data_size,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);
      }
    break;
    case 0x00000002 : //REG_EXPAND_SZ, chaine ASCII et Unicodes, contient des path type %path%
      if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
      {
        if (vk_h->data_size < 5)
        {
          strcpy(data,vk_h->cdata_offset);
          data[4]=0;
        }else snprintf(data,data_size,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);
      }
    break;    //traitement des sous clées nk ^^
    case 0x00000006 : //REG_LINK, chaine ASCII et Unicodes, lien lien
      if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
      {
        if (vk_h->data_size < 5)
        {
          strcpy(data,vk_h->cdata_offset);
          data[4]=0;
        }else snprintf(data,data_size,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);
      }
    break;
    case 0x00000007 : //REG_MULTI_SZ, multiples chaine ASCII et Unicodes, lien
        if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
        {
          if (vk_h->data_size < 5)
          {
            strcpy(data,vk_h->cdata_offset);
            data[4]=0;
          }else
          {
            //first text
            snprintf(data,data_size,"%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE]);
            DWORD i = strlen(data)*2+1;

            //next ^^
            char tmp[MAX_PATH];
            while(i<vk_h->data_size)
            {
              snprintf(tmp,MAX_PATH,";%S",(wchar_t *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+i]);
              strncat(data,tmp,data_size);
              i+=strlen(tmp)*2+1;
            }
            strncat(data,"\0",data_size);
          }
        }
      break;
      case 0x00000003 : //REG_BINARY, données binaires
        if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_size>0 && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
        {
          char tmp[11];
          unsigned int k;
          if (vk_h->data_size < 5)
          {
            for (k=0;k<vk_h->data_size && k/2<data_size;k++)
            {
              snprintf(tmp,10,"%02X",vk_h->cdata_offset[k]&0xff);
              strncat(data,tmp,data_size);
            }
            strncat(data,"\0",data_size);
          }else
          {
            for (k=0;k<vk_h->data_size && k/2<data_size;k++)
            {
              snprintf(tmp,10,"%02X",buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+k]&0xff);
              strncat(data,tmp,data_size);
            }
            strncat(data,"\0",data_size);
          }
        }
      break;
      case 0x00000004 : //REG_DWORD, données numériques 32bitschar
      case 0x00000005 : //REG_DWORD, données numériques 32bits signées
        if (vk_h->data_size > 0)
          snprintf(data,data_size,"0x%08X",(unsigned int)(vk_h->data_offset&0xFFFFFFFF)); // dword offset = value
      break;
      case 0x0000000b : //REG_QWORD, données numériques 64bits signées
        if (vk_h->data_offset > 0 && vk_h->data_offset+4 < taille_fic)
        {
          char *c = &buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE];
          snprintf(data,data_size,"0x%02X%02X%02X%02X%02X%02X%02X%02X",c[7]&0xff,c[6]&0xff,c[5]&0xff,c[4]&0xff,c[3]&0xff,c[2]&0xff,c[1]&0xff,c[0]&0xff);
        }
      break;
      default :
        //on ajoute pas au liste view les éléments inconnus non valides
        if (vk_h->data_size < 0xFFFFFFFF && vk_h->data_offset > 0 && vk_h->data_offset < taille_fic)
        {
          char tmp[11];
          unsigned int k;
          for (k=0;k<vk_h->data_size && k/2<data_size;k++)
          {
            snprintf(tmp,10,"%02X",buffer[pos_fhbin-HBIN_HEADER_SIZE+vk_h->data_offset+HBIN_CELL_VK_DATA_PADDING_SIZE+k]&0xff);
            strncat(data,tmp,data_size);
          }
          strncat(data,"\0",data_size);
        }
      break;
  }
  return vk_h->data_type;
}
//------------------------------------------------------------------------------
BOOL Readnk_Infos(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                  char *last_update, unsigned int last_update_size, char *rid, unsigned int rid_size,char *sid, unsigned int sid_size)
{
  //init
  if (last_update!=NULL)last_update[0]  = 0;
  if (rid!=NULL)rid[0]          = 0;
  if (sid!=NULL)sid[0]          = 0;

  //load key path
  HBIN_CELL_NK_HEADER *nk_h;
  if (nk_h_t != NULL)nk_h = nk_h_t;
  else if (reg_path!=NULL)nk_h = GetRegistryNK(buffer, taille_fic, position, pos_fhbin, reg_path);
  else return FALSE;

  //good or not
  if (nk_h != NULL)
  {
    //last_update
    if (last_update != NULL)filetimeToString_GMT(nk_h->last_write,last_update,last_update_size);

    //get rid and sid
    if (rid != NULL && sid != NULL)
    {
      if (nk_h->sk_offset != 0xFFFFFFFF && nk_h->sk_offset >0 && (pos_fhbin+nk_h->sk_offset-HBIN_HEADER_SIZE)<taille_fic)
      {
        //read owner and this rid + sid
        HBIN_CELL_SK_HEADER *sk = (HBIN_CELL_SK_HEADER *)&buffer[pos_fhbin-HBIN_HEADER_SIZE+nk_h->sk_offset];

        DWORD sz_owner = sk->group_offset-sk->owner_offset;
        if (sz_owner > 0 && (pos_fhbin+sz_owner+sk->owner_offset-HBIN_HEADER_SIZE)<taille_fic)
        {
          typedef struct
          {
            unsigned short type;  //0x001F
            unsigned short nb;    //2(0) to 5(3 parts)
            char head[8];
            DWORD id[5];
          }SK_SID_DATA;

          SK_SID_DATA *sk_owner_sid = (SK_SID_DATA *)&buffer[pos_fhbin+sk->owner_offset-HBIN_HEADER_SIZE];
          if(sk_owner_sid->nb == 0)return TRUE;

          //rid
          if (sk_owner_sid->nb > 4)snprintf(rid,rid_size,"%lu",sk_owner_sid->id[4]);

          //sid
          unsigned int i, nb = (sk_owner_sid->nb)-1;
          if (nb>5)nb = 5;

          snprintf(sid,sid_size,"S-1-%d",(sk_owner_sid->head[7]) & 0xFF);
          for (i=0;i<nb;i++)snprintf(sid+strlen(sid),sid_size-strlen(sid),"-%lu",sk_owner_sid->id[i]);
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL Readnk_Class(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                  char *Class, unsigned int Class_size)
{
  //init
  if (Class!=NULL)Class[0]  = 0;

  //load key path
  HBIN_CELL_NK_HEADER *nk_h;
  if (nk_h_t != NULL)nk_h = nk_h_t;
  else if (reg_path!=NULL)nk_h = GetRegistryNK(buffer, taille_fic, position, pos_fhbin, reg_path);
  else return FALSE;

  //good or not
  if (nk_h != NULL)
  {
    if (nk_h->class_name_offset != 0xFFFFFFFF && nk_h->class_name_size > 0 && (pos_fhbin+nk_h->class_name_offset-HBIN_HEADER_SIZE)<taille_fic)
    {
      //read
      if (nk_h->class_name_size > 4) // bypass header 0xe8FFFFFF
        snprintf(Class,Class_size,"%S",&buffer[pos_fhbin+nk_h->class_name_offset-HBIN_HEADER_SIZE+4]);
      return TRUE;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//read value from key path
BOOL Readnk_Value(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                  char *read_value, char *data, unsigned int data_size)
{
  //reads data to search
  unsigned int i;
  char tmp_read_value[MAX_PATH],tmpvalue[MAX_PATH];
  if (data!=NULL)data[0] = 0;
  strncpy(tmp_read_value,read_value,MAX_PATH);
  charToLowChar(tmp_read_value);

  //load key path
  HBIN_CELL_NK_HEADER *nk_h;
  if (nk_h_t != NULL)nk_h = nk_h_t;
  else if (reg_path!=NULL)nk_h = GetRegistryNK(buffer, taille_fic, position, pos_fhbin, reg_path);
  else return FALSE;

  if (nk_h != NULL)
  {
    if (nk_h->nb_values>0&& nk_h->nb_values<taille_fic && (pos_fhbin+nk_h->val_ls_offset-HBIN_HEADER_SIZE)<taille_fic)
    {
      S_ITEM_LS *item_ls;
      HBIN_CELL_VK_HEADER *vk_h;
      for (i=0;i<nk_h->nb_values;i++)
      {
        //pour le 1er il faut passer les 4 premier octets représentant une taille ?
        item_ls = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+pos_fhbin+i*HBIN_CELL_ITEM_LS-HBIN_HEADER_SIZE+4];
        //read value offset
        if (item_ls->val_of>0 && (item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic)
        {
          vk_h = (HBIN_CELL_VK_HEADER *)&buffer[item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE];

          //get value
          strncpy(tmpvalue,vk_h->value,MAX_PATH);
          if (vk_h->name_size>=MAX_PATH)tmpvalue[MAX_PATH-1]=0;
          else tmpvalue[vk_h->name_size]=0;

          //ok thread it
          if (strcmp(tmp_read_value,charToLowChar(tmpvalue)) == 0)
          {
            GetRegistryData(vk_h, taille_fic, buffer, pos_fhbin, data, data_size);
            if (data[0] != 0)return TRUE;
            break;
          }
        }
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//read binary value from key path
DWORD ReadBinarynk_Value(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *reg_path, HBIN_CELL_NK_HEADER *nk_h_t,
                         char *read_value, void *d, DWORD *data_size)
{
  //reads data to search
  unsigned int i;
  char *data = d;
  char tmp_read_value[MAX_PATH],tmpvalue[MAX_PATH];
  if (data!=NULL)data[0] = 0;
  strncpy(tmp_read_value,read_value,MAX_PATH);
  charToLowChar(tmp_read_value);

  //load key path
  HBIN_CELL_NK_HEADER *nk_h;
  if (nk_h_t != NULL)nk_h = nk_h_t;
  else if (reg_path!=NULL)nk_h = GetRegistryNK(buffer, taille_fic, position, pos_fhbin, reg_path);
  else return FALSE;

  if (nk_h != NULL)
  {
    if (nk_h->nb_values>0&& nk_h->nb_values<taille_fic && (pos_fhbin+nk_h->val_ls_offset-HBIN_HEADER_SIZE)<taille_fic)
    {
      S_ITEM_LS *item_ls;
      HBIN_CELL_VK_HEADER *vk_h;

      for (i=0;i<nk_h->nb_values;i++)
      {
        //pour le 1er il faut passer les 4 premier octets représentant une taille ?
        item_ls = (S_ITEM_LS *)&buffer[nk_h->val_ls_offset+pos_fhbin+i*HBIN_CELL_ITEM_LS-HBIN_HEADER_SIZE+4];
        //read value offset
        if (item_ls->val_of>0 && (item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic)
        {
          vk_h = (HBIN_CELL_VK_HEADER *)&buffer[item_ls->val_of+pos_fhbin-HBIN_HEADER_SIZE];

          //get value
          strncpy(tmpvalue,vk_h->value,MAX_PATH);
          if (vk_h->name_size>=MAX_PATH)tmpvalue[MAX_PATH-1]=0;
          else tmpvalue[vk_h->name_size]=0;

          //ok thread it
          if (strcmp(tmp_read_value,charToLowChar(tmpvalue)) == 0)
          {
            //GetRegistryData(vk_h, taille_fic, buffer, pos_fhbin, data, data_size);
            DWORD type = GetBinaryRegistryData(vk_h, taille_fic, buffer, pos_fhbin, data, data_size);
            if (data_size != 0)return type;
            break;
          }
        }
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//Open and load registry file
BOOL OpenRegFiletoMem(HK_F_OPEN *hks, char *file)
{
  //init
  hks->buffer     = NULL;
  hks->position   = 0;
  hks->pos_fhbin  = 0;

  if (file == NULL) return FALSE;

  //open file
  HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile == INVALID_HANDLE_VALUE)return FALSE;

  hks->taille_fic = GetFileSize(hfile,NULL);
  if (hks->taille_fic<1 || hks->taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(hfile);
    return FALSE;
  }

  //memalloc
  hks->buffer = (char *) HeapAlloc(GetProcessHeap(), 0, hks->taille_fic+1);
  if (hks->buffer == NULL)
  {
    CloseHandle(hfile);
    return FALSE;
  }

  //load file by 10M datas
  DWORD copiee, increm = 0;
  if (hks->taille_fic > DIXM)increm = DIXM;
  else increm = hks->taille_fic;

  while (hks->position<hks->taille_fic && increm!=0)
  {
    copiee = 0;
    ReadFile(hfile, (hks->buffer)+hks->position, increm,&copiee,0);
    hks->position=hks->position+copiee;
    if (hks->taille_fic-hks->position < increm)increm = hks->taille_fic-hks->position ;
  }
  CloseHandle(hfile);

  //working
  if (hks->position>0)
  {
    if (((REGF_HEADER*)hks->buffer)->id == 0x66676572) //Fichier REG standard
    {
      hks->taille_fic = hks->position;
      hks->position = 0x1000; //début du 1er hbin

      //pointeurs pour les différents types de structure
      HBIN_HEADER *hb_h;

      //recherche du 1er hbin !! (en cas de bug)
      while(hks->position<hks->taille_fic-4)
      {
        hb_h = (HBIN_HEADER *)&(hks->buffer[hks->position]);
        if (hb_h->id == 0x6E696268 )  //hbin
        {
          if (hks->pos_fhbin == 0)hks->pos_fhbin = hks->position;
          hks->position=hks->position+HBIN_HEADER_SIZE;//entête hbin
          break;
        }else hks->position++;
      }
      strncpy(hks->file,file,MAX_PATH);
      return TRUE;
    }
  }
  HeapFree(GetProcessHeap(), 0, hks->buffer);
  return FALSE;
}
//------------------------------------------------------------------------------
//free mem
void CloseRegFiletoMem(HK_F_OPEN *hks)
{
  HeapFree(GetProcessHeap(), 0, hks->buffer);
}

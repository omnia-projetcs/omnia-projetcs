//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
/*

*/
//------------------------------------------------------------------------------
BOOL MSCASH_valide(char *hexa_value)
{
  if (hexa_value == NULL)return FALSE;
  if (strlen(hexa_value)<65)return FALSE;
  if (hexa_value[65] != '0' && hexa_value[66] != '0')return TRUE;
}

//------------------------------------------------------------------------------
BOOL registry_user_cache_dump_MSCASH(HANDLE hlv, char *file, char *key, char *value, char *data, unsigned int size)
{
  if (data[0] ==0 && data[1] ==0 && data[2] ==0){return FALSE;}
  else
  {
    LINE_ITEM lv_line[SIZE_UTIL_ITEM];
    strcpy(lv_line[0].c,file);
    strcpy(lv_line[1].c,key);
    strcpy(lv_line[2].c,value);
    strcpy(lv_line[5].c,"MSCASH");

    //data to hexa
    lv_line[3].c[0]=0;
    char tmp[11];
    int j;
    for (j=0;j<size && j/2<MAX_LINE_SIZE;j++)
    {
      snprintf(tmp,10,"%02X",data[j]&0xff);
      strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
    }
    strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

    //traitement !!
    if (MSCASH_valide(lv_line[3].c))
    {

      AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
    }
    return TRUE;
  }

  //syskey : session.sys.registry.open_key(HKEY_LOCAL_MACHINE, "SECURITY\\Policy\\Secrets\\NL$KM\\CurrVal", KEY_READ)

  //ajout
  //AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
}

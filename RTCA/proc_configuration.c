//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//convert WILD caractères to ANSI
void ConvertWA(char *tmp)
{
  unsigned int i,j,s = strlen(tmp);
  for (i=0,j=0;i<s;i++,j++)
  {
    if ((tmp[i] & 0xff) == 0xC3)
    {
           if ((tmp[i+1] & 0xff) == 0xA0){tmp[j]='à';i++;}
      else if ((tmp[i+1] & 0xff) == 0xA2){tmp[j]='â';i++;}
      else if ((tmp[i+1] & 0xff) == 0xA4){tmp[j]='ä';i++;}
      else if ((tmp[i+1] & 0xff) == 0xA8){tmp[j]='è';i++;}
      else if ((tmp[i+1] & 0xff) == 0xA9){tmp[j]='é';i++;}
      else if ((tmp[i+1] & 0xff) == 0xAA){tmp[j]='ê';i++;}
      else if ((tmp[i+1] & 0xff) == 0xAB){tmp[j]='ë';i++;}
      else if ((tmp[i+1] & 0xff) == 0xAE){tmp[j]='î';i++;}
      else if ((tmp[i+1] & 0xff) == 0xAF){tmp[j]='ï';i++;}
      else if ((tmp[i+1] & 0xff) == 0xB4){tmp[j]='ô';i++;}
      else if ((tmp[i+1] & 0xff) == 0xB6){tmp[j]='ö';i++;}
      else if ((tmp[i+1] & 0xff) == 0xB9){tmp[j]='ù';i++;}
      else if ((tmp[i+1] & 0xff) == 0xBB){tmp[j]='û';i++;}
      else if ((tmp[i+1] & 0xff) == 0xA7){tmp[j]='ç';i++;}
      else j--;
    }else if ((tmp[i] & 0xff) == 0xC2)
    {
      if ((tmp[i+1] & 0xff) == 0xA9){tmp[j]='ç';i++;}
      else j--;
    }else if ((tmp[i] & 0xff) < 0x20 || (tmp[i] & 0xff) > 0x7E)j--;
    else tmp[j]=tmp[i];
  }
  tmp[j]=0;
}
//------------------------------------------------------------------------------
void FiltreConfData(LINE_ITEM *item)
{
  if (!State_Enable)return;
  if (strlen(item[6].c)>=DATE_SIZE/2)
  {
    EnterCriticalSection(&Sync);

    HANDLE hlv_r = GetDlgItem(Tabl[TABL_STATE],LV_VIEW);
    DWORD ref_item = ListView_GetItemCount(hlv_r);
    char tmp[MAX_LINE_SIZE];

    //ajout de l'item
    LVITEM lvi;
    lvi.mask = LVIF_TEXT|LVIF_PARAM;
    lvi.iSubItem = 0;
    lvi.lParam = LVM_SORTITEMS;
    lvi.pszText="";
    lvi.iItem = ref_item;
    ListView_InsertItem(hlv_r, &lvi);


    //Date
    ListView_SetItemText(hlv_r,ref_item,0,item[6].c);
    //Source
    ListView_SetItemText(hlv_r,ref_item,1,"Settings");
    //User
    ListView_SetItemText(hlv_r,ref_item,3,"");
    //Description
    snprintf(tmp,MAX_LINE_SIZE,"[Create] %s-%s %s:%s (%s)",item[3].c,item[4].c,item[1].c,item[2].c,item[5].c);
    ListView_SetItemText(hlv_r,ref_item,2,tmp);

    StateH(item,6,-1);
    LeaveCriticalSection(&Sync);
  }
if (strlen(item[7].c)>=DATE_SIZE/2)
  {
    EnterCriticalSection(&Sync);

    HANDLE hlv_r = GetDlgItem(Tabl[TABL_STATE],LV_VIEW);
    DWORD ref_item = ListView_GetItemCount(hlv_r);
    char tmp[MAX_LINE_SIZE];

    //ajout de l'item
    LVITEM lvi;
    lvi.mask = LVIF_TEXT|LVIF_PARAM;
    lvi.iSubItem = 0;
    lvi.lParam = LVM_SORTITEMS;
    lvi.pszText="";
    lvi.iItem = ref_item;
    ListView_InsertItem(hlv_r, &lvi);


    //Date
    ListView_SetItemText(hlv_r,ref_item,0,item[7].c);
    //Source
    ListView_SetItemText(hlv_r,ref_item,1,"Settings");
    //User
    ListView_SetItemText(hlv_r,ref_item,3,"");
    //Description
    snprintf(tmp,MAX_LINE_SIZE,"[Last access] %s-%s %s:%s (%s)",item[3].c,item[4].c,item[1].c,item[2].c,item[5].c);
    ListView_SetItemText(hlv_r,ref_item,2,tmp);

    StateH(item,7,-1);
    LeaveCriticalSection(&Sync);
  }
}
//------------------------------------------------------------------------------
void AddToLVConf(HANDLE hlv, LINE_ITEM *item, unsigned short nb_colonne)
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

  //gestion du filtre pour ajout aux autres onglets
  FiltreConfData(item);
}
//------------------------------------------------------------------------------
void EnumRegToConfValues(HKEY hkey, char *chkey, char *path, char *source, char *type, char *desc, HANDLE hlv)
{
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];

  // on ouvre la cle
  HKEY CleTmp=0;
  if (RegOpenKey(hkey,path,&CleTmp)!=ERROR_SUCCESS)
     return;

  FILETIME DernierMAJ;
  DWORD nbValue = 0,i;
  if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValue,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
  {
    if (nbValue)
    {
      //remplissage
      snprintf(lv_line[0].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);
      strncpy(lv_line[3].c,source,MAX_LINE_SIZE);
      strncpy(lv_line[4].c,type,MAX_LINE_SIZE);
      strncpy(lv_line[5].c,desc,MAX_LINE_SIZE);

      DWORD NameSize = MAX_LINE_SIZE, DataSize = MAX_LINE_SIZE;
      for (i=0;i<nbValue;i++)
      {
        NameSize = MAX_LINE_SIZE;
        DataSize = MAX_LINE_SIZE;
        lv_line[1].c[0]=0;
        lv_line[2].c[0]=0;

        //date
        SYSTEMTIME SysTime;
        if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
          snprintf(lv_line[6].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
        else lv_line[6].c[0]=0;

        if (RegEnumValue (CleTmp,i,lv_line[1].c,(LPDWORD)&NameSize,0,0,(LPBYTE)lv_line[2].c,(LPDWORD)&DataSize)==ERROR_SUCCESS)
        {
          if (lv_line[1].c[0]!=0 || lv_line[2].c[0]!=0)
            AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
        }
      }
    }
  }
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
void LireValueRegToConf(HKEY hkey, char *chkey, char *path, char *value, char *source, char *type, char *desc, HANDLE hlv)
{
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];

  snprintf(lv_line[0].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);
  strncpy(lv_line[1].c,value,MAX_LINE_SIZE);
  lv_line[2].c[0]=0;
  LireValeur(hkey,path,lv_line[1].c,lv_line[2].c,MAX_LINE_SIZE);
  strncpy(lv_line[3].c,source,MAX_LINE_SIZE);
  strncpy(lv_line[4].c,type,MAX_LINE_SIZE);
  strncpy(lv_line[5].c,desc,MAX_LINE_SIZE);
  lv_line[6].c[0]=0;
  lv_line[7].c[0]=0;
  LireKeyUpdate(hkey,path, lv_line[7].c, MAX_LINE_SIZE);
  AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_configuration(LPVOID lParam)
{
  //on vide les listeview
  BOOL local        = FALSE;
  HANDLE hlv        = GetDlgItem(Tabl[TABL_CONFIGURATION],LV_VIEW);
  if (lParam == FALSE || lParam == TRUE)
  {
    local        = (BOOL)lParam;
    ListView_DeleteAllItems(hlv);
  }

  char tmp[MAX_PATH];
  DWORD tmp_d;

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_CONFIGURATION,TRUE);

  if (local)
  {

//********************************
//IE HISTORY
    //Settings
    Enum_IE_conf(hlv);

    //bookmarks


    //passwords


    //historiques
    //recherche de fichier .dat et lecture des fichiers !!!
    int i, m=0, nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);
    if (nblecteurs)
    {
      SEARCH_C files_conf[13];
      char tmp_path[MAX_PATH];
      //IE
      strcpy(files_conf[m++].c,"index.dat");
      //Firefox
      strcpy(files_conf[m++].c,"content-prefs.sqlite");
      strcpy(files_conf[m++].c,"addons.sqlite");
      strcpy(files_conf[m++].c,"extensions.sqlite");
      strcpy(files_conf[m++].c,"cookies.sqlite");
      strcpy(files_conf[m++].c,"downloads.sqlite");
      strcpy(files_conf[m++].c,"formhistory.sqlite");
      strcpy(files_conf[m++].c,"places.sqlite");
      strcpy(files_conf[m++].c,"signons.sqlite");
      //chrome
      strcpy(files_conf[m++].c,"Archived History");
      strcpy(files_conf[m++].c,"History");
      strcpy(files_conf[m++].c,"Cookies");
      strcpy(files_conf[m++].c,"Web Data");

      for (i=0;i<nblecteurs;i+=4)
      {
        switch(GetDriveType(&tmp[i]))
        {
          case DRIVE_FIXED :
            //traitement de recherche
            strcpy(tmp_path,"_:\\*.*\0");
            tmp_path[0]=tmp[i];

            //configuration
            MultiFileSearc(tmp_path,files_conf,m,TRV_HTREEITEM[TRV_CONF]);
          break;
        }
      }

      SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS,TVM_SORTCHILDREN, TRUE,(LPARAM)TRV_HTREEITEM[TRV_CONF]);
      SupDoublon(Tabl[TABL_CONF],TRV_CONF_TESTS,TRV_HTREEITEM[TRV_CONF]);
      Scan_configuration((LPVOID)-1);
    }
//********************************
  }else
  {
    //récupération du 1er fils dans le treeview
    HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_CONF]);
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_CONF-1, (LPARAM)"");

    do
    {
      //récupération du texte de l'item
      TVITEM tvitem;
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        tmp_d = strlen(tmp);
        //********************************
        //IE HISTORY : suivant si un fichier .dat
        if (tmp[tmp_d-4]=='.' && (tmp[tmp_d-3]=='d'||tmp[tmp_d-3]=='D'))
        {
          ReadDATFile(tmp, hlv, "");

        //********************************
        //Firefox+chrome+android : .sqlite + db et sans extension
        }else //if (tmp[tmp_d-7]=='.' && ((tmp[tmp_d-6]=='s' || tmp[tmp_d-6]=='S')&& (tmp[tmp_d-5]=='q' || tmp[tmp_d-5]=='Q')))
        {
          OpenSQLite(hlv, tmp, 0xFFFFFFFF);
        }
        //********************************
      }
    }while((hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)) && ScanStart);

    snprintf(tmp,MAX_PATH,"load %u records",ListView_GetItemCount(hlv));
    SB_add_T(SB_ONGLET_CONF-1, tmp);
  }

  /*a coder*/
/*
Applications a prendre en compte :
  - IE
  - firefox
  - chrome
  - fichiers Android


  - fichiers temporaire
  - Wifi

  - IIS
  - Apache
  - Sharepoint

  - thunderbird

  - lotusnotes
  - outlook
  - outlook express
  - Tomcat
  - Citrix
  - Opera
  - Safari
  - MySQL
  - MSSQL
  - Oracle
  - LDAP
  - OpenVPN
  - AD
  - Bind/DNS
  - Bluetooth
  - exchange
  - firewall
*/

  h_scan_configuration = NULL;
  if (!h_scan_logs && !h_scan_files && !h_scan_registry && !h_scan_configuration)
  {
    ScanStart = FALSE;
    SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_START),"Start");
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_INFO, (LPARAM)"");
  }

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_CONFIGURATION,FALSE);
  return 0;
}

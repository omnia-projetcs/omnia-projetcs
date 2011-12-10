//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
#include "d3des.h"
//------------------------------------------------------------------------------
// il faut modifier la clé last Key !
//[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Applets\Regedit]
//"LastKey"="\\HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
void OpenRegeditKey(char *key)
{
  //write last key to use
  char tmp[MAX_PATH];
  BOOL ok = FALSE;
  snprintf(tmp,MAX_PATH,"\\%s",key);
  HKEY CleTmp=0;
  DWORD cbData=strlen(tmp)+1;

  // on ouvre la cle
  if (RegOpenKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit",&CleTmp)!=ERROR_SUCCESS)
     return;

 if (RegSetValueEx(CleTmp,"LastKey",0,REG_SZ,(const BYTE *)tmp,cbData)==ERROR_SUCCESS)ok = TRUE;

  //On ferme la cle
  RegCloseKey(CleTmp);

  //Open regedit
  if(ok)ShellExecute(Tabl[TABL_MAIN], "open","regedit","",NULL,SW_SHOW);
}
//------------------------------------------------------------------------------
int LireGValeur(HKEY ENTETE,char *chemin,char *nom,char *Valeur)
{
    //variables
    DWORD tailleCle=0;
    HKEY CleTmp=0;

    // on ouvre la cle
    if (RegOpenKey(ENTETE,chemin,&CleTmp)!=ERROR_SUCCESS)
       return FALSE;

    //On recupere la taille de la cle qui va être lue
    if (RegQueryValueEx(CleTmp, nom, 0, 0, 0, &tailleCle)!=ERROR_SUCCESS)
       return FALSE;

    //permet de gérer les chaines énormes
    if (Valeur == NULL) return tailleCle;
    else Valeur[0]=0;

      //On lit la valeur
      if (RegQueryValueEx(CleTmp, nom, 0, 0, (LPBYTE)Valeur, &tailleCle)!=ERROR_SUCCESS)
         return FALSE;

    //On ferme la cle
    RegCloseKey(CleTmp);
    return tailleCle;
}
//------------------------------------------------------------------------------
int LireDValeur(HKEY ENTETE,char *chemin,char *nom,DWORD *Valeur)
{
    //variables
    DWORD tailleCle=0;
    HKEY CleTmp=0;

    // on ouvre la cle
    if (RegOpenKey(ENTETE,chemin,&CleTmp)!=ERROR_SUCCESS)
       return FALSE;

    //On recupere la taille de la cle qui va être lue
    if (RegQueryValueEx(CleTmp, nom, 0, 0, 0, &tailleCle)!=ERROR_SUCCESS)
       return FALSE;

    //permet de gérer les chaines énormes
    if (Valeur == NULL) return tailleCle;
    else Valeur[0]=0;

      //On lit la valeur
      if (RegQueryValueEx(CleTmp, nom, 0, 0, (LPBYTE)Valeur, &tailleCle)!=ERROR_SUCCESS)
         return FALSE;

    //On ferme la cle
    RegCloseKey(CleTmp);
    return tailleCle;
}
//------------------------------------------------------------------------------
int LireValeur(HKEY ENTETE,char *chemin,char *nom,char *val,unsigned int taille_max)
{
    char *Valeur;
    DWORD ret, taille_val = LireGValeur(ENTETE,chemin,nom,NULL);
    val[0]=0;

    //allocation de la mémoire !
    Valeur = (char *)HeapAlloc(GetProcessHeap(), 0, taille_val*sizeof(char)+1);
    if (!Valeur) return FALSE;

    if ((ret = LireGValeur(ENTETE,chemin,nom,Valeur)))
    {
      if (ret<=taille_max)
      {
        // on garde les sprintf car quelque fois des problèmes avec strcpy, strcat
        if (((Valeur[0]&255)<126)&&((Valeur[0]&255)>31) && ((Valeur[1]&255)<126)&&((Valeur[1]&255)>31))
            snprintf(val,taille_max,"%s",Valeur);
      }
    }else
    {
      HeapFree(GetProcessHeap(), 0, Valeur);
      return FALSE;
    }
    HeapFree(GetProcessHeap(), 0, Valeur);

    return TRUE;
}
//------------------------------------------------------------------------------
HTREEITEM AddItemTvAndLv(LINE_ITEM *lv_line, unsigned int id_icon , HANDLE hlv, HTREEITEM hparent)
{
  char tmp[MAX_LINE_SIZE];
  snprintf(tmp,MAX_LINE_SIZE,"%s=%s",lv_line[2].c,lv_line[3].c);
  HTREEITEM hitem = AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,hparent,id_icon);

  //listeview
  AddToLVRegBin(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
  AddToLV_Registry(lv_line);
return hitem;
}

//------------------------------------------------------------------------------
void EnumKeyAndValue(HKEY ENTETE, char*chemin, HANDLE hlv, HTREEITEM hparent, char *maj)
{
  // on ouvre la cle
  HKEY CleTmp=0;
  if (RegOpenKey(ENTETE,chemin,&CleTmp)!=ERROR_SUCCESS)
     return;

  //lecture du nombre de sous clé et valeure
  //lecture de la dernière mise à jour
  FILETIME DernierMAJ;
  DWORD nbSubKey = 0, nbValue = 0,i;
  if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,&nbValue,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
  {
    if (nbValue)
    {
      LINE_ITEM lv_line[SIZE_UTIL_ITEM];
      lv_line[0].c[0]=0;
      strcpy(lv_line[5].c,maj);

      if (ENTETE == HKEY_CLASSES_ROOT)sprintf(lv_line[1].c,"HKEY_CLASSES_ROOT\\%s",chemin);
      else if (ENTETE == HKEY_CURRENT_CONFIG)sprintf(lv_line[1].c,"HKEY_CURRENT_CONFIG\\%s",chemin);
      else if (ENTETE == HKEY_CURRENT_USER)sprintf(lv_line[1].c,"HKEY_CURRENT_USER\\%s",chemin);
      else if (ENTETE == HKEY_LOCAL_MACHINE)sprintf(lv_line[1].c,"HKEY_LOCAL_MACHINE\\%s",chemin);
      else if (ENTETE == HKEY_USERS)sprintf(lv_line[1].c,"HKEY_USERS\\%s",chemin);
      else if (ENTETE == HKEY_PERFORMANCE_DATA)sprintf(lv_line[1].c,"HKEY_PERFORMANCE_DATA\\%s",chemin);
      else if (ENTETE == HKEY_DYN_DATA)sprintf(lv_line[1].c,"HKEY_DYN_DATA\\%s",chemin);
      else sprintf(lv_line[1].c,"%lu(UNKONW)\\%s",(DWORD)ENTETE,chemin);

      DWORD NameSize = MAX_LINE_SIZE, DataSize = MAX_LINE_SIZE;
      DWORD type =0,j;

      char tmp[MAX_LINE_SIZE];

      for (i=0;i<nbValue;i++)
      {
        NameSize = MAX_LINE_SIZE;
        DataSize = MAX_LINE_SIZE;
        lv_line[2].c[0]=0;
        if (RegEnumValue (CleTmp,i,lv_line[2].c,(LPDWORD)&NameSize,0,(LPDWORD)&type,(LPBYTE)lv_line[4].c,(LPDWORD)&DataSize)==ERROR_SUCCESS)
        {
          switch(type)
          {
            //chaînes
            case REG_EXPAND_SZ:
              strcpy(lv_line[3].c,lv_line[4].c);
              //ajout au treeview + listeview
              strcpy(lv_line[4].c,"REG_EXPAND_SZ");
              AddItemTvAndLv(lv_line, ICON_FILE_STR, hlv, hparent);
            break;
            case REG_SZ:
              strcpy(lv_line[3].c,lv_line[4].c);
              //ajout au treeview + listeview
              strcpy(lv_line[4].c,"REG_SZ");
              AddItemTvAndLv(lv_line, ICON_FILE_STR, hlv, hparent);
            break;
            //chaîne unicode
            case REG_LINK:
              sprintf(lv_line[3].c,"%S",lv_line[4].c);
              //ajout au treeview + listeview
              strcpy(lv_line[4].c,"REG_LINK");
              AddItemTvAndLv(lv_line, ICON_FILE_STR, hlv, hparent);
            break;
            //chaîne multiple
            case REG_MULTI_SZ:
              lv_line[3].c[0]=0;
              unsigned int pos =0;
              do
              {
                strncpy(tmp,&lv_line[4].c[pos],MAX_LINE_SIZE);
                pos+=strlen(tmp)+1;
                strncat (lv_line[3].c,tmp,MAX_LINE_SIZE);
                strncat (lv_line[3].c," \0",MAX_LINE_SIZE);
              }while (pos<DataSize);
              //ajout au treeview + listeview
              strcpy(lv_line[4].c,"REG_MULTI_SZ");
              AddItemTvAndLv(lv_line, ICON_FILE_STR, hlv, hparent);
            break;
            case REG_BINARY :
              lv_line[3].c[0]=0;
              for (j=0;j<DataSize;j++)
              {
                snprintf(tmp,9,"%02X",lv_line[4].c[j]&0xff);
                strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
              }
              strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
              strcpy(lv_line[4].c,"REG_BINARY");
              AddItemTvAndLv(lv_line, ICON_FILE_BIN, hlv, hparent);
            break;
            case 0x00000000 :
              lv_line[3].c[0]=0;
              for (j=0;j<DataSize;j++)
              {
                snprintf(tmp,9,"%02X",lv_line[4].c[j]&0xff);
                strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
              }
              strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
              strcpy(lv_line[4].c,"REG_NONE");
              AddItemTvAndLv(lv_line, ICON_FILE_BIN, hlv, hparent);
            break;
            case 0x00000004 :
            case 0x00000005 :
              lv_line[3].c[0]=0;
              for (j=0;j<DataSize;j++)
              {
                snprintf(tmp,9,"%02X",lv_line[4].c[j]&0xff);
                strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
              }
              strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
              strcpy(lv_line[4].c,"REG_DWORD");
              AddItemTvAndLv(lv_line, ICON_FILE_BIN, hlv, hparent);
            break;
            case 0x0000000b :
              lv_line[3].c[0]=0;
              for (j=0;j<DataSize;j++)
              {
                snprintf(tmp,9,"%02X",lv_line[4].c[j]&0xff);
                strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
              }
              strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
              strcpy(lv_line[4].c,"REG_QWORD");
              AddItemTvAndLv(lv_line, ICON_FILE_BIN, hlv, hparent);
            break;
            default : //le reste en binaire
              lv_line[3].c[0]=0;
              for (j=0;j<DataSize;j++)
              {
                snprintf(tmp,9,"%02X",lv_line[4].c[j]&0xff);
                strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
              }
              strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);
              snprintf(lv_line[4].c,MAX_LINE_SIZE,"0x%08X",(unsigned int)(type & 0xFFFFFFFF));
              AddItemTvAndLv(lv_line, ICON_FILE_UNK, hlv, hparent);
            break;
          }
        }
      }
    }
    if (nbSubKey)
    {
      SYSTEMTIME SysTime;
      FILETIME DernierMAJ;
      char tmp[MAX_PATH];
      char tmp_maj[MAX_PATH];
      char tmp_path[MAX_PATH];
      char path[MAX_PATH];
      strcpy(path,chemin);
      HTREEITEM fils=hparent;
      DWORD DataSize;

      for (i=0;i<nbSubKey;i++)
      {
        DataSize = MAX_PATH;
        if (RegEnumKeyEx (CleTmp,i,tmp,(LPDWORD)&DataSize,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
        {
          if (path[0]==0)snprintf(tmp_path,MAX_PATH,"%s",tmp);
          else snprintf(tmp_path,MAX_PATH,"%s\\%s",path,tmp);

          if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
            snprintf(tmp_maj,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
          else tmp_maj[0]=0;

          fils = AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,tmp,hparent,ICON_FILE_DOSSIER);
          EnumKeyAndValue(ENTETE, tmp_path, hlv, fils, tmp_maj);
        }
      }
    }
  }
  RegCloseKey(CleTmp);
}

//------------------------------------------------------------------------------
int GetWindowsCDKey(char * lpszBuffer)
{
	HKEY hKey;

	int nCur			= 0;
	int bStatus		= 1;

	BYTE lpRawData[MAX_PATH];
	BYTE lpEncSerial[MAX_PATH];

	char lpszSerial[MAX_PATH];
	char lpszTemp[MAX_PATH];

	char lpKeyChars[25] = "BCDFGHJKMPQRTVWXY2346789";
	int i,c;

	DWORD dwDataLen		= MAX_PATH;

	if(RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion",&hKey) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey,"DigitalProductId",0,0,lpRawData,&dwDataLen) == ERROR_SUCCESS)
		{
			for(i=52;i<=66;i++)
				lpEncSerial[i-52] = lpRawData[i];

			for(i=24;i>=0;i--)
			{
				nCur = 0;
				for(c=14;c>-1;c--)
				{
					nCur = nCur * 256;
					nCur ^= lpEncSerial[c];
					lpEncSerial[c] = nCur / 24;
					nCur %= 24;
				}
				lpszSerial[i] = lpKeyChars[nCur];
			}

			for(i=0;lpszSerial[i] && (i+i/5) < 30;i++){
				if(i % 5 == 0 && i>0) lstrcat(lpszBuffer,"-");
				wsprintf(lpszTemp,"%c",lpszSerial[i]);
				lstrcat(lpszBuffer,lpszTemp);
				ZeroMemory(lpszTemp,MAX_PATH);
			}

		}else bStatus = 0;
		RegCloseKey(hKey);
	}else bStatus = 0;

	return bStatus;
}
//------------------------------------------------------------------------------
void registry_configuration(HANDLE hlv)
{
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];

  lv_line[0].c[0]=0; //pas de fichier source puisque en local

  //path
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion");

  //les informations ^^
  //OS Vesion
  lv_line[3].c[0]=0;
  strcpy(lv_line[2].c,"ProductName");
  strcpy(lv_line[4].c,"(Configuration) Operating System");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion","ProductName",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);


  lv_line[3].c[0]=0;
  strcpy(lv_line[2].c,"CSDVersion");
  strcpy(lv_line[4].c,"(Configuration) Service Pack");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion","CSDVersion",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //OS path install
  lv_line[3].c[0]=0;
  strcpy(lv_line[2].c,"SystemRoot");
  strcpy(lv_line[4].c,"(Configuration) System path");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion","SystemRoot",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //license
  strcpy(lv_line[2].c,"DigitalProductId");
  lv_line[3].c[0]=0;
  GetWindowsCDKey(lv_line[3].c);
  lv_line[3].c[29]=0;
  strcpy(lv_line[4].c,"(Serial) MS product serial");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //AppInit_DLLs
  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion\\Windows");
  strcpy(lv_line[2].c,"AppInit_DLLs");
  strcpy(lv_line[4].c,"(malware) DLL load in GUI Windows");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows","AppInit_DLLs",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //Debugger
  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\MICROSOFT\\Windows NT\\CurrentVersion\\Image File Execution Options\\Your Image File Name Here without a path");
  strcpy(lv_line[2].c,"Debugger");
  strcpy(lv_line[4].c,"(attack vector) Use for redirect application");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\Your Image File Name Here without a path","Debugger",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //CMD
  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\MICROSOFT\\Command Processor");
  strcpy(lv_line[2].c,"Autorun");
  strcpy(lv_line[4].c,"(malware) Command to execute with all cmd command");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Command Processor","Autorun",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\SOFTWARE\\MICROSOFT\\Command Processor");
  strcpy(lv_line[2].c,"Autorun");
  strcpy(lv_line[4].c,"(malware) Command to execute with all cmd command");
  if (LireGValeur(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Command Processor","Autorun",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //Open
  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\exefile\\shell\\open\\command");
  strcpy(lv_line[2].c,"");
  strcpy(lv_line[4].c,"(malware) Run when popup menu run command");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\exefile\\shell\\open\\command","",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\SOFTWARE\\Classes\\exefile\\shell\\open\\command");
  strcpy(lv_line[2].c,"");
  strcpy(lv_line[4].c,"(malware) Run when popup menu run command");
  if (LireGValeur(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\exefile\\shell\\open\\command","",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //Guide de sécurité
  char tmp[MAX_PATH]="";
  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer");
  strcpy(lv_line[2].c,"NoDriveTypeAutoRun");
  strcpy(lv_line[4].c,"(malware) Autorun usb/CDROM/..., 0xff:disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer","NoDriveTypeAutoRun",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  tmp[0]=0;
  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters");
  strcpy(lv_line[2].c,"Autorun");
  strcpy(lv_line[4].c,"(malware) Autorun CDROM, 0x00:disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters","Autorun",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters");
  strcpy(lv_line[2].c,"RestrictNullSessAccess");
  strcpy(lv_line[4].c,"(attack vector) Null session, 0x01:Disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters","RestrictNullSessAccess",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters");
  strcpy(lv_line[2].c,"dontdisplaylastusername");
  strcpy(lv_line[4].c,"(authentication) 0x01:Do not display last username in logon screen");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters","dontdisplaylastusername",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManWorkstation\\Parameters");
  strcpy(lv_line[2].c,"dontdisplaylastusername");
  strcpy(lv_line[4].c,"(authentication) 0x01:Do not display last username in logon screen");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManWorkstation\\Parameters","dontdisplaylastusername",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters");
  strcpy(lv_line[2].c,"enableplaintextpassword");
  strcpy(lv_line[4].c,"(authentication) 0x00:Disable send unencrypted password to connect to third-party SMB servers");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManServer\\Parameters","enableplaintextpassword",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManWorkstation\\Parameters");
  strcpy(lv_line[2].c,"enableplaintextpassword");
  strcpy(lv_line[4].c,"(authentication) 0x00:Disable send unencrypted password to connect to third-party SMB servers");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManWorkstation\\Parameters","enableplaintextpassword",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\LanManWorkstation\\Parameters");
  strcpy(lv_line[2].c,"RestrictNullSessAccess");
  strcpy(lv_line[4].c,"(attack vector) Null session, 0x01:Disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\LanManWorkstation\\Parameters","RestrictNullSessAccess",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"RestrictAnonymousSAM");
  strcpy(lv_line[4].c,"(attack vector) Anonymous connexion for SAM enumeration (revers SID), 0x01:Disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","RestrictAnonymousSAM",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"RestrictAnonymous");
  strcpy(lv_line[4].c,"(attack vector) Anonymous connexion, 0x01:Disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","RestrictAnonymous",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System");
  strcpy(lv_line[2].c,"DontDisplayLastUserName");
  strcpy(lv_line[4].c,"(attack vector) Display last user login, 0x01:Disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System","DontDisplayLastUserName",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System");
  strcpy(lv_line[2].c,"DisableRegistryTools");
  strcpy(lv_line[4].c,"(attack vector) Disable use of windows registry editor, 0x01:Disable");
  if (LireGValeur(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System","DisableRegistryTools",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System");
  strcpy(lv_line[2].c,"EnableLUA");
  strcpy(lv_line[4].c,"(malware) 0x01:Disable UAC (User Account Control)");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System","EnableLUA",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");
  strcpy(lv_line[2].c,"disablecad");
  strcpy(lv_line[4].c,"(malware) 0x01:Enable CTRL+ALT+DEL requirement for logon");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon","disablecad",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"LmCompatibilityLevel");
  strcpy(lv_line[4].c,"(authentication) Authentication method 0x00:only LM or NTLM, 0x01:NTLMv2 if enable, 0x02:NTLM only, 0x03:NTLMv2 only, 0x04:LM deny, 0x05:LM and NTLM deny, NTLMv2 only");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","LmCompatibilityLevel",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"NoLMHash");
  strcpy(lv_line[4].c,"(authentication) 0x01:Disable LM HASH");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","NoLMHash",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows");
  strcpy(lv_line[2].c,"legalnoticetext");
  strcpy(lv_line[4].c,"(authentication) Message title for user attempting to log on");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows","legalnoticetext",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows");
  strcpy(lv_line[2].c,"legalnoticecaption");
  strcpy(lv_line[4].c,"(authentication) Message for user attempting to log on");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows","legalnoticecaption",lv_line[3].c))
  {
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"auditbaseobjects");
  strcpy(lv_line[4].c,"(audit) Audit the access of global system objects, 0x01:Enable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","auditbaseobjects",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"fullprivilegeauditing");
  strcpy(lv_line[4].c,"(audit) Audit use of Backup and Restore privilege, 0x01:Enable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","fullprivilegeauditing",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  tmp[0]=0;
  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
  strcpy(lv_line[2].c,"everyoneincludesanonymous");
  strcpy(lv_line[4].c,"(attack vector) Permissions \"everyone\" are applied to anonymous users, 0x00:Disable");
  if (LireGValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Lsa","everyoneincludesanonymous",tmp))
  {
    sprintf(lv_line[3].c,"%02X%02X%02X%02X",tmp[3]&0xff,tmp[2]&0xff,tmp[1]&0xff,tmp[0]&0xff);
  }else strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\Control Panel\\Desktop");
  strcpy(lv_line[2].c,"ScreenSaveActive");
  strcpy(lv_line[4].c,"(ScreenSaver) Enable screensaver, 0x01:Enable");
  if (!LireGValeur(HKEY_CURRENT_USER,"Control Panel\\Desktop","ScreenSaveActive",lv_line[3].c))
    strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\Control Panel\\Desktop");
  strcpy(lv_line[2].c,"ScreenSaverIsSecure");
  strcpy(lv_line[4].c,"(ScreenSaver) Password to exit the screen saver, 0x01:Enable");
  if (!LireGValeur(HKEY_CURRENT_USER,"Control Panel\\Desktop","ScreenSaverIsSecure",lv_line[3].c))
    strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  lv_line[3].c[0]=0;
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\Control Panel\\Desktop");
  strcpy(lv_line[2].c,"ScreenSaveTimeOut");
  strcpy(lv_line[4].c,"(ScreenSaver) Time to activate the screen saver (second).");
  if (!LireGValeur(HKEY_CURRENT_USER,"Control Panel\\Desktop","ScreenSaveTimeOut",lv_line[3].c))
    strcpy(lv_line[3].c,"<NO VALUE>");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);

  //registry hive path
  reg_liste_DataValeurSpec(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SYSTEM\\CurrentControlSet\\Control\\hivelist\\","","(configuration) Registry hive path",hlv);
}
//------------------------------------------------------------------------------
//liste des applications installées + date de maj + filtre de validation
void registry_software(HANDLE hlv)
{
  HKEY CleTmp=0,CleTmp2;
  DWORD nbSubKey = 0,i,TailleNomSubKey;
  char NomSubKey[MAX_PATH]="\0",chemin2[MAX_PATH];
  char editor[MAX_PATH];
  FILETIME DernierMAJ;// dernière mise a jour ou creation de la cle
  SYSTEMTIME SysTime;

  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      lv_line[0].c[0]=0;
      for (i=0;i<nbSubKey;i++)
      {
        TailleNomSubKey=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
        if (RegEnumKeyEx (CleTmp,i,NomSubKey,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
        {
          // on cree la trame pour recupere les donnees
          strcpy(chemin2,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
          strncat(chemin2,NomSubKey,MAX_PATH);
          strncat(chemin2,"\\\0",MAX_PATH);
          snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s",NomSubKey);

          lv_line[2].c[0]=0;
          if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"DisplayName",lv_line[2].c,MAX_LINE_SIZE))
          {
            LireValeur(HKEY_LOCAL_MACHINE,chemin2,"Comments",lv_line[2].c,MAX_LINE_SIZE);
          }

          editor[0]=0;
          lv_line[4].c[0]=0;
          //date d'installation
          //test et récupération de la date de dernière modif
          if (RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp2)==ERROR_SUCCESS)
          {
            if(RegQueryInfoKey(CleTmp2,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
            {
              if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
                snprintf(lv_line[4].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
            }
            RegCloseKey(CleTmp2);
          }

          //ajout de la version
          if(LireValeur(HKEY_LOCAL_MACHINE,chemin2,"DisplayVersion",editor,MAX_PATH))
          {
            strcat(lv_line[2].c," ");
            strncat(lv_line[2].c,editor,MAX_LINE_SIZE);
          }

          //editeur
          lv_line[3].c[0]=0;
          LireValeur(HKEY_LOCAL_MACHINE,chemin2,"Publisher",lv_line[3].c,MAX_LINE_SIZE);

          ////UninstallString
          lv_line[5].c[0]=0;
          if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"UninstallString",lv_line[5].c,MAX_PATH))
          {
            if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"InstallLocation",lv_line[5].c,MAX_PATH))
            {
              if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"Inno Setup: App Path",lv_line[5].c,MAX_PATH))
              {
                LireValeur(HKEY_LOCAL_MACHINE,chemin2,"QuietUninstallString",lv_line[5].c,MAX_PATH);
              }
            }
          }

          //Installed by
          lv_line[6].c[0]=0;
          LireValeur(HKEY_LOCAL_MACHINE,chemin2,"Inno Setup: User",lv_line[6].c,MAX_PATH);

          //URL
          lv_line[7].c[0]=0;
          if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"URLInfoAbout",lv_line[7].c,MAX_PATH))
          {
            if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"URLUpdateInfo",lv_line[7].c,MAX_PATH))
            {
              if(!LireValeur(HKEY_LOCAL_MACHINE,chemin2,"InstallSource",lv_line[7].c,MAX_PATH))
              {
                LireValeur(HKEY_LOCAL_MACHINE,chemin2,"HelpLink",lv_line[7].c,MAX_PATH);
              }
            }
          }

          //application reconnues ?
          if ((lv_line[2].c[0] == 'M' &&(lv_line[2].c[5] == 'w' || lv_line[2].c[7] == 'j' || lv_line[2].c[8] == 't' || (lv_line[2].c[1] == 'S' && lv_line[2].c[2] == 'D' && lv_line[2].c[3] == 'N')))
           || (lv_line[2].c[0] == 'C' && lv_line[2].c[8] == 'f')||(lv_line[2].c[0] == 'H' && lv_line[2].c[5] == 'x')||(lv_line[2].c[0] == 'S' && lv_line[2].c[9] == 'U')
           || (lv_line[2].c[0] == 'W' && lv_line[2].c[6] == 's' && lv_line[2].c[8] != 'L')
           || (lv_line[2].c[0] == 'L' && lv_line[2].c[8] != 'W' && lv_line[2].c[16] != 'M')) strcpy(lv_line[8].c,"OK");
          else lv_line[8].c[0]=0;

          if (lv_line[2].c[0]!=0 || lv_line[5].c[0]!=0)
          {
            if (lv_line[2].c[0]==0)strcpy(lv_line[2].c,NomSubKey);

            AddToLV_Registry2(lv_line[4].c, "", "Software", lv_line[2].c);
            AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_LOGICIEL_NB_COL]);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//liste des maj installées + date de maj + filtre de validation
void registry_update(HANDLE hlv)
{
  HKEY CleTmp,CleTmp2,CleTmp3,CleTmp4;
  DWORD nbSubKey=0,nbSubKey2,nbSubKey3,TailleNomSubKey,TailleNomSubKey2,TailleNomSubKey3,i,j,k;
  char chemin[MAX_PATH],chemin2[MAX_PATH],chemin3[MAX_PATH];
  char tmp_add[MAX_PATH];
  FILETIME DernierMAJ;// dernière mise a jour ou creation de la cle
  SYSTEMTIME SysTime;

  LINE_ITEM lv_line[SIZE_UTIL_ITEM];

  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Component Based Servicing\\Packages\\",&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      lv_line[0].c[0]=0;
      for (i=0;i<nbSubKey;i++)
      {
        TailleNomSubKey=MAX_PATH;
        if (RegEnumKeyEx (CleTmp,i,lv_line[2].c,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(chemin,MAX_PATH,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Component Based Servicing\\Packages\\%s\\",lv_line[2].c);
          snprintf(lv_line[1].c,MAX_PATH,"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Component Based Servicing\\Packages\\%s",lv_line[2].c);

          //lecture des infos
          lv_line[6].c[0]=0;
          LireValeur(HKEY_LOCAL_MACHINE,chemin,"InstallUser",lv_line[6].c,MAX_PATH);

          lv_line[5].c[0]=0;
          if(RegOpenKey(HKEY_LOCAL_MACHINE,chemin,&CleTmp4)== ERROR_SUCCESS)
          {
            if(RegQueryInfoKey(CleTmp4,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
              if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)
                snprintf(lv_line[5].c,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
            RegCloseKey(CleTmp4);
          }

          //Name
          lv_line[3].c[0]=0;
          LireValeur(HKEY_LOCAL_MACHINE,chemin,"InstallName",lv_line[3].c,MAX_PATH);

          //Path
          lv_line[4].c[0]=0;
          LireValeur(HKEY_LOCAL_MACHINE,chemin,"InstallLocation",lv_line[4].c,MAX_PATH);

          snprintf(tmp_add,MAX_PATH,"%s,%s,%s",lv_line[2].c,lv_line[3].c,lv_line[4].c);
          AddToLV_Registry2(lv_line[5].c, lv_line[6].c, "Update/Packages", tmp_add);
          AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
        }
      }
    }
    RegCloseKey(CleTmp);
  }

  if(RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Updates\\",&CleTmp)== ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      lv_line[0].c[0]=0;
      for (i=0;i<nbSubKey;i++)
      {
        TailleNomSubKey=MAX_PATH;
        //intitulé ou nom du KB
        lv_line[2].c[0]=0;
        if (RegEnumKeyEx (CleTmp,i,lv_line[2].c,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(chemin,MAX_PATH,"SOFTWARE\\Microsoft\\Updates\\%s\\",lv_line[2].c);
          if(RegOpenKey(HKEY_LOCAL_MACHINE,chemin,&CleTmp2)== ERROR_SUCCESS)
          {
            if (RegQueryInfoKey (CleTmp2,0,0,0,&nbSubKey2,0,0,0,0,0,0,0)==ERROR_SUCCESS)
            {
              for (j=0;j<nbSubKey2;j++)
              {
                TailleNomSubKey2=MAX_PATH;
                //Nom du KB ou SP
                lv_line[3].c[0]=0;
                if (RegEnumKeyEx (CleTmp2,j,lv_line[3].c,&TailleNomSubKey2,0,0,0,0)==ERROR_SUCCESS)
                {
                  snprintf(chemin2,MAX_PATH,"%s%s\\",chemin,lv_line[3].c);
                  snprintf(lv_line[1].c,MAX_PATH,"HKEY_LOCAL_MACHINE\\%s%s",chemin,lv_line[3].c);
                  lv_line[3].c[0]=0;
                  if (LireGValeur(HKEY_LOCAL_MACHINE,chemin2,"PackageName",lv_line[4].c))
                  {
                    //un KB
                    //Source = NomSubKey, KB = NomSubKey2, InstalledBy, InstalledDate, Description = PackageName
                    //5%,10%,50%,10%,25%
                    lv_line[6].c[0]=0;
                    lv_line[5].c[0]=0;
                    LireGValeur(HKEY_LOCAL_MACHINE,chemin2,"InstalledBy",lv_line[6].c);

                    if(RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp4)== ERROR_SUCCESS)
                    {
                      if(RegQueryInfoKey(CleTmp4,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
                        if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)
                          snprintf(lv_line[5].c,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                      RegCloseKey(CleTmp4);
                    }
                    snprintf(tmp_add,MAX_PATH,"%s,%s,%s",lv_line[2].c,lv_line[3].c,lv_line[4].c);
                    AddToLV_Registry2(lv_line[5].c, lv_line[6].c, "Update/Packages", tmp_add);
                    AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
                  }else
                  {
                    //un SP
                    if(RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp3)== ERROR_SUCCESS)
                    {
                      if (RegQueryInfoKey (CleTmp3,0,0,0,&nbSubKey3,0,0,0,0,0,0,0)==ERROR_SUCCESS)
                      {
                        for (k=0;k<nbSubKey3;k++)
                        {
                          TailleNomSubKey3=MAX_PATH;
                          lv_line[3].c[0]=0;
                          if (RegEnumKeyEx (CleTmp3,k,lv_line[3].c,&TailleNomSubKey3,0,0,0,0)==ERROR_SUCCESS)
                          {
                            snprintf(chemin3,MAX_PATH,"%s%s\\",chemin2,lv_line[3].c);

                            //lecture des infos
                            lv_line[6].c[0]=0;
                            lv_line[5].c[0]=0;
                            lv_line[4].c[0]=0;
                            LireGValeur(HKEY_LOCAL_MACHINE,chemin2,"PackageName",lv_line[4].c);
                            LireGValeur(HKEY_LOCAL_MACHINE,chemin2,"InstalledBy",lv_line[6].c);

                            if(RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp4)== ERROR_SUCCESS)
                            {
                              if(RegQueryInfoKey(CleTmp4,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
                                if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)
                                  snprintf(lv_line[5].c,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                              RegCloseKey(CleTmp4);
                            }
                            snprintf(tmp_add,MAX_PATH,"%s,%s,%s",lv_line[2].c,lv_line[3].c,lv_line[4].c);
                            AddToLV_Registry2(lv_line[5].c, lv_line[6].c, "Update/Packages", tmp_add);
                            AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
                          }
                        }
                      }
                      RegCloseKey(CleTmp3);
                    }
                  }
                }
              }
            }
            RegCloseKey(CleTmp2);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//liste des services et drivers
void registry_services(HANDLE hlv)
{
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  HKEY CleTmp=0,CleTmp2;
  DWORD nbSubKey,i,TailleNomSubKey, val;
  char NomSubKey[MAX_PATH]="", chemin2[MAX_PATH], tmp_add[MAX_PATH];
  FILETIME DernierMAJ;// dernière mise a jour ou creation de la cle
  SYSTEMTIME SysTime;

  // on ouvre la cle
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\",&CleTmp)!=ERROR_SUCCESS)return;

  // on recupere le nombre de subkey ( de cle en dessous de la cle actuelle)
  if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)!=ERROR_SUCCESS)
  {
     RegCloseKey(CleTmp);
     return;
  }

  lv_line[0].c[0]=0;
  for (i=0;i<nbSubKey;i++)
  {
    TailleNomSubKey=MAX_PATH;
    if (RegEnumKeyEx (CleTmp,i,NomSubKey,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
    {
      strcpy(chemin2,"SYSTEM\\CurrentControlSet\\Services\\");
      strncat(chemin2,NomSubKey,MAX_PATH);
      strncat(chemin2,"\\\0",MAX_PATH);
      snprintf(lv_line[1].c,MAX_PATH,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s",NomSubKey);

      //Date de mise à jour
      //test et récupération de la date de dernière modif
      if (RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp2)==ERROR_SUCCESS)
      {
        if(RegQueryInfoKey(CleTmp2,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
        {
          if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
            snprintf(lv_line[7].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
        }
        RegCloseKey(CleTmp2);
      }

      //type
      if(LireDValeur(HKEY_LOCAL_MACHINE,chemin2,"Type",&val))
      {
        if (val == 1)strcpy(lv_line[6].c,"DRIVER");
        else strcpy(lv_line[6].c,"SERVICE");
      }else lv_line[6].c[0]=0;

      //name
      lv_line[2].c[0]=0;
      LireValeur(HKEY_LOCAL_MACHINE,chemin2,"DisplayName",lv_line[2].c,MAX_LINE_SIZE);

      //état
      lv_line[3].c[0]=0;
      LireDValeur(HKEY_LOCAL_MACHINE,chemin2,"Start",&val);
      switch(val)
      {                    //val3
        case 0:strcpy(lv_line[3].c,"Kernel module");break;
        case 1:strcpy(lv_line[3].c,"Starting by system");break;
        case 2:strcpy(lv_line[3].c,"Automatic start");break;
        case 3:strcpy(lv_line[3].c,"Manual start");break;
        case 4:strcpy(lv_line[3].c,"Disable");break;
        default : snprintf(lv_line[3].c,MAX_PATH,"%02X (unknow)",(unsigned int)(val&255));break;
      };

      //Path
      lv_line[4].c[0]=0;
      LireGValeur(HKEY_LOCAL_MACHINE,chemin2,"ImagePath",lv_line[4].c);

      //Description
      lv_line[5].c[0]=0;
      LireGValeur(HKEY_LOCAL_MACHINE,chemin2,"Description",lv_line[5].c);

      snprintf(tmp_add,MAX_PATH,"%s,%s,%s",lv_line[6].c,lv_line[3].c,lv_line[1].c);
      AddToLV_Registry2(lv_line[7].c, "", "Service/Driver", tmp_add);
      AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
    }
  }
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// decoder une chaine de caractères de ROD13 en lisible
//---------------------------------------------------------------------------------------------------------------
void ROTTOASCII(char *donnee,char *resultat,int taille)
{
 int i;

 resultat[0]=0;

 for (i=0;i<taille;i++)
 {
    switch(donnee[i])
    {
     case 'a': strcat(resultat,"n"); break;
     case 'b': strcat(resultat,"o"); break;
     case 'c': strcat(resultat,"p"); break;
     case 'd': strcat(resultat,"q"); break;
     case 'e': strcat(resultat,"r"); break;
     case 'f': strcat(resultat,"s"); break;
     case 'g': strcat(resultat,"t"); break;
     case 'h': strcat(resultat,"u"); break;
     case 'i': strcat(resultat,"v"); break;
     case 'j': strcat(resultat,"w"); break;
     case 'k': strcat(resultat,"x"); break;
     case 'l': strcat(resultat,"y"); break;
     case 'm': strcat(resultat,"z"); break;
     case 'n': strcat(resultat,"a"); break;
     case 'o': strcat(resultat,"b"); break;
     case 'p': strcat(resultat,"c"); break;
     case 'q': strcat(resultat,"d"); break;
     case 'r': strcat(resultat,"e"); break;
     case 's': strcat(resultat,"f"); break;
     case 't': strcat(resultat,"g"); break;
     case 'u': strcat(resultat,"h"); break;
     case 'v': strcat(resultat,"i"); break;
     case 'w': strcat(resultat,"j"); break;
     case 'x': strcat(resultat,"k"); break;
     case 'y': strcat(resultat,"l"); break;
     case 'z': strcat(resultat,"m"); break;
     case 'A': strcat(resultat,"N"); break;
     case 'B': strcat(resultat,"O"); break;
     case 'C': strcat(resultat,"P"); break;
     case 'D': strcat(resultat,"Q"); break;
     case 'E': strcat(resultat,"R"); break;
     case 'F': strcat(resultat,"S"); break;
     case 'G': strcat(resultat,"T"); break;
     case 'H': strcat(resultat,"U"); break;
     case 'I': strcat(resultat,"V"); break;
     case 'J': strcat(resultat,"W"); break;
     case 'K': strcat(resultat,"X"); break;
     case 'L': strcat(resultat,"Y"); break;
     case 'M': strcat(resultat,"Z"); break;
     case 'N': strcat(resultat,"A"); break;
     case 'O': strcat(resultat,"B"); break;
     case 'P': strcat(resultat,"C"); break;
     case 'Q': strcat(resultat,"D"); break;
     case 'R': strcat(resultat,"E"); break;
     case 'S': strcat(resultat,"F"); break;
     case 'T': strcat(resultat,"G"); break;
     case 'U': strcat(resultat,"H"); break;
     case 'V': strcat(resultat,"I"); break;
     case 'W': strcat(resultat,"J"); break;
     case 'X': strcat(resultat,"K"); break;
     case 'Y': strcat(resultat,"L"); break;
     case 'Z': strcat(resultat,"M"); break;
     case '\\': strcat(resultat,"\\"); break;
     case ' ': strcat(resultat," "); break;
     case ':': strcat(resultat,":"); break;
     case '+': strcat(resultat,"+"); break;
     case '-': strcat(resultat,"-"); break;
     case '_': strcat(resultat,"_"); break;
     case '.': strcat(resultat,"."); break;
     case ']': strcat(resultat,"]"); break;
     case '[': strcat(resultat,"["); break;
     case '(': strcat(resultat,"("); break;
     case ')': strcat(resultat,")"); break;
     case '#': strcat(resultat,"#"); break;
     case '0': strcat(resultat,"0"); break;
     case '1': strcat(resultat,"1"); break;
     case '2': strcat(resultat,"2"); break;
     case '3': strcat(resultat,"3"); break;
     case '4': strcat(resultat,"4"); break;
     case '5': strcat(resultat,"5"); break;
     case '6': strcat(resultat,"6"); break;
     case '7': strcat(resultat,"7"); break;
     case '8': strcat(resultat,"8"); break;
     case '9': strcat(resultat,"9"); break;
     case 'é': strcat(resultat,"é"); break;
     case 'à': strcat(resultat,"à"); break;
     case 'è': strcat(resultat,"è"); break;
     case '~': strcat(resultat,"~"); break;
     case '&': strcat(resultat,"&"); break;
     case '"': strcat(resultat,"\""); break;
     case '|': strcat(resultat,"|"); break;
     case 'ç': strcat(resultat,"ç"); break;
     case '@': strcat(resultat,"@"); break;
     case '$': strcat(resultat,"$"); break;
     case '{': strcat(resultat,"{"); break;
     case '}': strcat(resultat,"}"); break;
     case '<': strcat(resultat,"<"); break;
     case '>': strcat(resultat,">"); break;
     case '°': strcat(resultat,"°"); break;
     case '%': strcat(resultat,"%"); break;
     case 'ù': strcat(resultat,"ù"); break;
     case '*': strcat(resultat,"*"); break;
     case 'µ': strcat(resultat,"µ"); break;
     case '§': strcat(resultat,"§"); break;
     case '^': strcat(resultat,"^"); break;
     case 'â': strcat(resultat,"â"); break;
     case 'ê': strcat(resultat,"ê"); break;
     case 'î': strcat(resultat,"î"); break;
     case 'ë': strcat(resultat,"ë"); break;
     case 'ï': strcat(resultat,"ï"); break;
     case '²': strcat(resultat,"²"); break;
     case '\'': strcat(resultat,"'"); break;
     case '\0': strcat(resultat,"\0");
     return;
     };
  };
}
//---------------------------------------------------------------------------------------------------------------
// on donne une valeur hexa de 8 caracteres et on obtient le resultat
// gestion des chiffres + caractères minuscules et majuscules
//---------------------------------------------------------------------------------------------------------------
DWORD HexaToDec(char *hexa,unsigned int taille)
{
 DWORD k=0;//resultat
 DWORD j=1,i; // j =coef multiplicateur et i variable de boucle
    //traitement de la valeur hexa

    for (i=(taille-1);i>0;i--)//calcul de la valeur hexa en decimal
    {
        switch(hexa[i])
        {
         case '1':k=k+j;break;
         case '2':k=k+(2*j);break;
         case '3':k=k+(3*j);break;
         case '4':k=k+(4*j);break;
         case '5':k=k+(5*j);break;
         case '6':k=k+(6*j);break;
         case '7':k=k+(7*j);break;
         case '8':k=k+(8*j);break;
         case '9':k=k+(9*j);break;
         case 'a':k=k+(10*j);break;
         case 'A':k=k+(10*j);break;
         case 'b':k=k+(11*j);break;
         case 'B':k=k+(11*j);break;
         case 'c':k=k+(12*j);break;
         case 'C':k=k+(12*j);break;
         case 'd':k=k+(13*j);break;
         case 'D':k=k+(13*j);break;
         case 'e':k=k+(14*j);break;
         case 'E':k=k+(14*j);break;
         case 'f':k=k+(15*j);break;
         case 'F':k=k+(15*j);break;
        };
        j=j*16;
    };
  return k;
};
//---------------------------------------------------------------------------------------------------------------
// lecture de données binaires dans la base de registre, transformés suivant le besoin et écrite dans le fichier
// & mise en forme dans le fichier
// [DATE](ID SESSION - NB USE) CHAINE
//---------------------------------------------------------------------------------------------------------------
void LireValBin(HKEY ENTETE,char *chemin,char *nom,char *donnes,char *user, HANDLE hlv)
{
    //variables
    DWORD tailleCle=MAX_PATH;
    HKEY CleTmp=NULL;
    char Valeur[MAX_PATH];//16=taille max des données
    //char Id[8];///récupération en hexa
    char Date1[8];//récupération en hexa
    char Date2[8];//récupération en hexa

    //convertion de la date
    FILETIME FileTime, LocalFileTime;
    SYSTEMTIME SysTime;

   //lecture de la valeur binaire
   // on ouvre la cle
   if (RegOpenKey(ENTETE,chemin, &CleTmp)==ERROR_SUCCESS)
   {
     LINE_ITEM lv_line[SIZE_UTIL_ITEM];
     lv_line[0].c[0]=0;
     lv_line[3].c[0]=0;
     lv_line[4].c[0]=0;

     //user
     strcpy(lv_line[5].c,user);

     //chemin
     strncpy(lv_line[2].c,donnes,MAX_LINE_SIZE);

     //KEY
     if (ENTETE == HKEY_USERS)snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_USERS\\%s",chemin);
     else if (ENTETE == HKEY_CURRENT_USER)snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_CURRENT_USER\\%s",chemin);
     else if (ENTETE == HKEY_LOCAL_MACHINE)snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_LOCAL_MACHINE\\%s",chemin);
     else snprintf(lv_line[1].c,MAX_LINE_SIZE,"0x%02X\\%s",(unsigned int) ENTETE,chemin);

     //On recupere la taille de la cle qui va être lue
     Valeur[0]=0;
     //On lit la valeur
     if (RegQueryValueEx(CleTmp, nom, NULL, NULL, (LPBYTE)Valeur, &tailleCle)==ERROR_SUCCESS)
     {
       //décortication
       if (tailleCle==8 )//si =8 alors il n'y a que le nombre d'utilisation
       {
         sprintf(lv_line[3].c,"0x%02x%02x%02x%02x",Valeur[7]&255,Valeur[6]&255,Valeur[5]&255,Valeur[4]&255);
       }else if (tailleCle==16) //si = 16 = 4 nb utilisation, 4 id de session, 8 date utilisation
       {
         //lecture du nombre d'exécution
         //transformation ASCII to HEXA et HEXA to DEC
         sprintf(lv_line[3].c,"0x%02x%02x%02x%02x",Valeur[7]&255,Valeur[6]&255,Valeur[5]&255,Valeur[4]&255);

         //lecture 1er partie de la date
         sprintf(Date1,"%02x%02x%02x%02x",Valeur[11]&255,Valeur[10]&255,Valeur[9]&255,Valeur[8]&255);

         //lecture 2ème partie de la date
         sprintf(Date2,"%02x%02x%02x%02x",Valeur[15]&255,Valeur[14]&255,Valeur[13]&255,Valeur[12]&255);

         FileTime.dwLowDateTime = (DWORD)HexaToDec(Date1,8);
         FileTime.dwHighDateTime = (DWORD)HexaToDec(Date2,8);

         //dans le cas ou ==0 non utilisable et pas de modification sur  le nombre d'utilisation
         if (( FileTime.dwLowDateTime!=0)||(FileTime.dwHighDateTime!=0))
         {
           if(FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
           {
             if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
              sprintf(lv_line[4].c,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
             else lv_line[4].c[0]=0;
           }else lv_line[4].c[0]=0;
         }
       }else if (tailleCle>=68) // Sous Seven/2008/Vista
       {
         //lecture du nombre d'exécution
         sprintf(lv_line[3].c,"0x%02x%02x%02x%02x",Valeur[1]&255,Valeur[2]&255,Valeur[3]&255,Valeur[4]&255);

         //lecture 1er partie de la date
         sprintf(Date1,"%02x%02x%02x%02x",Valeur[63]&255,Valeur[62]&255,Valeur[61]&255,Valeur[60]&255);

         //lecture 2ème partie de la date
         sprintf(Date2,"%02x%02x%02x%02x",Valeur[67]&255,Valeur[66]&255,Valeur[65]&255,Valeur[64]&255);

         FileTime.dwLowDateTime = (DWORD)HexaToDec(Date1,8);
         FileTime.dwHighDateTime = (DWORD)HexaToDec(Date2,8);

         //dans le cas ou ==0 non utilisable et pas de modification sur  le nombre d'utilisation
         if (( FileTime.dwLowDateTime!=0)||(FileTime.dwHighDateTime!=0))
         {
           if(FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
           {
             if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
               sprintf(lv_line[4].c,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
             else lv_line[4].c[0]=0;
           }else lv_line[4].c[0]=0;
         }
       }

       AddToLV_Registry2(lv_line[4].c, "", "UserAssist", lv_line[2].c);
       AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL] );

       //fermeture de la clé
       RegCloseKey(CleTmp);
       return;
     }
     //fermeture de la clé
     RegCloseKey(CleTmp);
   }//si on arrive pas a récupérer les données
    //formatage et écriture dans le fichier
}
//------------------------------------------------------------------------------
//historique caché des applications exécutés par utilisateur +  infos
void resgistry_userassist(HANDLE hlv)
{
  HKEY CleTmp=0,CleTmp2=0,CleTmp3=0;
  DWORD nbSubKey2=0,nbSubKey3=0,j,k,l;
  int TailleNomSubKey2,TailleNomSubKey3;
  DWORD nbValue;
  DWORD TailleNomValue=MAX_PATH,TailleMAXValue=MAX_PATH;
  char NomSubKey2[MAX_PATH],NomSubKey3[MAX_PATH],chemintmp[MAX_PATH],chemin[MAX_PATH],tmp[MAX_PATH];
  char NomValeur[MAX_PATH],Valeur[MAX_PATH],user[MAX_PATH],resultat[MAX_PATH];

  BOOL ROT13ENABLE,JOURNAUXENABLE;

  //racine du HKEY_USERS
  if (RegOpenKey((HKEY)HKEY_USERS,"",&CleTmp2)==ERROR_SUCCESS)
  {
    //nombre de sous clés
    if (RegQueryInfoKey (CleTmp2,NULL,NULL,NULL,&nbSubKey2,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
    {
      //on traite chacune des clé une par une
      for (k=0;k<nbSubKey2;k++)
      {
        TailleNomSubKey2 = MAX_PATH;
        if (RegEnumKeyEx (CleTmp2,k,NomSubKey2,(LPDWORD)&TailleNomSubKey2,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
        {
          //création du chemin de récupération
          snprintf(chemintmp,MAX_PATH,"%s\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\",NomSubKey2);

          //lecture du nom d'utilisateur :
          user[0] = 0;
          snprintf(tmp,MAX_PATH,"%s\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\",NomSubKey2);
          if (LireValeur(HKEY_USERS,tmp,"Logon User Name",user,MAX_PATH)) strcat(user," SID :");
          else strcpy(user,"SID :");

          strcat(user,NomSubKey2);
          strcat(user,"\0");

          if (RegOpenKey((HKEY)HKEY_USERS,chemintmp,&CleTmp3)==ERROR_SUCCESS)
          {
            //nombre de sous clé
            if ((RegQueryInfoKey (CleTmp3,NULL,NULL,NULL,&nbSubKey3,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==ERROR_SUCCESS))
            if (nbSubKey3>0)
            {
              //codage activé ?
              tmp[0] = 0;
              ROT13ENABLE = TRUE;
              if (LireGValeur(HKEY_USERS,chemintmp,"NoEncrypt",tmp))
              {
                if (tmp[0] == '1')ROT13ENABLE = FALSE;
              }

              //journalisation activé ?
              tmp[0] = 0;
              JOURNAUXENABLE = TRUE;
              if (LireGValeur(HKEY_USERS,chemintmp,"NoLog",tmp))
              {
                if (tmp[0] == '1')JOURNAUXENABLE = FALSE;
              }

              for (l=0;l<nbSubKey3;l++)
              {
                TailleNomSubKey3=MAX_PATH;
                if (RegEnumKeyEx (CleTmp3,l,NomSubKey3,(LPDWORD)&TailleNomSubKey3,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)
                {
                  sprintf(chemin,"%s%s\\Count\\",chemintmp,NomSubKey3);
                  //application et liens exécutés
                  if (RegOpenKey((HKEY)HKEY_USERS,chemin,&CleTmp)==ERROR_SUCCESS)
                  {
                    if (RegQueryInfoKey (CleTmp,NULL,NULL,NULL,NULL,NULL,NULL,&nbValue,&TailleNomValue,&TailleMAXValue,NULL,NULL)==ERROR_SUCCESS)
                    {
                      for (j=0;j<nbValue;j++)
                      {
                        TailleNomValue=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
                        TailleMAXValue=MAX_PATH;

                        //énumération de la valeur (j)
                        if (RegEnumValue (CleTmp,j,NomValeur,&TailleNomValue,NULL,NULL,(LPBYTE)Valeur,&TailleMAXValue)==ERROR_SUCCESS)
                        {
                          if (ROT13ENABLE)//si le chiffrement en ROD13 est activé
                          {
                             ROTTOASCII(NomValeur,resultat,strlen(NomValeur));
                             LireValBin(HKEY_USERS,chemin,NomValeur,resultat,user,hlv);
                          }
                          else // si le chiffrement n'est pas activé
                          {
                              LireValBin(HKEY_USERS,chemin,NomValeur,&NomValeur[13],user,hlv);
                          }
                        }
                      }
                    }
                    RegCloseKey(CleTmp);
                  }
                }
              }
            }
            RegCloseKey(CleTmp3);
          }
        }
      }
    }
    RegCloseKey(CleTmp2);
  }
}
//------------------------------------------------------------------------------
//historique USB
void registry_usb(HANDLE hlv)
{
  HKEY CleTmp=0,CleTmp2=0,CleTmp3=0;
  DWORD nbSubKey, TailleNomSubKey,TailleNomSubKey2;
  char NomSubKey[MAX_PATH], NomSubKey2[MAX_PATH], chemin2[MAX_PATH];
  char val[MAX_PATH],val2[MAX_PATH],val3[MAX_PATH],tmp_add[MAX_PATH];
  int i;
  FILETIME DernierMAJ;// dernière mise a jour ou creation de la cle
  SYSTEMTIME SysTime;

  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",&CleTmp)==ERROR_SUCCESS)
  {
    // on recupere le nombre de subkey ( de cle en dessous de la cle actuelle)
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      LINE_ITEM lv_line[SIZE_UTIL_ITEM];
      lv_line[0].c[0]=0;

      for (i=0;i<nbSubKey;i++)
      {
        TailleNomSubKey=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
        if (RegEnumKeyEx (CleTmp,i,NomSubKey,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
        {
          // on cree la trame pour recupere les donnees
          strncpy(chemin2,"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",MAX_PATH);
          strncat(chemin2,NomSubKey,MAX_PATH);
          strncat(chemin2,"\\\0",MAX_PATH);
          if (RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp2)==ERROR_SUCCESS)
          {
            // init de la taille
            TailleNomSubKey2=MAX_PATH;
            // on recupere le nom de la sous cle
            if (RegEnumKeyEx (CleTmp2,0,NomSubKey2,&TailleNomSubKey2,0,0,0,0)==ERROR_SUCCESS)
            {
              strcpy(val3,NomSubKey2);
              if (strlen(val3)>3)
              {
                if (val3[strlen(val3)-2] == '&')
                  val3[strlen(val3)-2]=0; //suppression du &0 de fin d'ID
              }

              // on cree la trame pour recupere les donnees
              strncpy(&chemin2[38+TailleNomSubKey+1],NomSubKey2,MAX_PATH);
              strncpy(&chemin2[38+TailleNomSubKey+1+TailleNomSubKey2],"\\",MAX_PATH);

              //récupération du type
              if (LireValeur(HKEY_LOCAL_MACHINE,chemin2,"Class",val,MAX_PATH))
              {
                   val2[0]=0;
                   LireValeur(HKEY_LOCAL_MACHINE,chemin2,"Mfg",val2,MAX_PATH);

                   //test et récupération de la date de dernière modif
                   lv_line[4].c[0]=0;
                   if (RegOpenKey(HKEY_LOCAL_MACHINE,chemin2,&CleTmp3)==ERROR_SUCCESS)
                   {
                      if(RegQueryInfoKey(CleTmp3,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
                      {
                        if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
                          snprintf(lv_line[4].c,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                      }
                      RegCloseKey(CleTmp3);
                   }

                   //Nom
                   lv_line[2].c[0]=0;
                   LireValeur(HKEY_LOCAL_MACHINE,chemin2,"FriendlyName",lv_line[2].c,MAX_PATH);

                   //Description
                   snprintf(lv_line[3].c,MAX_LINE_SIZE,"%s %s (%s)",val,val2,val3);

                   //KEY
                   snprintf(lv_line[1].c,MAX_LINE_SIZE,"HKEY_LOCAL_MACHINE\\%s",chemin2);

                   snprintf(tmp_add,MAX_PATH,"%s,%s",lv_line[2].c,lv_line[3].c);
                   AddToLV_Registry2(lv_line[4].c, "", "USB", tmp_add);

                   AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
              }
            }
            RegCloseKey(CleTmp2);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//lister toutes les valeurs et leurs données sous une clée
void reg_liste_DataValeur(HKEY hkey,char *chkey,char *path,HANDLE hlv)
{
  HKEY CleTmp=0;
  DWORD nbValue,i,TailleNomValue,TailleMAXValue;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValue,0,0,0,0)==ERROR_SUCCESS)
    {
      if (nbValue>0)
      {
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];
        lv_line[0].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);

        //lecture des valeurs et données
        for (i=0;i<nbValue;i++)
        {
          TailleNomValue = MAX_LINE_SIZE;
          TailleMAXValue = MAX_LINE_SIZE;
          if (RegEnumValue (CleTmp,i,lv_line[2].c,&TailleNomValue,0,0,(LPBYTE)lv_line[3].c,&TailleMAXValue)==ERROR_SUCCESS)
          {
            if (strlen(lv_line[2].c)>0 || strlen(lv_line[3].c)>0)
              AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_START_NB_COL]);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//lister toutes les valeurs et leurs données sous une clée en ajoutant une description
// et en enlevant les valeur du nom de exclu
void reg_liste_DataValeurSpec(HKEY hkey,char *chkey,char *path,char *exclu,char* description,HANDLE hlv)
{
  HKEY CleTmp=0;
  DWORD nbValue,i,TailleNomValue,TailleMAXValue;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValue,0,0,0,0)==ERROR_SUCCESS)
    {
      if (nbValue>0)
      {
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];
        lv_line[0].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);
        strcpy(lv_line[4].c,description);

        //lecture des valeurs et données
        for (i=0;i<nbValue;i++)
        {
          TailleNomValue = MAX_LINE_SIZE;
          TailleMAXValue = MAX_LINE_SIZE;
          if (RegEnumValue (CleTmp,i,lv_line[2].c,&TailleNomValue,0,0,(LPBYTE)lv_line[3].c,&TailleMAXValue)==ERROR_SUCCESS)
          {
            if ((strlen(lv_line[2].c)>0 || strlen(lv_line[3].c)>0) && strcmp(lv_line[2].c,exclu)!=0)
              AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//lister toutes les valeurs et leurs données sous une clée en ajoutant une description
// et en enlevant les valeur du nom de exclu avec gestion des répertoires fils
void reg_liste_DataValeurSpecR(HKEY hkey,char *chkey,char *path,char *exclu,char* description,HANDLE hlv)
{
  HKEY CleTmp=0;
  DWORD nbSubKey=0,nbValue=0,i,TailleNomValue,TailleMAXValue,TailleNomSubKey;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,&nbValue,0,0,0,0)==ERROR_SUCCESS)
    {
      //les clées directes
      if (nbValue>0)
      {
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];
        lv_line[0].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);
        strcpy(lv_line[4].c,description);

        //lecture des valeurs et données
        for (i=0;i<nbValue;i++)
        {
          TailleNomValue = MAX_LINE_SIZE;
          TailleMAXValue = MAX_LINE_SIZE;
          if (RegEnumValue (CleTmp,i,lv_line[2].c,&TailleNomValue,0,0,(LPBYTE)lv_line[3].c,&TailleMAXValue)==ERROR_SUCCESS)
          {
            if ((strlen(lv_line[2].c)>0 || strlen(lv_line[3].c)>0) && strcmp(lv_line[2].c,exclu)!=0)
              AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
          }
        }
      }
      //les sous clés
      if (nbSubKey>0)
      {
          char tmp[MAX_PATH],path_t[MAX_PATH];
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey = MAX_PATH;
            if (RegEnumKeyEx (CleTmp,i,tmp,(LPDWORD)&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              //création du path
              snprintf(path_t,MAX_PATH,"%s%s\\",path,tmp);
              reg_liste_DataValeurSpec(hkey,chkey,path_t,exclu,description,hlv);
            }
          }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//lister toutes les valeurs et leurs données sous une clée en ajoutant une description
// et en enlevant les valeur du nom de exclu avec des datas en wildstring
void reg_liste_DataValeurSpecW(HKEY hkey,char *chkey,char *path,char *exclu,char* description,HANDLE hlv)
{
  HKEY CleTmp=0;
  DWORD nbValue,i,TailleNomValue,TailleMAXValue;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValue,0,0,0,0)==ERROR_SUCCESS)
    {
      if (nbValue>0)
      {
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];
        lv_line[0].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);
        strcpy(lv_line[4].c,description);

        char tmp[MAX_LINE_SIZE];

        //lecture des valeurs et données
        for (i=0;i<nbValue;i++)
        {
          TailleNomValue = MAX_LINE_SIZE;
          TailleMAXValue = MAX_LINE_SIZE;
          if (RegEnumValue (CleTmp,i,lv_line[2].c,&TailleNomValue,0,0,(LPBYTE)tmp,&TailleMAXValue)==ERROR_SUCCESS)
          {
            if ((strlen(lv_line[2].c)>0 || strlen(tmp)>0) && strcmp(lv_line[2].c,exclu)!=0)
            {
              //2 chaines en wildstring
              sprintf(lv_line[3].c,"%S",tmp);
              AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//lister toutes les valeurs et leurs données sous une clée en ajoutant une description
// et en enlevant les valeur du nom de exclu avec 2 chaines dans les datas en wildstring
void reg_liste_DataValeurSpecWs(HKEY hkey,char *chkey,char *path,char *exclu,char* description,HANDLE hlv)
{
  HKEY CleTmp=0;
  DWORD nbValue,i,TailleNomValue,TailleMAXValue;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValue,0,0,0,0)==ERROR_SUCCESS)
    {
      if (nbValue>0)
      {
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];
        lv_line[0].c[0]=0;
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%s\\%s",chkey,path);
        strcpy(lv_line[4].c,description);

        char tmp[MAX_LINE_SIZE],tmp2[MAX_LINE_SIZE];

        //lecture des valeurs et données
        for (i=0;i<nbValue;i++)
        {
          TailleNomValue = MAX_LINE_SIZE;
          TailleMAXValue = MAX_LINE_SIZE;
          if (RegEnumValue (CleTmp,i,lv_line[2].c,&TailleNomValue,0,0,(LPBYTE)tmp,&TailleMAXValue)==ERROR_SUCCESS)
          {
            if ((strlen(lv_line[2].c)>0 || strlen(tmp)>0) && strcmp(lv_line[2].c,exclu)!=0)
            {
              //2 chaines en wildstring
              sprintf(tmp2,"%S",tmp);
              sprintf(lv_line[3].c,"%s %S",tmp2,tmp+(strlen(tmp2)+1)*2);
              AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
void registry_start(HANDLE hlv)
{
  //lister les clés contenus dans :
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Startup\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Logon\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Shutdown\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Logoff\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Load\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\AppSetup\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Shell\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\Install\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\Install\\Software\\Microsoft\\Windows\\CurrentVersion\\Runonce\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\Install\\Software\\Microsoft\\Windows\\CurrentVersion\\RunonceEx\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Group Policy\\Scripts\\Startup\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Group Policy\\Scripts\\Shutdown\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\Shell\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows CE Services\\AutoStartOnConnect\\",hlv);
  reg_liste_DataValeur(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows CE Services\\AutoStartOnDisconnect\\",hlv);

  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Startup\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Logon\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Shutdown\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Policies\\Microsoft\\Windows\\System\\Scripts\\Logoff\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Load\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\AppSetup\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Shell\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\Install\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\Install\\Software\\Microsoft\\Windows\\CurrentVersion\\Runonce\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Terminal Server\\Install\\Software\\Microsoft\\Windows\\CurrentVersion\\RunonceEx\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Group Policy\\Scripts\\Startup\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Group Policy\\Scripts\\Shutdown\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\Shell\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows CE Services\\AutoStartOnConnect\\",hlv);
  reg_liste_DataValeur(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows CE Services\\AutoStartOnDisconnect\\",hlv);
}
//------------------------------------------------------------------------------
//lecture des infos SSID WIFI
void LireConfSSID(char *GUID,char *resultat,unsigned int size)
{
  char tmp[MAX_PATH],
  tmp1[MAX_PATH],
  tmp2[MAX_PATH]="ControlFlags: 0x",
  tmp3[MAX_PATH],
  tmp5[15]="Static#0000",
  cSSID[MAX_PATH]="",chiffrement[MAX_PATH]="";
  HKEY CleTmp;
  FILETIME DernierMAJ;
  SYSTEMTIME SysTime;

  //création du chemin
  snprintf(tmp,MAX_PATH,"SOFTWARE\\MICROSOFT\\WZCSVC\\Parameters\\interfaces\\%s",GUID);

  //si ControlFlags n'existe pas alors pas de clé
  tmp1[0]=0;
  if (LireValeur(HKEY_LOCAL_MACHINE,tmp,"ControlFlags",tmp1,MAX_PATH))
  {
    if (tmp1[0]!=0)
    {
      //ControlFlags  + LayoutVersion
      tmp3[0]=0;
      if (LireValeur(HKEY_LOCAL_MACHINE,tmp,"LayoutVersion",tmp3,MAX_PATH))
      {
        snprintf(tmp2,MAX_PATH,"ControlFlags: 0x%s LayoutVersion:0x%s",tmp1,tmp3);
      }else snprintf(tmp2,MAX_PATH,"ControlFlags:0x%s",tmp1);


      //gestion de la configuration en static
      //Static#0000
      tmp3[0]=0;
      char *c;
      if (LireGValeur(HKEY_LOCAL_MACHINE,tmp,tmp5,tmp1)>52)
      {
        //SSID
        c = tmp1+2;
        while (*c<32 && *c)c++;//le 2ème caractère représente le début du SSID
        strcpy(cSSID,c);

        // si on regarde les prochain octets et est égale a 4: WPA-TKIP  et 6: WPA-AES si 01: non chiffré , sinon WEP
        // si 5ème octet = 03 = PSK si 02 partagé?
        chiffrement[0]=0;
        switch (tmp1[52])
        {
          case 0:strcpy(chiffrement,"/WEP");break;
          case 1:strcpy(chiffrement,"/Open");break;
          case 4:tmp1[4]==3?strcpy(chiffrement,"/WPA-PSK:TKIP"):strcpy(chiffrement,"/WPA:TKIP");break;
          case 6:tmp1[4]==3?strcpy(chiffrement,"/WPA-PSK:AES"):strcpy(chiffrement,"/WPA:AES");break;
          default:
          if (tmp1[4] == 3)//PSK
            sprintf(chiffrement,"/WPA2-PSK ? : CODE: %d",tmp1[52]);
          else
            sprintf(chiffrement,"/WPA2 ? : CODE: %d",tmp1[52]);
          break;
        }
      }

      //date de dernière modification
      if (RegOpenKey(HKEY_LOCAL_MACHINE,tmp,&CleTmp)==ERROR_SUCCESS)
      {
          if(RegQueryInfoKey(CleTmp,0,0,0,0,0,0,0,0,0,0,&DernierMAJ)==ERROR_SUCCESS)
          {
            if (FileTimeToSystemTime(&DernierMAJ, &SysTime))
            //traitement de l'affichage de la date
              snprintf(tmp1,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
            else tmp1[0]=0;
          }
          RegCloseKey(CleTmp);
      }
      snprintf(resultat,size,"SSID:%s%s Last update:%s %s",cSSID,chiffrement,tmp1,tmp2);
    }
  }
}
//------------------------------------------------------------------------------
//configuration réseau dans la base
void registry_lan(HANDLE hlv)
{
  HKEY CleTmp=0;
  DWORD nbSubKey=0,i,TailleNomSubKey;
  char path[MAX_PATH],dhcp[MAX_PATH],tmp[MAX_PATH];
  BOOL bdhcp;

  //ouverture de la clé de registre
  //XP, 2003, 7, 2008, Vista
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\",&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      //énumération des sous clés
      lv_line[0].c[0]=0;
      lv_line[2].c[0]=0;
      for (i=0;i<nbSubKey;i++)
      {
        lv_line[7].c[0]=0;
        TailleNomSubKey=MAX_PATH;
        tmp[0]=0;
        if (RegEnumKeyEx (CleTmp,i,tmp,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(path,MAX_PATH,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s\\",tmp);
          snprintf(lv_line[1].c,MAX_PATH,"HKEY_LOCAL_MACHINE\\%s",path);

          //conf WIFI
          LireConfSSID(tmp,lv_line[7].c,MAX_LINE_SIZE);

          //DHCP
          bdhcp = FALSE;
          lv_line[6].c[0]=0;
          if (LireValeur(HKEY_LOCAL_MACHINE,path,"EnableDHCP",dhcp,MAX_PATH))
          {
             if (dhcp[7]==48) strcpy(lv_line[6].c,"NO");
             else
             {
               if (LireValeur(HKEY_LOCAL_MACHINE,path,"DhcpServer",dhcp,MAX_PATH))
               {
                 snprintf(lv_line[6].c,MAX_PATH,"YES (Server : %s)",dhcp);
                 bdhcp = TRUE;
               }else strcpy(lv_line[6].c,"NO");
             }
          }else strcpy(lv_line[6].c,"NO");

          if (bdhcp)
          {
            //IP
            lv_line[3].c[0]=0;
            if (LireValeur(HKEY_LOCAL_MACHINE,path,"DhcpIPAddress",lv_line[3].c,20))
            {
               //adresse IP + mask
               tmp[0]=0;
               if (LireValeur(HKEY_LOCAL_MACHINE,path,"DhcpSubnetMask",tmp,20))
               {
                strncat(lv_line[3].c,"/",MAX_PATH);
                strncat(lv_line[3].c,tmp,MAX_PATH);
                strncat(lv_line[3].c,"\0",MAX_PATH);
               }
            }

            //DNS
            lv_line[4].c[0]=0;
            LireValeur(HKEY_LOCAL_MACHINE,path,"DhcpNameServer",lv_line[4].c,MAX_PATH);

            //passerelle
            lv_line[5].c[0]=0;
            LireValeur(HKEY_LOCAL_MACHINE,path,"DhcpDefaultGateway",lv_line[5].c,MAX_PATH);

          }else
          {
            //IP
            lv_line[3].c[0]=0;
            if (LireValeur(HKEY_LOCAL_MACHINE,path,"IPAddress",lv_line[3].c,20))
            {
               //adresse IP + mask
               tmp[0]=0;
               if (LireValeur(HKEY_LOCAL_MACHINE,path,"SubnetMask",tmp,20))
               {
                strncat(lv_line[3].c,"/",MAX_PATH);
                strncat(lv_line[3].c,tmp,MAX_PATH);
                strncat(lv_line[3].c,"\0",MAX_PATH);
               }
            }

            //DNS
            lv_line[4].c[0]=0;
            LireValeur(HKEY_LOCAL_MACHINE,path,"NameServer",lv_line[4].c,MAX_PATH);

            //passerelle
            lv_line[5].c[0]=0;
            LireValeur(HKEY_LOCAL_MACHINE,path,"DefaultGateway",lv_line[5].c,MAX_PATH);
          }

          //ADD
          AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
        }
      }
    }
    RegCloseKey(CleTmp);
  }//NT4 : non pris en charge

  //WIFI
 /* nbSubKey = 0;
  CleTmp = 0;
  char id[MAX_PATH]=0;
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\MICROSOFT\\WZCSVC\\Parameters\\interfaces\\",&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbSubKey;i++)
      {
        TailleNomSubKey=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
        if (RegEnumKeyEx (CleTmp,i,id,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
        {
           LireConfSSID(id,hlv);
        }
      }
    }
    RegCloseKey(CleTmp);
  }*/
}
//------------------------------------------------------------------------------
//test si le SID existe déja dans la base
BOOL TestSIDShadowCopy(HANDLE hlv, char *sid)
{
  hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS);
  unsigned long int l, k = ListView_GetItemCount(hlv);
  char tmp[MAX_LINE_SIZE];
  for (l=0;l<k && sid[0]!=0;l++)
  {
    tmp[0]=0;
    ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
    if (!strcmp(tmp,sid))
    {
      ListView_SetItemText(hlv,l,9,"YES");
      return TRUE;
      break;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//liste des utilisateurs et leurs SID
void registry_users(HANDLE hlv)
{
  //si impossible d'accéder au registre on passe par les api locaux
  if (registry_users_extract()) return;

  //si ne fonctionne pas chargement par les API (sans hash de mot de passe)
  //chargement des fonction de DLL systèmes
  HMODULE hDLL;
  typedef DWORD (WINAPI *NETAPIBUFFERFREE)(LPVOID Buffer);
  NETAPIBUFFERFREE NetApiBufferFree;
  typedef DWORD (WINAPI *NETUSERENUM)(LPCWSTR servername, DWORD level, DWORD filter, LPBYTE* bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries, LPDWORD resume_handle);
  NETUSERENUM NetUserEnum;
  typedef DWORD (WINAPI *NETUSERGETINFO)( LPCWSTR servername, LPCWSTR username, DWORD level, LPBYTE* bufptr);
  NETUSERGETINFO NetUserGetInfo;
  typedef DWORD (WINAPI *NETUSERGETLOCALGROUPS)( LPCWSTR servername, LPCWSTR username, DWORD level, DWORD flags, LPBYTE* bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries);
  NETUSERGETLOCALGROUPS NetUserGetLocalGroups;


  typedef struct _USER_INFO_0 {
    LPWSTR usri0_name;
  }*LPUSER_INFO_0;

  typedef struct _USER_INFO_2 {
    LPWSTR usri2_name;
    LPWSTR usri2_password;
    DWORD  usri2_password_age;
    DWORD  usri2_priv;
    LPWSTR usri2_home_dir;
    LPWSTR usri2_comment;
    DWORD  usri2_flags;
    LPWSTR usri2_script_path;
    DWORD  usri2_auth_flags;
    LPWSTR usri2_full_name;
    LPWSTR usri2_usr_comment;
    LPWSTR usri2_parms;
    LPWSTR usri2_workstations;
    DWORD  usri2_last_logon;
    DWORD  usri2_last_logoff;
    DWORD  usri2_acct_expires;
    DWORD  usri2_max_storage;
    DWORD  usri2_units_per_week;
    PBYTE  usri2_logon_hours;
    DWORD  usri2_bad_pw_count;
    DWORD  usri2_num_logons;
    LPWSTR usri2_logon_server;
    DWORD  usri2_country_code;
    DWORD  usri2_code_page;
  } USER_INFO_2, *PUSER_INFO_2, *LPUSER_INFO_2;

  typedef struct _GROUP_USERS_INFO_0 {
    LPWSTR grui0_name;
  } GROUP_USERS_INFO_0, *PGROUP_USERS_INFO_0, *LPGROUP_USERS_INFO_0;

  if ((hDLL = LoadLibrary( "NETAPI32.dll"))!=NULL)
  {
    NetApiBufferFree = (NETAPIBUFFERFREE) GetProcAddress(hDLL,"NetApiBufferFree");
    NetUserEnum = (NETUSERENUM) GetProcAddress(hDLL,"NetUserEnum");
    NetUserGetInfo = (NETUSERGETINFO) GetProcAddress(hDLL,"NetUserGetInfo");
    NetUserGetLocalGroups = (NETUSERGETLOCALGROUPS) GetProcAddress(hDLL,"NetUserGetLocalGroups");

    if (NetApiBufferFree && NetUserEnum && NetUserGetInfo && NetUserGetLocalGroups)
    {
      DWORD nStatus;
      LPUSER_INFO_0 pBuf = 0, Buffer;
      DWORD nb = 0,total = 0;
      unsigned int i;
      USER_INFO_2 * pBuf_info=0;
      char group[MAX_PATH]="\0", tmp[MAX_PATH]="\0",tmp_add[MAX_PATH];

      BYTE BSid[MAX_PATH];
      PSID Sid = (PSID) BSid;
      DWORD TailleSid = MAX_PATH;
      char NomDeDomaine[MAX_PATH];
      DWORD TailleNomDeDomaine = MAX_PATH;
      SID_NAME_USE TypeSid;

      LPGROUP_USERS_INFO_0 LBuffer = 0,pTmpBuf;
      DWORD dwEntriesRead = 0,dwTotalEntries = 0,j=0;
      SYSTEMTIME SysTime;
      FILETIME DernierMAJ;
      LINE_ITEM lv_line[SIZE_UTIL_ITEM];

      //énumération des utilisateurs
      do{
        nStatus = NetUserEnum(0,0,2,(LPBYTE*)&pBuf,-1,&nb,&total,0);
        if ((nStatus == 0) || (nStatus == 234))
        {
          if ((Buffer = pBuf) != 0)
          {
            for (i = 0; i < nb; i++)
            {
              //information sur l'utilisateur
              if (Buffer->usri0_name!=0)
              {
                snprintf(lv_line[2].c,MAX_PATH,"%S",Buffer->usri0_name);
                lv_line[11].c[0]=0;
                lv_line[10].c[0]=0;
                lv_line[9].c[0]=0;
                lv_line[6].c[0]=0;
                lv_line[5].c[0]=0;
                lv_line[4].c[0]=0;
                lv_line[3].c[0]=0;
                lv_line[1].c[0]=0;
                lv_line[0].c[0]=0;

                //type de compte et droits
                if (NetUserGetInfo(0,Buffer->usri0_name,2,(LPBYTE *)&pBuf_info) == 0)
                {
                  switch (pBuf_info->usri2_priv)
                  {
                    case 0:snprintf(lv_line[4].c,MAX_LINE_SIZE,"Guest (%S %S %S)",pBuf_info->usri2_full_name,pBuf_info->usri2_usr_comment,pBuf_info->usri2_comment);break;
                    case 1:snprintf(lv_line[4].c,MAX_LINE_SIZE,"User (%S %S %S)",pBuf_info->usri2_full_name,pBuf_info->usri2_usr_comment,pBuf_info->usri2_comment);break;
                    case 2:snprintf(lv_line[4].c,MAX_LINE_SIZE,"Administrator (%S %S %S)",pBuf_info->usri2_full_name,pBuf_info->usri2_usr_comment,pBuf_info->usri2_comment);break;
                    default: snprintf(lv_line[4].c,MAX_LINE_SIZE,"(%S %S %S)",pBuf_info->usri2_full_name,pBuf_info->usri2_usr_comment,pBuf_info->usri2_comment);break;
                  };

                  //Enable
                  if (pBuf_info->usri2_flags&0x2)strcpy(lv_line[8].c,"Disable");
                  else strcpy(lv_line[8].c,"Enable");

                  if (pBuf_info->usri2_flags&0x10)strcat(lv_line[8].c," and lock\0");

                  #define TIMEQ_FOREVER ((ULONG)-1)
                  if (pBuf_info->usri2_acct_expires == TIMEQ_FOREVER)strcat(lv_line[8].c," and password never expire\0");
                  else
                  {
                    //conversion du ctime en FILETIME
                    LONGLONG ll = Int32x32To64(pBuf_info->usri2_last_logon, 10000000) + 116444736000000000;
                    DernierMAJ.dwLowDateTime = (DWORD) ll;
                    DernierMAJ.dwHighDateTime = ll >>32;

                    if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
                      snprintf(tmp,MAX_PATH,"and expire %02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                    strcat(lv_line[8].c,tmp);
                    strcat(lv_line[8].c,"\0");
                  }

                  //if (pBuf_info->usri2_acct_expires)strcat(lv_line[8].c," and expire account\0");

                  //last logon
                  if (pBuf_info->usri2_last_logon != 0)
                  {
                    //conversion du ctime en FILETIME
                    LONGLONG ll = Int32x32To64(pBuf_info->usri2_last_logon, 10000000) + 116444736000000000;
                    DernierMAJ.dwLowDateTime = (DWORD) ll;
                    DernierMAJ.dwHighDateTime = ll >>32;

                    if (FileTimeToSystemTime(&DernierMAJ, &SysTime) != 0)//traitement de l'affichage de la date
                      snprintf(lv_line[6].c,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  }

                  //nombre de connexion
                  snprintf(lv_line[7].c,MAX_LINE_SIZE,"%lu",pBuf_info->usri2_num_logons);

                  //SID :
                  if(LookupAccountName(NULL,(LPCTSTR)lv_line[2].c,Sid,(LPDWORD)&TailleSid,(LPTSTR)NomDeDomaine,(LPDWORD)&TailleNomDeDomaine,(PSID_NAME_USE)&TypeSid))
                  {
                    SidtoUser(Sid, group, tmp, MAX_PATH,MAX_PATH,TRUE);
                    strncpy(lv_line[3].c,tmp,MAX_PATH);

                    if (TestSIDShadowCopy(hlv, tmp))
                      strcpy(lv_line[10].c,"YES");
                    //snprintf(lv_line[3].c,MAX_PATH,"%s %s",group,tmp);
                  }

                  //Groupes
                  if (NetUserGetLocalGroups(0,Buffer->usri0_name,0,1,(LPBYTE*)&LBuffer,-1,&dwEntriesRead,&dwTotalEntries) == 0)
                  {
                    if ((pTmpBuf = LBuffer) != 0)
                    {
                       for (j = 0; j < dwEntriesRead; j++)
                       {
                          if(pTmpBuf != 0)
                          {
                            snprintf(tmp,MAX_PATH," %S",pTmpBuf->grui0_name);
                            strncat(lv_line[5].c,tmp,MAX_PATH);
                          }
                          pTmpBuf++;
                       }
                    }
                  }
                }
                snprintf(tmp_add,MAX_PATH,"Last logon : %s,%s",lv_line[2].c,lv_line[4].c);
                AddToLV_Registry2(lv_line[6].c, lv_line[3].c, "Users & groups", tmp_add);
                AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
                Buffer++;
              }else break;
            }
          }
        }
      }while (nStatus == ERROR_MORE_DATA);
      if (pBuf != 0) NetApiBufferFree(pBuf);;
    }
    FreeLibrary( hDLL );
  }
}
//------------------------------------------------------------------------------
void vncpwd(unsigned char *pwd, int bytelen)
{     // if bytelen is 0 it's a hex string
  int     len,
          tmp;
  unsigned char  fixedkey[8] = { 23,82,107,6,35,78,88,7 },
          *p,
          *o;

  if(bytelen)
  {
      o = pwd + bytelen;
  } else {
      for(p = o = pwd; *p; p += 2, o++)
      {
          sscanf(p, "%02x", &tmp);
          *o = tmp;
      }
  }

  len = o - pwd;
  tmp = len % 8;
  len /= 8;
  if(tmp) len++;

  deskey(fixedkey, DE1);
  for(p = pwd; len--; p += 8) des(p, p);

  *o = 0;
}
//------------------------------------------------------------------------------
void registry_password(HANDLE hlv)
{
  //permet déchiffrement (codage XOR avec la clé de windows)
  const BYTE key[128] = {
  0x48, 0xEE, 0x76, 0x1D, 0x67, 0x69, 0xA1, 0x1B, 0x7A, 0x8C, 0x47, 0xF8, 0x54, 0x95, 0x97, 0x5F,
  0x78, 0xD9, 0xDA, 0x6C, 0x59, 0xD7, 0x6B, 0x35, 0xC5, 0x77, 0x85, 0x18, 0x2A, 0x0E, 0x52, 0xFF,
  0x00, 0xE3, 0x1B, 0x71, 0x8D, 0x34, 0x63, 0xEB, 0x91, 0xC3, 0x24, 0x0F, 0xB7, 0xC2, 0xF8, 0xE3,
  0xB6, 0x54, 0x4C, 0x35, 0x54, 0xE7, 0xC9, 0x49, 0x28, 0xA3, 0x85, 0x11, 0x0B, 0x2C, 0x68, 0xFB,
  0xEE, 0x7D, 0xF6, 0x6C, 0xE3, 0x9C, 0x2D, 0xE4, 0x72, 0xC3, 0xBB, 0x85, 0x1A, 0x12, 0x3C, 0x32,
  0xE3, 0x6B, 0x4F, 0x4D, 0xF4, 0xA9, 0x24, 0xC8, 0xFA, 0x78, 0xAD, 0x23, 0xA1, 0xE4, 0x6D, 0x9A,
  0x04, 0xCE, 0x2B, 0xC5, 0xB6, 0xC5, 0xEF, 0x93, 0x5C, 0xA8, 0x85, 0x2B, 0x41, 0x37, 0x72, 0xFA,
  0x57, 0x45, 0x41, 0xA1, 0x20, 0x4F, 0x80, 0xB3, 0xD5, 0x23, 0x02, 0x64, 0x3F, 0x6C, 0xF1, 0x0F};

  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  lv_line[0].c[0] = 0; //file

  //VNC :
  char tmp[MAX_PATH]="";

  if (!LireValeur(HKEY_CURRENT_USER,"SOFTWARE\\ORL\\WinVNC3\\","Password",tmp,MAX_PATH))
  {
    LireValeur(HKEY_LOCAL_MACHINE,"SOFTWARE\\ORL\\WinVNC3\\Default\\","Password",tmp,MAX_PATH);
    strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SOFTWARE\\ORL\\WinVNC3\\Default\\");
  }else strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\SOFTWARE\\ORL\\WinVNC3\\");

  if (strlen(tmp)>3)
  {
    sprintf(lv_line[3].c,"%02x%02x%02x%02x",tmp[0]&255,tmp[1]&255,tmp[2]&255,tmp[3]&255);
    //transformation du hash du mot de passe VNC en clair
    vncpwd((u_char *)tmp,8);

    strcpy(lv_line[2].c,"Password");
    strcpy(lv_line[4].c,"VNC");
    strcpy(lv_line[5].c,tmp);
    AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
  }

  //Screen saver
  tmp[0] = 0;
  if (!LireValeur(HKEY_CURRENT_USER,".DEFAULT\\CONTROL PANEL\\DESKTOP\\","ScreenSave_Data",tmp,MAX_PATH))
  {
    LireValeur(HKEY_USERS,".DEFAULT\\CONTROL PANEL\\DESKTOP\\","ScreenSave_Data",tmp,MAX_PATH);
    strcpy(lv_line[1].c,"HKEY_USERS\\.DEFAULT\\CONTROL PANEL\\DESKTOP\\");
  }else strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\.DEFAULT\\CONTROL PANEL\\DESKTOP\\");

  int i,j=0,k=0,taille = strlen(tmp);
  if (taille>0)
  {
    if (taille>256)taille=256;
    strcpy(lv_line[2].c,"ScreenSave_Data");
    strcpy(lv_line[3].c,tmp);
    strcpy(lv_line[4].c,"Screen saver");

    for (i=0; i<taille/2; i++)
    {
      k = 0;
      // transformer en entier
      j -= (j=tmp[i*2  ])>0x39 ? 0x37:0x30;
      k += j*0x10;
      j -= (j=tmp[i*2+1])>0x39 ? 0x37:0x30;
      k += j;
      // effectuer le décryptage
      k ^= key[i];
      tmp[i] = k;
    }
    tmp[taille/2] = 0;
    strcpy(lv_line[5].c,tmp);
    AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
  }

//terminal serveur
  tmp[0] = 0;
  if (LireValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\DefaultUserConfiguration\\","password",tmp,MAX_PATH))
    strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\DefaultUserConfiguration\\");
  else if (LireValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\Console\\","password",tmp,MAX_PATH))
    strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\Console\\");
  else if (LireValeur(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp\\","password",tmp,MAX_PATH))
    strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp\\");

  if (tmp[0]!=0 && strcmp("0022f600",tmp)!=0 && strcmp("00000000",tmp)!=0)//valeures par défaut
  {
    strcpy(lv_line[2].c,"password");
    strcpy(lv_line[3].c,tmp);
    strcpy(lv_line[4].c,"Terminal Server");
    strcpy(lv_line[5].c,tmp);
    AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
  }

//Session login
  tmp[0] = 0;
  char tmp1[MAX_PATH]="", tmp2[MAX_PATH]="";
  LireValeur(HKEY_LOCAL_MACHINE,"Software\\Windows NT\\CurrentVersion\\Winlogon\\","DefaultUserName",tmp,MAX_PATH);
  LireValeur(HKEY_LOCAL_MACHINE,"Software\\Windows NT\\CurrentVersion\\Winlogon\\","DefaultPassword",tmp1,MAX_PATH);
  LireValeur(HKEY_LOCAL_MACHINE,"Software\\Windows NT\\CurrentVersion\\Winlogon\\","DefaultDomainName",tmp2,MAX_PATH);

  if (tmp[0] != 0 || tmp1[0] != 0 || tmp2[0] != 0)
  {
    strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\Software\\Windows NT\\CurrentVersion\\Winlogon\\");
    strcpy(lv_line[2].c,"DefaultUserName/DefaultPassword/DefaultDomainName");
    sprintf(lv_line[3].c,"%s/%s/%s",tmp,tmp1,tmp2);
    strcpy(lv_line[4].c,"Windows auto-logon");
    strcpy(lv_line[5].c,tmp1);
    AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
  }
}
//------------------------------------------------------------------------------
void registry_mru(HANDLE hlv)
{
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU\\","MRUList","Command history",hlv);
  reg_liste_DataValeurSpecWs(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\LastVisitedMRU\\","MRUList","Open history",hlv);
  reg_liste_DataValeurSpecR(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSaveMRU\\","MRUList","Save history",hlv);
  reg_liste_DataValeurSpecW(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs\\","MRUListEx","Recent history",hlv);
  reg_liste_DataValeurSpecW(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StreamMRU\\","MRUListEx","Stream history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComputerDescriptions\\","","Connected network device history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Map Network Drive MRU\\","MRUList","Map network drive history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Search Assistant\\ACMru\\5001\\","","Internet search assistant history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Search Assistant\\ACMru\\5603\\","","Search assistant history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Search Assistant\\ACMru\\5604\\","","Search picture, music and video history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Search Assistant\\ACMru\\5647\\","","Search printers, computers and people history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Internet Explorer\\TypedURLs\\","","Internet Explorer URL history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\MediaPlayer\\Player\\RecentFileList\\","","Media player history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\MediaPlayer\\Player\\RecentURLList\\","","Media player history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\Paint\\Recent File List\\","","Paint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\Wordpad\\Recent File List\\","","Wordpad history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\9.0\\Publisher\\Recent File List\\","","Publisher history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\10.0\\Publisher\\Recent File List\\","","Publisher history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\11.0\\Publisher\\Recent File List\\","","Publisher history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\9.0\\PowerPoint\\Recent File List\\","","PowerPoint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\10.0\\PowerPoint\\Recent File List\\","","PowerPoint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\11.0\\PowerPoint\\Recent File List\\","","PowerPoint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\12.0\\PowerPoint\\Recent File List\\","","PowerPoint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\13.0\\PowerPoint\\Recent File List\\","","PowerPoint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\14.0\\PowerPoint\\Recent File List\\","","PowerPoint history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\9.0\\Excel\\Recent File List\\","","Excel history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\10.0\\Excel\\Recent File List\\","","Excel history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\11.0\\Excel\\Recent File List\\","","Excel history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\12.0\\Excel\\Recent File List\\","","Excel history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\13.0\\Excel\\Recent File List\\","","Excel history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\14.0\\Excel\\Recent File List\\","","Excel history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\9.0\\Word\\Data\\","Toolsbars","Word history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\10.0\\Word\\Data\\","Toolsbars","Word history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\11.0\\Word\\Data\\","Toolsbars","Word history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\12.0\\Word\\Data\\","Toolsbars","Word history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\13.0\\Word\\Data\\","Toolsbars","Word history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\14.0\\Word\\Data\\","Toolsbars","Word history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\9.0\\FrontPage Explorer\\Recent File List\\","","FrontPage Explorer history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\10.0\\FrontPage Explorer\\Recent File List\\","","FrontPage Explorer history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Microsoft\\Office\\11.0\\FrontPage Explorer\\Recent File List\\","","FrontPage Explorer history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\HexEditor\\1.x\\wnd_hex_Main\\Twnd_hex_Main.MRUMan\\","","MiTeC HexEditor history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\HexEditor\\2.x\\wnd_hex_Main\\Twnd_hex_Main.MRUMan\\","","MiTeC HexEditor history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\HexEditor\\3.x\\wnd_hex_Main\\Twnd_hex_Main.MRUMan\\","","MiTeC HexEditor history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\HexEditor\\4.x\\wnd_hex_Main\\Twnd_hex_Main.MRUMan\\","","MiTeC HexEditor history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\HexEditor\\5.x\\wnd_hex_Main\\Twnd_hex_Main.MRUMan\\","","MiTeC HexEditor history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\WRR\\1.x\\Main\\MRUHistory\\","MaxFiles","MiTeC WRR history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\MiTeC\\WRR\\2.x\\Main\\MRUHistory\\","MaxFiles","MiTeC WRR history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Foxit Software\\Foxit Reader 1.0\\Recent File List\\","","Foxit reader history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Foxit Software\\Foxit Reader 2.0\\Recent File List\\","","Foxit reader history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Foxit Software\\Foxit Reader 3.0\\Recent File List\\","","Foxit reader history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Foxit Software\\Foxit Reader 4.0\\Recent File List\\","","Foxit reader history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Foxit Software\\Foxit Reader 5.0\\Recent File List\\","","Foxit reader history",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Foxit Software\\Foxit Reader 6.0\\Recent File List\\","","Foxit reader history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","SOFTWARE\\Morpheus\\GUI\\SearchRecent\\","","P2P - Morpheus search history",hlv);

  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache\\","","MUICache",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache\\","","MUICache",hlv);
  reg_liste_DataValeurSpec(HKEY_CURRENT_USER,"HKEY_CURRENT_USER","Software\\Microsoft\\Windows\\Shell\\LocalizedResourceName\\","","MUICache",hlv);

  //historique : dernière clée de registre ouverte
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  lv_line[0].c[0]=0;
  lv_line[3].c[0]=0;
  LireGValeur(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit\\","LastKey",lv_line[3].c);
  strcpy(lv_line[1].c,"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit\\");
  strcpy(lv_line[2].c,"LastKey");
  strcpy(lv_line[4].c,"Last open registry key");
  AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
}
//------------------------------------------------------------------------------
void registry_path(HANDLE hlv)
{
  //liste des path de tous les utilisateurs
  //racine du HKEY_USERS
  HKEY CleTmp=0;
  DWORD i,nbSubKey=0, TailleNomSubKey;
  char path[MAX_PATH], NomSubKey[MAX_PATH];
  if (RegOpenKey((HKEY)HKEY_USERS,"",&CleTmp)!=ERROR_SUCCESS)return;

  //liste des sous clés ^^
  //nombre de sous clés
  if (RegQueryInfoKey (CleTmp,NULL,NULL,NULL,&nbSubKey,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
  {
    for (i=0;i<nbSubKey;i++)
    {
      TailleNomSubKey = MAX_PATH;
      if (RegEnumKeyEx (CleTmp,i,NomSubKey,(LPDWORD)&TailleNomSubKey,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
      {
        snprintf(path,MAX_PATH,"%s\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",NomSubKey);
        reg_liste_DataValeur(HKEY_USERS,"HKEY_USERS",path,hlv);
      }
    }
  }
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
void registry_syskey(HANDLE hlv, char*sk)
{
  HKEY CleTmp=0;
  char cJD[SZ_PART_SYSKEY]="", cSkew1[SZ_PART_SYSKEY]="", cGBG[SZ_PART_SYSKEY]="", cData[SZ_PART_SYSKEY]="";
  DWORD size;

  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Lsa\\JD\\",&CleTmp)==ERROR_SUCCESS)
  {
    size = SZ_PART_SYSKEY;
    if (RegQueryInfoKey(CleTmp, cJD, &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)return;
    RegCloseKey(CleTmp);
  }else return;
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Lsa\\Skew1\\",&CleTmp)==ERROR_SUCCESS)
  {
    size = SZ_PART_SYSKEY;
    if (RegQueryInfoKey(CleTmp, cSkew1, &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)return;
    RegCloseKey(CleTmp);
  }else return;
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Lsa\\GBG\\",&CleTmp)==ERROR_SUCCESS)
  {
    size = SZ_PART_SYSKEY;
    if (RegQueryInfoKey(CleTmp, cGBG, &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)return;
    RegCloseKey(CleTmp);
  }else return;
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Lsa\\Data\\",&CleTmp)==ERROR_SUCCESS)
  {
    size = SZ_PART_SYSKEY;
    if (RegQueryInfoKey(CleTmp, cData, &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)return;
    RegCloseKey(CleTmp);
  }else return;

  //traitement
  AddLvSyskey("",cJD, cSkew1, cGBG, cData,sk);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_registry(LPVOID lParam)
{
  //init
  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_REGISTRY,TRUE);
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_VIEW));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LOGICIEL));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_SERVICES));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_HISTORIQUE));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USB));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_START));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PASSWORD));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MRU));
  ListView_DeleteAllItems(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PATH));

  SendDlgItemMessage(Tabl[TABL_REGISTRY],TV_VIEW,TVM_DELETEITEM,(WPARAM)0, (LPARAM)TVI_ROOT);
  char tmp[MAX_PATH];

  item_ref_current_key_test=-1;
  BOOL cong_reg_global = FALSE;
  if (IsDlgButtonChecked(Tabl[TABL_CONF],CHK_CONF_REG_GLOBAL_LOCAL) == BST_CHECKED)cong_reg_global = TRUE;

  if ((BOOL)lParam || cong_reg_global) //local
  {
    if (cong_reg_global)
    {
      HANDLE hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_VIEW);
      EnumKeyAndValue(HKEY_CURRENT_USER, "", hlv, AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,"HKEY_CURRENT_USER",TVI_ROOT,ICON_FILE_DOSSIER), "");
      EnumKeyAndValue(HKEY_LOCAL_MACHINE, "", hlv, AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,"HKEY_LOCAL_MACHINE",TVI_ROOT,ICON_FILE_DOSSIER), "");   //ajouter la gestion des privilèges ^^
      EnumKeyAndValue(HKEY_USERS, "", hlv, AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,"HKEY_USERS",TVI_ROOT,ICON_FILE_DOSSIER), "");
      EnumKeyAndValue(HKEY_CLASSES_ROOT, "", hlv, AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,"HKEY_CLASSES_ROOT",TVI_ROOT,ICON_FILE_DOSSIER), "");
      EnumKeyAndValue(HKEY_CURRENT_CONFIG, "", hlv, AjouterItemTreeViewFile(Tabl[TABL_REGISTRY], TV_VIEW,"HKEY_CURRENT_CONFIG",TVI_ROOT,ICON_FILE_DOSSIER), "");

      snprintf(tmp,MAX_PATH,"load %u values",ListView_GetItemCount(hlv));
      SB_add_T(SB_ONGLET_REGISTRY, tmp);
    }else
    {
      registry_configuration(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF));
      registry_software(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LOGICIEL));
      registry_update(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ));
      registry_services(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_SERVICES));
      resgistry_userassist(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_HISTORIQUE));
      registry_usb(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USB));
      registry_start(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_START));
      registry_lan(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN));
      registry_users(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS));
      registry_password(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PASSWORD));
      registry_mru(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MRU));
      registry_path(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PATH));

      SB_add_T(SB_ONGLET_REGISTRY, "");
    }
  }else
  {
    //récupération du 1er fils dans le treeview
    HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);

    //lecture du nombre d'item ^^
    DWORD test_nb=0;
    do
    {
      test_nb++;
    }while((hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));

    //on énumère tous les items fils du treeview
    hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_REGISTRY, (LPARAM)"");
    nb_items = 0;

    unsigned int size = 0;

    BOOL rescup = FALSE;
    if (IsDlgButtonChecked(Tabl[TABL_CONF],CHK_CONF_REG_FILE_RECOVERY) ==BST_CHECKED)rescup = TRUE;

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
        //suivant si un fichier .reg ou non ^^
        size = strlen(tmp);
        if (size>4)
        {
          if (tmp[size-4]=='.' && (tmp[size-3]=='r'||tmp[size-3]=='R'))//suivant si un fichier .reg
          {
            AnalyseFichierReg(tmp);
          }else //fichier registre binaire
          {
            if (tmp[size-3]=='.' && (tmp[size-2]=='h'||tmp[size-2]=='H')&& (tmp[size-1]=='v'||tmp[size-1]=='V'))//windows CE file
              AnalyseFichierRegWCEBin(tmp);
            else if (rescup)AnalyseFichierRegBin2(tmp);
            else AnalyseFichierRegBin(tmp);
          }
        }else //fichier registre binaire
        {
          if (rescup)AnalyseFichierRegBin2(tmp);
          else AnalyseFichierRegBin(tmp);
        }
      }
    }while((hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)) && ScanStart);
    SendDlgItemMessage(Tabl[TABL_CONF],TV_VIEW, TVM_SORTCHILDREN,(WPARAM)TRUE, (LPARAM)TVI_ROOT);//tri par orde alphabetique

    if (nb_items)
    {
      //test si les hash users ont été déchiffrés
      HANDLE hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS);
      DWORD i, k = ListView_GetItemCount(hlv);

      if (syskey_ && secret_ && k > 0)
      {
        char data[MAX_LINE_SIZE], user[256], rid_c[256];
        char *c;
        for (i=0;i<k;i++)
        {
          //lecture du hash de chaque utilisateur et vérification si le bon format ^^
          data[0]=0;
          ListView_GetItemText(hlv,i,9,data,MAX_LINE_SIZE);
          if(data[0]!=0 && data[0]==':')
          {
            user[0]=0;
            ListView_GetItemText(hlv,i,2,user,256);
            rid_c[0]=0;
            ListView_GetItemText(hlv,i,3,rid_c,256);
            if (user[0] != 0 && rid_c[0]!=0)
            {
              //extraxion rid_c
              c = rid_c;
              while (*c)c++;
              while (*c!='-')c--;
              c++;

              DecodeSAMHash(sk_c,data,atoi(c), user, secret_c);
              ListView_SetItemText(hlv,i,9,data);
            }
          }
        }
      }
    }else SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_REGISTRY, (LPARAM)"");

    snprintf(tmp,MAX_PATH,"load %u values",ListView_GetItemCount(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_VIEW)));
    SB_add_T(SB_ONGLET_REGISTRY, tmp);
  }

  h_scan_registry = NULL;
  if (!h_scan_logs && !h_scan_files && !h_scan_registry)
  {
    ScanStart = FALSE;
    SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_START),"Start");
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_INFO, (LPARAM)"");
  }

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_REGISTRY,FALSE);

  return 0;
}

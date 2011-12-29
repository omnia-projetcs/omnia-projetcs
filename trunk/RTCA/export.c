//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
DWORD WINAPI BackupRegFile(LPVOID lParam)
{
  //choix du répertoire de destination ^^
  BROWSEINFO browser;
  LPITEMIDLIST lip;
  char tmp[MAX_PATH+1]="";
  char cmd[MAX_PATH*2]="";
  browser.hwndOwner = Tabl[TABL_MAIN];;
  browser.pidlRoot = 0;
  browser.lpfn = 0;
  browser.iImage = 0;
  browser.ulFlags = BIF_NEWDIALOGSTYLE; //permet la création d'un dossier
  browser.lParam = 0;
  browser.pszDisplayName = tmp;  //résultat ici
  browser.lpszTitle = "Directory to save :";
  lip = SHBrowseForFolder(&browser);
  if (lip != NULL)
  {
    SHGetPathFromIDList(lip,tmp);
    if (strlen(tmp)>0)
    {
      //http://msdn.microsoft.com/en-us/library/windows/desktop/ms724877%28v=vs.85%29.aspx
      //Software
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\Software \"%s\\SOFTWARE\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //System
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\System \"%s\\SYSTEM\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //SAM
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\SAM \"%s\\SAM\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Security
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\Security \"%s\\SECURITY\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Current user
      snprintf(cmd,MAX_PATH*2,"SAVE HKCU \"%s\\NTUSER.DAT\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Default
      snprintf(cmd,MAX_PATH*2,"SAVE HKU\\.DEFAULT \"%s\\DEFAULT\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Config hardware volatile keys
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\HARDWARE \"%s\\HARDWARE_volatile_key\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      MessageBox(0,tmp,"Done - Registry copy to:",MB_OK|MB_TOPMOST);
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
void ExportLVSelecttoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE], buffer[MAX_LINE_SIZE];
        DWORD copiee;

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          strncat(ligne,"\"",MAX_LINE_SIZE);
          strncat(ligne,buffer,MAX_LINE_SIZE);
          strncat(ligne,"\";\0",MAX_LINE_SIZE);

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //on vérifie que la ligne est bien sélectionnée
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
          {
            ligne[0]=0;
            for (i=0;i<nb_colonne;i++)//colonne par colonne
            {
              buffer[0]=0;
              ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\"",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\";\0",MAX_LINE_SIZE);
            }
            strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
          }
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVtoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE+1], buffer[MAX_LINE_SIZE+1];
        DWORD copiee;

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          if (strlen(buffer)>0)
          {
            strncat(ligne,"\"",MAX_LINE_SIZE);
            strncat(ligne,buffer,MAX_LINE_SIZE);
            strncat(ligne,"\";\0",MAX_LINE_SIZE);
          }

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          ligne[0]=0;
          for (i=0;i<nb_colonne;i++)//colonne par colonne
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              strncat(ligne,"\"",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE-MAX_PATH);
              strncat(ligne,"\";",MAX_LINE_SIZE);
            }else strncat(ligne,"\"\";",MAX_LINE_SIZE);
          }
          strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
          copiee = 0;
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void strToREG_MULTI_SZ(char *buffer)
{
  char tmp[MAX_LINE_SIZE];
  char ct[3];
  char *c = buffer;
  while (*c++)
  {
    if (*c == ' ')strncat(tmp,"00",MAX_LINE_SIZE);
    else
    {
      snprintf(ct,3,"%02X",*c & 0XFF);
      strncat(tmp,ct,MAX_LINE_SIZE);
    }
  }
  strncat(tmp,"\0",MAX_LINE_SIZE);
  strcpy(buffer,tmp);
}
//------------------------------------------------------------------------------
void ExportLVINF(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>=5)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne[MAX_LINE_SIZE+1]="", buffer[MAX_LINE_SIZE+1];
        DWORD copiee;

        //header
        strcpy(ligne,
        "; Install example : RunDll32.exe setupapi,InstallHinfSection DefaultInstall 132 FILE.inf"
        "[Version]\r\n"
        "Signature=$CHICAGO$\r\n"
        "Provider=RTCA from http://code.google.com/p/omnia-projetcs/\r\n\r\n"
        "[DefaultInstall]\r\n"
        "AddReg=Add.Settings\r\n\r\n"
        "[Add.Settings]\r\n");
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des données
        DWORD j;

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //extraction de la source
          buffer[0]=0;
          ListView_GetItemText(hlv,j,1,buffer,MAX_LINE_SIZE);

          if (buffer[5] == 'U')//HKEY_USERS
          {
            snprintf(ligne,MAX_LINE_SIZE,"HKU, %s,",&buffer[strlen("HKEY_USERS")+1]);
          }else if (buffer[5] == 'L')//HKEY_LOCAL_MACHINE
          {
            snprintf(ligne,MAX_LINE_SIZE,"HKLM, %s,",&buffer[strlen("HKEY_LOCAL_MACHINE")+1]);
          }else if (buffer[5] == 'C')
          {
            if (buffer[13] == 'U')//HKEY_CURRENT_USER
            {
              snprintf(ligne,MAX_LINE_SIZE,"HKCU, %s,",&buffer[strlen("HKEY_CURRENT_USER")+1]);
            }else if (buffer[13] == 'R')//HKEY_CLASSES_ROOT
            {
              snprintf(ligne,MAX_LINE_SIZE,"HKCR, %s,",&buffer[strlen("HKEY_CLASSES_ROOT")+1]);
            }else continue;
          }else continue;

          //nom de la valeur
          buffer[0]=0;
          ListView_GetItemText(hlv,j,2,buffer,MAX_LINE_SIZE);
          strncat(ligne,buffer,MAX_LINE_SIZE);
          strncat(ligne,",",MAX_LINE_SIZE);

          //Type+data
          buffer[0]=0;
          ListView_GetItemText(hlv,j,4,buffer,MAX_LINE_SIZE);
          switch(buffer[4])
          {
            case 'S'://REG_SZ
              strncat(ligne,"0x00000000,\"",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'B'://REG_BINARY
              strncat(ligne,"0x00000001,",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'M'://REG_MULTI_SZ
              strncat(ligne,"0x00010000,",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strToREG_MULTI_SZ(buffer);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'E'://REG_EXPAND_SZ
              strncat(ligne,"0x00020000,\"",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'D'://REG_DWORD
            case 'Q'://REG_QWORD
              strncat(ligne,"0x00010001,0x",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'N'://REG_NONE
              strncat(ligne,"0x00020001,",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            default : continue;
          }

          //ajout
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVSelectINF(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>=5)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne[MAX_LINE_SIZE+1]="", buffer[MAX_LINE_SIZE+1];
        DWORD copiee;

        //header
        strcpy(ligne,
        "; Install example : RunDll32.exe setupapi,InstallHinfSection DefaultInstall 132 FILE.inf"
        "[Version]\r\n"
        "Signature=$CHICAGO$\r\n"
        "Provider=RTCA from http://code.google.com/p/omnia-projetcs/\r\n\r\n"
        "[DefaultInstall]\r\n"
        "AddReg=Add.Settings\r\n\r\n"
        "[Add.Settings]\r\n");
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des données
        DWORD j;

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) != LVIS_SELECTED)
            continue;

          //extraction de la source
          buffer[0]=0;
          ListView_GetItemText(hlv,j,1,buffer,MAX_LINE_SIZE);

          if (buffer[5] == 'U')//HKEY_USERS
          {
            snprintf(ligne,MAX_LINE_SIZE,"HKU, %s,",&buffer[strlen("HKEY_USERS")+1]);
          }else if (buffer[5] == 'L')//HKEY_LOCAL_MACHINE
          {
            snprintf(ligne,MAX_LINE_SIZE,"HKLM, %s,",&buffer[strlen("HKEY_LOCAL_MACHINE")+1]);
          }else if (buffer[5] == 'C')
          {
            if (buffer[13] == 'U')//HKEY_CURRENT_USER
            {
              snprintf(ligne,MAX_LINE_SIZE,"HKCU, %s,",&buffer[strlen("HKEY_CURRENT_USER")+1]);
            }else if (buffer[13] == 'R')//HKEY_CLASSES_ROOT
            {
              snprintf(ligne,MAX_LINE_SIZE,"HKCR, %s,",&buffer[strlen("HKEY_CLASSES_ROOT")+1]);
            }else continue;
          }else continue;

          //nom de la valeur
          buffer[0]=0;
          ListView_GetItemText(hlv,j,2,buffer,MAX_LINE_SIZE);
          strncat(ligne,buffer,MAX_LINE_SIZE);
          strncat(ligne,",",MAX_LINE_SIZE);

          //Type+data
          buffer[0]=0;
          ListView_GetItemText(hlv,j,4,buffer,MAX_LINE_SIZE);
          switch(buffer[4])
          {
            case 'S'://REG_SZ
              strncat(ligne,"0x00000000,\"",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'B'://REG_BINARY
              strncat(ligne,"0x00000001,",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'M'://REG_MULTI_SZ
              strncat(ligne,"0x00010000,",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strToREG_MULTI_SZ(buffer);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'E'://REG_EXPAND_SZ
              strncat(ligne,"0x00020000,\"",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'D'://REG_DWORD
            case 'Q'://REG_QWORD
              strncat(ligne,"0x00010001,0x",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            case 'N'://REG_NONE
              strncat(ligne,"0x00020001,0x",MAX_LINE_SIZE);
              buffer[0]=0;
              ListView_GetItemText(hlv,j,3,buffer,MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            break;
            default : continue;
          }

          //ajout
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVREG(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>=5)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne[MAX_LINE_SIZE+1]="", last_path[MAX_LINE_SIZE+1]="";
        char name[MAX_LINE_SIZE+1], value[MAX_LINE_SIZE+1], type[MAX_LINE_SIZE+1];
        DWORD copiee;

        //header
        strcpy(ligne,"Windows Registry Editor Version 5.00\r\n;Extract from RTCA http://code.google.com/p/omnia-projetcs/\r\n;\r\n\r\n");
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des données
        DWORD i =0,j;

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //lecture de la colonne du path !
          ListView_GetItemText(hlv,j,1,type,MAX_LINE_SIZE);
          snprintf(ligne,MAX_LINE_SIZE,"\r\n[%s]\r\n",type);
          if (strcmp(ligne,last_path)!=0)
          {
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            strcpy(last_path,ligne);
          }

          //lecture des données écrites seulement si plus grande que 0
          name[0]=0;
          value[0]=0;
          type[0]=0;
          ListView_GetItemText(hlv,j,2,name,MAX_LINE_SIZE);
          ListView_GetItemText(hlv,j,3,value,MAX_LINE_SIZE);
          ListView_GetItemText(hlv,j,4,type,MAX_LINE_SIZE);

          if ((strlen(name)>0 || strlen(value)>0) && strlen(type)>0)
          {
            //suivant le format
            if (!strcmp("REG_SZ",type))
            {
              if (name[0]==0)snprintf(ligne,MAX_LINE_SIZE,"@=\"%s\"\r\n",value);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=\"%s\"\r\n",name,value);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }else if (!strcmp("REG_EXPAND_SZ",type))//--
            {
              if (name[0]==0)snprintf(ligne,MAX_LINE_SIZE,"@=hex(2):%02X",(value[0]&0xff));
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=hex(2):%02X",name,value[0]&0xff);

              for (i=1;i<strlen(value);i++)
              {
                sprintf(type,",%02X",value[0]&0xff);
                strncat(ligne,type,MAX_LINE_SIZE);
              }
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            }else if (!strcmp("REG_BINARY",type))//--
            {
              if (name[0]==0)snprintf(ligne,MAX_LINE_SIZE,"@=hex:%c%c",value[0],value[1]);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=hex:%c%c",name,value[0],value[1]);

              for (i=2;i<strlen(value);i+=2)
              {
                sprintf(type,",%c%c",value[i],value[i+1]);
                strncat(ligne,type,MAX_LINE_SIZE);
              }
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            }else if (!strcmp("REG_DWORD",type))
            {
              if (name[0]==0) snprintf(ligne,MAX_LINE_SIZE,"@=dword:%s\r\n",value);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=dword:%s\r\n",name,value);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }else if (!strcmp("REG_NONE",type))
            {
              if (name[0]==0) strcpy(ligne,"@=\"\"\r\n");
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=\"\"\r\n",name);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }
          }
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVSelectREG(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>=5)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne[MAX_LINE_SIZE+1]="", last_path[MAX_LINE_SIZE+1]="";
        char name[MAX_LINE_SIZE+1], value[MAX_LINE_SIZE+1], type[MAX_LINE_SIZE+1];
        DWORD copiee;

        //header
        strcpy(ligne,"Windows Registry Editor Version 5.00\r\n;Extract from RTCA http://code.google.com/p/omnia-projetcs/\r\n;\r\n\r\n");
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des données
        DWORD i =0,j;

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) != LVIS_SELECTED)
            continue;

          //lecture de la colonne du path !
          ListView_GetItemText(hlv,j,1,type,MAX_LINE_SIZE);
          snprintf(ligne,MAX_LINE_SIZE,"\r\n[%s]\r\n",type);
          if (strcmp(ligne,last_path)!=0)
          {
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            strcpy(last_path,ligne);
          }

          //lecture des données écrites seulement si plus grande que 0
          name[0]=0;
          value[0]=0;
          type[0]=0;
          ListView_GetItemText(hlv,j,2,name,MAX_LINE_SIZE);
          ListView_GetItemText(hlv,j,3,value,MAX_LINE_SIZE);
          ListView_GetItemText(hlv,j,4,type,MAX_LINE_SIZE);

          if ((strlen(name)>0 || strlen(value)>0) && strlen(type)>0)
          {
            //suivant le format
            if (!strcmp("REG_SZ",type))
            {
              if (name[0]==0)snprintf(ligne,MAX_LINE_SIZE,"@=\"%s\"\r\n",value);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=\"%s\"\r\n",name,value);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }else if (!strcmp("REG_EXPAND_SZ",type))
            {
              if (name[0]==0)snprintf(ligne,MAX_LINE_SIZE,"@=hex(2):%02X",value[0]&0xff);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=hex(2):%02X",name,value[0]&0xff);

              for (i=1;i<strlen(value);i++)
              {
                sprintf(type,",%02X",value[0]&0xff);
                strncat(ligne,type,MAX_LINE_SIZE);
              }
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            }else if (!strcmp("REG_BINARY",type))
            {
              if (name[0]==0)snprintf(ligne,MAX_LINE_SIZE,"@=hex:%c%c",value[0],value[1]);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=hex:%c%c",name,value[0],value[1]);

              for (i=2;i<strlen(value);i+=2)
              {
                sprintf(type,",%c%c",value[i],value[i+1]);
                strncat(ligne,type,MAX_LINE_SIZE);
              }
              strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            }else if (!strcmp("REG_DWORD",type))
            {
              if (name[0]==0) snprintf(ligne,MAX_LINE_SIZE,"@=dword:%s\r\n",value);
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=dword:%s\r\n",name,value);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }else if (!strcmp("REG_NONE",type))
            {
              if (name[0]==0) strcpy(ligne,"@=\"\"\r\n");
              else snprintf(ligne,MAX_LINE_SIZE,"\"%s\"=\"\"\r\n",name);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }
          }
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVtoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE+1], buffer[MAX_LINE_SIZE+1];
        DWORD copiee;

        //start
        char head[]="<html>\r\n<head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n<table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n<tr bgcolor=\"#C0C0C0\">";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          if (strlen(buffer)>0)
          {
            strncat(ligne,"<th>",MAX_LINE_SIZE);
            strncat(ligne,buffer,MAX_LINE_SIZE);
            strncat(ligne,"</th>\0",MAX_LINE_SIZE);
          }

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          if (j%2==1)strcpy(ligne,"<tr bgcolor=\"#ddddff\">");
          else strcpy(ligne,"<tr>");

          for (i=0;i<nb_colonne;i++)//colonne par colonne
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              strncat(ligne,"<td>",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE-MAX_PATH);
              strncat(ligne,"</td>",MAX_LINE_SIZE);
            }else strncat(ligne,"<td></td>",MAX_LINE_SIZE);
          }
          strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
          copiee = 0;
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }

        //fin de ligne
        WriteFile(MyhFile,"</table>\r\n</html>",17,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVSelecttoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE], buffer[MAX_LINE_SIZE];
        DWORD copiee;

        //start
        char head[]="<html>\r\n<head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n<table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n<tr bgcolor=\"#C0C0C0\">";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          strncat(ligne,"<th>",MAX_LINE_SIZE);
          strncat(ligne,buffer,MAX_LINE_SIZE);
          strncat(ligne,"</th>\0",MAX_LINE_SIZE);

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //on vérifie que la ligne est bien sélectionnée
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
          {
            if (j%2==1)strcpy(ligne,"<tr bgcolor=\"#ddddff\">");
            else strcpy(ligne,"<tr>");
            for (i=0;i<nb_colonne;i++)//colonne par colonne
            {
              buffer[0]=0;
              ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
              strncat(ligne,"<td>",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"</td>",MAX_LINE_SIZE);
            }
            strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
          }
        }

        //fin de ligne
        WriteFile(MyhFile,"</table>\r\n</html>",17,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVtoXML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE+1], buffer[MAX_LINE_SIZE+1];
        DWORD copiee;
        LINE_ITEM lv_line[SIZE_UTIL_ITEM+1];

        //start
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n  <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = lv_line[0].c;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          //réinit
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[++i].c;
        }

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          WriteFile(MyhFile,"  <Data>\r\n",10,&copiee,0);
          ligne[0]=0;
          for (i=0;i<nb_colonne;i++)//colonne par colonne
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(ligne,MAX_LINE_SIZE,"     <%s><![CDATA[%s]]><%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }
          }
          WriteFile(MyhFile,"  </Data>\r\n",11,&copiee,0);
        }
        //fin de ligne
        WriteFile(MyhFile,"</RtCA>",6,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVSelecttoXML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE], buffer[MAX_LINE_SIZE];
        DWORD copiee;
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];

        //start
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n  <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = lv_line[0].c;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          //réinit
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[++i].c;
        }

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //on vérifie que la ligne est bien sélectionnée
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
          {
            WriteFile(MyhFile,"  <Data>\r\n",10,&copiee,0);
            ligne[0]=0;
            for (i=0;i<nb_colonne;i++)//colonne par colonne
            {
              buffer[0]=0;
              ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
              if (buffer != NULL && strlen(buffer)>0)
              {
                snprintf(ligne,MAX_LINE_SIZE,"     <%s><![CDATA[%s]]><%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
                WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
              }
            }
            WriteFile(MyhFile,"  </Data>\r\n",11,&copiee,0);
          }
        }

        //fin de ligne
        WriteFile(MyhFile,"</RtCA>",6,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVColto(char *path, unsigned int id_tabl, int lv, unsigned short col)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    //traitement
    HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      char ligne [MAX_LINE_SIZE];
      DWORD copiee;
      DWORD j;

      //traitement des lignes
      HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
      for (j=0;j<NBLigne;j++)//ligne par ligne
      {
        ligne[0]=0;
        ListView_GetItemText(hlv,j,col,ligne,MAX_LINE_SIZE);
        if (ligne[0]!=0)
        {
          strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }
      }
    }
    CloseHandle(MyhFile);
  }
}
//------------------------------------------------------------------------------
void ExportLVSelectColto(char *path, unsigned int id_tabl, int lv, unsigned short col)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    //traitement
    HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      char ligne [MAX_LINE_SIZE];
      DWORD copiee;
      DWORD j;

      //traitement des autres lignes
      HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
      for (j=0;j<NBLigne;j++)//ligne par ligne
      {
        //on vérifie que la ligne est bien sélectionnée
        if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
        {
          ligne[0]=0;
          ListView_GetItemText(hlv,j,col,ligne,MAX_LINE_SIZE);
          if (ligne[0]!=0)
          {
            strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
          }
        }
      }
    }
    CloseHandle(MyhFile);
  }
}
//------------------------------------------------------------------------------
void LVSaveAll(unsigned int id_tabl, int lv,unsigned short nb_colonne,BOOL selection_only,BOOL pwdump, BOOL front_registry)
{
  char path[MAX_PATH]="";

  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = Tabl[TABL_MAIN];
  ofn.lpstrFile = path;
  ofn.nMaxFile = MAX_PATH;
  if (pwdump) ofn.lpstrFilter ="File CSV\0*.csv\0File HTML\0*.html\0File XML\0*.xml\0File Pwdump\0*.pwdump\0";
  else if (front_registry) ofn.lpstrFilter ="File CSV\0*.csv\0File HTML\0*.html\0File XML\0*.xml\0File REG\0*.reg\0File INF\0*.inf\0";
  else ofn.lpstrFilter ="File CSV\0*.csv\0File HTML\0*.html\0File XML\0*.xml\0";

  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt =".csv\0";

  if (GetSaveFileName(&ofn)==TRUE)
  {
    if (ofn.nFilterIndex == 1) //csv
    {
      //sauvegarde
      if (selection_only)ExportLVSelecttoCSV(path, id_tabl, lv, nb_colonne);
      else ExportLVtoCSV(path, id_tabl, lv, nb_colonne);
    }else if (ofn.nFilterIndex == 2) //html
    {
      if (selection_only)ExportLVSelecttoHTML(path, id_tabl, lv, nb_colonne);
      else ExportLVtoHTML(path, id_tabl, lv, nb_colonne);
    }else if (ofn.nFilterIndex == 3) //XML
    {
      if (selection_only)ExportLVSelecttoXML(path, id_tabl, lv, nb_colonne);
      else ExportLVtoXML(path, id_tabl, lv, nb_colonne);
    }else if (ofn.nFilterIndex == 4)
    {
      if (pwdump)//pwdump
      {
        if (selection_only)ExportLVSelectColto(path, id_tabl, lv, 9);
        else ExportLVColto(path, id_tabl, lv, 9);
      }else //registry
      {
        if (selection_only)ExportLVSelectREG(path, id_tabl, lv,5);
        else ExportLVREG(path, id_tabl, lv,5);
      }
    }else if (ofn.nFilterIndex == 5)
    {
      if (selection_only)ExportLVSelectINF(path, id_tabl, lv,5);
      else ExportLVINF(path, id_tabl, lv,5);
    }
  }
}
//------------------------------------------------------------------------------
void ExportTrv(HANDLE htree, char *path)
{
  //traitement
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    char ligne [MAX_PATH], tmp[MAX_PATH];
    DWORD copiee;
    TVITEM tvitem;

    //création de la première ligne de titre
    strcpy(ligne,"\"Type\";\"Path\"\r\n");
    WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

    //traitement des ruches une par une
    //récupération du 1er fils dans le treeview
    HTREEITEM hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_LOGS]);
    do
    {
      //récupération du texte de l'item
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendMessage(htree, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        snprintf(ligne,MAX_PATH,"\"LOGS\";\"%s\"\r\n",tmp);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
      }

    }while((hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));

    hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_FILES]);
    do
    {
      //récupération du texte de l'item
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendMessage(htree, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        snprintf(ligne,MAX_PATH,"\"FILES\";\"%s\"\r\n",tmp);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
      }

    }while((hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));

    hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);
    do
    {
      //récupération du texte de l'item
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendMessage(htree, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        snprintf(ligne,MAX_PATH,"\"REGISTRY\";\"%s\"\r\n",tmp);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
      }

    }while((hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));
  }
  CloseHandle(MyhFile);
}
//------------------------------------------------------------------------------
void TreeExport(HANDLE htree)
{
  char path[MAX_PATH]="";

  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = Tabl[TABL_MAIN];
  ofn.lpstrFile = path;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter ="File CSV\0*.csv\0";
  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt =".csv\0";

  if (GetSaveFileName(&ofn)==TRUE)
  {
    //sauvegarde
    ExportTrv(htree, path);
  }
}
//------------------------------------------------------------------------------
//export global des résultats
DWORD WINAPI Export(LPVOID lParam)
{
  int type = (int)lParam;

  //emplacement de la sauvegarde répertoire
  BROWSEINFO browser;
  ITEMIDLIST *lip;
  char *c,tmp[MAX_PATH]="",path[MAX_PATH]="";

   browser.hwndOwner          = Tabl[TABL_MAIN];
   browser.pidlRoot           = NULL;
   browser.pszDisplayName     = path;
   browser.lpszTitle          = "Destination directory :";
   browser.ulFlags            = BIF_NEWDIALOGSTYLE; //permet la création de répertoire en direct
   browser.lpfn               = NULL;
   if((lip=SHBrowseForFolder(&browser))!=NULL)
   {
     SHGetPathFromIDList(lip,path);
     if (strlen(path)>0)
     {
      //création du path initial + date
      strncat(path,"\\RTCA[\0",MAX_PATH);
      time_t dateEtHMs;
      time(&dateEtHMs);

      //on supprime les caractères incompatibles avec le nom d'un fichier
      strncpy(tmp,(char *)ctime(&dateEtHMs),MAX_PATH);
      tmp[strlen(tmp)-1]=0;
      c = tmp;
      *(c+=3) = '_';
      *(c+=4) = '_';
      *(c+=3) = '_';
      *(c+=3) = 'h';
      *(c+=3) = 'm';
      *(c+=3) = '_';
      strncat(path,tmp,MAX_PATH);
      strncat(path,"]\0",MAX_PATH);


      switch(type)
      {
        case HTML_TYPE :
           //création des rapports
           strcpy(tmp,path);
           strncat(tmp,"_AUDIT.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_LOGS, LV_LOGS_VIEW, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_FILES.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_FILES, LV_FILES_VIEW, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_REGISTRY.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_CONGIGURATION.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_CONF, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_SOFTWARE.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_LOGICIEL, NB_COLONNE_LV[LV_REGISTRY_LOGICIEL_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_UPDATE.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_MAJ, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_SERVICES_DRIVERS.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_SERVICES, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USERASSIST.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_HISTORIQUE, NB_COLONNE_LV[LV_REGISTRY_HISTORIQUE_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USB.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_USB, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_START.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_START, NB_COLONNE_LV[LV_REGISTRY_START_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_NETWORK.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_LAN, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USERS.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_USERS, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_PASSWORD.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_PASSWORD, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_MRU.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_MRU, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_Path.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_REGISTRY, LV_REGISTRY_PATH, NB_COLONNE_LV[LV_REGISTRY_PATH_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_PROCESS.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_PROCESS, LV_VIEW, NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_ALL_STATE.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_CRITICAL_STATE.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_STATE, LV_VIEW_CRITICAL, NB_COLONNE_LV[LV_STATE_H_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_DAY_STATE.html",MAX_PATH);
           ExportLVtoHTML(tmp, TABL_STATE, LV_VIEW_H, NB_COLONNE_LV[LV_STATE_H_VIEW_NB_COL]);
        break;
        case XML_TYPE :
           //création des rapports
           strcpy(tmp,path);
           strncat(tmp,"_AUDIT.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_LOGS, LV_LOGS_VIEW, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_FILES.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_FILES, LV_FILES_VIEW, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_REGISTRY.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_CONGIGURATION.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_CONF, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_SOFTWARE.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_LOGICIEL, NB_COLONNE_LV[LV_REGISTRY_LOGICIEL_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_UPDATE.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_MAJ, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_SERVICES_DRIVERS.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_SERVICES, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USERASSIST.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_HISTORIQUE, NB_COLONNE_LV[LV_REGISTRY_HISTORIQUE_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USB.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_USB, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_START.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_START, NB_COLONNE_LV[LV_REGISTRY_START_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_NETWORK.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_LAN, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USERS.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_USERS, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_PASSWORD.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_PASSWORD, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_MRU.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_MRU, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_Path.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_REGISTRY, LV_REGISTRY_PATH, NB_COLONNE_LV[LV_REGISTRY_PATH_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_PROCESS.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_PROCESS, LV_VIEW, NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_ALL_STATE.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_DAY_STATE.xml",MAX_PATH);
           ExportLVtoXML(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_H_VIEW_NB_COL]);
        break;
        case CSV_TYPE :
        default:
           //création des rapports
           strcpy(tmp,path);
           strncat(tmp,"_AUDIT.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_LOGS, LV_LOGS_VIEW, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_FILES.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_FILES, LV_FILES_VIEW, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_REGISTRY.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_CONGIGURATION.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_CONF, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_SOFTWARE.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_LOGICIEL, NB_COLONNE_LV[LV_REGISTRY_LOGICIEL_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_UPDATE.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_MAJ, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_SERVICES_DRIVERS.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_SERVICES, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USERASSIST.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_HISTORIQUE, NB_COLONNE_LV[LV_REGISTRY_HISTORIQUE_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USB.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_USB, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_START.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_START, NB_COLONNE_LV[LV_REGISTRY_START_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_NETWORK.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_LAN, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_USERS.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_USERS, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_PASSWORD.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_PASSWORD, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_MRU.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_MRU, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_REG_Path.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_PATH, NB_COLONNE_LV[LV_REGISTRY_PATH_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_PROCESS.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_PROCESS, LV_VIEW, NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_ALL_STATE.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_VIEW_NB_COL]);
           strcpy(tmp,path);
           strncat(tmp,"_DAY_STATE.csv",MAX_PATH);
           ExportLVtoCSV(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_H_VIEW_NB_COL]);
        break;
      }
      //pwdump
      strcpy(tmp,path);
      strncat(tmp,"_REG_USERS.pwdump",MAX_PATH);
      ExportLVColto(tmp, TABL_REGISTRY, LV_REGISTRY_USERS, 9);
     }
   }

  //fin
  ExportStart = FALSE;
  SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_EXPORT),"Export");
  return 0;
}
